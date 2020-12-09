#ifndef UUID_565BAB59_6F52_4619_B355_104317D0EEE7
#define UUID_565BAB59_6F52_4619_B355_104317D0EEE7

#include <boost/asio/error.hpp>
#include <boost/system/system_error.hpp>
#include <ce/socket_session.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/tcp_stream.hpp>

namespace ce
{
    using socket_executor_t = ba::strand<ba::io_context::executor_type>;
    using tcp_socket = ba::basic_stream_socket<ba::ip::tcp,socket_executor_t>;
    using tcp_stream = bb::basic_stream<ba::ip::tcp,
        socket_executor_t,
        bb::simple_rate_policy>;

    class chat;

    class chat_session final : public socket_session<chat_session,tcp_stream>
    {
        constexpr static std::size_t text_limit_ = 1024,
                                     bytes_per_second_limit = 1024;
    public:
        chat_session(ba::io_context::executor_type ex,chat& c);
        ~chat_session() override;

        void start_protocol();

        template<typename CompletionToken>
        decltype(auto) async_write(std::string_view message,CompletionToken&& token)
        {
            return boost::asio::async_write(this->stream_,
                boost::asio::const_buffer{message.data(),message.size()},
                [token=std::forward<CompletionToken>(token)]
                        (boost::system::error_code ec,std::size_t /*n*/) mutable {
                    if(ec){
                        if(ec==boost::asio::error::operation_aborted)
                            return;
                        throw boost::system::system_error{ec};
                    }
                    boost::asio::dispatch(token);
                });
        }
    private:
        chat& c_;
        std::string in_buf_;

        void read_line();
    };
}

#endif
