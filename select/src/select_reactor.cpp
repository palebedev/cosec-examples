#include "select_reactor.hpp"

#include <cassert>

#include <sys/select.h>

void select_reactor::run()
{
    // We only drop the lock during a select call.
    std::unique_lock lock{mutex_};
    for(;;){
        // Make a copy of file descriptor sets which select modifies in-place.
        auto fd_sets = fd_sets_;
        // Calculate maximum fd number.
        int max_fd[2];
        for(int i=0;i<2;++i)
            max_fd[i] = events_[i].empty()?-1:std::prev(events_[i].end())->first;
        lock.unlock();
        int count = select(std::max(max_fd[0],max_fd[1])+1,&fd_sets[0],&fd_sets[1],
                           nullptr,nullptr);
        lock.lock();
        to_be_closed_.clear();
        if(count<0){
            if(errno!=EINTR)
                ce::throw_errno("select");
            // EINTR means we were interrupted by a signal.
            // We'll check our stopping flag but no masks.
        }
        if(stopping_)
            return;
        if(count>0)
            // We have events to process, loop through both fd sets.
            for(std::size_t set=0;set<2;++set)
                for(int i=0;i<=max_fd[set];++i)
                    if(FD_ISSET(i,&fd_sets[set])){
                        auto it = events_[set].find(i);
                        if(it==events_[set].end())
                            throw std::runtime_error{"invalid select return mask"};
                        if(it->second){
                            // We have a handler, dispatch it. Erase the entry
                            // before dispatching in case dispatch executes a handler
                            // inside itself which and reinstalls a new handler for same fd/op.
                            auto handler = std::move(it->second);
                            events_[set].erase(it);
                            FD_CLR(i,&fd_sets_[set]);
                            executor_(execution_kind::post,[handler=std::move(handler)]{
                                handler();
                            });
                        }else{
                            // No handler means this is our pipe, drain it.
                            std::byte buf[16];
                            ssize_t ret;
                            while((ret=pipe_[0].read(buf))==ssize_t(sizeof(buf)));
                            if(!ret)
                                throw std::runtime_error{"end of pipe"};
                            if(ret<0&&errno!=EWOULDBLOCK)
                                ce::throw_errno("read");
                        }
                        if(--count)
                            // If we've processed a total of count fds, there are
                            // no more, we can break both loops.
                            goto done;
                    }
done:
        ;
    }
}

void select_reactor::stop()
{
    std::lock_guard lock{mutex_};
    events_[0].clear();
    events_[1].clear();
    stopping_ = true;
    interrupt_select();
}

void select_reactor::handle(ce::file_descriptor& fd,reactor_op op,task_t handler)
{
    assert(fd>=0);
    if(fd>=FD_SETSIZE)
        throw std::runtime_error{"fd too big for select"};
    bool interrupt = false;
    std::size_t op_index = static_cast<std::size_t>(op);
    std::lock_guard lock{mutex_};
    if(handler){
        if(!FD_ISSET(fd,&fd_sets_[op_index])){
            FD_SET(fd,&fd_sets_[op_index]);
            interrupt = true;
        }
        events_[op_index][fd] = std::move(handler);
    }else if(FD_ISSET(fd,&fd_sets_[op_index])){
        FD_CLR(fd,&fd_sets_[op_index]);
        events_[op_index].erase(fd);
    }
    if(interrupt)
        interrupt_select();
}

void select_reactor::close(ce::file_descriptor fd)
{
    assert(fd>=0);
    if(fd>=FD_SETSIZE)
        return;
    std::lock_guard lock{mutex_};
    for(std::size_t set=0;set<2;++set)
        if(FD_ISSET(fd,&fd_sets_[set])){
            events_[set].erase(fd);
            FD_CLR(fd,&fd_sets_[set]);
        }
    // We'll delay closing fd even if masks show it is unused by our
    // reactor in case a previous call to handle(...) with an empty handler
    // has removed the last of use of fd from our reactor but it hasn't restarted
    // the system call yet.
    to_be_closed_.emplace_back(std::move(fd));
    interrupt_select();
}

void select_reactor::init()
{
    FD_ZERO(&fd_sets_[0]);
    FD_ZERO(&fd_sets_[1]);
    FD_SET(pipe_[0],&fd_sets_[0]);
    events_[0][pipe_[0]];
    pipe_[0].set_non_blocking();
    pipe_[1].set_non_blocking();
}

void select_reactor::interrupt_select()
{
    // This function must be called with a lock held,
    // if we write to a pipe without taking a lock, we might
    // do that exactly when it is being drained, losing an update.
    std::byte buf{};
    // We ignore return value as all real exceptions are thrown
    // and 0 in case of EWOULDBLOCK means our pipe is already
    // overflown so a wake up is definetly queued.
    pipe_[1].write_as_chunks({&buf,1});
}
