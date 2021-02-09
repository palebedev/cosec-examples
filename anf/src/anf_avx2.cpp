#include "anf_avx2.hpp"

#include <immintrin.h>

#include <bit>
#include <cassert>
#include <cstring>
#include <memory>

namespace ce
{
    void anf_avx2(std::span<std::byte> f) noexcept
    {
        __m256i x,y;
        assert(f.size()>=sizeof x);
        assert(std::has_single_bit(f.size()));
        for(std::size_t i=0;i<f.size();i+=sizeof x){
            std::byte* p = std::assume_aligned<32>(f.data()+i);
            std::memcpy(&x,p,sizeof x);
            x = _mm256_xor_si256(x,_mm256_and_si256(_mm256_slli_epi16(x,1),
                                                    _mm256_set1_epi32(int(0xaaaaaaaa))));
            x = _mm256_xor_si256(x,_mm256_and_si256(_mm256_slli_epi16(x,2),
                                                    _mm256_set1_epi32(int(0xcccccccc))));
            x = _mm256_xor_si256(x,_mm256_and_si256(_mm256_slli_epi16(x,4),
                                                    _mm256_set1_epi32(int(0xf0f0f0f0))));
            x = _mm256_xor_si256(x,_mm256_slli_epi16(x,8));
            x = _mm256_xor_si256(x,_mm256_slli_epi32(x,16));
            x = _mm256_xor_si256(x,_mm256_slli_epi64(x,32));
            x = _mm256_xor_si256(x,_mm256_slli_si256(x,8));
            x = _mm256_xor_si256(x,_mm256_permute2x128_si256(x,x,0x08));
            std::memcpy(p,&x,sizeof x);
        }
        for(size_t i=sizeof x;i<f.size();i*=2)
            for(size_t j=0;j<f.size();j+=2*i)
                for(size_t k=0;k<i;k+=sizeof x){
                    auto p1 = std::assume_aligned<32>(f.data()+j+k),
                         p2 = std::assume_aligned<32>(f.data()+i+j+k);
                    std::memcpy(&x,p1,sizeof x);
                    std::memcpy(&y,p2,sizeof y);
                    x = _mm256_xor_si256(x,y);
                    std::memcpy(p2,&x,sizeof x);
                }
    }
}
