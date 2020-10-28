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
    GooseEYE::Ensemble ensemble({101, 101});

    for (size_t i = 0; i < 5; ++i)
    {
        auto I = GooseEYE::dummy_circles({200, 200});
        ensemble.S2(I, I);
    }

    auto S2 = ensemble.result();

    // check against previous versions
    H5Easy::File data("S2_ensemble.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::allclose(S2, H5Easy::load<decltype(S2)>(data, "S2")));

    return 0;
}
