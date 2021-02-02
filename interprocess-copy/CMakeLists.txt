add_executable(interprocess-copy
    src/event.hpp
    src/fork.hpp
    src/knem.hpp
    src/main.cpp
    src/memfd.hpp
)
target_compile_features(interprocess-copy PRIVATE cxx_std_20)

target_link_libraries(interprocess-copy PRIVATE
    benchmark::benchmark
    ce::utils
)

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("#include <knem_io.h>
int main(){
    return KNEM_DEVICE_FILENAME[0];
}" HAVE_KNEM)

ntc_target(interprocess-copy HEADER_PREFIX /)
