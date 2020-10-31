#include <ce/socket_session.hpp>

#include <boost/log/attributes/constant.hpp>

namespace ce
{
    socket_session_logger_t make_socket_session_logger(const ba::ip::tcp::socket& socket)
    {
        socket_session_logger_t logger;
        logger.add_attribute(remote_attr_name,
            boost::log::attributes::constant<ba::ip::tcp::socket::endpoint_type>(
                socket.remote_endpoint()
            )
        );
        return logger;
    } 
}
