#ifndef UUID_8B859C8C_15E8_463C_9B05_74B4ABD0DE94
#define UUID_8B859C8C_15E8_463C_9B05_74B4ABD0DE94

#include "async_file_descriptor.hpp"
#include "reactor.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

class tcp_listener
{
public:
    constexpr static int queue_len = 64;

    template<typename Reactor>
    tcp_listener(Reactor& reactor,std::uint16_t port,
                 std::function<void (async_file_descriptor,const sockaddr_in6&)> acceptor)
        : listener_{create_listener(port)},
          listen_task_{[&,this,acceptor=std::move(acceptor)]{
              sockaddr_in6 si;
              socklen_t len = sizeof si;
              int fd;
              while((fd = accept(listener_,reinterpret_cast<sockaddr*>(&si),&len))>=0){
                  if(len!=sizeof si)
                      throw std::runtime_error{"invalid accept len"};
                  acceptor({fd,"accept"},si);
              }
              if(errno!=EWOULDBLOCK)
                  ce::throw_errno("accept");
              reactor.handle(listener_,reactor_op::read,listen_task_);
          }},
          unlisten_task_{[&,this]{
              reactor.close(std::move(listener_));
          }}
    {
        reactor.handle(listener_,reactor_op::read,listen_task_);
    }

    ~tcp_listener()
    {
        unlisten_task_();
    }
private:
    ce::file_descriptor listener_;
    task_t listen_task_,unlisten_task_;

    static ce::file_descriptor create_listener(std::uint16_t port);
};

#endif
