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


#ifndef ENSIGHTWRITER_H
#define ENSIGHTWRITER_H

#include <string>

#include "../include/ensightdef.h"

class EnsightObj;
class QString;


namespace Ensight
{
namespace Writer
{

/**
 * @brief Writes an EnsightObj to EnSight Gold format
 * @param[in] ensight The EnsightObj to write
 * @param[in] filename The name of the ".case" file to write. Names of other
 * files of the data set, e.g. geometry files, are deduced from this.
 * @param[in] binary Write file in binary or ASCII format.
 * @param[in] timestep Optionally write only a single time step. This is
 * useful for incrementally creating data sets during a long running
 * computation, i.e. sequentially writing individual time steps as they are
 * computed. The .case file is only written when the first time step
 * (timestep=0) is written, otherwise only the corresponding geometry and
 * variable files are written.
 * The default argument of -1 writes the entire data set, i.e. .case file
 * and data files for all time steps.
 */
bool write(EnsightObj* ensight, const QString& filename, bool binary, int timestep = -1);
bool write(EnsightObj* ensight, const std::string& filename, bool binary, int timestep = -1);

}
}

#endif // ENSIGHTWRITER_H
