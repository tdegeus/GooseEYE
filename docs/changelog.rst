*********
Changelog
*********

v0.6.1
======

*   Using scikit-build, doxygen
*   Renaming "test" -> "tests"
*   [Python] Fixing setup dependencies

v0.6.0
======

*   General code-style update (#68)

*   Templating all functions (#66)

*   [GooseEYE::detail] Deprecating unused functions

*   Adding historical tests (#64)

    - Minor code-style updates
    - Separating seed to be manual -> generate the same result on each run unless the seed is set.
    - Adding `GooseEYE::random::random` and `GooseEYE::random::normal` to help with reproducible results.
    - Saving/comparing against earlier run examples.
    - Adding numerical tests to check against previous versions
    - [CI] Not all platforms run all examples to save time.

*   Switching to templates for 'clusters' API (#63)

*   Generalizing "pad" (#62)

*   Adding "center_of_mass" and "pos2img" (#61)

*   Code-simplification (#57)

    - Applying pep8-like style, added clang-format style-file.
    - Simplifying implementation: using xt::xtensor<T,3> and atleast_3d internally.
    - Clarifying comments.
    - Adding tests.

*   Update CMake style (#55)
