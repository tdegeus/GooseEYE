#include <GooseEYE/GooseEYE.h>
#include <xtensor.hpp>
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

    // determine clusters, based on the binary image
    auto C = GooseEYE::clusters(I);

    // 2-point cluster function
    auto C2 = GooseEYE::C2({101, 101}, C, C);

    // check against previous versions
    // note that the stored data based unix,
    // the random data is expected to be different on Windows
#ifndef _WIN32
    H5Easy::File data("C2.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::all(xt::equal(I, H5Easy::load<decltype(I)>(data, "I"))));
    MYASSERT(xt::all(xt::equal(C, H5Easy::load<decltype(C)>(data, "C"))));
    MYASSERT(xt::allclose(C2, H5Easy::load<decltype(C2)>(data, "C2")));
#endif

    return 0;
}
