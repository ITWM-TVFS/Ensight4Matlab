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


#include "../include/ensightasciiwriter.h"

#include <QFile>
#include <QTextStream>
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

bool writeAscii(EnsightObj* ensight, const QString& name, const QString& path,
                int timestep)
{
    if (timestep < 0)
    {
        for (int i = 0; i < ensight->getNumberOfTimesteps(); i++)
            if (!writeAsciiGeo(ensight, name, path, i))
                return false;
    }
    else
    {
        if (!writeAsciiGeo(ensight, name, path, timestep))
            return false;
    }

    for (int i = 0; i < ensight->getNumberOfVariables(); i++)
    {
        EnsightVariableIdentifier var = ensight->getVariable(i);

        if (timestep < 0)
        {
            for (int j = 0; j < ensight->getNumberOfTimesteps(); j++)
            {
                if (!writeAsciiVar(ensight, name, var.getName(), path, j,
                                   var.getDim()))
                    return false;
            }
        }
        else
        {
            if (!writeAsciiVar(ensight, name, var.getName(), path, timestep,
                               var.getDim()))
                return false;
        }
    }
    return true;
}

bool writeAsciiVar(EnsightObj* ensight, const QString& name, const QString& var,
                   const QString& path, int timestep, int dim)
{
    QString filename = QString("%0/%1.%2").arg(path).arg(name).arg(var);

    // For more than one timestep add timestep at end of filename
    if (ensight->isTransient())
    {
        int wildcards = log10(double(ensight->getNumberOfTimesteps()))+1;
        filename.append(QString("%0").arg(timestep, wildcards, 10, QLatin1Char('0')));
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        EnsightObj::ERROR_STR =
            "ERROR in [writeAsciiVar()] Unable to open file <" +
            file.fileName() + ">";
        return false;
    }

    QTextStream out(&file);

    out << "description line 1\n";

    for (int i = 0; i < ensight->getNumberOfParts(); i++)
    {
        EnsightPart* part = ensight->getPart(i);
        if (!part->hasVariable(var, timestep))
            continue;
        out << "part\n";
        out << QString("%0\n").arg(part->getId(), 10, 10, QLatin1Char(' '));
        out << "coordinates\n";
        // Write variable values for each node
        Matx values = part->getVariableValues(var, timestep);
        for (int j = 0; j < dim; j++)
            for (int k = 0; k < values.cols(); k++)
                out << QString("%0\n").arg(values(j, k), 12, 'e', 5, ' ');
    }
    file.close();
    return true;
}

bool writeAsciiGeo(EnsightObj* ensight, const QString& name,
                   const QString& path, int timestep)
{
    QString filename = QString("%0/%1%2").arg(path).arg(name).arg(".geo");

    // For more than one timestep add timestep at end of filename
    if (ensight->isTransient())
    {
        int wildcards = log10(double(ensight->getNumberOfTimesteps()))+1;
        filename.append(QString("%0").arg(timestep, wildcards, 10, QLatin1Char('0')));
    }


    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        EnsightObj::ERROR_STR =
            "ERROR IN [writeAsciiGeo()] Unable to open file <" +
            file.fileName() + ">";
        return false;
    }

    QTextStream out(&file);

    out << "description line 1\n";
    out << "description line 2\n";
    out << "node id assign\n";
    out << "element id assign\n";
    // extents

    for (int i = 0; i < ensight->getNumberOfParts(); i++)
    {
        EnsightPart *part = ensight->getPart(i);
        out << "part\n";
        out << QString("%0\n").arg(part->getId(), 10, 10, QLatin1Char(' '));
        out << part->getName() << "\n";

        if (part->getVertexCount(timestep) > 0)
        {
            out << "coordinates\n";

            // Vertices
            Matx vertices = part->getVertices(timestep);

            // Write number of vertices
            out << QString("%0\n").arg(vertices.cols(), 10, 10, QLatin1Char(' '));

            // Write vertices
            for (int j = 0; j < vertices.rows(); j++)
                for (int k = 0; k < vertices.cols(); k++)
                    out << QString("%0\n").arg(vertices(j, k), 12, 'e', 5, ' ');

            // Cells
            QList<EnsightCellList*> cells = part->getCells(timestep);
            for (auto cell : cells)
            {
                Ensight::Cell type = cell->getType();
                Mati values = cell->getValues();

                // Write cell type
                out << Ensight::strCell[type] << "\n";
                out << QString("%0\n").arg(values.cols(), 10, 10, QLatin1Char(' '));
                for (int j = 0; j < values.cols(); j++)
                {
                    // Write vertex indices defining cells
                    for (int k = 0; k < values.rows(); k++)
                        out << QString("%0").arg(values(k, j) + 1, 10, 10,
                                                 QLatin1Char(' '));
                    out << "\n";
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
