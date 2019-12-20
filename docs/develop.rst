
****************
Developers notes
****************

.. warning::

  Page outdated: Under construction!

.. todo::

  Under construction!

Graphical User Interface (GUI) - Qt
===================================

Static compilation
------------------

Below the details of what I have used to statically compile the Qt application. Before continuing two general remarks:

*   **Be careful with using copy-paste**.

*   The configuring, building, and installation each may take quite some time (up to several hours).

.. note::

  The following references may be helpful:

  *   Blog of `Dimitris Apeiro <http://dimitris.apeiro.gr/2015/06/24/build-a-static-qt5-for-windows-by-compiling/>`_.

  *   Wiki page of `WohlsSoft.ru <http://wohlsoft.ru/pgewiki/Building_static_Qt_5#Linux.2FMac_OS_X>`_.

MacOS
^^^^^

1.  Download the entire source code: `version 5.9 <http://download.qt.io/official_releases/qt/5.9/5.9.0/single/qt-everywhere-opensource-src-5.9.0.tar.xz>`_, or use the `Qt open-source download page <https://www.qt.io/download-open-source/#section-5>`_.

2.  Unpack everything from the tarball into any folder:

    .. code-block:: bash

      tar -xvf qt-everywhere-opensource-src-5.9.0.tar.gz

3.  Run:

    .. code-block:: bash

      ./configure -static -release -silent -prefix ~/Qt/5.9.0_static -opensource -confirm-license -opengl -nomake examples -skip wayland -skip purchasing -skip serialbus -skip qtserialport -skip script -skip scxml -skip speech -qt-libpng -no-libjpeg -qt-zlib -qt-pcre -qt-harfbuzz -qt-freetype -c++std c++14

    .. note::

      The option ``-c++std c++14`` may be omitted if the C++ version needed is not younger that ``C++11``.

4.  Run:

    .. code-block:: bash

      make

    .. note::

      To speed-up one can increase the number of concurrent jobs. For example: ``make -r -j 4``.

5.  Run:

    .. code-block:: bash

      make install

6.  The statically built Qt will be located at ``~/Qt/5.9.0_static``. To configure your Qt application to be built with static Qt version, you need to call ``~/Qt/5.9.0_static/bin/qmake`` while you are in your project folder. Other steps are same as when you build with the regular (dynamic) Qt version.

    If you want to plug static build of Qt into Qt Creator, just make a new toolchain with your default compiler (clang) and debugger, and with your static build of Qt (find QMake in the bin subdirectory of your static Qt build).

Windows
^^^^^^^

In principle static compilation of the GUI under Windows is a piece of cake. However, there are some things that have to be preprepared, and it can take quite some time if you are not pointed to the right direction.

I ended up using the blog-post of `Dimitris Apeiro <http://dimitris.apeiro.gr/2015/06/24/build-a-static-qt5-for-windows-by-compiling/>`_, which contained exactly what I needed. Following this post, here is a short summary on my set-up.

.. note::

  The following links may also be relevant:

  *   `Qt notes on Windows deployment <http://doc.qt.io/qt-5/windows-deployment.html>`_. Specifically notice that when static compilation is unsuccessful one can also use ``windeployqt```. To do this:

      1.  Open the ``Qt 5.8 for Desktop`` command-prompt (this has the environment set correctly).

      2.  Navigate to the ``build/release`` folder.

      3.  Run ``windeployqt name-of-executable.exe``.

1.  Download the Qt framework from `qt.io <http://www.qt.io>`_. I have used to following options:

    *   Qt 5.8

        -   MinGW 5.3.0 32 bit
        -   msvc 2015 32-bit *(not used by me)*
        -   msvc 2015 64-bit *(not used by me)*
        -   Sources **(important, non-default)**
        -   Qt Charts *(default)*
        -   Qt Data Visualization *(default)*
        -   Qt Purchasing *(default)*
        -   Qt Virtual Keyboard *(default)*
        -   Qt WebEngine *(default)*
        -   Qt Gamepad (TP) *(default)*
        -   Qt Network Auth (TP) *(default)*
        -   Qt Speech *(default)*
        -   Qt Script (Deprecated) *(default)*

    *   Tools

        - Qt Creator 4.2.1
        - MinGW 5.3.0 **(important, non-default)**

    After the installation is finished one should already be able to compile and run the GUI, however making use of dynamic linking of the Qt libraries. For static compilation, and thus stand-alone deployment, follow the steps below.

2.  Add the following lines to the end of the file ``C:\Qt\5.5\Src\qtbase\mkspecs\win32-g++\qmake.conf`` *(be sure to make a back-up first)*:

    .. code-block:: bash

        QMAKE_LFLAGS += -static -static-libgcc -no-exceptions
        QMAKE_CFLAGS_RELEASE -= -O2
        QMAKE_CFLAGS_RELEASE += -Os -momit-leaf-frame-pointer
        DEFINES += QT_STATIC_BUILD

    .. note::

        `Dimitris Apeiro <http://dimitris.apeiro.gr/2015/06/24/build-a-static-qt5-for-windows-by-compiling/>`_ suggests the additional option ``-static-libgcc`` for ``QMAKE_LFLAGS``.

3.  Open a Qt environment command prompt: ``Start`` > ``All apps`` > ``Qt`` > ``Qt 5.8 for Desktop (MinGW 5.3.0 32 bit)``.

    *   Go to the `Src` folder:

        .. code-block:: bash

            cd ..
            cd Src

    *   Run:

        .. code-block:: bash

            configure -static -platform win32-g++ -prefix "C:\Qt\Static" -debug-and-release -opensource -confirm-license -nomake examples -nomake tests -nomake tools -opengl desktop -no-angle -sql-sqlite -make libs -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-freetype

        .. note::

            `Dimitris Apeiro <http://dimitris.apeiro.gr/2015/06/24/build-a-static-qt5-for-windows-by-compiling/>`_ adding the following line to `C:\Qt\Src\qtbase\mkspecs\features\default_post.prf`:

            .. code-block:: bash

                static:win32: QMAKE_LFLAGS += $$QMAKE_LFLAGS_STATIC_LIB

    *   Run:

        .. code-block:: bash

            mingw32-make -k -j4
            mingw32-make -k install

        .. note::

            There might be errors, usually this is nothing to worry about.

4.    Add the following line to the file ``C:\Qt\Static\mkspecs\win32-g++\qmake.conf``:

      .. code-block:: bash

          CONFIG += static

5.    Add the newly created environment to Qt Creator, to facilitate easy usage.

      *   ``Tools`` > ``Options`` > ``Qt Versions`` > ``Add...``

          .. code-block:: bash

              C:\Qt\Static\bin\qmake.exe

          Hit ``Apply``

      *    ``Tools`` > ``Options`` > ``Kits`` > ``Add``

          -   Give this a meaningful name (e.g. ``Desktop Qt 5.8.0 MinGW Static 32 bit``).

          -   Under ``Qt Version``, select the newly created ``Qt 5.8.0 (Static)``.

      *   In the project, select this environment (and release), and compile. All done!

New release
===========

1.  Update the version numbers as follows:

    -   Modify ``__version__`` in ``setup.py``.

2.  Upload the changes to GitHub and create a new release there (with the correct version number).

3.  Upload the package to PyPi:

    .. code-block:: bash

      $ python3 setup.py bdist_wheel --universal
      $ twine upload dist/*


