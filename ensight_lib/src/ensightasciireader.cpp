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

#include "../include/ensightasciireader.h"

#include <QFile>

#include "../include/ensightbinaryreader.h"
#include "../include/ensightobj.h"
#include "../include/ensightpart.h"
#include "../include/ensightreader.h"

namespace Ensight
{
namespace Reader
{
namespace detail
{


bool parseAsciiHeader(QFile& file, IdMode& nodeIds, IdMode& elementIds)
{
    // Skip over 2 description lines
    file.readLine();
    file.readLine();

    // Read line "node id <off/given/assign/ignore>"
    QByteArray buffer;
    buffer = file.readLine().trimmed();
    const char* node_id_zstr = "node id ";
    nodeIds = parseIdType(buffer.constData(), node_id_zstr);

    // Read line "element id <off/given/assign/ignore>"
    buffer = file.readLine().trimmed();
    const char* element_id_zstr = "element id ";
    elementIds = parseIdType(buffer.constData(), element_id_zstr);

    bool success = nodeIds != IdMode::Invalid &&
                   elementIds != IdMode::Invalid;
    return success;
}

bool readAsciiGeometry(EnsightObj& ensight, const QString& filename, int timestep)
{
    // Open File
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readGeometry()] Cannot open file " + filename;
        return false;
    }

    // Local variables
    EnsightPart* part = nullptr;
    IdMode nodeIdMode, elementIdMode;

    if (!parseAsciiHeader(file, nodeIdMode, elementIdMode))
    {
        EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readGeometry()] Invalid file header.";
        return false;
    }

    while (!file.atEnd())
    {
        QString line = file.readLine();
        if (line.contains("extents"))
        {
            // Skip over bounding box values (3 lines with 2 values)
            file.readLine();
            file.readLine();
            file.readLine();
        }
        else if (line.contains("part"))
        {
            // read id
            line = file.readLine();
            int part_id = line.toInt();

            // read name
            line = file.readLine();
            QString part_name = line.trimmed();

            if (timestep > 0)
            {
                part = ensight.getPartByName(part_name);
                if (!part || part->getId() != part_id)
                {
                    QString name = part ? part->getName() : part_name;
                    EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readGeometry()] Mismatch between time steps and part name / id for part <" + name + ">.";
                    return false;
                }
            }
            else
            {
                part = ensight.createEnsightPart(part_name, part_id);
                if (!part)
                    return false;
            }
        }
        else if (line.contains("coordinates"))
        {
            if (!part)
            {
                EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readGeometry()] at line <" + line + ">.";
                return false;
            }
            // read vertex  count
            line = file.readLine();
            int n_vertices = line.toInt();

            // skip node IDs
            if (idsStoredInFile(nodeIdMode))
            {
                for (int j = 0; j < n_vertices; j++)
                    file.readLine();
            }

            // read coordinate values
            Matx vertices = Matx(3, n_vertices);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < n_vertices; j++)
                {
                    QByteArray bytes = file.readLine().trimmed();
                    bool conversionOk;
                    vertices(i, j) = bytes.toDouble(&conversionOk);
                    if (!conversionOk)
                    {
                        EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readGeometry()] invalid value <" + bytes + ">.";
                        return false;
                    }
                }
            }
            part->setVertices(vertices, timestep);
        }
        else
        {
            Ensight::Cell cellType = Ensight::Unknown;
            for (int i = 0; i < Ensight::numCellTypes; i++)
            {
                if (line.startsWith(Ensight::strCell[i]))
                {
                    cellType = static_cast<Ensight::Cell>(i);
                    break;
                }
            }
            if (cellType == Ensight::Unknown)
            {
                EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readGeometry()] at line <" + line + ">. Unknown cell type identifier";
                return false;
            }

            // read number of elements
            line = file.readLine();
            int n_cells = line.toInt();

            // skip element IDs
            if (idsStoredInFile(elementIdMode))
            {
                for (int j = 0; j < n_cells; j++)
                    file.readLine();
            }

            // read element indices
            int n_nodes = Ensight::numCellNodes[cellType];
            Mati cells = Mati(n_nodes, n_cells);
            for (int i = 0; i < n_cells; i++)
            {
                //char buf128[128];
                //file.readLine(buf128, 128);
                QByteArray lineBytes = file.readLine();
                for (int j = 0; j < n_nodes; j++)
                {
                    QByteArray bytes = lineBytes.mid(j*10, 10).trimmed();
                    bool conversionOk;
                    //cells(j,i) = atoi(buf128 + j*10)-1;
                    cells(j, i) = bytes.toInt(&conversionOk) - 1;
                    if (!conversionOk)
                    {
                        EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readGeometry()] invalid value <" + lineBytes + ">.";
                        return false;
                    }
                }
            }
            ensight.setCells(part, cells, timestep, cellType);
        }
    }
    file.close();
    return true;
}

bool readAsciiGeometry(EnsightObj &ensight, const std::string &filename, int timestep)
{
    return readAsciiGeometry(ensight, QString::fromStdString(filename), timestep);
}

bool readAsciiVariable(EnsightObj& ensight, const QString& filename,
                       const QString& name, int timestep,
                       Ensight::VarTypes type, int dim)
{
    // Open File
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readVariable()] Cannot open file " + filename;
        return false;
    }

    while (!file.atEnd())
    {
        QString line = file.readLine();
        line = line.left(line.indexOf("#")).trimmed();
        if (line.startsWith("part"))
        {
            //read part id
            QString line = file.readLine();
            int part_id = line.toInt();

            //get correct part
            EnsightPart* part = ensight.getPartById(part_id);
            if (!part || part->getId() != part_id)
            {
                EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readVariable()] Part with ID <" + QString::number(part_id) + "> not defined in geometry file.";
                return false;
            }

            // Next line should contain
            line = file.readLine().trimmed();
            if (!line.startsWith("coordinates"))
            {
                EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readVariable()] keyword >coordinates> expected at line<" + line + ">.";
                return false;
            }
            if (line.contains("undef") || line.contains("partial"))
            {
                EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readVariable()] <undef> and <partial> variable values are not supported.";
                return false;
            }


            int n_vertices = part->getVertexCount(timestep);
            Matx values(dim, n_vertices);

            for (int i = 0; i < dim; i++)
            {
                for (int j = 0; j < n_vertices; j++)
                {
                    QByteArray bytes = file.readLine().trimmed();
                    bool conversionOk;
                    values(i, j) = bytes.toDouble(&conversionOk);
                    if (!conversionOk)
                    {
                        EnsightObj::ERROR_STR = "In [EnsightAsciiReader::readVariable()] invalid value <" + line + ">.";
                        return false;
                    }
                }
            }
            ensight.setVariable(part, name, values, type, timestep);
        }
    }
    file.close();
    return true;
}

} // namespace detail
} // namespace Reader
} // namespace Ensight
