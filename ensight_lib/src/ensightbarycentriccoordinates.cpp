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


#include "../include/ensightbarycentriccoordinates.h"

#include "../include/ensightcell.h"

EnsightBarycentricCoordinates::EnsightBarycentricCoordinates()
    : cell_(nullptr)
{
    pos_.setZero();
    baryCoords_.setZero();
}

EnsightBarycentricCoordinates::EnsightBarycentricCoordinates(const Vec3& pos, EnsightCellIdentifier* cell)
{
    compute(pos, cell);
}

bool EnsightBarycentricCoordinates::compute(const Vec3& pos, const EnsightCellIdentifier* cell,
                                            bool ignore2dCells)
{
    // Set valid status to false until it is clear that pos lies inside of the cell.
    isComputationSuccessful_ = false;
    cell_ = cell;
    pos_ = pos;

    const Veci& iNodes = cell->getCell();
    int n = iNodes.rows();
    baryCoords_.resize(n);
    baryCoords_.setZero();

    Veci iLocalSubNodes(n);
    for (int i = 0; i < n; ++i)
        iLocalSubNodes(i) = i;


    // Check for cell type
    switch (cell->getType())
    {
    case Ensight::Tetradhedron:
        computeTetraCell(pos, cell, iLocalSubNodes);
        break;
    case Ensight::Pyramid:
        computePyramidCell(pos, cell, iLocalSubNodes);
        break;
    case Ensight::Wedge:
        computeWedgeCell(pos, cell, iLocalSubNodes);
        break;
    case Ensight::Hexahedron:
        computeHexaCell(pos, cell, iLocalSubNodes);
        break;
    case Ensight::Triangle:
        if (ignore2dCells)
            return false;
        computeTriCell(pos, cell, iLocalSubNodes);
        break;
    case Ensight::Quadrangle:
        if (ignore2dCells)
            return false;
        computeQuadCell(pos, cell, iLocalSubNodes);
        break;
    default:
        return false;
    }

    isComputationSuccessful_ = true;

    return computeIsInside();
}

bool EnsightBarycentricCoordinates::computeIsInside()
{
    for (int i = 0; i < baryCoords_.rows(); ++i)
    {
        if (!(baryCoords_(i) > -insideCellTolerance_))
        {
            return false;
        }
    }
    return true;
}

bool EnsightBarycentricCoordinates::isValid()
{
    return isComputationSuccessful_;
}

Vecx EnsightBarycentricCoordinates::evaluate(const QString &name)
{
    if (!cell_)
        return Vecx();
    const Matx& data = cell_->getValues(name);
    return evaluate(data);
}

Vecx EnsightBarycentricCoordinates::evaluate(const std::string &name)
{
    if (!cell_)
        return Vecx();
    const Matx& data = cell_->getValues(QString::fromStdString(name));
    return evaluate(data);
}

Vecx EnsightBarycentricCoordinates::evaluate(const Matx &data)
{
    Vecx value(data.rows());
    value.setZero();

    if (!isValid())
    {
        // Trying to evaluate an invalid coordinate object.
        // TODO: Issue warning. Return NAN?
        return value;
    }

    // Interpolate data
    const Veci& iNodes = cell_->getCell();
    for (int i = 0; i < baryCoords_.rows(); ++i)
    {
        value += baryCoords_(i) * data.col(iNodes(i));
    }
    return value;
}

Vecx EnsightBarycentricCoordinates::evaluateUnsteady(EnsightBarycentricCoordinates &localCoord0,
                                                     EnsightBarycentricCoordinates &localCoord1,
                                                     double t0, double t1, double tCurrent,
                                                     const QString &name)
{
    // Compute prev timestep value
    Vecx value0;
    value0.setZero();
    if (localCoord0.isValid())
    {
        value0 = localCoord0.evaluate(name);
    }

    // Compute next timestep value
    Vecx value1;
    value1.setZero();
    if (localCoord1.isValid())
    {
        value1 = localCoord1.evaluate(name);
    }

    Vecx value;
    value.setZero();
    if (localCoord0.isValid() && localCoord1.isValid())
    {
        double lambda = (t1 - tCurrent) / (t1 - t0);
        value = lambda * value0 + (1.0 - lambda) * value1;
    }
    else if (localCoord0.isValid())
    {
        value = value0;
    }
    else if (localCoord1.isValid())
    {
        value = value1;
    }
    else
    {
        // This case should not occur.
        assert(false);
    }

    return value;
}

Vecx EnsightBarycentricCoordinates::evaluateUnsteady(EnsightBarycentricCoordinates &localCoord0,
                                                     EnsightBarycentricCoordinates &localCoord1,
                                                     double t0, double t1, double tCurrent,
                                                     const std::string& name)
{
    return evaluateUnsteady(localCoord0, localCoord1, t0, t1, tCurrent, QString::fromStdString(name));
}

Vecx EnsightBarycentricCoordinates::getCoords() const
{
    return baryCoords_;
}

bool EnsightBarycentricCoordinates::isBarycentricInterpolation()
{
    if (!isComputationSuccessful_)
        return false;

    const Veci& iNodes = cell_->getCell();

    Vec3 pos2;
    pos2.setZero();
    for (int i = 0; i < baryCoords_.rows(); ++i)
    {
        pos2 += baryCoords_(i) * cell_->getVertex(iNodes(i));
    }

    return (pos_ - pos2).norm() < 1e-6;
}


void EnsightBarycentricCoordinates::computeTetraCell(const Vec3& pos,
                                                     const EnsightCellIdentifier* cell,
                                                     Veci& iLocalSubNodes)
{
    const Veci& iNodes = cell->getCell();

    // Vertices of the tetrahedron
    Vec3 v0 = cell->getVertex(iNodes(iLocalSubNodes(0)));
    Vec3 v1 = cell->getVertex(iNodes(iLocalSubNodes(1)));
    Vec3 v2 = cell->getVertex(iNodes(iLocalSubNodes(2)));
    Vec3 v3 = cell->getVertex(iNodes(iLocalSubNodes(3)));

    Vec3 q2 = v1 - v0;
    Vec3 q3 = v2 - v0;
    Vec3 q4 = v3 - v0;

    double det = q2.dot(q3.cross(q4));

    baryCoords_(iLocalSubNodes(1)) = (pos-v0).dot((q3.cross(q4)) / det);
    baryCoords_(iLocalSubNodes(2)) = (pos-v0).dot((q4.cross(q2)) / det);
    baryCoords_(iLocalSubNodes(3)) = (pos-v0).dot((q2.cross(q3)) / det);

    baryCoords_(iLocalSubNodes(0)) = 1.0 - baryCoords_(iLocalSubNodes(1))
                                         - baryCoords_(iLocalSubNodes(2))
                                         - baryCoords_(iLocalSubNodes(3));
}

void EnsightBarycentricCoordinates::computePyramidCell(const Vec3& pos,
                                                       const EnsightCellIdentifier* cell,
                                                       Veci& iLocalSubNodes)
{
    const Veci& iNodes = cell->getCell();

    // Define split plane for cell
    Vec3i iSplitNodes;
    iSplitNodes << iNodes(iLocalSubNodes(0)), iNodes(iLocalSubNodes(2)), iNodes(iLocalSubNodes(4));

    // Split into two tetra
    Veci vertexMap(4);
    bool isAbove = isOnSameSideOfSplitPlane(pos, iNodes(iLocalSubNodes(3)), cell, iSplitNodes);
    if (isAbove)
        vertexMap << 0, 2, 3, 4;
    else
        vertexMap << 0, 1, 2, 4;

    // Create node vector for new sub cell.
    Veci iLocalSubNodesNew(vertexMap.rows());
    for (int i = 0; i < vertexMap.rows(); ++i)
    {
        iLocalSubNodesNew(i) = iLocalSubNodes(vertexMap(i));
    }

    // Compute barycentric coordinates for sub cell.
    computeTetraCell(pos, cell, iLocalSubNodesNew);
}

void EnsightBarycentricCoordinates::computeWedgeCell(const Vec3 &pos,
                                                     const EnsightCellIdentifier *cell,
                                                     Veci &iLocalSubNodes)
{
    const Veci& iNodes = cell->getCell();

    // Define split plane for cell
    Vec3i iSplitNodes;
    iSplitNodes << iNodes(iLocalSubNodes(0)), iNodes(iLocalSubNodes(1)), iNodes(iLocalSubNodes(5));

    // Split into two wedges
    Veci vertexMap;
    bool isAbove = isOnSameSideOfSplitPlane(pos, iNodes(iLocalSubNodes(2)), cell, iSplitNodes);
    if (isAbove)
    {

        vertexMap.resize(4);
        vertexMap << 0, 1, 2, 5;
    }
    else
    {
        vertexMap.resize(5);
        vertexMap << 3, 4, 1, 0, 5;
    }

    // Create node vector for new sub cell.
    Veci iLocalSubNodesNew(vertexMap.rows());
    for (int i = 0; i < vertexMap.rows(); ++i)
    {
        iLocalSubNodesNew(i) = iLocalSubNodes(vertexMap(i));
    }

    // Compute barycentric coordinates for sub cell.
    if (isAbove)
        computeTetraCell(pos, cell, iLocalSubNodesNew);
    else
        computePyramidCell(pos, cell, iLocalSubNodesNew);
}

void EnsightBarycentricCoordinates::computeHexaCell(const Vec3 &pos,
                                                    const EnsightCellIdentifier *cell,
                                                    Veci &iLocalSubNodes)
{
    const Veci &iNodes = cell->getCell();

    // Define split plane for cell
    Veci vertexMap;
    Vec3i iSplitNodes;
    iSplitNodes << iNodes(iLocalSubNodes(1)), iNodes(iLocalSubNodes(5)), iNodes(iLocalSubNodes(7));
    if (isOnSameSideOfSplitPlane(pos, iNodes(iLocalSubNodes(6)), cell, iSplitNodes))
    {

        iSplitNodes << iNodes(iLocalSubNodes(1)), iNodes(iLocalSubNodes(6)), iNodes(iLocalSubNodes(7));
        if (isOnSameSideOfSplitPlane(pos, iNodes(iLocalSubNodes(5)), cell, iSplitNodes))
        {
            vertexMap.resize(4);
            vertexMap << 1, 6, 7, 5;
        }
    }
    else
    {
        iSplitNodes << iNodes(iLocalSubNodes(1)), iNodes(iLocalSubNodes(7)), iNodes(iLocalSubNodes(4));
        if (isOnSameSideOfSplitPlane(pos, iNodes(iLocalSubNodes(5)), cell, iSplitNodes))
        {
            vertexMap.resize(4);
            vertexMap << 1, 7, 4, 5;
        }
    }

    if (vertexMap.rows() == 0)
    {
        iSplitNodes << iNodes(iLocalSubNodes(1)), iNodes(iLocalSubNodes(7)), iNodes(iLocalSubNodes(3));
        if (isOnSameSideOfSplitPlane(pos, iNodes(iLocalSubNodes(2)), cell, iSplitNodes))
        {

            vertexMap.resize(5);
            vertexMap << 3, 2, 6, 7, 1;
        }
        else
        {
            vertexMap.resize(5);
            vertexMap << 0, 3, 7, 4, 1;
        }
    }

    // Create node vector for new sub cell.
    Veci iLocalSubNodesNew(vertexMap.rows());
    for (int i = 0; i < vertexMap.rows(); ++i)
    {
        iLocalSubNodesNew(i) = iLocalSubNodes(vertexMap(i));
    }

    // Compute barycentric coordinates for sub cell.
    if (iLocalSubNodesNew.rows() == 4)
        computeTetraCell(pos, cell, iLocalSubNodesNew);
    else
        computePyramidCell(pos, cell, iLocalSubNodesNew);
}

void EnsightBarycentricCoordinates::computeTriCell(const Vec3 &pos, const EnsightCellIdentifier *cell, Veci &iLocalSubNodes)
{
    const Veci &iNodes = cell->getCell();

    Vec3 a = cell->getVertex(iNodes(iLocalSubNodes(0)));
    Vec3 b = cell->getVertex(iNodes(iLocalSubNodes(1)));
    Vec3 c = cell->getVertex(iNodes(iLocalSubNodes(2)));

    Vec3 v0 = b - a;
    Vec3 v1 = c - a;
    Vec3 v2 = pos - a;

    double d00 = v0.dot(v0);
    double d01 = v0.dot(v1);
    double d11 = v1.dot(v1);
    double d20 = v2.dot(v0);
    double d21 = v2.dot(v1);
    double denom = d00 * d11 - d01 * d01;

    baryCoords_(iLocalSubNodes(1)) = (d11 * d20 - d01 * d21) / denom;
    baryCoords_(iLocalSubNodes(2)) = (d00 * d21 - d01 * d20) / denom;
    baryCoords_(iLocalSubNodes(0)) = 1.0 - baryCoords_(iLocalSubNodes(1))
                                         - baryCoords_(iLocalSubNodes(2));
}

void EnsightBarycentricCoordinates::computeQuadCell(const Vec3 &pos, const EnsightCellIdentifier *cell, Veci &iLocalSubNodes)
{
    const Veci& iNodes = cell->getCell();

    //Define split line for cell
    Vec2i iSplitNodes;
    iSplitNodes << iNodes(iLocalSubNodes(0)), iNodes(iLocalSubNodes(2));

    //Split into two triangles
    Vec3i vertexMap;
    bool isAbove = isOnSameSideOfSplitLine(pos, iNodes(iLocalSubNodes(3)), cell, iSplitNodes);
    if (isAbove)
        vertexMap << 0, 2, 3;
    else
        vertexMap << 0, 1, 2;

    //Create node vector for new sub cell
    Veci iLocalSubNodesNew(vertexMap.rows());
    for (int i = 0; i < vertexMap.rows(); ++i)
    {
        iLocalSubNodesNew(i) = iLocalSubNodes(vertexMap(i));
    }

    //Compute barycentric coordinates for sub cell.
    computeTriCell(pos, cell, iLocalSubNodesNew);

}

bool EnsightBarycentricCoordinates::isOnSameSideOfSplitPlane(const Vec3& pos,
                                                             int iNodeSideSpecifier,
                                                             const EnsightCellIdentifier* cell,
                                                             Vec3i& iSplitNodes)
{
    Vec3 v0 = cell->getVertex(iSplitNodes(0));
    Vec3 v1 = cell->getVertex(iSplitNodes(1));
    Vec3 v2 = cell->getVertex(iSplitNodes(2));

    Vec3 vSideSpecifier = cell->getVertex(iNodeSideSpecifier);

    // compute normal
    Vec3 n = (v0-v1).cross(v2-v1);

    return (n.dot(pos-v0) >= 0) == (n.dot(vSideSpecifier-v0) >= 0);
}

bool EnsightBarycentricCoordinates::isOnSameSideOfSplitLine(const Vec3 &pos,
                                                            int iNodeSideSpecifier,
                                                            const EnsightCellIdentifier *cell,
                                                            Vec2i &iSplitNodes)
{
    Vec3 v0 = cell->getVertex(iSplitNodes(0));
    Vec3 v1 = cell->getVertex(iSplitNodes(1));

    Vec3 vDiag = v1 - v0;
    Vec3 vSideSpecifier = cell->getVertex(iNodeSideSpecifier);

    //normal
    Vec3 n;
    n << -vDiag(1), vDiag(0), 0;

    return (n.dot(pos-v0) >= 0) == (n.dot(vSideSpecifier-v0) >= 0);
}
