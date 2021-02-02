#include "chat.hpp"

#include <ce/asio-main.hpp>
#include <ce/charconv.hpp>
#include <ce/format.hpp>
#include <ce/io_context_signal_interrupter.hpp>
#include <ce/tcp_listener.hpp>

#include <boost/asio/execution/execute.hpp>
#include <boost/asio/static_thread_pool.hpp>

#include <memory>
#include <stdexcept>
#include <thread>

namespace ce
{
    int main(std::span<const char* const> args)
    {
        if(args.size()<2||args.size()>3)
            throw std::runtime_error(format("Usage: ",args[0]," <listen-port> [threads]"));
        auto port = from_chars<std::uint16_t>(args[1]);
        if(!port||!*port)
            throw std::runtime_error("Port must be in [1;65535]");
        unsigned threads;
        if(args.size()==3){
            auto t = from_chars<unsigned>(args[2]);
            if(!t||!*t)
                throw std::runtime_error("Threads must be a non-zero unsigned integer");
            threads = *t;
        }else
            threads = std::thread::hardware_concurrency();
        using namespace boost::log::trivial;
        BOOST_LOG_TRIVIAL(info) << "Using " << threads << " threads.";
        ba::io_context ctx{int(threads)};
        io_context_signal_interrupter iosi{ctx};
        chat c{ctx.get_executor()};
        tcp_listener<chat_session,ba::io_context::executor_type> tl{ctx.get_executor(),*port,[&]{
            return std::make_shared<chat_session>(ctx.get_executor(),c);
        }};
        ba::static_thread_pool tp{threads-1};
        for(unsigned i=1;i<threads;++i)
            bae::execute(tp.get_executor(),[&]{
                ctx.run();
            });
        ctx.run();
        return 0;
    }
}
