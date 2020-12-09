#ifndef UUID_85957A5B_6292_4893_8556_CAC1F385215C
#define UUID_85957A5B_6292_4893_8556_CAC1F385215C

#include "chat_session.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>

#include <list>
#include <unordered_map>

namespace ce
{
    class chat
    {
    public:
        constexpr static std::size_t max_queue = 100;

        chat(boost::asio::io_context::executor_type ex)
            : strand_{std::move(ex)}
        {}
        void session_created(chat_session& session);
        void session_destroyed(chat_session& session);
        void message_received(chat_session& session,std::string text);
    private:
        struct message
        {
            chat_session* sender_;
            std::string text_;
            std::size_t senders_remaining_;

            message(chat_session& sender,std::string&& text,std::size_t senders_remaining) noexcept
                : sender_{&sender},
                  text_{std::move(text)},
                  senders_remaining_{senders_remaining}
            {}
        };

        using queue_t = std::list<message>;

        queue_t queue_;
        std::unordered_map<chat_session*,queue_t::iterator> sessions_;
        boost::asio::strand<boost::asio::io_context::executor_type> strand_;

        void send(chat_session& session,queue_t::iterator it);
    };
}

#endif
