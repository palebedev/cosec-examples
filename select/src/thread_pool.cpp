#include "thread_pool.hpp"

#include <cassert>

namespace
{
    thread_local thread_pool* this_thread_pool = {};
    thread_local task_t deferred_task;
}

thread_pool::thread_pool(unsigned threads)
{
    assert(threads);
    workers_.reserve(threads);
    for(unsigned i=0;i<threads;++i)
        workers_.emplace_back([this]{
            this_thread_pool = this;
            task_t task;
            std::unique_lock lock{mutex_};
            for(;;){
                queue_not_empty_.wait(lock,[this]{
                    return !tasks_.empty()||stopping_.load(std::memory_order::acquire);
                });
                if(stopping_.load(std::memory_order::acquire))
                    return;
                auto task = std::move(tasks_.front());
                tasks_.pop();
                ++busy_workers_;
                lock.unlock();
                do{
                    task();
                    task = std::move(deferred_task);
                }while(!stopping_.load(std::memory_order::acquire)&&task);
                lock.lock();
                if(!--busy_workers_)
                    no_busy_workers_.notify_all();
            }
        });
}

thread_pool::~thread_pool()
{
    join();
}

void thread_pool::join()
{
    {
        std::unique_lock lock{mutex_};
        if(!stopping_.load(std::memory_order::acquire)){
            no_busy_workers_.wait(lock,[this]{
                return !busy_workers_;
            });
            stopping_.store(true,std::memory_order::release);
            queue_not_empty_.notify_all();
        }
    }
    for(auto& thread:workers_)
        thread.join();
}

void thread_pool::stop()
{
    std::lock_guard lock{mutex_};
    tasks_ = {};
    stopping_.store(true,std::memory_order::release);
    queue_not_empty_.notify_all();
}

void thread_pool::operator()(execution_kind kind,task_t f)
{
    if(stopping_.load(std::memory_order::acquire))
        return;
    if(kind!=execution_kind::post&&!running_in_this_thread())
        kind = execution_kind::post;
    if(kind==execution_kind::dispatch){
        f();
        return;
    }
    if(kind==execution_kind::defer&&!deferred_task){
        deferred_task = std::move(f);
        return;
    }
    std::lock_guard lock{mutex_};
    tasks_.emplace(std::move(f));
    if(busy_workers_<workers_.size())
        queue_not_empty_.notify_one();
}

bool thread_pool::running_in_this_thread() const noexcept
{
    return this_thread_pool==this;
}
