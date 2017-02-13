# Ensight4Matlab

Ensight4Matlab let's you read, process, and write files in EnSight Case Gold format from your C++ or MATLAB&reg; code.


The Ensight format is used to save CFD and CAE data and widely supported by many software tools. The format is
specified by CEI software, Inc., see http://www.ceisoftware.com for more details.
This packages provides a C++ based library which you can directly link (statically or dynamically) to your application. Additionally, it provides a language binding for MATLAB to easily use the C++ library in your MATLAB scripts.

Table of Contents
=================

  * [Installation](#installation)
    * [Requirements](#requirements)
    * [Compiling the EnsightLib C++ library](#building-the-ensightlib-c-library)
    * [Compiling the MATLAB interface](#building-the-matlab-interface)
  * [Example usage](#example-usage)
    * [C++ Example](#c-example)
    * [MATLAB Example](#matlab-example)
  * [License](#license)
  * [Contributors](#contributors)


Installation
============

The Ensight4Matlab package consists of two major parts, a C++ library and a wrapper which provides a MATLAB language binding. The C++ library can be used on its own, whereas the MATLAB wrapper needs the C++ library installed first.

Requirements
------------

The C++ library has dependencies on the following third party libraries:
  * Eigen (version >= 3.2): A fast, header-only library for linear algebra, available at http://eigen.tuxfamily.org/
  * Qt, either version 4 or 5.

  For use with MATLAB, Qt 4 is recommended. (see below)
  * OpenGL is needed for the [example application](#c-example).

The MATLAB wrapper depends on a compiled version of the C++ library and needs the MEX compiler to compile the MATLAB/C++ interface.

Warning:
MATLAB ships with its own Qt 5 library which may be a different minor version than the one installed on your system. This may lead to version conflicts if you compile the C++ Ensight library with a Qt 5 version higher than the one provided by MATLAB. In this case, we recommend you compile with Qt 4 to avoid conflicts. Additionally, we have experienced stability problems with the R2015a/b releases of MATLAB, so we recommend against using Ensight4Matlab with these releases.

Building the EnsightLib C++ library
-----------------------------------
The EnsightLib library uses the Qt qmake build system. From the directory `ensight_lib` first run qmake to create a Makefile, then run make.
```bash
> qmake-qt4 EnsightLib.pro
> make
```
This will create a file `libEnsightLib.so` in `ensight_lib/lib` (or a dll on Windows). The qmake-based Makefile does not create a target for `make install`, so you have to manually copy or link the .so file to e.g. `/usr/lib` if you want to make it globally accessible on your machine.

If you use a locally installed version of the Eigen library, you have to edit the include path defined in EnsightLib.pro. The corresponding line is:
```
# Include path to Eigen library: must contain the directory "Eigen/Dense"
INCLUDEPATH += /usr/local/include
```
You can also edit the .pro file to compile a static library (instead of an .so) by uncommenting the config option:
```
CONFIG += staticlib
```

Building the MATLAB interface
-----------------------------


Example usage
=============

Description of examples...

C++ Example
-----------

The directory `ensight_lib/examples/ensight_viewer` contains an example application for the C++ library. This application is a simple OpenGL-based viewer for EnSight files. It is build using the qmake build process the same way as the EnsightLib library, see build instructions [build instructions above](#building-the-ensightlib-c-library).

MATLAB Example
--------------

Description of MATLAB example...

License
=======
This library is copyright &copy; Fraunhofer ITWM and licensed under the MIT License. For details see [LICENSE](LICENSE).

The MATLAB wrapper uses the third party file "[class_handle.hpp](EnsightMatlab/mexfiles/class_handle.hpp)", which is copyright &copy; 2012, Oliver Woodford, and licensed under the 2-Clause BSD [License](EnsightMatlab/mexfiles/LICENSE.txt).

MATLAB is a registered trademark of The MathWorks, Inc. (see www.mathworks.com/trademarks).


This package is an independent project and in no way affiliated with The MathWorks, Inc or CEI software, Inc. All trademarks are property of their respective owners.

Contributors
============

Andre Schmeißer, Daniel Burkhart, Dominik Linn, Manuel Ettmüller, Simone Gramsch
