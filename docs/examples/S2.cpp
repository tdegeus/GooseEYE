#include <GooseEYE/GooseEYE.h>
#include <highfive/H5Easy.hpp>

#define MYASSERT(expr) MYASSERT_IMPL(expr, __FILE__, __LINE__)
#define MYASSERT_IMPL(expr, file, line) \
    if (!(expr)) { \
        throw std::runtime_error( \
            std::string(file) + ':' + std::to_string(line) + \
            ": assertion failed (" #expr ") \n\t"); \
    }

int main()
{
    // generate image
    auto I = GooseEYE::dummy_circles({500, 500});

    // 2-point probability
    auto S2 = GooseEYE::S2({101, 101}, I, I);

    // check against previous versions
    H5Easy::File data("S2.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::all(xt::equal(I, H5Easy::load<decltype(I)>(data, "I"))));
    MYASSERT(xt::allclose(S2, H5Easy::load<decltype(S2)>(data, "S2")));

    return 0;
}
