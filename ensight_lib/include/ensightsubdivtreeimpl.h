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


#ifndef SUBDIVTREEIMPL_H
#define SUBDIVTREEIMPL_H

#include <array>
#include <memory>
#include <QList>
#include <QVector>

#include "bbox.h"
#include "ensightsubdivtree.h"

class EnsightCellIdentifier;
class EnsightBarycentricCoordinates;
class EnsightPart;


// **** template class SubdivTreeImpl ****
template <typename Node> class SubdivTreeImpl : public EnsightSubdivTree
{
public:
    SubdivTreeImpl(Bbox bounds, int maxLevel, int maxCells);
    ~SubdivTreeImpl();

    EnsightCellIdentifier* search(const Vec3& pos,
                                  EnsightBarycentricCoordinates& baryCoordOut) const override;

    QList<EnsightCellIdentifier*> searchAll(const Vec3& pos) const override;

    void insert(EnsightPart* part, int timestep, double sizeOffset = 0.0) override;

    Bbox getBounds() const override;
    const Node& getRoot() const;

private:
    using Index = uint32_t;
    using IndexContainer = QVector<Index>;
    using CellContainer = QVector<EnsightCellIdentifier*>;
    //using Index = typename Node::Index;
    //using IndexContainer = typename Node::IndexContainer;
    //using CellContainer = typename Node::CellContainer;

    std::unique_ptr<Node> root_;
    CellContainer allCells_;
    int maxLevel_;
    int maxCells_;

};



// **** template class NodeBase ****
template <int N, typename Derived> class NodeBase
{
protected:
    using Index = uint32_t;
    using IndexContainer = QVector<Index>;
    using CellContainer = QVector<EnsightCellIdentifier*>;

public:

    NodeBase();
    NodeBase(Bbox bounds);

    const Bbox& getBounds() const;
    void setBounds(Bbox bounds);

    using Children = std::unique_ptr<std::array<Derived, N>>;
    bool hasChildren() const;
    const Children& getChildren() const;

    void insert(Index cellIdx, const CellContainer& cells, int maxLevel,
                int maxCells, int level);

    ptrdiff_t search(const Vec3& pos, EnsightBarycentricCoordinates &baryCoordOut,
                     const CellContainer& cells) const;
    QList<ptrdiff_t> searchAll(const Vec3& pos,
                               const CellContainer& cells) const;

private:
    Bbox boundsForSubNode(int SubNode) const;
    void allocateChildren();
    void distributeCellsToChildren(const CellContainer& cells, int maxLevel,
                                   int maxCells, int level);
    bool cellIntersects(EnsightCellIdentifier* c) const;


    Children children_;
    IndexContainer cells_;
    Bbox bounds_;
};

// **** Actual Quad-/OctreeNode classes
class QuadtreeNode : public NodeBase<4, QuadtreeNode>
{
public:
    QuadtreeNode() = default;
    QuadtreeNode(Bbox bounds) : NodeBase<4, QuadtreeNode>(bounds) {}
    const QuadtreeNode* findChild(const Vec3 &pos, const Vec3 &mid) const;
    Bbox boundsForSubNode(int subNode) const;

    static const bool ignore2dCells = false;
};

class OctreeNode : public NodeBase<8, OctreeNode>
{
public:
    OctreeNode() = default;
    OctreeNode(Bbox bounds) : NodeBase<8, OctreeNode>(bounds) {}
    const OctreeNode* findChild(const Vec3 &pos, const Vec3 &mid) const;
    Bbox boundsForSubNode(int SubNode) const;

    static const bool ignore2dCells = true;
};

#endif // SUBDIVTREEIMPL_H
