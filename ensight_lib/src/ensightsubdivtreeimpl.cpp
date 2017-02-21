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


#include "../include/ensightsubdivtreeimpl.h"

#include <iostream>
#include <limits>

#include "../include/ensightbarycentriccoordinates.h"
#include "../include/ensightcell.h"
#include "../include/ensightpart.h"


// **** interface class EnsightSubdivTree ****
EnsightSubdivTree::EnsightSubdivTree() = default;
EnsightSubdivTree::~EnsightSubdivTree() = default;


EnsightCellIdentifier* EnsightSubdivTree::search(const Vec3 &pos) const
{
    EnsightBarycentricCoordinates baryCoords;
    return search(pos, baryCoords);
}



// **** template class SubdivTreeImpl ****
template <typename Node>
SubdivTreeImpl<Node>::SubdivTreeImpl(Bbox bounds, int maxLevel, int maxCells)
    : root_(new Node(bounds)), allCells_(), maxLevel_(maxLevel),
      maxCells_(maxCells)
{
}

template <typename Node>
SubdivTreeImpl<Node>::~SubdivTreeImpl()
{
    for (auto c : allCells_)
        delete c;
}

template <typename Node>
EnsightCellIdentifier* SubdivTreeImpl<Node>::search(
    const Vec3& pos, EnsightBarycentricCoordinates& baryCoordOut) const
{
    ptrdiff_t idx = root_->search(pos, baryCoordOut, allCells_);
    if (idx < 0)
        return nullptr;
    return allCells_[idx];
}

template <typename Node>
QList<EnsightCellIdentifier*> SubdivTreeImpl<Node>::searchAll(const Vec3& pos) const
{
    auto indices = root_->searchAll(pos, allCells_);
    QList<EnsightCellIdentifier*> results;
    for (ptrdiff_t idx : indices)
        results.push_back(allCells_[idx]);
    return results;
}

template <typename Node>
void SubdivTreeImpl<Node>::insert(EnsightPart* part, int timestep, double sizeOffset)
{
    QList<EnsightCellList*> cells = part->getCells(timestep);

    for (auto cellList : cells)
    {
        Mati cellsi = cellList->getValues();
        for (int k = 0; k < cellsi.cols(); k++)
        {
            Veci cell = cellsi.col(k);
            Bbox bounds = part->getCellBounds(cell, timestep);
            bounds.increaseBy(sizeOffset);
            auto idx = static_cast<Index>(allCells_.size());
            Q_ASSERT((size_t) idx < (size_t) std::numeric_limits<Index>::max());

            auto cellId = new EnsightCellIdentifier(part, cellList, timestep, k, bounds);
            allCells_.push_back(cellId);
            root_->insert(idx, allCells_, maxLevel_, maxCells_, 0);
        }
    }
}

template <typename Node>
Bbox SubdivTreeImpl<Node>::getBounds() const
{
    return root_->getBounds();
}

template <typename Node>
const Node& SubdivTreeImpl<Node>::getRoot() const
{
    return *root_;
}



// **** template class NodeBase ****

template <int N, typename Derived>
NodeBase<N, Derived>::NodeBase() = default;

template <int N, typename Derived>
NodeBase<N, Derived>::NodeBase(Bbox bounds) : children_(), cells_(), bounds_(bounds)
{
}

template <int N, typename Derived>
const Bbox& NodeBase<N, Derived>::getBounds() const
{
    return bounds_;
}

template <int N, typename Derived>
void NodeBase<N, Derived>::setBounds(Bbox bounds)
{
    bounds_ = bounds;
}

template <int N, typename Derived>
bool NodeBase<N, Derived>::hasChildren() const
{
    return children_ != nullptr;
}

template <int N, typename Derived>
const typename NodeBase<N, Derived>::Children& NodeBase<N, Derived>::getChildren() const
{
    return children_;
}

template <int N, typename Derived>
ptrdiff_t NodeBase<N, Derived>::search(const Vec3& pos,
                                       EnsightBarycentricCoordinates& baryCoordOut,
                                       const CellContainer& cells) const
{
    if (!hasChildren())
    {
        // Note: the promotion from Index (unsigned 32 bit) to ptrdiff_t
        // (signed 64 bit) is intentional. The compiler may be able to generate
        // better code because it can assume that no overflow is allowed.
        for (ptrdiff_t idx : cells_)
        {
            auto cell = cells[idx];
            if (cell->contains(pos, baryCoordOut, Derived::ignore2dCells))
                return idx;
        }
        return -1;
    }

    auto self = static_cast<const Derived*>(this);
    return self->findChild(pos, bounds_.center())->search(pos, baryCoordOut, cells);
}

template <int N, typename Derived>
QList<ptrdiff_t> NodeBase<N, Derived>::searchAll(const Vec3& pos,
                                                 const CellContainer& cells) const
{
    if (!hasChildren())
    {
        QList<ptrdiff_t> results;
        for (ptrdiff_t idx : cells_)
        {
            Q_ASSERT(idx >= 0 && idx < static_cast<ptrdiff_t>(cells.size()));
            auto id = cells[idx];
            Q_ASSERT(id != nullptr);
            if (id->getBounds().contains(pos))
                results << idx;
        }
        return results;
    }

    auto self = static_cast<const Derived*>(this);
    return self->findChild(pos, bounds_.center())->searchAll(pos, cells);
}

template <int N, typename Derived>
void NodeBase<N, Derived>::insert(Index cellIdx, const CellContainer& cells,
                                   int maxLevel, int maxCells, int level)
{
    if ((cells_.size() < maxCells || level == maxLevel) && !hasChildren())
    {
        cells_.push_back(cellIdx);
    }
    else
    {
        // Split node into N subnodes and distribute the cells into these subnodes
        if (!hasChildren())
        {
            // allocate children and distribute existing cells
            allocateChildren();
            distributeCellsToChildren(cells, maxLevel, maxCells, level);
        }

        // Insert in a child cell
        auto id = cells[cellIdx];
        for (auto& child : *children_)
        {
            if (child.cellIntersects(id))
            {
                child.insert(cellIdx, cells, maxLevel, maxCells, level+1);
            }
        }
    }
}

template <int N, typename Derived>
Bbox NodeBase<N, Derived>::boundsForSubNode(int SubNode) const
{
    auto self = static_cast<const Derived*>(this);
    return self->boundsForSubNode(SubNode);
}

template <int N, typename Derived>
void NodeBase<N, Derived>::allocateChildren()
{
    assert(!hasChildren());
    children_.reset(new std::array<Derived, N>());
    for (int i = 0; i < N; i++)
    {
        (*children_)[i].setBounds(boundsForSubNode(i));
    }
}

template <int N, typename Derived>
void NodeBase<N, Derived>::distributeCellsToChildren(const CellContainer& cells,
                                                     int maxLevel, int maxCells,
                                                     int level)
{
    assert(hasChildren());
    for (auto& child : *children_)
    {
        // Insert existing cell identifiers
        for (auto cellIdx : cells_)
        {
            auto cell = cells[cellIdx];
            if (child.cellIntersects(cell))
                child.insert(cellIdx, cells, maxLevel, maxCells, level+1);
        }
    }
    cells_.clear();
}

template <int N, typename Derived>
bool NodeBase<N, Derived>::cellIntersects(EnsightCellIdentifier* c) const
{
    return c->getBounds().intersects(bounds_);
}


const QuadtreeNode* QuadtreeNode::findChild(const Vec3& pos, const Vec3& mid) const
{
    // Find child which contains pos
    const QuadtreeNode* child;
    if (pos[0] < mid[0])  // children 01
    {
        if (pos[1] < mid[1])
            child = &(*getChildren())[0];
        else
            child = &(*getChildren())[1];
    }
    else  // children 23
    {
        if (pos[1] < mid[1])
            child = &(*getChildren())[2];
        else
            child = &(*getChildren())[3];
    }
    return child;
}

Bbox QuadtreeNode::boundsForSubNode(int subNode) const
{
    // Determine bounds on information of parent bounds and quadrant number
    Vec3 min = getBounds().minCorner();
    Vec3 max = getBounds().maxCorner();
    Vec3 len = (max - min) / 2.;

    switch (subNode)
    {
    case 1: min[1] += len[1]; break;
    case 2: min[0] += len[0]; break;
    case 3: min    += Vec3(len[0], len[1], 0); break;
    }
    max = min + len;

    return Bbox(min, max);
}


const OctreeNode* OctreeNode::findChild(const Vec3& pos, const Vec3& mid) const
{
    // Find child which contains pos
    const OctreeNode* child;
    if (pos[0] < mid[0])  // children 0123
    {
        if (pos[1] < mid[1])  // children 01
        {
            if (pos[2] < mid[2])
                child = &(*getChildren())[0];
            else
                child = &(*getChildren())[1];
        }
        else  // children 23
        {
            if (pos[2] < mid[2])
                child = &(*getChildren())[2];
            else
                child = &(*getChildren())[3];
        }
    }
    else  // children 4567
    {
        if (pos[1] < mid[1])  // children 45
        {
            if (pos[2] < mid[2])
                child = &(*getChildren())[4];
            else
                child = &(*getChildren())[5];
        }
        else  // children 67
        {
            if (pos[2] < mid[2])
                child = &(*getChildren())[6];
            else
                child = &(*getChildren())[7];
        }
    }
    return child;
}

Bbox OctreeNode::boundsForSubNode(int SubNode) const
{
    // Determine bounds on information of parent bounds and octant number
    Vec3 min = getBounds().minCorner();
    Vec3 max = getBounds().maxCorner();
    Vec3 len = (max - min) / 2.;

    switch(SubNode)
    {
    case 1: min[2] += len[2]; break;
    case 2: min[1] += len[1]; break;
    case 3: min    += Vec3(0, len[1], len[2]); break;
    case 4: min[0] += len[0]; break;
    case 5: min    += Vec3(len[0], 0, len[2]); break;
    case 6: min    += Vec3(len[0], len[1], 0); break;
    case 7: min    += len; break;
    }
    max = min + len;

    return Bbox(min, max);
}

const bool OctreeNode::ignore2dCells;
const bool QuadtreeNode::ignore2dCells;

template class NodeBase<4, QuadtreeNode>;
template class NodeBase<8, OctreeNode>;
template class SubdivTreeImpl<QuadtreeNode>;
template class SubdivTreeImpl<OctreeNode>;
