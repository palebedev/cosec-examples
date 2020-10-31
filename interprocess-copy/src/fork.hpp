#ifndef UUID_88E8E365_9FC7_4188_9784_CD5F0B417DE1
#define UUID_88E8E365_9FC7_4188_9784_CD5F0B417DE1

#include "utils.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class fork
{
public:
    fork()
        : child_pid_{::fork()}
    {
        throw_errno_if_negative(child_pid_,"fork");
    }

    fork(fork&& other) noexcept
        : child_pid_{std::exchange(other.child_pid_,0)}
    {}

    // Assignment makes no sense.

    ~fork()
    {
        if(child_pid_)
            waitpid(child_pid_,nullptr,0);
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
