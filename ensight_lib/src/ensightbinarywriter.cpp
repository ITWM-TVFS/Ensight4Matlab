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


#include "../include/ensightbinarywriter.h"

#include <cstdint>
#include <fstream>
#include "../include/ensightobj.h"
#include "../include/ensightcell.h"
#include "../include/ensightpart.h"
#include "../include/ensightvariable.h"

namespace Ensight
{
namespace Writer
{
namespace detail
{

// Ensight format stores 32 bit ints and floats
using std::int32_t;
static_assert(sizeof(float) == sizeof(int32_t), "float has the wrong size");

void write_ensight_string(const char* val, std::ofstream& str)
{
    const int lineLength = 80;
    char buffer[lineLength] = {0};
    strncpy(buffer, val, lineLength);
    str.write(buffer, lineLength);
}

void write_ensight_int(const int32_t val, std::ofstream& str)
{
    str.write(reinterpret_cast<const char*>(&val), sizeof(int32_t));
}

void write_ensight_float(const float val, std::ofstream& str)
{
    str.write(reinterpret_cast<const char*>(&val), sizeof(float));
}

bool writeBinary(EnsightObj* ensight, const QString& name, const QString& path,
                 int timestep)
{
    if (timestep < 0)
    {
        for (int i = 0; i < ensight->getNumberOfTimesteps(); i++)
            if (!writeBinaryGeo(ensight, name, path, i))
                return false;
    }
    else
    {
        if (!writeBinaryGeo(ensight, name, path, timestep))
            return false;
    }
    for (int i = 0; i < ensight->getNumberOfVariables(); i++)
    {
        EnsightVariableIdentifier var = ensight->getVariable(i);
        if (timestep < 0)
        {
            for (int j = 0; j < ensight->getNumberOfTimesteps(); j++)
            {
                if (!writeBinaryVar(ensight, name, var.getName(), path, j,
                                    var.getDim()))
                    return false;
            }
        }
        else
        {
            if (!writeBinaryVar(ensight, name, var.getName(), path, timestep,
                                var.getDim()))
                return false;
        }
    }
    return true;
}

bool writeBinaryVar(EnsightObj* ensight, const QString& name,
                    const QString& var, const QString& path, int timestep,
                    int dim)
{
    QString filename = QString("%0/%1.%2").arg(path).arg(name).arg(var);

    // For more than one timestep add timestep at end of filename
    if (ensight->isTransient())
    {
        int wildcards = log10(double(ensight->getNumberOfTimesteps())) + 1;
        filename.append(QString("%0").arg(timestep, wildcards, 10, QLatin1Char('0')));
    }

    std::ofstream file;
    file.open(filename.toStdString().c_str(), std::ios::binary | std::ios::out);
    if (!file.is_open())
    {
        EnsightObj::ERROR_STR  = "In [writeBinaryVar()] Unable to open file <"
                                 + filename + ">";
        return false;
    }

    write_ensight_string(var.toStdString().c_str(), file);
    for (int i = 0; i < ensight->getNumberOfParts(); i++)
    {
        EnsightPart* part = ensight->getPart(i);

        write_ensight_string("part", file);
        write_ensight_int(part->getId(), file);

        // This is correct (according to enschecker) and saves time and space,
        // but does not work with Paraview.
        /*
        if (!part->hasVariable(var, timestep))
        {
            write_ensight_string("coordinates partial", file);
            write_ensight_int(0, file);
            continue;
        }
        */

        // This works with Paraview, but writes a lot of data which is not required.
        // But binary format has still better performance than Ascii.
        if (!part->hasVariable(var, timestep))
        {
            write_ensight_string("coordinates", file);
            Matx vertices = part->getVertices(timestep);
            for (int j = 0; j < dim; j++)
                for (int k = 0; k < vertices.cols(); k++)
                    write_ensight_float(0.0, file);
            continue;
        }

        write_ensight_string("coordinates", file);
        // Write variable values for each node
        Matx values = part->getVariableValues(var, timestep);
        for (int j = 0; j < dim; j++)
            for (int k = 0; k < values.cols(); k++)
                write_ensight_float(values(j, k), file);
    }
    file.close();
    return true;
}

bool writeBinaryGeo(EnsightObj* ensight, const QString& name,
                    const QString& path, int timestep)
{
    QString filename = QString("%0/%1%2").arg(path).arg(name).arg(".geo");

    // For more than one timestep add timestep at end of filename
    if (ensight->isTransient())
    {
        int wildcards = log10(double(ensight->getNumberOfTimesteps())) + 1;
        filename.append(QString("%0").arg(timestep, wildcards, 10, QLatin1Char('0')));
    }

    std::ofstream file;
    file.open(filename.toStdString().c_str(), std::ios::binary | std::ios::out);
    if (!file.is_open())
    {
        EnsightObj::ERROR_STR  = "ERROR IN [writeBinaryGeo()] Unable to open file <"
                                 + filename + ">";
        return false;
    }

    write_ensight_string("C Binary", file);
    write_ensight_string("Fraunhofer ITWM Ensight Lib", file);
    write_ensight_string("Geometry File", file);
    write_ensight_string("node id assign", file);
    write_ensight_string("element id assign", file);

    for (int i = 0; i < ensight->getNumberOfParts(); i++)
    {
        EnsightPart* part = ensight->getPart(i);
        write_ensight_string("part", file);
        write_ensight_int(part->getId(), file);
        write_ensight_string(part->getName().toStdString().c_str(), file);
        if (part->getVertexCount(timestep) > 0)
        {
            write_ensight_string("coordinates", file);

            // Vertices
            Matx vertices = part->getVertices(timestep);

            // Write number of vertices
            write_ensight_int(vertices.cols(), file);


            // Write vertices
            for (int j = 0; j < vertices.rows(); j++)
                for (int k = 0; k < vertices.cols(); k++)
                    write_ensight_float(vertices(j, k), file);

            // Cells
            QList<EnsightCellList*> cells = part->getCells(timestep);
            for (auto cell : cells)
            {
                Ensight::Cell type = cell->getType();
                Mati values = cell->getValues();

                // Write cell type
                write_ensight_string(Ensight::strCell[type], file);
                write_ensight_int(values.cols(), file);
                for (int j = 0; j < values.cols(); j++)
                {
                    // Write vertex indices defining cells
                    for (int k = 0; k < values.rows(); k++)
                        write_ensight_int(values(k, j) + 1, file);
                }
            }
        }
    }
    file.close();
    return true;
}

} // namespace detail
} // namespace Writer
} // namespace Ensight
