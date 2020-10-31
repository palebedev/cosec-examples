#ifndef UUID_9B12FE85_0EE7_4FE9_8719_37E79229088C
#define UUID_9B12FE85_0EE7_4FE9_8719_37E79229088C

#include "utils.hpp"

#include <unistd.h>

#include <utility>

class file_descriptor
{
public:
    file_descriptor(int fd,const char* what)
        : fd_{fd}
    {
        throw_errno_if_negative(fd_,what);
    }

    file_descriptor(file_descriptor&& other) noexcept
        : fd_{std::exchange(other.fd_,-1)}
    {}

    file_descriptor& operator=(file_descriptor&& other) noexcept
    {
        if(this!=&other){
            destroy();
            fd_ = std::exchange(other.fd_,-1);    
        }
        return *this;
    }

    ~file_descriptor()
    {
        destroy();
    }
protected:
    int fd_;
private:
    void destroy()
    {
        if(fd_>=0)
            close(fd_);
    }
};

#endif
