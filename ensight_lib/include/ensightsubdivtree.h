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

/**
 * @brief The EnsightSubdivTree class provides the interface to a spatial
 * subdivision tree used for fast point lookup. Users should not use this class
 * directly but instead use EnsightObj::createSubdivTree in conjunction with
 * EnsightObj::interpolate.
 *
 * Implementations of this interface are provided by EnsightOctree for 3D data
 * sets and EnsightQuadtree for 2D data sets (i.e. all z-values are 0).
 */
class EnsightSubdivTree
{
public:
    EnsightSubdivTree();
    virtual ~EnsightSubdivTree() = 0;

    /**
     * @brief Find the cell containing a given spatial position
     * @param[in] pos The position as 3D coordinates
     */
    EnsightCellIdentifier* search(const Vec3& pos) const;
    /**
     * @brief Find the cell containing a given spatial position
     * @param[in] pos The position as 3D coordinates
     * @param[out] The barycentric coordinates of position pos with respect to
     * the returned cell
     */
    virtual EnsightCellIdentifier* search(const Vec3& pos,
                                          EnsightBarycentricCoordinates& baryCoordout) const = 0;

    /**
     * @brief Find all cells whose bounding box contain the given position.
     * @param[in] pos The position as 3D coordinates
     */
    virtual QList<EnsightCellIdentifier*> searchAll(const Vec3& pos) const = 0;
    /**
     * @brief Insert the cells of a part into this subdivision tree
     * @param part The part to insert
     * @param timestep The timestep to use
     * @param sizeOffset An optional size offset added to the inserted cells'
     * bounding boxes, useful to search for "almost matching" cells using
     * searchAll.
     */
    virtual void insert(EnsightPart* part, int timestep, double sizeOffset = 0.0) = 0;
    /**
     * @brief get the bounding box of the cells contained in the tree.
     */
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
