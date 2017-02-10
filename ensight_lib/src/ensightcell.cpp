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


#include "../include/ensightcell.h"

#include <iostream>
#include <QDebug>
#include <QMultiHash>
#include <QSet>

#include "../include/ensightbarycentriccoordinates.h"
#include "../include/ensightpart.h"
#include "../include/ensightvariable.h"


EnsightCellList::EnsightCellList(Ensight::Cell type, const Mati& values, QString partName)
{
    type_ = type;
    values_ = values;
    partName_ = partName;

    // For each cell store for each face its neighboring cell index
    neighbors_ = Mati::Constant(Ensight::numCellFaces[type], values.cols(), -1);

    QMultiHash<int, QPair<int, int>> faces;
    for (int i = 0; i < values.cols(); i++)
    {
        for (int j = 0; j < Ensight::numCellFaces[type]; j++)
        {
            QPair<int, int> newFace(i, j);
            int hash = getFaceHash(type, newFace);

            bool success = false;
            if (faces.contains(hash))
            {
                // Iter over existing faces with identical hash code

                QMultiHash<int, QPair<int, int>>::iterator iter = faces.find(hash);
                while (iter != faces.end() && iter.key() == hash)
                {
                    QPair<int, int> oldFace = iter.value();
                    if (doFacesMatch(oldFace, newFace))
                    {
                        // Save the neighboring cell at the corresponding entry
                        neighbors_(oldFace.second, oldFace.first) = newFace.first;
                        neighbors_(newFace.second, newFace.first) = oldFace.first;
                        success = true;
                        faces.remove(hash, oldFace);
                        break;
                    }
                    ++iter;
                }
            }
            if (!success)
                faces.insert(hash, QPair<int, int>(i, j));
        }
    }

    // All faces that are still in the multihash faces have no neighbor, i.e. they are boundary faces
    // Create a 2xF matrix (F is the count of boundary faces)
    // first row is the cell index and second row is the face index of the boundary faces
    boundary_ = Mati(2, faces.count());
    QList<int> keys = faces.uniqueKeys();
    QSet<int> nodes;
    int k = 0;
    for (int key : keys)
    {
        QList<QPair<int, int>> fvalues = faces.values(key);
        for (const auto& fvalue : fvalues)
        {
            int i0 = fvalue.first;
            int i1 = fvalue.second;
            boundary_(0, k) = i0;
            boundary_(1, k) = i1;
            k++;

            // Detect boundary nodes of this face
            for (int l = 0; l < 4; l++)
            {
                int nodeIndex = Ensight::cellFaces[getType()][i1][l];
                if (nodeIndex != -1)
                    nodes.insert(values(nodeIndex, i0));
            }
        }
    }
    boundaryVertices_ = Mati(1, nodes.size());

    int j = 0;
    foreach (int node, nodes)
        boundaryVertices_(0, j++) = node;

    normals = Matx(0, 0);
    side_normals = Matx(0, 0);
}

EnsightCellList::EnsightCellList(Ensight::Cell type, const Mati &values, std::string partName)
{
    EnsightCellList(type, values, QString::fromStdString(partName));
}

Ensight::Cell EnsightCellList::getType() const
{
    return type_;
}

const Mati& EnsightCellList::getValues() const
{
    return values_;
}


const Mati& EnsightCellList::getNeighbors() const
{
    return neighbors_;
}

const Mati& EnsightCellList::getBoundaryVertices() const
{
    return boundaryVertices_;
}

const Mati& EnsightCellList::getBoundary() const
{
    return boundary_;
}

const QString& EnsightCellList::getPartName() const
{
    return partName_;
}


bool EnsightCellList::doFacesMatch(const QPair<int, int>& oldFace, const QPair<int, int>& newFace)
{
    if (oldFace.first == newFace.first) // same cell
        return false;

    // a Face has at most 4 nodes
    for (int i = 0; i < Ensight::maxNumNodesPerFace; i++)
    {
        int ii = Ensight::cellFaces[type_][oldFace.second][i];
        if (ii < 0)
            continue;
        int iPointer = values_(ii, oldFace.first);
        bool success = false;
        for (int j = 0; j < Ensight::maxNumNodesPerFace; j++)
        {
            int jj = Ensight::cellFaces[type_][newFace.second][j];
            if (jj < 0)
                continue;
            int jPointer = values_(jj, newFace.first);
            if (iPointer == jPointer)
            {
                success = true;
                break;
            }
        }
        if (!success)
            return false;
    }

    return true;
}

int EnsightCellList::getFaceHash(const Ensight::Cell& type, const QPair<int, int>& face)
{
    int hash = 0;
    for (int node = 0; node < Ensight::maxNumNodesPerFace; node++)
    {
        int pointer = Ensight::cellFaces[type][face.second][node];
        if (pointer == -1)
            break;
        hash += values_(pointer, face.first);
    }
    return hash;
}


EnsightCellIdentifier::EnsightCellIdentifier(EnsightPart* part, EnsightCellList* cellList,
                                             int timestep, int index, const Bbox& bounds)
    : part_(part), cellList_(cellList), timestep_(timestep), index_(index),
      bounds_(bounds)
{
}


Veci EnsightCellIdentifier::getCell() const
{
    return cellList_->getValues().col(index_);
}

Vec3 EnsightCellIdentifier::getVertex(int vertexId) const
{
    return part_->getVertices(timestep_).col(vertexId);
}

const Matx& EnsightCellIdentifier::getValues(const QString& name) const
{
    const Matx& data = part_->getVariable(name, timestep_)->getValues();
    return data;
}

const Matx &EnsightCellIdentifier::getValues(const std::string &name) const
{
    const Matx& data = part_->getVariable(QString::fromStdString(name), timestep_)->getValues();
    return data;
}

Ensight::Cell EnsightCellIdentifier::getType() const
{
    return getCellList()->getType();
}

const EnsightCellList* EnsightCellIdentifier::getCellList() const
{
    return cellList_;
}

const Bbox& EnsightCellIdentifier::getBounds() const
{
    return bounds_;
}

int EnsightCellIdentifier::getIndex() const
{
    return index_;
}

void EnsightCellIdentifier::print() const
{
    bounds_.print();
    std::cout << "   " << index_ << std::endl;
    std::cout << "   " << cellList_->getType() << std::endl;
}

bool EnsightCellIdentifier::contains(const Vec3& pos, bool ignore2dCells) const
{
    EnsightBarycentricCoordinates baryCoord;
    return contains(pos, baryCoord, ignore2dCells);
}

bool EnsightCellIdentifier::contains(const Vec3& pos,
                                     EnsightBarycentricCoordinates& baryCoordOut,
                                     bool ignore2dCells) const
{
    // First check if pos is contained in bounding box.
    if (!getBounds().contains(pos))
        return false;

    // Compute barycentric coordinates of pos and return success status
    return baryCoordOut.compute(pos, this, ignore2dCells);
}

double EnsightCellIdentifier::computeVolume() const
{
    Vec3 P, Q, R;

    Veci nodes = getCell();
    int type = static_cast<int>(getType());

    double volume = 0.0;

    switch (type)
    {
    case 0: // Point
        volume = 0.0;
        break;
    case 1: // Bar
        volume = (getVertex(nodes[1]) - getVertex(nodes[0])).norm();
        break;
    case 2: // Triangle
        P = getVertex(nodes[1]) - getVertex(nodes[0]);
        Q = getVertex(nodes[2]) - getVertex(nodes[0]);

        volume = (P.cross(Q)).norm() / 2.0;

        break;
    case 3: // Rectangle
        P = getVertex(nodes[3]) - getVertex(nodes[0]);
        Q = getVertex(nodes[1]) - getVertex(nodes[0]);

        volume = (P.cross(Q)).norm();

        break;
    case 4: // Tetrahedron
        P = getVertex(nodes[3]) - getVertex(nodes[0]);
        Q = getVertex(nodes[1]) - getVertex(nodes[0]);
        R = getVertex(nodes[2]) - getVertex(nodes[0]);

        volume = fabs(P.dot(Q.cross(R))) / 6.0;

        break;
    case 5: // Pyramid
        P = getVertex(nodes[4]) - getVertex(nodes[0]);
        Q = getVertex(nodes[1]) - getVertex(nodes[0]);
        R = getVertex(nodes[3]) - getVertex(nodes[0]);
        volume = fabs(P.dot(Q.cross(R))) / 6.0;

        P = getVertex(nodes[4]) - getVertex(nodes[2]);
        Q = getVertex(nodes[3]) - getVertex(nodes[2]);
        R = getVertex(nodes[1]) - getVertex(nodes[2]);
        volume += fabs(P.dot(Q.cross(R))) / 6.0;

        break;
    case 6: // Wedge
        P = getVertex(nodes[1]) - getVertex(nodes[0]);
        Q = getVertex(nodes[2]) - getVertex(nodes[0]);
        R = getVertex(nodes[3]) - getVertex(nodes[0]);
        volume = fabs(P.dot(Q.cross(R))) / 6.0;

        P = getVertex(nodes[1]) - getVertex(nodes[5]);
        Q = getVertex(nodes[3]) - getVertex(nodes[5]);
        R = getVertex(nodes[2]) - getVertex(nodes[5]);
        volume += fabs(P.dot(Q.cross(R))) / 6.0;

        P = getVertex(nodes[3]) - getVertex(nodes[4]);
        Q = getVertex(nodes[5]) - getVertex(nodes[4]);
        R = getVertex(nodes[1]) - getVertex(nodes[4]);
        volume += fabs(P.dot(Q.cross(R))) / 6.0;

        break;
    case 7: // Hexahedron
        P = getVertex(nodes[4]) - getVertex(nodes[0]);
        Q = getVertex(nodes[1]) - getVertex(nodes[0]);
        R = getVertex(nodes[3]) - getVertex(nodes[0]);
        volume = fabs(P.dot(Q.cross(R)));

        break;
    default:
#ifndef NDEBUG
        qDebug() << "Undefined celltype. Cannot compute volume.";
#endif
        assert(false);
    }

    return volume;
}
