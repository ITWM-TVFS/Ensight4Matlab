# Ensight4Matlab

Ensight4Matlab let's you read, process, and write files in EnSight Case Gold format from your C++ or MATLAB&reg; code.

<img align="right" src="images/ensightviewer_velocity_small.png">
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
To build the MATLAB interface, you need to make sure MATLAB can find the EnsightLib library. For this, you can either build the library as described above and copy it to a location in the global library search path, or use it from a local directory by specifying the `LD_LIBRARY_PATH` environment variable (Linux systems). For example, if the path to your compiled library is `/home/USER/Ensight4Matlab/ensight_lib/lib/libEnsightLib.so`, you would set the following:
```bash
> export LD_LIBRARY_PATH=/home/USER/Ensight4Matlab/ensight_lib/lib/:${LD_LIBRARY_PATH}
```
This needs to be set before you start MATLAB.

Building the MATLAB interface is aided by two scripts: `SETUP.m` and `runmex.m`. In MATLAB, navigate to the `EnsightMatlab` directory containing these scripts, then run `SETUP` first followed by `runmex`.

The SETUP script will ask you to specify several include and library paths. These paths refer to:
 * QT_INCLUDE_PATH: The include files for Qt, e.g. `/usr/include`
 * QT_LIB_PATH: The Qt libraries, e.g. `/usr/lib64`
 * EIGEN_PATH: The Eigen library. Same as used in your EnsightLib.pro
 * ENSIGHT_INCLUDE_PATH: The includes for the EnsightLib library, e.g. `../ensight_lib/include`
 * ENSIGHT_LIB_PATH: The path to your compiled EnsightLib library, same as specified in `LD_LIBRARY_PATH` above

Once you have specified these paths, call `runmex` to start the MEX compiler. If everything works, you get the message `MEX completed successfully.` Now run the examples to test the installation.

Example usage
=============

This package comes with a full [C++ application example](#c-example) and two [MATLAB scripts](#matlab-example) to demonstrate the usage of the library. For further details you can read the Doxygen documentation for the C++ library. The Matlab interface provides a list of available methods by typing `methods EnsightLib`, and help on the individual methods with `help EnsightLib.METHOD`, where METHOD is any of the listed methods.


C++ Example
-----------

The directory `ensight_lib/examples/ensight_viewer` contains a demo application for the C++ library. This application is a simple OpenGL-based viewer for EnSight files. It is build using the qmake build process the same way as the EnsightLib library, see build instructions [build instructions above](#building-the-ensightlib-c-library).

Unpack the example data in `data/jet.tar.bz2` and load it in the viewer demo. You can see that the data consists of a several named parts, containing different types of cells (quadrangles, hexahedra), and several variables such as "temperature" and "velocity".

![Image description](images/ensightviewer_scr1.png)
<div align=center>Figure 1: The Ensight Viewer demo shows the structure of the example data file (left) and a visualization of the "velocity" variable of all parts selected as "active" (right).</div>
<div align=center>
<img src="images/ensightviewer_temperature_wire.png"><br>
Figure 2: A zoomed view of the "temperature" variable in wireframe mode.</div>

In the main window (left) you can select a representation mode, and either coloring according to a variable or solid color. For data sets consisting of multiple time steps, you can also select which time step to display. The provided example data set is static, i.e. contains only a single time step.

The displayed table details the structure of the data set. For each part, there are two lines, one for the currently selected time step and one for the data in all time steps. The different columns describe the bounding box of the part, whether it is displayed in the viewer ("active"), how many vertices it contains, as well as which types of cells it contains. Also, there is a colum for each variable containing the min and max values of the variable in this part.

-------

All code dealing with the Ensight Library is in `mainwidget.cpp`, the rest is just visualization and boilerplate.
Basic usage is as follows:

A complete data set in Ensight format is represented by an Object of class `EnsightObj`. You can create a new (empty) object by using the default constructor, or load an existing file using:
```c++
std::string fileName = "data/exampleData.case";
auto ensObject = EnsightLib::readEnsight();
```
Similiarly, you can then use
```c++
bool writeBinaryMode = true;
int timestepsToWrite = -1; // write all timesteps
EnsightLib::writeEnsight(ensObject, fileName, writeBinaryMode, timestepsToWrite);
```
to save the data to disk. Here, you can specify two additional parameters:
 * the parameter `writeBinaryMode` specifies whether the file is written as Ensight Binary format or as Ensight Ascii format.
 * for data sets containing multiple time steps, the `timestepsToWrite` parameter lets you save only a certain timestep, whereas passing -1 writes the complete data set. This is useful if your program generates data for time steps sequentially and you want to save only the new data as it is generated.


MATLAB Example
--------------

Description of MATLAB example...

License
=======
This package is copyright &copy; Fraunhofer ITWM and licensed under the MIT License. For details see [LICENSE](LICENSE).

The MATLAB wrapper uses the third party file "[class_handle.hpp](EnsightMatlab/mexfiles/class_handle.hpp)", which is copyright &copy; 2012, Oliver Woodford, and licensed under the 2-Clause BSD [License](EnsightMatlab/mexfiles/LICENSE.txt).

MATLAB is a registered trademark of The MathWorks, Inc. (see www.mathworks.com/trademarks). Ensight is a trademark of Computational Engineering International, Inc.


This package is an independent project and in no way affiliated with The MathWorks, Inc. or Computational Engineering International, Inc. All trademarks are property of their respective owners.

Contributors
============

Andre Schmeißer, Daniel Burkhart, Dominik Linn, Johannes Schnebele, Manuel Ettmüller, Simone Gramsch, Walter Arne
