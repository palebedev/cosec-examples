#include <ce/tcp_listener.hpp>

#include <ce/socket_session.hpp>

#include <boost/asio/ip/v6_only.hpp>
#include <boost/log/trivial.hpp>

namespace ce
{
    tcp_listener::tcp_listener(ba::any_io_executor ex,std::uint16_t port,
            executor_factory_t executor_factory,session_factory_t session_factory)
        : acceptor_{std::move(ex)},
          executor_factory_{std::move(executor_factory)},
          session_factory_{std::move(session_factory)}
    {
        // acceptor can be constructed with an executor and port directly,
        // but we want to reset ipv6_only option to uniformly accept
        // ipv4/ipv6 connections everywhere.
        ba::ip::tcp::endpoint ep{ba::ip::tcp::v6(),port};
        acceptor_.open(ep.protocol());
        acceptor_.set_option(ba::ip::v6_only{false});
        // While we don't want to load-balance multiple servers binding
        // to the same address, this makes quickly restarting debugging
        // sessions much more painless due to TIME_WAIT socket timeouts.
        acceptor_.set_option(ba::socket_base::reuse_address{true});
        acceptor_.bind(ep);
        acceptor_.listen();
        BOOST_LOG_TRIVIAL(info) << "Listening on port " << port << "...";
        accept();
    }

    void tcp_listener::accept()
    {
        acceptor_.async_accept(executor_factory_(),
                               [this](boost::system::error_code ec,
                                      ba::ip::tcp::socket socket){
            if(ec==ba::error::operation_aborted)
                return;
            accept();
            if(ec)
                BOOST_LOG_TRIVIAL(error) << "Failed to accept connection: " << ec.message();
            else{
                auto log = make_socket_session_logger(socket);
                BOOST_LOG_SEV(log,boost::log::trivial::info) << "Connection accepted";
                try{
                    session_factory_(std::move(socket));
                }
                catch(const std::exception& e){
                    BOOST_LOG_SEV(log,boost::log::trivial::error)
                        << "Failed to create session: " << e.what();
                }
            }
        });
    }
}
