#include <ce/asio-main.hpp>
#include <ce/charconv.hpp>
#include <ce/format.hpp>
#include <ce/io_context_signal_interrupter.hpp>
#include <ce/socket_session.hpp>
#include <ce/tcp_listener.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <memory>
#include <stdexcept>

namespace ce
{
    namespace
    {
        using bigint = boost::multiprecision::cpp_int;

        // We don't need type-erased executor for our socket,
        // which boost::asio::ip::tcp::socket provides.
        using tcp_socket = ba::basic_stream_socket<ba::ip::tcp,ba::io_context::executor_type>;

        class calc_session final : public socket_session<calc_session,tcp_socket>
        {
        public:
            using socket_session<calc_session,tcp_socket>::socket_session;

            void start_protocol()
            {
                // We are going to handle every source of exception explicitly for this
                // example to precisely log the source. Usually this is overkill.
                using namespace boost::log::trivial;
                // At the point of call to process(), there is a shared_ptr
                // outside this function that will be destroyed after the call.
                // We take another ownership by the callback by capturing another
                // shared_ptr to this.
                // If async_read_until fails to initiate allocation, out handler
                // and its shared ownership of this will be destroyed.
                // After logging the exception and returning from this function,
                // session will be destroyed with the last shared_ptr owner.
                // Otherwise ownership will be held by the shared_ptr inside
                // callback and carried until the callback is invoced.
                try{
                    async_read_until(stream_,ba::dynamic_string_buffer{in_buf_},'\n',
                            [s=shared_from_this()](boost::system::error_code ec,std::size_t n) mutable {
                        // Both `this` and s refer to our session, we should use s
                        // everywhere to avoid an extra capture of `this`.
                        if(ec){
                            // If we didn't read any data at all due to end of stream,
                            // this is not an error, but a graceful disconnect.
                            if(ec!=boost::asio::error::eof||n)
                                BOOST_LOG_SEV(s->log(),error) << "Failed to read a: " << ec.message();
                            else
                                BOOST_LOG_SEV(s->log(),info) << "Connection closed";
                            // In either case, we return from the callback.
                            // Since callback owns calc_session through captured shared_ptr,
                            // it will be cleaned up properly.
                            return;
                        }
                        try{
                            s->a_ = s->read_buffered_number(n);
                        }
                        catch(const std::exception& e){
                            BOOST_LOG_SEV(s->log(),error) << "Failed to parse a: " << e.what();
                            return;
                        }
                        try{
                            // Copy shared_ptr that owns our session to the next callback.
                            // We can't move ownership, since we might need it in exception
                            // handler in case initiating function throws.
                            async_read_until(s->stream_,ba::dynamic_string_buffer{s->in_buf_},'\n',
                                    [s](boost::system::error_code ec,std::size_t n) mutable {
                                if(ec){
                                    BOOST_LOG_SEV(s->log(),error)
                                        << "Failed to read b: " << ec.message();
                                    return;
                                }
                                bigint b;
                                try{
                                    b = s->read_buffered_number(n);
                                }
                                catch(const std::exception& e){
                                    BOOST_LOG_SEV(s->log(),error)
                                        << "Failed to parse b: " << e.what();
                                    return;
                                }
                                try{
                                    s->a_ *= b;
                                    s->out_buf_ = s->a_.str()+'\n';
                                }
                                catch(const std::exception& e){
                                    BOOST_LOG_SEV(s->log(),error)
                                        << "Failed to compute result: " << e.what();
                                    return;
                                }
                                try{
                                    async_write(s->stream_,ba::buffer(s->out_buf_),[s]
                                            (boost::system::error_code ec,std::size_t /*n*/) mutable {
                                        if(ec){
                                            BOOST_LOG_SEV(s->log(),error)
                                                << "Failed to write result: " << ec.message();
                                            return;
                                        }
                                        // Start our callback chain from the beginning.
                                        // It will pick up our ownership.
                                        s->start_protocol();
                                    });
                                }
                                catch(const std::exception& e){
                                    BOOST_LOG_SEV(s->log(),error)
                                        << "Failed to initiate write: " << e.what();
                                }
                            });
                        }
                        catch(const std::exception& e){
                            BOOST_LOG_SEV(s->log(),error)
                                << "Failed to initiate read of b: " << e.what();
                        }
                    });
                }
                catch(const std::exception& e){
                    BOOST_LOG_SEV(log(),error) << "Failed to initiate read of a: " << e.what();
                }
            }
        private:
            std::string in_buf_,out_buf_;
            bigint a_;

            bigint read_buffered_number(std::size_t n)
            {
                // boost::multiprecision can only indicate string to number
                // conversion failure with exceptions.
                // There is no string_view-like constructor, but null-terminated
                // strings are accepted. To avoid copy, replace '\n' with '\0'.
                // -1 is to account for \n at end.
                in_buf_[n-1] = '\0';
                bigint x{in_buf_.data()};
                in_buf_.erase(0,n);
                return x;
            }
        };
    }

    int main(std::span<const char* const> args)
    {
        if(args.size()!=2)
            throw std::runtime_error(format("Usage: ",args[0]," <listen-port>"));
        auto port = from_chars<std::uint16_t>(args[1]);
        if(!port||!*port)
            throw std::runtime_error("Port must be in [1;65535]");
        // Infor io_context it can apply single-threaded-optimizations.
        ba::io_context ctx{1};
        io_context_signal_interrupter iosi{ctx};
        tcp_listener<calc_session,ba::io_context::executor_type> tl{ctx.get_executor(),*port};
        ctx.run();
        return 0;
    }
}
