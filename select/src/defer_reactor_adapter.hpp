#ifndef UUID_6F15B82D_D8FC_4A4F_B2C4_D27072A984B4
#define UUID_6F15B82D_D8FC_4A4F_B2C4_D27072A984B4

#include "reactor.hpp"

#include <ce/file_descriptor.hpp>

template<typename Reactor>
class defer_reactor_adapter
{
public:
    defer_reactor_adapter(Reactor& reactor)
        : reactor_{reactor}
    {}

    auto get_executor()
    {
        return [this](execution_kind kind,task_t f){
            reactor_.get_executor()(kind==execution_kind::post?execution_kind::defer:kind,
                                    std::move(f));
        };
    }

    void handle(ce::file_descriptor& fd,reactor_op op,task_t handler)
    {
        reactor_.handle(fd,op,std::move(handler));
    }
private:
    Reactor& reactor_;
};

#endif
