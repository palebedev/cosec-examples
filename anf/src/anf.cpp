#include "anf.hpp"

#include <bit>
#include <cassert>
#include <cstring>

namespace ce
{
    void anf(std::span<std::byte> f) noexcept
    {
        std::uint64_t x,y;
        assert(f.size()>=sizeof x);
        assert(std::has_single_bit(f.size()));
        for(std::size_t i=0;i<f.size();i+=sizeof x){
            std::memcpy(&x,f.data()+i,sizeof x);
            x ^= (x<<1)&0xaaaaaaaaaaaaaaaa;
            x ^= (x<<2)&0xcccccccccccccccc;
            x ^= (x<<4)&0xf0f0f0f0f0f0f0f0;
            x ^= (x<<8)&0xff00ff00ff00ff00;
            x ^= (x<<16)&0xffff0000ffff0000;
            x ^= x<<32;
            std::memcpy(f.data()+i,&x,sizeof x);
        }
        for(size_t i=sizeof x;i<f.size();i*=2)
            for(size_t j=0;j<f.size();j+=2*i)
                for(size_t k=0;k<i;k+=sizeof x){
                    std::memcpy(&x,f.data()+j+k,sizeof x);
                    std::memcpy(&y,f.data()+i+j+k,sizeof y);
                    x ^= y;
                    std::memcpy(f.data()+i+j+k,&x,sizeof x);
                }
    }
}
