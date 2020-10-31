#ifndef UUID_7AE8C09A_5708_44CB_A929_FCC861F4E827
#define UUID_7AE8C09A_5708_44CB_A929_FCC861F4E827

#include "utils.hpp"

#include <unistd.h>

#include <array>
#include <span>
#include <stdexcept>

// See also boost::process::{async_,}pipe.

class pipe
{
public:
    pipe()
    {
        throw_errno_if_negative(::pipe(&fds_[0]),"pipe");
    }

    pipe(pipe&& other) noexcept
        : fds_{std::exchange(other.fds_,{-1,-1})}
    {}

    pipe& operator=(pipe&& other) noexcept
    {
        if(this!=&other){
            destroy();
            fds_ = std::exchange(other.fds_,{-1,-1});
        }
        return *this;
    }

    ~pipe()
    {
        destroy();
    }

    void write(std::span<const std::byte> data)
    {
        // A write of >PIPE_LEN bytes will block and write in chunks.
        ssize_t ret = ::write(fds_[1],reinterpret_cast<const char*>(data.data()),data.size());
        throw_errno_if_negative(ret,"pipe:write");
        if(std::size_t(ret)!=data.size())
            throw std::runtime_error{"pipe:write: short write"};
    }

    void read(std::span<std::byte> data)
    {
        // A read of >PIPE_LEN will return data in chunks.
        auto p = data.data();
        auto n = data.size();
        while(n){
            ssize_t ret = ::read(fds_[0],reinterpret_cast<char*>(p),n);
            throw_errno_if_negative(ret,"pipe:read");
            auto s = std::size_t(ret);
            if(s>n)
                throw std::runtime_error{"pipe:read: long read"};
            p += s;
            n -= s;
        }
    }
private:
    std::array<int,2> fds_;

    void destroy()
    {
        if(fds_[0]>=0)
            close(fds_[0]);
        if(fds_[1]>=0)
            close(fds_[1]);
    }
};

#endif
