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


#ifndef ENSIGHTCELL_H
#define ENSIGHTCELL_H

#include <string>

#include <QPair>
#include <QString>

#include "bbox.h"
#include "eigentypes.h"
#include "ensightdef.h"


// Forward declarations
class EnsightPart;
class EnsightBarycentricCoordinates;

/**
 * @brief The EnsightCellList class represents a list of cells of a certain cell type.
 *
 * The class provides methods to get specific information about its elements.
 */
class EnsightCellList
{
public:
    /**
     * @brief Creates an EnsightCellList based on the given parameters.
     *
     * This method will also create a matrix which contains the cell neighbor indices for each cell.
     * Additionally, there will be a matrix which contains all boundary faces.
     *
     * For additional information on Cell Types have a look at Ensight::Cell.
     *
     * @param type Cell type
     * @param values MxN matrix, with N cells each consisting of M vertices
     * @param partName Parent part name
     */
    EnsightCellList(Ensight::Cell type, const Mati& values, QString partName);
    EnsightCellList(Ensight::Cell type, const Mati& values, std::string partName);

    /**
     * @brief Get cell type of the elements in the CellList.
     */
    Ensight::Cell getType() const;

    /**
     * @brief Get the matrix which contains all cell vertices.
     */
    const Mati& getValues() const;

    /**
     * @brief Get the matrix representing neighborhood information between the cells.
     *
     * For N cells this is a MxN matrix, where M is the number of faces per cell.
     * In neighbors(i,j) an index is stored that references the cell which is neighbor face i of cell j or -1 if
     * this is a boundary cell.
     */
    const Mati& getNeighbors() const;

    /**
     * @brief Get the matrix which contains information about the boundary faces.
     *
     * It returns a 2xK matrix where K is the number of boundary faces of the boundary cells.
     * In each column, the first entry stores the cell index, the second one stores the boundary face of this cell.
     */
    const Mati& getBoundary() const;

    /**
     * @brief Get the defined parent part name.
     */
    const QString& getPartName() const;

    /**
     * @brief Get a 1xL matrix with L indices to boundary vertices.
     */
    const Mati& getBoundaryVertices() const;

private:
    bool doFacesMatch(const QPair<int, int>& oldFace, const QPair<int, int>& newFace);

    int getFaceHash(const Ensight::Cell& type, const QPair<int, int>& face);

public:
    Matx normals;
    Matx side_normals;

private:
    /**
     * @brief partName Name of the parent part
     */
    QString partName_;

    /**
     * @brief type Type of the cells in this list
     */
    Ensight::Cell type_;

    /**
     * @brief values  All cells in this list, represented as MxN matrix, with N cells each consisting of M vertices
     */
    Mati values_;

    /**
     * @brief Matrix representing neighborhood information between the cells. For N cells this is
     * a MxN matrix, where M is the number of faces per cell.
     * In neighbors(i,j) an index is stored that references the cell which is neighbor face i of cell j or -1 if
     * this is a boundary cell
     */
    Mati neighbors_;

    /**
     * @brief This is a 2xK matrix where K is the number of boundary faces of these cells.
     * Each column stores first the index of the boundary cell and then the index of the boundary face of this cell
     */
    Mati boundary_;


    /**
     * @brief 1xL matrix with L indices to boundary vertices.
     */
    Mati boundaryVertices_;

};


/**
 * @brief The EnsightCellIdentifier class represents one cell out of a EnsightCellList to a given timestep.
 *
 * It provides methods to get further information about the given cell.
 */
class EnsightCellIdentifier
{
public:
    /**
     * @brief Creates an EnsightCellIdentifier based on the given parameters.
     * @param part Part where this cell originates from
     * @param cellList Cell list which contains the cell
     * @param timestep Timestep of the cell
     * @param index Cell list index of the cell
     * @param bounds Bounds of the represented cell
     */
    EnsightCellIdentifier(EnsightPart* part, EnsightCellList* cellList, int timestep,
                          int index, const Bbox& bounds);

    /**
     * @brief Get the cell indices.
     */
    Veci getCell() const;
    /**
     * @brief Get the part vertices from given ID.
     */
    Vec3 getVertex(int vertexId) const;
    /**
     * @brief Get the part values of a variable.
     * @param name Variable name
     */
    const Matx& getValues(const QString& name) const;
    const Matx& getValues(const std::string& name) const;

    /**
     * @brief Get the cell type.
     */
    Ensight::Cell getType() const;

    /**
     * @brief Get the EnsightCellList which contains the cell.
     */
    const EnsightCellList* getCellList() const;
    /**
     * @brief Get the cell boundaries.
     */
    const Bbox& getBounds() const;

    /**
     * @brief Get cell index in its Cell list.
     */
    int getIndex() const;

    void print() const;

    /**
     * @brief Checks if pos is contained in current cell, by computing barycentric coordinates.
     */
    bool contains(const Vec3 &pos, bool ignore2dCells = true) const;

    /**
     * @brief Checks if pos is contained in current cell, by computing barycentric coordinates.
     * @param pos Position
     * @param baryCoordOut Barycentric coordinates can be retrieved here.
     * @param ignore2dCells ignore 2D cells. Use this for 3D geometry where only
     * volumetric cells should be considered as containing a point.
     */
    bool contains(const Vec3& pos, EnsightBarycentricCoordinates& baryCoordOut,
                  bool ignore2dCells = true) const;

    /**
     * @brief Computes the volume spanned by the cells' convex hull.
     **/
    double computeVolume() const;

private:
    /**
     * @brief part_ part where this cell originates from
     */
    EnsightPart* part_;

    /**
     * @brief cellList This CellIdentifier represents one cell of the cellList
     */
    EnsightCellList* cellList_;

    /**
     * @brief timestep_ timestep of the cell
     */
    int timestep_;

    /**
     * @brief index The index of the represented cell in cellList
     */
    int index_;

    /**
     * @brief bounds The geometric bounds of the represented cell
     */
    Bbox bounds_;
};


#endif // ENSIGHTCELL_H
