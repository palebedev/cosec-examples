#include "chat_session.hpp"

#include "chat.hpp"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/read_until.hpp>

namespace ce
{
    chat_session::chat_session(ba::io_context::executor_type ex,chat& c)
        : socket_session<chat_session,tcp_stream>{std::move(ex)},
          c_{c}
    {
        stream_.rate_policy().read_limit(bytes_per_second_limit);
    }

    chat_session::~chat_session()
    {
        c_.session_destroyed(*this);
    }

    void chat_session::start_protocol()
    {
        c_.session_created(*this);
        read_line();
    }

    void chat_session::read_line()
    {
        using namespace boost::log::trivial;
        async_read_until(stream_,ba::dynamic_string_buffer{in_buf_,text_limit_},'\n',
                ba::bind_executor(this->cont_executor(),
                    [s=shared_from_this()](boost::system::error_code ec,std::size_t n) mutable {
            if(ec){
                if(ec!=boost::asio::error::eof||n)
                    BOOST_LOG_SEV(s->log(),error) << "Failed to read message: " << ec.message();
                else
                    BOOST_LOG_SEV(s->log(),info) << "Connection closed";
                return;
            }
            s->c_.message_received(*s,s->in_buf_.substr(0,n));
            s->in_buf_.erase(0,n);
            s->read_line();
        }));
    }
}
