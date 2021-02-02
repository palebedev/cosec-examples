#ifndef UUID_0D13CAE3_B4D2_4519_ABB9_4176C4870EA4
#define UUID_0D13CAE3_B4D2_4519_ABB9_4176C4870EA4

#include "reactor.hpp"

#include <ce/file_descriptor.hpp>
#include <ce/pipe.hpp>

#include <compare>
#include <map>
#include <mutex>
#include <vector>

#include <sys/select.h>

class select_reactor
{
public:
    // We type-erase ExecutorLike that we need a single method from
    // so that the rest of select_reactor is not a template.
    template<typename ExecutorLike>
    select_reactor(ExecutorLike& ex)
        : executor_([&](execution_kind kind,task_t task){
              ex(kind,std::move(task));
          })
    {
        init();
    }

    const auto& get_executor() const noexcept
    {
        return executor_;
    }

    void run();
    void stop();
    // Registers handler to be called when op becomes possible on fd.
    void handle(ce::file_descriptor& fd,reactor_op op,task_t handler);
    // Destroy fd that is potentially used by this reactor.
    // This is needed because closing file descriptors while waiting on them with select
    // is non-portable.
    void close(ce::file_descriptor fd);
private:
    std::function<void (execution_kind,task_t)> executor_;
    ce::pipe pipe_;
    std::mutex mutex_;
    std::map<int,task_t> events_[2];
    std::array<fd_set,2> fd_sets_;
    std::vector<ce::file_descriptor> to_be_closed_;
    bool stopping_ = false;

    void init();
    void interrupt_select();
};

#endif
