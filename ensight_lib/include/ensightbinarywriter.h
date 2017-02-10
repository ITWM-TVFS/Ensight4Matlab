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


#ifndef ENSIGHTBINARYWRITER_H
#define ENSIGHTBINARYWRITER_H

#include <string>

class EnsightObj;
class QString;

/**
 * @brief   Methods to write an Ensight case file with corresponding geometry and variable files in Ensight Gold Binary format
 */
class EnsightBinaryWriter
{
public:
    /**
     * @brief Writes an Ensight case file with corresponding geometry and variable files based on a given Ensight object.
     * @param ensight Ensight Object
     * @param name Case file name
     * @param path File path
     * @param timestep Timestep
     */
    static bool writeBinary(EnsightObj* ensight, const QString& name, const QString& path, int timestep);
private:
    static bool writeBinaryGeo(EnsightObj* ensight, const QString& name, const QString& path, int timestep);
    static bool writeBinaryVar(EnsightObj* ensight, const QString& name, const QString& var,
                               const QString& path, int timestep, int dim);
};

#endif // ENSIGHTBINARYWRITER_H
