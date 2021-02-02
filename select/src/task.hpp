#ifndef UUID_53BAF104_487A_42D9_B32D_04784D415857
#define UUID_53BAF104_487A_42D9_B32D_04784D415857

#include <functional>

enum struct execution_kind
{
    dispatch,
    post,
    defer
};

using task_t = std::function<void ()>;

#endif
