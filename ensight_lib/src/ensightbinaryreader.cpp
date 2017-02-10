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


#include "../include/ensightbinaryreader.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <string>

#include "../include/ensightobj.h"
#include "../include/ensightpart.h"
#include "../include/ensightreader.h"

// Ensight format stores 32 bit ints and floats
using std::int32_t;
static_assert(sizeof(float) == sizeof(int32_t), "float has the wrong size");


namespace Ensight
{
namespace Reader
{
namespace detail
{

struct FixedSizeLine
{
    FixedSizeLine()
    {
        // Initialize as empty C string
        data[0] = 0;
        // Last character is never written to. Ensure that reading a line
        // into the buffer without a zero-terminator in it is still correctly
        // terminated.
        data[length] = 0;
    }
    operator char*() { return &data[0]; }

    static const int length = 80;
    std::array<char, length+1> data;
};

template <typename T> void trimRightSZ(T& zstring)
{
    auto lastNonWs = std::find_if_not(zstring.rbegin(), zstring.rend(),
                                      [](char c) { return c == 0 || isspace(c); });
    auto ws = lastNonWs.base();
    if (ws != std::end(zstring))
        *ws = 0;
}

template <typename T> void toUpper(T& str)
{
    std::transform(str.begin(), str.end(), str.begin(), toupper);
}

bool startsWith(const char* str, const char* substr)
{
    while (*substr)
        if (*str++ != *substr++)
            return false;
    return true;
}

void readLine(std::istream& in, FixedSizeLine& line)
{
    in.read(line, line.length);
    trimRightSZ(line.data);
}

void readInt(std::istream& in, int32_t& value)
{
    in.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
}

void readFloat(std::istream& in, float& value)
{
    in.read(reinterpret_cast<char*>(&value), sizeof(float));
}

IdMode parseIdType(const char* line, const char* idTypePrefix)
{
    // parse line "[prefix] <off/given/assign/ignore>"
    if (!startsWith(line, idTypePrefix))
        return IdMode::Invalid;

    const char* restOfLine = line + strlen(idTypePrefix);
    if (startsWith(restOfLine, "off"))
        return IdMode::Off;
    if (startsWith(restOfLine, "given"))
        return IdMode::Given;
    if (startsWith(restOfLine, "assign"))
        return IdMode::Assign;
    if (startsWith(restOfLine, "ignore"))
        return IdMode::Ignore;

    return IdMode::Invalid;
}

bool idsStoredInFile(IdMode mode)
{
    return mode == IdMode::Given || mode == IdMode::Ignore;
}

bool parseBinaryHeader(std::ifstream& in, IdMode& nodeIds,
                       IdMode& elementIds)
{
    FixedSizeLine line;

    // Read line "C Binary"
    readLine(in, line);
    toUpper(line.data);
    if (strcmp(line, "C BINARY") != 0)
        return false;

    // Skip over 2 description lines
    in.seekg(2*FixedSizeLine::length, std::ios_base::cur);

    // Read line "node id <off/given/assign/ignore>"
    readLine(in, line);
    const char* node_id_zstr = "node id ";
    nodeIds = parseIdType(line, node_id_zstr);

    // Read line "element id <off/given/assign/ignore>"
    readLine(in, line);
    const char* element_id_zstr = "element id ";
    elementIds = parseIdType(line, element_id_zstr);

    bool success = !in.fail() && nodeIds != IdMode::Invalid &&
                   elementIds != IdMode::Invalid;
    return success;
}

bool readBinaryGeometry(EnsightObj& ensight, const QString &filename, int timestep)
{
    // Open file and check if opened
    std::ifstream in(filename.toStdString().c_str(), std::ios::binary);

    if(!in.is_open())
    {
        EnsightObj::ERROR_STR = "In [EnsightBinaryReader::readGeometry()] Cannot open file <" + filename + ">";
        return false;
    }

    // local variables
    FixedSizeLine line;
    EnsightPart *part = nullptr;
    IdMode nodeIdMode, elementIdMode;

    if (!parseBinaryHeader(in, nodeIdMode, elementIdMode))
    {
        EnsightObj::ERROR_STR =  "In [EnsightBinaryReader::readGeometry()] Invalid file header.";
        return false;
    }

    while (!in.eof())
    {
        readLine(in, line);
        if (in.fail())
            break;

        if (strcmp(line, "extents") == 0)
        {
            // Skip over bounding box values (6 floats)
            in.seekg(6*sizeof(float), std::ios_base::cur);
        }
        else if (strcmp(line, "part") == 0)
        {
            // part id and part name
            int32_t part_id = -1;
            readInt(in, part_id);
            readLine(in, line);
            QString part_name = QString(line);

            if(timestep>0)
            {
                part = ensight.getPartByName(part_name);
                if (!part || part->getId() != part_id)
                {
                    QString name = part ? part->getName() : part_name;
                    EnsightObj::ERROR_STR = "In [EnsightBinaryReader::readGeometry()] Mismatch between time steps and part name / id for part <" + name + ">.";
                    return false;
                }
            }
            else
            {
                part = ensight.createEnsightPart(part_name, part_id);
            }
        }
        else if (strcmp(line, "coordinates") == 0)
        {
            if (!part)
            {
                EnsightObj::ERROR_STR = "In [EnsightBinaryReader::readGeometry()] at line <" + QString(line) + ">.";
                return false;
            }

            // read number of coordinates
            int32_t num_coords = -1;
            readInt(in, num_coords);

            // skip node IDs
            if (idsStoredInFile(nodeIdMode))
                in.seekg(num_coords*sizeof(int32_t), std::ios_base::cur);

            // read coordinate values
            Matx vertices = Matx(3, num_coords);
            for (int i = 0; i < 3; i++)
            {
                float value = 0;
                for(int j = 0; j < num_coords; j++)
                {
                    readFloat(in, value);
                    vertices(i, j) = value;
                }
            }
            part->setVertices(vertices, timestep);
        }
        else
        {
            Ensight::Cell cellType = Ensight::Unknown;
            for (int i = 0; i < Ensight::numCellTypes; i++)
            {
                if (strcmp(line, Ensight::strCell[i]) == 0)
                {
                    cellType = static_cast<Ensight::Cell>(i);
                    break;
                }
            }

            if (cellType == Ensight::Unknown)
            {
                EnsightObj::ERROR_STR = "In [EnsightBinaryReader::readGeometry()] at line <" + QString(line)
                                        + ">. Cell type identifier not supported";
                return false;
            }

            // read number of elements
            int32_t num_elements = -1;
            readInt(in, num_elements);

            // skip element IDs
            if (idsStoredInFile(elementIdMode))
                in.seekg(num_elements*sizeof(int32_t), std::ios_base::cur);

            // read element indices
            int n_nodes = Ensight::numCellNodes[cellType];
            Mati cells = Mati(n_nodes, num_elements);
            int32_t indexOneBased = -1;
            for(int i = 0; i < num_elements; i++)
            {
                for(int j = 0; j < n_nodes; j++)
                {
                    readInt(in, indexOneBased);
                    cells(j, i) = indexOneBased-1; // convert to zero based index
                }
            }
            ensight.setCells(part, cells, timestep, cellType);
        }
    }
    return true;
}

bool readBinaryVariable(EnsightObj& ensight, const QString& filename,
                        const QString& name, int timestep,
                        Ensight::VarTypes type, int dim)
{
    std::ifstream in(filename.toStdString().c_str(), std::ios::binary);
    if(!in.is_open())
    {
        EnsightObj::ERROR_STR = "In [EnsightBinaryReader::readVariable()] Cannot open file " + filename;
        return false;
    }

    FixedSizeLine line;
    readLine(in, line);  // read description line

    while (!in.eof())
    {
        readLine(in, line);  // read until a line with "part" is found
        if (strcmp(line, "part") == 0)
        {
            int32_t part_id;
            readInt(in, part_id);

            // get correct part
            EnsightPart *part = ensight.getPartById(part_id);
            if (!part || part->getId() != part_id)
            {
                EnsightObj::ERROR_STR =  "In [EnsightBinaryReader::readVariable()] Part with ID <" + QString::number(part_id) + "> not defined in geometry file.";
                return false;
            }

            readLine(in, line);
            if (strcmp(line, "coordinates") != 0)
            {
                EnsightObj::ERROR_STR =  "In [EnsightBinaryReader::readVariable()] keyword >coordinates> expected at line<" + QString(line) + ">.";
                return false;
            }

            int n_vertices = part->getVertexCount(timestep);
            Matx values(dim, n_vertices);
            float value;
            for (int i = 0; i < dim; i++)
            {
                for (int j = 0; j < n_vertices; j++)
                {
                    readFloat(in, value);
                    values(i, j) = value;
                }
            }
            ensight.setVariable(part, name, values, type, timestep);
        }
    }

    return true;
}

FileType getEnsightFileType(const QString& filename)
{
    std::ifstream in(filename.toStdString().c_str(), std::ios::binary);
    if(!in.is_open())
        return FileType::Ascii;

    FixedSizeLine line;
    readLine(in, line);
    toUpper(line.data);

    if (strcmp(line, "C BINARY") == 0)
    {
        return FileType::C_Binary;
    }
    if (std::string(line, line.length).find("FORTRAN BINARY") != std::string::npos)
    {
        return FileType::Fortran_Binary;
    }
    return FileType::Ascii;
}

} // namespace detail
} // namespace Reader
} // namespace Ensight
