#include "chat.hpp"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/execution/blocking.hpp>
#include <boost/asio/execution/execute.hpp>
#include <boost/asio/execution/relationship.hpp>
#include <boost/asio/require.hpp>

namespace ce
{
    void chat::session_created(chat_session& session)
    {
        boost::asio::execution::execute(strand_,[this,&session]{
            sessions_[&session];
        });
    }

    void chat::session_destroyed(chat_session& session)
    {
        boost::asio::execution::execute(strand_,[this,&session]{
            auto it = sessions_.find(&session);
            auto pos = it->second,end = queue_.end();
            sessions_.erase(it);
            if(pos==queue_t::iterator{})
                return;
            if(pos==queue_.begin()&&pos->senders_remaining_==1){
                pos = std::find_if(std::next(pos),end,[](const message& m){
                    return m.senders_remaining_>1;
                });
                BOOST_LOG_TRIVIAL(trace) << "Dropping " <<
                    std::distance(queue_.begin(),pos) << "messages due to disconnect";
                queue_.erase(queue_.begin(),pos);
            }
            for(;pos!=end;++pos)
                --pos->senders_remaining_;
        });
    }

    void chat::message_received(chat_session& session,std::string text)
    {
        boost::asio::execution::execute(strand_,[&,this]{
            if(sessions_.size()==1){
                BOOST_LOG_TRIVIAL(info) << "Dropping message as there is only one client";
                return;
            }
            if(queue_.size()==max_queue){
                BOOST_LOG_TRIVIAL(info) << "Dropping message due to queue overflow";
                return;
            }
            queue_.emplace_back(session,std::move(text),sessions_.size()-1);
            auto it = std::prev(queue_.end());
            for(auto& [s,pos]:sessions_)
                if(s!=&session&&pos==queue_t::iterator{})
                    send(*s,it);
        });
    }

    void chat::send(chat_session& session,queue_t::iterator it)
    {
        sessions_[&session] = it;
        session.async_write(it->text_,boost::asio::bind_executor(strand_,[&session,it,this]{
            auto rem = --it->senders_remaining_;
            auto next = it;
            do
                ++next;
            while(next!=queue_.end()&&next->sender_==&session);
            if(!rem)
                queue_.erase(it);
            if(next!=queue_.end())
                send(session,next);
            else
                sessions_[&session] = {};
        }));
    }
}
