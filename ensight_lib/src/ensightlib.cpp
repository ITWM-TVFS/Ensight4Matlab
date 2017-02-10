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


#include "../include/ensightlib.h"

#include <QString>

#include "../include/ensightobj.h"
#include "../include/ensightreader.h"
#include "../include/ensightwriter.h"

EnsightObj* EnsightLib::createEnsight()
{
    return new EnsightObj();
}

bool EnsightLib::writeEnsight(EnsightObj* ensight, const QString& filename,
                              bool binary, int timestep)
{
    if (ensight->inEditMode())
    {
        EnsightObj::ERROR_STR = "ERROR in [write()] In Edit Mode, call "
                                "endEdit() first to verify data before writing "
                                "data to disk.";
        return false;
    }
    return Ensight::Writer::write(ensight, filename, binary, timestep);
}

bool EnsightLib::writeEnsight(EnsightObj *ensight, const std::string &filename,
                              bool binary, int timestep)
{
    return writeEnsight(ensight,QString::fromStdString(filename), binary, timestep);
}

EnsightObj* EnsightLib::readEnsight(const QString& filename, int timestep)
{
    return Ensight::Reader::read(filename, timestep);
}

EnsightObj* EnsightLib::readEnsight(const std::string& filename, int timestep)
{
    return Ensight::Reader::read(QString::fromStdString(filename), timestep);
}
