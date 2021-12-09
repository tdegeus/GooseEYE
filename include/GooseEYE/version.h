/**
\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the GPLv3 License.
*/

#ifndef GOOSEEYE_VERSION_H
#define GOOSEEYE_VERSION_H

#include "config.h"

/**
Current version.

Either:

-   Configure using CMake at install time. Internally uses::

        python -c "from setuptools_scm import get_version; print(get_version())"

-   Define externally using::

        MYVERSION=`python -c "from setuptools_scm import get_version; print(get_version())"`
        -DGOOSEEYE_VERSION="$MYVERSION"

    From the root of this project. This is what ``setup.py`` does.

Note that both ``CMakeLists.txt`` and ``setup.py`` will construct the version using
``setuptools_scm``. Tip: use the environment variable ``SETUPTOOLS_SCM_PRETEND_VERSION`` to
overwrite the automatic version.
*/
#ifndef GOOSEEYE_VERSION
#define GOOSEEYE_VERSION "@PROJECT_VERSION@"
#endif

namespace GooseEYE {

/**
Return version string, e.g. `"0.8.0"`
\return String.
*/
inline std::string version();

} // namespace GooseEYE

#include "version.hpp"

#endif
