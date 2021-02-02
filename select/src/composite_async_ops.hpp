#ifndef UUID_DF18B1A3_99C4_403A_8956_4EDBC941311B
#define UUID_DF18B1A3_99C4_403A_8956_4EDBC941311B

#include "task.hpp"

#include <algorithm>
#include <exception>
#include <span>
#include <system_error>

// Read the whole span or fail. EOF is reported as std::errc::broken_pipe in std::system_error.
template<typename AsyncReadStream,typename Reactor,typename CallBack>
void async_read(AsyncReadStream& stream,Reactor& reactor,std::span<std::byte> buf,CallBack cb)
{
    stream.async_read_some(reactor,buf,[&,buf,cb=std::move(cb)]
            (std::exception_ptr e,std::size_t n){
        if(e||!n||n==buf.size())
            reactor.get_executor()(execution_kind::dispatch,[e=e?std::move(e):n?std::exception_ptr{}:
                                   std::make_exception_ptr(std::system_error{
                                       std::make_error_code(std::errc::broken_pipe)})
                    ]() mutable {
                cb(std::move(e));
            });
        else
            async_read(stream,reactor,buf.subspan(n),std::move(cb));
    });
}

namespace detail
{
    template<typename DynamicBuffer,typename T,typename = void>
    struct is_match_condition : std::false_type {};

    template<typename DynamicBuffer,typename T>
    struct is_match_condition<DynamicBuffer,T,std::enable_if_t<std::is_same_v<std::ptrdiff_t,
        decltype(std::declval<T>()(std::declval<DynamicBuffer&>()))>>> : std::true_type {};

    template<typename AsyncReadStream,typename Reactor,typename DynamicBuffer,typename Predicate,
            typename CallBack>
    void async_read_until_impl(AsyncReadStream& stream,Reactor& reactor,DynamicBuffer& buf,
                               Predicate pred,CallBack cb,execution_kind kind)
    {
        static_assert(sizeof(decltype(*buf.data()))==1,"buffer must have byte-size elements");
        // First, check if we already have a match.
        if(std::ptrdiff_t match_len = pred(std::as_const(buf));match_len>=0){
            reactor.get_executor()(kind,[match_len=std::size_t(match_len),cb=std::move(cb)]
                                        () mutable {
                cb(std::exception_ptr{},match_len);
            });
            return;
        }
        // If we've read all of the allocated memory, allocate more.
        try{
            // To support simple inheritance on DynamicBuffer that just hides
            // parent max_size(), we cannot depende on push_back respecting max_size,
            // so we check manually.
            if(buf.size()==buf.max_size())
                throw std::length_error{"buffer max_size reached without a match"};
            if(buf.size()==buf.capacity()){
                // Push a single character and take it back to let DynamicBuffer choose
                // its growth strategy.
                buf.push_back({});
                buf.pop_back();
            }
        }
        catch(...){
            reactor.get_executor()(kind,[e=std::current_exception(),cb=std::move(cb)]
                                        () mutable {
                cb(std::move(e),0);
            });
        }
        // Try to read at most up to capacity or max_size.
        std::size_t len = std::min(buf.max_size(),buf.capacity())-buf.size();
        buf.resize(buf.size()+len);
        stream.async_read_some(reactor,{reinterpret_cast<std::byte*>(buf.data())+buf.size()-len,
                            len},[&,len,pred=std::move(pred),cb=std::move(cb)]
                (std::exception_ptr e,std::size_t n){
            // Resize back to actual data read.
            buf.resize(buf.size()-len+n);
            if(e||!n)
                reactor.get_executor()(execution_kind::dispatch,[e=e?std::move(e):
                                           std::make_exception_ptr(std::system_error{
                                               std::make_error_code(std::errc::broken_pipe)}),
                                done=buf.size(),cb=std::move(cb)]() mutable {
                    cb(std::move(e),done);
                });
            else
                async_read_until_impl(stream,reactor,buf,std::move(pred),std::move(cb),
                                      execution_kind::dispatch);
        });
    }
}

// Read until failure, EOF (reported as above), data exceeding DynamicBuffer::max_size()
// (reported as std::length_error) or until Predicate returns a non-negative match length.
template<typename AsyncReadStream,typename Reactor,typename DynamicBuffer,typename Predicate,
         typename CallBack,typename = std::enable_if_t<
            detail::is_match_condition<DynamicBuffer,Predicate>{}>>
void async_read_until(AsyncReadStream& stream,Reactor& reactor,DynamicBuffer& buf,
                      Predicate pred,CallBack cb)
{
    detail::async_read_until_impl(stream,reactor,buf,std::move(pred),std::move(cb),
                                  execution_kind::post);
}

// As above, but match a single element as delimeter as end of data (including it).
template<typename AsyncReadStream,typename Reactor,typename DynamicBuffer,typename CallBack>
void async_read_until(AsyncReadStream& stream,Reactor& reactor,DynamicBuffer& buf,
                      typename DynamicBuffer::value_type delim,CallBack cb)
{
    async_read_until(stream,reactor,buf,[delim,i=std::size_t()]
                                        (const DynamicBuffer& buf) mutable {
        auto end = buf.data()+buf.size(),
             it = std::find(buf.data()+i,end,delim);
        if(it==end){
            i = buf.size();
            return std::ptrdiff_t(-1);
        }
        return it-buf.data()+1;
    },std::move(cb));
}

// Write the whole span or fail.
template<typename AsyncWriteStream,typename Reactor,typename CallBack>
void async_write(AsyncWriteStream& stream,Reactor& reactor,std::span<const std::byte> buf,
                 CallBack cb)
{
    stream.async_write_some(reactor,buf,[&,buf,cb=std::move(cb)]
            (std::exception_ptr e,std::size_t n){
        if(e||n==buf.size())
            reactor.get_executor()(execution_kind::dispatch,[e=e?std::move(e):std::exception_ptr{},
                                                             cb=std::move(cb)]() mutable {
                cb(std::move(e));
            });
        else
            async_write(stream,reactor,buf.subspan(n),std::move(cb));
    });
}

#endif
