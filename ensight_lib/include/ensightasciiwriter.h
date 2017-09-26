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


#ifndef ENSIGHTASCIIWRITER_H
#define ENSIGHTASCIIWRITER_H

#include <string>

class EnsightObj;
class QString;

/**
 * This file contains internal functions to write Ensight files in ASCII format.
 *
 * Do not use these directly, but use EnsightLib::writeEnsight instead.
 */

namespace Ensight
{
namespace Writer
{
namespace detail
{

/**
 * @brief Writes an Ensight file with corresponding geometry and variable files
 * based on a given Ensight object.
 * @param[in] ensight Ensight object
 * @param[in] name Case file name
 * @param[in] path File path
 * @param[in] timestep Timestep (-1 for all)
 */
bool writeAscii(EnsightObj* ensight, const QString& name, const QString& path,
                int timestep);

bool writeAsciiGeo(EnsightObj* ensight, const QString& name,
                   const QString& path, int timestep);
bool writeAsciiVar(EnsightObj* ensight, const QString& name, const QString& var,
                   const QString& path, int timestep, int dim);
}
}
}

#endif // ENSIGHTASCIIWRITER_H
