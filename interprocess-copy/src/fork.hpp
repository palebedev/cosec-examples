#ifndef UUID_88E8E365_9FC7_4188_9784_CD5F0B417DE1
#define UUID_88E8E365_9FC7_4188_9784_CD5F0B417DE1

#include "utils.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#include <cassert>
#include <exception>

class fork
{
public:
    fork()
        : child_pid_{::fork()}
    {
        // We expect to not be constructed during exception handling,
        // see destructor.
        assert(!std::uncaught_exceptions());
        throw_errno_if_negative(child_pid_,"fork");
    }

    fork(fork&& other) noexcept
        : child_pid_{std::exchange(other.child_pid_,0)}
    {}

    // Assignment makes no sense.

    ~fork()
    {
        if(child_pid_){
            if(std::uncaught_exceptions())
                // We are exiting due to a thrown exception.
                // We're not going to follow our parent-child protocol,
                // so just kill the child so that we don't hang
                // waiting for it.
                kill(child_pid_,SIGTERM);
            waitpid(child_pid_,nullptr,0);
        }
    }

    explicit operator bool() const noexcept
    {
        return child_pid_;
    }

    int child_pid() const noexcept
    {
        return child_pid_;
    }
private:
    int child_pid_;
};

#endif
