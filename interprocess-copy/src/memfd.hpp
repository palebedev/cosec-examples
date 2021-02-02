#ifndef UUID_08FC25FF_C5AD_4DF9_991D_3DA9C63B8A61
#define UUID_08FC25FF_C5AD_4DF9_991D_3DA9C63B8A61

#include <ce/file_descriptor.hpp>

#include <sys/mman.h>
#include <sys/types.h>

#include <span>

// See also boost::interprocess::shared_memory_object.
// We could do without a file descriptor altogether and
// just use plain mmap(MAP_SHARED|MAP_ANONYMOUS) before fork,
// but we use memfd to potentially use sealing (which we don't
// in this example).

class memfd : public ce::file_descriptor
{
public:
    class mapping
    {
    public:
        mapping(mapping&& other) noexcept
            : p_{std::exchange(other.p_,nullptr)},
              n_{std::exchange(other.n_,0)}
        {}

        mapping& operator=(mapping&& other) noexcept
        {
            if(this!=&other){
                destroy();
                p_ = std::exchange(other.p_,nullptr);
                n_ = std::exchange(other.n_,0);
            }
            return *this;
        }

        ~mapping()
        {
            destroy();
        }

        std::byte* data() const noexcept
        {
            return p_;
        }

        std::size_t size() const noexcept
        {
            return n_;
        }
    private:
        friend class memfd;

        std::byte* p_;
        std::size_t n_;

        mapping(std::byte* p,std::size_t n) noexcept
            : p_{p},
              n_{n}
        {}

        void destroy()
        {
            if(p_)
                munmap(p_,n_);
        }
    };

    memfd(std::size_t size,const char* name = "anonymous")
        : ce::file_descriptor{memfd_create(name,0),"memfd_create"}
    {
        ce::throw_errno_if_negative(ftruncate(fd_,off_t(size)),"memfd:ftruncate");
    }

    mapping map(std::size_t offset,std::size_t length)
    {
        void* ret = mmap(nullptr,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd_,off_t(offset));
        if(ret==MAP_FAILED)
            ce::throw_errno("memfd:mmap");
        return mapping{static_cast<std::byte*>(ret),length};
    }
};

#endif
