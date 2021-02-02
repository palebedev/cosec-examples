#include <ce/asio-main.hpp>
#include <ce/charconv.hpp>
#include <ce/format.hpp>
#include <ce/io_context_signal_interrupter.hpp>
#include <ce/socket_session.hpp>
#include <ce/tcp_listener.hpp>

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/execution/execute.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/static_thread_pool.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <memory>
#include <stdexcept>
#include <thread>

// Differences from asio-basic:
// - A thread pool is used to invoke io_context::run() with a number of
//   threads configurable via program argument.
// - Length of numbers read from stream is limited with a second parameter
//   to dynamic buffer.
// - boost::beast::tcp_stream is used instead of normal tcp socket which
//   provides additional functionality.
// - We use a simple_rate_policy to set a limit on bytes/second read.
// - I/O timeout is implemented using tcp_stream::expires_after.
// - Precise catching of exceptions is removed, we rely on catching executor
//   that socket_session has installed for the stream.
// - We actually use a catching executor that is constructed from
//   preferring continuation relationship on stream executor, so our
//   callbacks become continuations.

namespace ce
{
    namespace
    {
        using bigint = boost::multiprecision::cpp_int;

        using socket_executor_t = ba::strand<ba::io_context::executor_type>;
        using tcp_socket = ba::basic_stream_socket<ba::ip::tcp,socket_executor_t>;
        using tcp_stream = bb::basic_stream<ba::ip::tcp,
            socket_executor_t,
            bb::simple_rate_policy>;

        class calc_session final : public socket_session<calc_session,tcp_stream>
        {
            constexpr static std::size_t number_limit_ = 1024,
                                         bytes_per_second_limit = 1024;
            constexpr static boost::asio::steady_timer::duration time_limit_ =
                std::chrono::seconds(15);
        public:
            calc_session(ba::io_context::executor_type ex)
                : socket_session<calc_session,tcp_stream>{std::move(ex)}
            {
                stream_.rate_policy().read_limit(bytes_per_second_limit);
            }

            void start_protocol()
            {
                using namespace boost::log::trivial;
                stream_.expires_after(time_limit_);
                async_read_until(stream_,ba::dynamic_string_buffer{in_buf_,number_limit_},'\n',
                        ba::bind_executor(this->cont_executor(),
                            [s=shared_from_this()](boost::system::error_code ec,std::size_t n) mutable {
                    if(ec){
                        if(ec!=boost::asio::error::eof||n)
                            BOOST_LOG_SEV(s->log(),error) << "Failed to read a: " << ec.message();
                        else
                            BOOST_LOG_SEV(s->log(),info) << "Connection closed";
                        return;
                    }
                    s->a_ = s->read_buffered_number(n);
                    s->stream_.expires_after(time_limit_);
                    async_read_until(s->stream_,ba::dynamic_string_buffer{s->in_buf_,number_limit_},
                            '\n',ba::bind_executor(s->cont_executor(),
                                [s](boost::system::error_code ec,std::size_t n) mutable {
                        if(ec){
                            BOOST_LOG_SEV(s->log(),error)
                                << "Failed to read b: " << ec.message();
                            return;
                        }
                        bigint b;
                        b = s->read_buffered_number(n);
                        s->a_ *= b;
                        s->out_buf_ = s->a_.str()+'\n';
                        s->stream_.expires_after(time_limit_);
                        async_write(s->stream_,ba::buffer(s->out_buf_),
                                ba::bind_executor(s->cont_executor(),
                                    [s](boost::system::error_code ec,std::size_t /*n*/) mutable {
                            if(ec){
                                BOOST_LOG_SEV(s->log(),error) <<
                                    "Failed to write result: " << ec.message();
                                return;
                            }
                            s->start_protocol();
                        }));
                    }));
                }));
            }
        private:
            std::string in_buf_,out_buf_;
            bigint a_;

            bigint read_buffered_number(std::size_t n)
            {
                in_buf_[n-1] = '\0';
                bigint x{in_buf_.data()};
                in_buf_.erase(0,n);
                return x;
            }
        };
    }

    int main(std::span<const char* const> args)
    {
        if(args.size()<2||args.size()>3)
            throw std::runtime_error(format("Usage: ",args[0]," <listen-port> [threads]"));
        auto port = from_chars<std::uint16_t>(args[1]);
        if(!port||!*port)
            throw std::runtime_error("Port must be in [1;65535]");
        unsigned threads;
        if(args.size()==3){
            auto t = from_chars<unsigned>(args[2]);
            if(!t||!*t)
                throw std::runtime_error("Threads must be a non-zero unsigned integer");
            threads = *t;
        }else
            threads = std::thread::hardware_concurrency();
        using namespace boost::log::trivial;
        BOOST_LOG_TRIVIAL(info) << "Using " << threads << " threads.";
        ba::io_context ctx{int(threads)};
        io_context_signal_interrupter iosi{ctx};
        tcp_listener<calc_session,ba::io_context::executor_type> tl{ctx.get_executor(),*port};
        ba::static_thread_pool tp{threads-1};
        for(unsigned i=1;i<threads;++i)
            bae::execute(tp.get_executor(),[&]{
                ctx.run();
            });
        ctx.run();
        return 0;
    }
}

