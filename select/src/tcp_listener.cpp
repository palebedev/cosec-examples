#include "tcp_listener.hpp"

ce::file_descriptor tcp_listener::create_listener(std::uint16_t port)
{
    ce::file_descriptor sock{socket(AF_INET6,SOCK_STREAM,0),"socket"};
    sock.set_non_blocking();
    int value = 0;
    ce::throw_errno_if_negative(setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,&value,sizeof value),
                                "setsockopt:IPV6_V6ONLY");
    value = 1;
    ce::throw_errno_if_negative(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&value,sizeof value),
                                "setsockopt:SO_REUSEADDR");
    sockaddr_in6 si = {AF_INET6,htons(port)};
    ce::throw_errno_if_negative(bind(sock,reinterpret_cast<sockaddr*>(&si),sizeof si),"bind");
    ce::throw_errno_if_negative(listen(sock,queue_len),"listen");
    return sock;
}
