#include "add.hpp"

#include <iostream>

namespace
{
    // https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html

    inline std::uint64_t rdtsc() noexcept
    {
        std::uint32_t d,a;
        // Constraints 'd' and 'a' are machine-specific and
        // signify fixed rdx/rax registers (as rdtsc uses).
        // '=' is output-only paramter.
        asm volatile("rdtsc" : "=d"(d),"=a"(a));
        return (std::uint64_t(d)<<32)|a;
    }

    inline std::uint64_t rotate_left(std::uint64_t x,std::uint8_t y) noexcept
    {
        // Inline assembly is AT&T style, unless you provide both
        // AT&T and intel syntax.
        // ROL needs rotation amount in CL, but the value can be
        // in any general purpose register or memory - constraint "g".
        // "+" is combined input-output parameter.
        // "c" with a 8-bit object becomes fixed CL register.
        // "J" is an immediate shift value in [0;63] which
        // can be used when the optimizer has a constant shift amount.
        asm ("rolq %1,%0" : "+g"(x) : "cJ"(y));
        return x;
    }
}

int main()
{
    std::cout << ce::add(1,2,3,4,5,6,7,8,9) << "\n"
              << "rdtsc = " << rdtsc() << "\n"
              << "rotate_left(0x0123456789abcdef,12) = "
              << std::hex << rotate_left(0x0123456789abcdef,12) << '\n';
}
