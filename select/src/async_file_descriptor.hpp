#ifndef UUID_C33A750F_DB7A_4101_9D41_7FD0F2E2AFB3
#define UUID_C33A750F_DB7A_4101_9D41_7FD0F2E2AFB3

#include "reactor.hpp"

#include <ce/file_descriptor.hpp>

#include <exception>
#include <system_error>

class async_file_descriptor : public ce::file_descriptor
{
public:
    async_file_descriptor(int fd,const char* what)
        : ce::file_descriptor{fd,what}
    {
        set_non_blocking();
    }

    template<typename Reactor,typename CallBack>
    void async_read_some(Reactor& reactor,std::span<std::byte> buf,CallBack cb)
    {
        async_read_some_impl(reactor,buf,std::move(cb),execution_kind::post);
    }

    template<typename Reactor,typename CallBack>
    void async_write_some(Reactor& reactor,std::span<const std::byte> buf,CallBack cb)
    {
        async_write_some_impl(reactor,buf,std::move(cb),execution_kind::post);
    }
private:
    template<typename Reactor,typename CallBack>
    void async_read_some_impl(Reactor& reactor,std::span<std::byte> buf,CallBack cb,
                                execution_kind kind)
    {
        try{
            ssize_t ret = read(buf);
            if(ret>=0||errno!=EWOULDBLOCK){
                reactor.get_executor()(kind,[e=ret<0?std::make_exception_ptr(std::system_error{
                                                 errno,std::system_category(),"read"}):
                                                 std::exception_ptr{},
                                                 ret=ret>=0?std::size_t(ret):std::size_t{},
                                                 cb=std::move(cb)]() mutable {
                    cb(std::move(e),ret);
                });
                return;
            }
            reactor.handle(*this,reactor_op::read,[this,&reactor,buf,cb=std::move(cb)]{
                async_read_some_impl(reactor,buf,cb,execution_kind::dispatch);
            });
        }
        catch(...){
            reactor.get_executor()(kind,[e=std::current_exception(),cb=std::move(cb)]
                                    () mutable {
                cb(std::move(e),0);
            });
        }
    }

    template<typename Reactor,typename CallBack>
    void async_write_some_impl(Reactor& reactor,std::span<const std::byte> buf,CallBack cb,
                               execution_kind kind)
    {
        try{
            ssize_t ret = write(buf);
            if(ret>=0||errno!=EWOULDBLOCK){
                reactor.get_executor()(kind,[e=ret<0?std::make_exception_ptr(std::system_error{
                                                 errno,std::system_category(),"write"}):
                                                 std::exception_ptr{},
                                                 ret=ret>=0?std::size_t(ret):std::size_t{},
                                                 cb=std::move(cb)]() mutable {
                    cb(std::move(e),ret);
                });
                return;
            }
            reactor.handle(*this,reactor_op::read,[this,&reactor,buf,cb=std::move(cb)]{
                async_write_some_impl(reactor,buf,cb,execution_kind::dispatch);
            });
        }
        catch(...){
            reactor.get_executor()(kind,[e=std::current_exception(),cb=std::move(cb)]() mutable {
                cb(std::move(e),0);
            });
        }
    }
};

#endif
