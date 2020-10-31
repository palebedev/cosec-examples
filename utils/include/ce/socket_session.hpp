#ifndef UUID_A62EF946_4125_424B_AC63_B3B73F116664
#define UUID_A62EF946_4125_424B_AC63_B3B73F116664

#include <ce/asio_ns.hpp>
#include <ce/utils/export.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

#include <memory>
#include <string_view>
#include <type_traits>

namespace ce
{
    CE_UTILS_EXPORT constexpr inline const char* remote_attr_name = "Remote";

    // In many cases, especially when using boost::beast::tcp_stream,
    // we're guaranteed to be operating under strand and never using log
    // from multiple threads. Still, there are same cases like proxying,
    // where we might have several callbacks in flight that might be invoked
    // in parallel, so we're using a multi-threading safe logger.
    using socket_session_logger_t =
        boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;

    CE_UTILS_EXPORT socket_session_logger_t make_socket_session_logger(
        const ba::ip::tcp::socket& socket);

    BOOST_LOG_ATTRIBUTE_KEYWORD(remote,remote_attr_name,ba::ip::tcp::socket::endpoint_type)

    class socket_session_base
    {
    public:
        socket_session_logger_t& log() noexcept
        {
            return log_;
        }
    protected:
        socket_session_logger_t log_;

        explicit socket_session_base(const ba::ip::tcp::socket& socket)
            : log_{make_socket_session_logger(socket)}
        {}
    };

    // Base class for session objects that need enable_shared_from_this functionality,
    // store a stream object and provide a logger with severity and remote endpoint attributes.

    template<typename Derived,
             typename Stream = ba::ip::tcp::socket>
    class socket_session : public std::enable_shared_from_this<Derived>,
                           public socket_session_base
    {
    public:
        explicit socket_session(ba::ip::tcp::socket socket)
            : socket_session_base{socket},
              stream_{std::move(socket)}
        {}

        Stream& stream() noexcept
        {
            return stream_;
        }
    protected:
        Stream stream_;
    };
}

#endif
