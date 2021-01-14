#include <ce/generator.hpp>

#include <iostream>

namespace
{
    ce::generator<int> fibonacci()
    {
        int a = 1,b = 1;
        for(;;){
            co_yield +a;
            a = std::exchange(b,a+b);
        }
    }
}

int main()
{
    for(int i = 0 ; int f : fibonacci()){
        std::cout << f << '\n';
        if(++i==10)
            break;
    }
}
