/*
 * Copyright (c) 2016 Fraunhofer ITWM
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef ENSIGHTLIB_H
#define ENSIGHTLIB_H

#include <string>

class EnsightObj;
class QString;


/**
 * @mainpage
 * The Ensight Library is a C++ library to handle the EnSight Gold Data Format
 * (specified by CEI software, more details http://www.ceisoftware.com/).
 *
 * The Ensight Library can be used to:
 * - read files in EnSight Gold format (Ascii and binary)
 * - write files in EnSight Gold format (Ascii and binary)
 * - represent data according to the Ensight Gold format.
 *   Data can be static (single timestep) or transient (multiple timesteps).
 * - some basic geometric algorithms like fast cell lookup based on spatial
 *   subdivision (Octree/Quadtree), which can be used for interpolation.
 *
 * Short specification of the EnSight Gold format:
 *
 * The starting point of the EnSight Gold files is the so-called <tt>case</tt>
 * file, that points to all other files including model, geometry variables, and
 * possibly measured geometry and variables. All variable results for EnSight
 * Gold format are contained in disk files - one variable per file.
 * Additionally, if there are multiple time steps, there must either be a set of
 * disk files for each time step (transient multiple-file format), or all time
 * steps of a particular variable or geometry in one disk file each (transient
 * single-file format).
 * The EnSight Case file is an ASCII free format file that contains all the file
 * and name information for accessing model (and measured) geometry, variable,
 * and time information. It is comprised of five sections (FORMAT, GEOMETRY,
 * VARIABLE, TIME, FILE).
 *
 *
 * Since the Ensight Gold specification is more general than the data exported
 * from external tools we work with, some parts are not (yet) implemented:
 * - MATERIAL SETS (MATERIAL key word is completely ignored)
 * - FILE SETS (FILE key word is completely ignored)
 * - In Variable files UNDEFINED values and PARTIAL values are not supported.
 * - Only 1 TIME SET is supported. This TIME SET must be used for all variables
 *   and all geometries. For static data simply do not define a TIME SET and
 *   call setStatic().
 * - STRUCTURED data is not supported, only unstructured data, i.e. the BLOCK
 *   keyword is completely ignored.
 *
 * Some more limitations:
 * - GEOMETRY:
 *   - Only one model (measured etc. is not supported)
 * - VARIABLE:
 *   - The following key words are supported
 *     - Scalar per Node
 *     - Constant per Case
 *     - Vector per Node
 * - FILE:
 *   - No file Fileset are supported
 *
 * Data written by this library passes the EnSight Format Checker <ens_checker>
 * validation tool.
 */


/**
 * @brief The EnsightLib class is the main entry point of the Ensight Library
 * @author Andre Schmeißer, Daniel Burkhart, Simone Gramsch - Fraunhofer ITWM
 *
 * Use this class as entry point for the lib.
 * Once you have an EnsightObj object use the public member functions of this
 * object for all operations.
 */
class EnsightLib
{
public:
    static EnsightObj* createEnsight();
    static bool writeEnsight(EnsightObj* ensight, const QString& filename,
                             bool binary, int timestep);
    static bool writeEnsight(EnsightObj* ensight, const std::string& filename,
                             bool binary, int timestep);
    static EnsightObj* readEnsight(const QString& filename, int timestep = -1);
    static EnsightObj* readEnsight(const std::string& filename,
                                   int timestep = -1);
};

#endif // ENSIGHTLIB_H
