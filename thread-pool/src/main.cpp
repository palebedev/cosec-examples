#include <boost/asio/execution/blocking.hpp>
#include <boost/asio/execution/execute.hpp>
#include <boost/asio/execution/mapping.hpp>
#include <boost/asio/require.hpp>
#include <boost/asio/static_thread_pool.hpp>

#include <chrono>
#include <exception>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <thread>
#include <variant>
#include <vector>

// This first example doesn't use any of our own facilities in utils
// library to remain simple and standalone.

namespace ba = boost::asio;
namespace bae = boost::asio::execution;

template<typename Result>
class output_t : public std::variant<std::monostate,Result,std::exception_ptr>
{
public:
    using std::variant<std::monostate,Result,std::exception_ptr>::variant;

    friend std::ostream& operator<<(std::ostream& stream,const output_t& output)
    {
        if(std::holds_alternative<std::monostate>(output))
            stream << "(no output)";
        else if(auto p = std::get_if<Result>(&output))
            stream << *p;
        else
            try{
                std::rethrow_exception(*std::get_if<std::exception_ptr>(&output));
            }
            catch(const std::exception& e){
                std::cout << "(error: " << e.what() << ')';
            }
            catch(...){
                std::cout << "(unknown error)";
            }
        return stream;
    }
};

template<typename Result,typename F>
auto wrap_try_get_result(output_t<Result>& result,F&& f)
{
    return [f=std::forward<F>(f),&result]() mutable noexcept {
        try{
            result = std::forward<F>(f)();    
        }
        catch(...){
            result = std::current_exception();
        }
    };
}

// std::osyncstream (C++20) is not yet in libc++.
class logger
{
public:
    template<typename T>
    logger& operator<<(const T& x)
    {
        oss_ << x;
        return *this;
    }

    ~logger()
    {
        std::cout << std::move(oss_).str();
    }
private:
    std::ostringstream oss_;
};

int main()
{
    constexpr std::size_t jobs = 50;
    using my_output = output_t<int>;
    std::vector<my_output> results{jobs};
    ba::static_thread_pool pool{std::thread::hardware_concurrency()};
    auto ex = ba::require(pool.executor(),bae::blocking.never,bae::mapping.thread);
    using namespace std::chrono;
    for(std::size_t i=0;i<jobs;++i)
        bae::execute(ex,wrap_try_get_result(results[i],
            [i,execute_time=steady_clock::now()]{
                auto start_time = steady_clock::now();
                thread_local std::mt19937_64 prng{std::random_device{}()};
                std::this_thread::sleep_for(milliseconds{
                    std::uniform_int_distribution{500,5000}(prng)});
                if(i==17)
                    throw std::runtime_error("I don't like 17");
                int res = int(i*i);
                logger{} << "Task " << i << " waited in queue for "
                         << duration_cast<milliseconds>
                                (start_time-execute_time).count()
                         << " ms to execute on thread "
                         << std::this_thread::get_id()
                         << " and finished in "
                         << duration_cast<milliseconds>
                                (steady_clock::now()-start_time).count()
                         << " ms.\n";
                return res;
            }));
    std::cout << "Tasks have been posted.\n";
    pool.wait();
    std::cout << "Tasks are done.\n"
                 "Results:\n";
    std::copy(results.begin(),results.end(),
              std::ostream_iterator<my_output>{std::cout," "});
    std::cout << '\n';
}
