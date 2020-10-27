#include <GooseEYE/GooseEYE.h>
#include <xtensor.hpp>

int main()
{
    // generate image
    auto I = GooseEYE::dummy_circles({50, 50});

    // determine clusters, based on the binary image
    auto C = GooseEYE::clusters(I);

    // 2-point cluster function
    auto C2 = GooseEYE::C2({11, 11}, C, C);

    return 0;
}
