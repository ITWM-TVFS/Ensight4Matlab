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


#ifndef ENSIGHTBINARYREADER_H
#define ENSIGHTBINARYREADER_H

#include <string>

#include "ensightdef.h"
#include "ensightreader.h"

class EnsightObj;
class QString;


namespace Ensight
{
namespace Reader
{
namespace detail
{

/**
 * @brief Read an Ensight Geometry file from filename.
 * @param ensight The ensight file the loaded geometry is added to
 * @param filename The filename of the Geometry file
 * @param timestep
 * @return false in case of any errors, otherwise true
 */
bool readBinaryGeometry(EnsightObj& ensight, const QString& filename, int timestep);

/**
 * @brief Read an Ensight Variable file, MUST be scalar per node or vector per node
 * @param ensight The Ensight object the variable is added to
 * @param filename The filename to read from
 * @param name The name of the variable
 * @param timestep The timestep of the readed file
 * @param type The type of the variable
 * @param dim The dimension of the variable
 * @return  false in case of any errors, otherwise true
 *
 */
bool readBinaryVariable(EnsightObj& ensight, const QString& filename,
                        const QString& name, int timestep,
                        Ensight::VarTypes type, int dim);

IdMode parseIdType(const char* line, const char* idTypePrefix);
bool idsStoredInFile(IdMode mode);

}
}
}

#endif // ENSIGHTBINARYREADER_H
