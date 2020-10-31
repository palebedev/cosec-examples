#ifndef UUID_7B5C2BE2_7375_4527_AAE7_55EDAE50C1BB
#define UUID_7B5C2BE2_7375_4527_AAE7_55EDAE50C1BB

#include "file_descriptor.hpp"

#include <knem_io.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <span>

class knem : public file_descriptor
{
public:
    class read_region
    {
    public:
        read_region(read_region&& other) noexcept
            : parent_{std::exchange(other.parent_,nullptr)},
              cookie_{std::exchange(other.cookie_,{})}
        {}

        read_region& operator=(read_region&& other) noexcept
        {
            if(this!=&other){
                destroy();
                parent_ = std::exchange(other.parent_,nullptr);
                cookie_ = std::exchange(other.cookie_,{});
            }
            return *this;
        }

        ~read_region()
        {
            destroy();
        }

        const knem_cookie_t& cookie() const noexcept
        {
            return cookie_;
        }
    private:
        friend class knem;

        knem* parent_;
        knem_cookie_t cookie_;

        explicit read_region(knem& parent,knem_cookie_t cookie) noexcept
            : parent_{&parent},
              cookie_{cookie}
        {}

        void destroy()
        {
            if(parent_)
                ioctl(parent_->fd_,KNEM_CMD_DESTROY_REGION,&cookie_);
        }
    };

    knem()
        : file_descriptor{open(KNEM_DEVICE_FILENAME,O_RDWR),"knem:open"}
    {}

    read_region create_read_region(std::span<const std::byte> data)
    {
        iovec vec{const_cast<std::byte*>(data.data()),data.size()};
        knem_cmd_create_region ccr{
            .iovec_array = reinterpret_cast<decltype(knem_cmd_create_region::iovec_array)>(&vec),
            .iovec_nr = 1,
            .protection = PROT_READ
        };
        throw_errno_if_negative(ioctl(fd_,KNEM_CMD_CREATE_REGION,&ccr),"knem:ioctl(CREATE_REGION)");
        return read_region{*this,ccr.cookie};
    }

    void copy_from(knem_cookie_t cookie,std::size_t offset,std::span<std::byte> dest)
    {
        iovec vec{dest.data(),dest.size()};
        knem_cmd_inline_copy ic = {
            .local_iovec_array = reinterpret_cast<
                decltype(knem_cmd_inline_copy::local_iovec_array)>(&vec),
            .local_iovec_nr = 1,
            .remote_cookie = cookie,
            .remote_offset = offset,
        };
        throw_errno_if_negative(ioctl(fd_,KNEM_CMD_INLINE_COPY,&ic),"knem:ioctl(INLINE_COPY)");
    }
};

#endif
