#include "async_file_descriptor.hpp"
#include "composite_async_ops.hpp"
#include "defer_reactor_adapter.hpp"
#include "select_reactor.hpp"
#include "tcp_listener.hpp"
#include "thread_pool.hpp"

#include <ce/charconv.hpp>
#include <ce/format.hpp>

#include <boost/multiprecision/cpp_int.hpp>

#include <arpa/inet.h>
#include <signal.h>

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <exception>
#include <iostream>

namespace
{
    ce::pipe signal_pipe;

    void signal_handler(int)
    {
        std::byte b{};
        try{
            signal_pipe[1].write_as_chunks({&b,1});
        }
        catch(...){
            // Throwing exceptions from a signal handler is UB, if we're here
            // we're in big trouble anyway, just terminate.
            std::terminate();
        }
    }

#ifdef _WIN32
    class WSAInitializer
    {
        WSAInitializer()
        {
            WSADATA wsaData;
            constexpr std::uint16_t version = 0x0202;
            if(WSAStartup(version,&wsaData)||wsaData.wVersion!=version)
                throw std::runtime_error{"Failed to initialize WinSock"};
        }

        ~WSAInitializer()
        {
            WSACleanup();
        }
    };
#endif

    std::ostream& operator<<(std::ostream& stream,const sockaddr_in6& si)
    {
        char buf[INET6_ADDRSTRLEN];
        if(!inet_ntop(AF_INET6,&si.sin6_addr,buf,sizeof buf))
            std::strcpy(buf,"???");
        return stream << '[' << buf << "]:" << ntohs(si.sin6_port);
    }

    class logger
    {
    public:
        logger() = default;
        logger(logger&&) = default;

        ~logger()
        {
            if(lock_.owns_lock())
                std::cerr << '\n';
        }

        template<typename T>
        logger& operator<<(T&& x)
        {
            std::cerr << std::forward<T>(x);
            return *this;
        }
    private:
        static std::mutex mutex_;
        std::unique_lock<std::mutex> lock_{mutex_};
    };

    std::mutex logger::mutex_;

    using bigint = boost::multiprecision::cpp_int;

    class limited_string : public std::string
    {
    public:
        limited_string(std::size_t max_size) noexcept
            : max_size_{max_size}
        {}

        std::size_t max_size() const noexcept
        {
            return max_size_;
        }
    private:
        std::size_t max_size_;
    };

    class session
    {
    public:
        session(select_reactor& reactor,async_file_descriptor sock,const sockaddr_in6& si)
            : d_{std::make_shared<session_data>(reactor,std::move(sock),si)}
        {
            d_->get_logger() << "connection accepted";
            (*this)();
        }

        void operator()(bool reading_b = false)
        {
            d_->reading_b_ = reading_b;
            try{
                async_read_until(d_->sock_,d_->cont_reactor_,d_->in_buf_,'\n',*this);
            }
            catch(const std::exception& e){
                d_->get_logger() << "failed to initiate read of number: " << e.what();
            }
        }

        void operator()(std::exception_ptr e,std::size_t n)
        {
            if(e)
                try{
                    std::rethrow_exception(e);
                }
                catch(const std::exception& e){
                    if(auto se = dynamic_cast<const std::system_error*>(&e))
                        if(!n&&se->code()==std::make_error_code(std::errc::broken_pipe))
                            return;
                    d_->get_logger() << "failed to read number: " << e.what();
                    return;
                }
            bigint b;
            try{
                b = d_->read_buffered_number(n);
            }
            catch(const std::exception& e){
                d_->get_logger() << "failed to parse number: " << e.what();
                return;
            }
            if(d_->reading_b_){
                try{
                    d_->out_buf_ = bigint{d_->a_*b}.str()+'\n';
                }
                catch(const std::exception& e){
                    d_->get_logger() << "failed to calculate result: " << e.what();
                    return;
                }
                try{
                    async_write(d_->sock_,d_->cont_reactor_,
                                {reinterpret_cast<const std::byte*>(d_->out_buf_.data()),
                                 d_->out_buf_.size()},*this);
                }
                catch(const std::exception& e){
                    d_->get_logger() << "failed to initiate write of result: " << e.what();
                    return;
                }
            }else{
                d_->a_ = std::move(b);
                (*this)(true);
            }
        }

        void operator()(std::exception_ptr e)
        {
            if(e)
                try{
                    std::rethrow_exception(e);
                }
                catch(const std::exception& e){
                    d_->get_logger() << "failed to write result: " << e.what();
                    return;
                }
            (*this)();
        }
    private:
        constexpr static std::size_t max_length = 1024;

        struct session_data
        {
            select_reactor& reactor_;
            defer_reactor_adapter<select_reactor> cont_reactor_;
            async_file_descriptor sock_;
            sockaddr_in6 si_;
            limited_string in_buf_{max_length};
            std::string out_buf_;
            bigint a_;
            bool reading_b_;

            session_data(select_reactor& reactor,async_file_descriptor sock,
                         const sockaddr_in6& si) noexcept
                : reactor_{reactor},
                  cont_reactor_{reactor},
                  sock_{std::move(sock)},
                  si_{si}
            {}

            ~session_data()
            {
                get_logger() << "connection closed";
            }

            logger get_logger()
            {
                return std::move(logger{} << si_ << " : ");
            }

            bigint read_buffered_number(std::size_t n)
            {
                in_buf_[n-1] = '\0';
                bigint x{in_buf_.data()};
                in_buf_.erase(0,n);
                return x;
            }
        };

        std::shared_ptr<session_data> d_;
    };
}

int main(int argc,char* argv[])
{
    try{
        if(argc<2||argc>3)
            throw std::runtime_error(ce::format("Usage: ",argv[0]," <listen-port> [threads]"));
        auto port = ce::from_chars<std::uint16_t>(argv[1]);
        if(!port||!*port)
            throw std::runtime_error("Port must be in [1;65535]");
        unsigned threads;
        if(argc==3){
            auto t = ce::from_chars<unsigned>(argv[2]);
            if(!t||!*t)
                throw std::runtime_error("Threads must be a non-zero unsigned integer");
            threads = *t;
        }else
            threads = std::thread::hardware_concurrency();
#ifdef _WIN32
        WSAInitializer wsaInitializer;
#endif
        thread_pool tp{threads};
        select_reactor reactor{tp};
        reactor.handle(signal_pipe[0],reactor_op::read,[&]{
            logger{} << "Received termination signal";
            reactor.stop();
        });
        // signal has non-portable semantics in general, but Windows has nothing better
        // and for our single-shot use case signal is sufficient.
        signal(SIGINT,signal_handler);
        signal(SIGTERM,signal_handler);
        tcp_listener listener{reactor,*port,[&](async_file_descriptor sock,const sockaddr_in6& si){
            try{
                session{reactor,std::move(sock),si};
            }
            catch(const std::exception& e){
                logger{} << "Failed to allocate session for connection from " << si;
            }
        }};
        logger{} << "Listening on port " << *port << "...";
        reactor.run();
    }
    catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
