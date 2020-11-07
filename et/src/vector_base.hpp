#ifndef UUID_E8A66AFA_2C84_48D5_AADD_08751E50649D
#define UUID_E8A66AFA_2C84_48D5_AADD_08751E50649D

#include <boost/container/vector.hpp>

#include <algorithm>
#include <experimental/iterator>
#include <ostream>

template<typename T>
class vector_base : public boost::container::vector<T>
{
public:
    using boost::container::vector<T>::vector;

    friend std::ostream& operator<<(std::ostream& os,const vector_base& vb)
    {
        os << '[';
        std::copy(vb.begin(),vb.end(),
                  std::experimental::ostream_joiner{os,','});
        return os << ']';
    }
};

#endif
