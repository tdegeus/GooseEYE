:tocdepth: 3

===================================
Graphical User Interface (GUI) - Qt
===================================

.. contents::
  :local:
  :depth: 2
  :backlinks: top

------------------
Static compilation
------------------

Windows
-------

Getting things ready
````````````````````

In principle static compilation of the GUI under Windows is a piece of cake. However, there are some things that have to be preprepared, and it can take quite some time if you are not pointed to the right direction.

I ended up using the blog-post of `Dimitris Apeiro <http://dimitris.apeiro.gr/2015/06/24/build-a-static-qt5-for-windows-by-compiling/>`_, which contained exactly what I needed. Following this post, here is a short summary on my set-up.

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

    *   Run the following command *(be careful with copy-paste)*:

        .. code-block:: bash

            configure -static -platform win32-g++ -prefix "C:\Qt\Static" -debug-and-release -opensource -confirm-license -nomake examples -nomake tests -nomake tools -opengl desktop -no-angle -sql-sqlite -make libs -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-freetype

        *(this might take a wile)*

        .. note::

            `Dimitris Apeiro <http://dimitris.apeiro.gr/2015/06/24/build-a-static-qt5-for-windows-by-compiling/>`_ adding the following line to `C:\Qt\Src\qtbase\mkspecs\features\default_post.prf`:

            .. code-block:: bash

                static:win32: QMAKE_LFLAGS += $$QMAKE_LFLAGS_STATIC_LIB

    *   Run the following commands

        .. code-block:: bash

            mingw32-make -k -j4
            mingw32-make -k install

        *(each might take a wile)*

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
