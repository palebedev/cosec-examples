#ifndef UUID_BE300BBF_AFBC_405E_B8DF_CEFAC4E42FF8
#define UUID_BE300BBF_AFBC_405E_B8DF_CEFAC4E42FF8

#include <ce/file_descriptor.hpp>

#include <sys/eventfd.h>

#include <cassert>
#include <stdexcept>

// This is probably the cheapest way to send a word of data between
// processes, but see other interprocess communication/synchronization objects.

class event : public ce::file_descriptor
{
public:
    event()
        : ce::file_descriptor{eventfd(0,0),"eventfd"}
    {}

    void write(uint64_t x = 1)
    {
        assert(x);
        ssize_t ret = ce::file_descriptor::write(
            {reinterpret_cast<const std::byte*>(&x),sizeof x});
        ce::throw_errno_if_negative(ret,"eventfd:write");
        if(ret!=sizeof x)
            throw std::runtime_error("eventfd:write: short write");
    }

    uint64_t read()
    {
        uint64_t x;
        ssize_t ret = ce::file_descriptor::read(
            {reinterpret_cast<std::byte*>(&x),sizeof x});
        ce::throw_errno_if_negative(ret,"eventfd:read");
        if(ret!=sizeof x)
            throw std::runtime_error("eventfd::read: short read");
        return x;
    }
};

#endif
