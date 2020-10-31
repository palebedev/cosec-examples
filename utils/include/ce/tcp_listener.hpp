#ifndef UUID_2E00BB3D_3CAF_4FA6_A4C2_CF334E559CCA
#define UUID_2E00BB3D_3CAF_4FA6_A4C2_CF334E559CCA

#include <ce/asio_ns.hpp>
#include <ce/utils/export.h>

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <cstdint>
#include <functional>

namespace ce
{
    class CE_UTILS_EXPORT tcp_listener
    {
    public:
        using executor_factory_t = std::function<ba::any_io_executor ()>;
        using session_factory_t = std::function<void (ba::ip::tcp::socket)>;

        // Start a TCP server that listens on port.
        // Acceptor uses the specified executor.
        // Executor factory provides executors for sockets that accept
        // incoming connections.
        // These sockets are then passed to session factory.
        tcp_listener(ba::any_io_executor ex,std::uint16_t port,
                     executor_factory_t executor_factory,
                     session_factory_t session_factory);

        // As above, but new sockets reuse executor of the acceptor.
        tcp_listener(ba::any_io_executor ex,std::uint16_t port,
                     session_factory_t session_factory)
            : tcp_listener{ex,port,[ex]{ return ex; },session_factory}
        {}
    private:
        ba::ip::tcp::acceptor acceptor_;
        executor_factory_t executor_factory_;
        session_factory_t session_factory_;

        void accept();
    };
}

#endif
