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


#ifndef ENSIGHTSUBDIVTREE_H
#define ENSIGHTSUBDIVTREE_H

#include <array>
#include <memory>
#include <QList>
#include <QVector>

#include "bbox.h"

class EnsightCellIdentifier;
class EnsightBarycentricCoordinates;
class EnsightPart;

class EnsightSubdivTree
{
public:
    EnsightSubdivTree();
    virtual ~EnsightSubdivTree() = 0;

    EnsightCellIdentifier* search(const Vec3& pos);
    virtual EnsightCellIdentifier* search(const Vec3& pos,
                                          EnsightBarycentricCoordinates& baryCoordout) const = 0;
    virtual QList<EnsightCellIdentifier*> searchAll(const Vec3& pos) const = 0;
    virtual void insert(EnsightPart* part, int timestep, double sizeOffset = 0.0) = 0;
    virtual Bbox getBounds() const = 0;

};

// forward declarations
template <typename Node> class SubdivTreeImpl;
class QuadtreeNode;
class OctreeNode;

// Quadtree and Octree
using EnsightQuadtree = SubdivTreeImpl<QuadtreeNode>;
using EnsightOctree = SubdivTreeImpl<OctreeNode>;


#endif // ENSIGHTSUBDIVTREE_H
