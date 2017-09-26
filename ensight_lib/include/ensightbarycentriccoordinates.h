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


#ifndef ENSIGHTBARYCENTRICCOORDINATES_H
#define ENSIGHTBARYCENTRICCOORDINATES_H

#include <string>
#include <QString>
#include "eigentypes.h"

// Forward declaration
class EnsightCellIdentifier;

/**
 * @brief The EnsightBarycentricCoordinates class provides methods to compute barycentric coordinates in different cell types.
 *
 * Supported cell types are:  Ensight::Tetradhedron ,  Ensight::Pyramid ,  Ensight::Wedge ,  Ensight::Hexahedron.
 */
class EnsightBarycentricCoordinates
{
public:
    /**
     * @brief Creates a default barycentric coordinates object.
     */
    EnsightBarycentricCoordinates();
    /**
     * @brief Creates a barycentric coordinates object and calls compute to
     * initialize it.
     * @param[in] pos Position
     * @param[in] cell Cell Identifier
     */
    EnsightBarycentricCoordinates(const Vec3 &pos, const EnsightCellIdentifier* cell);

    /**
     * @brief Computes barycentric coordinates of position in cell. For 2D cells
     * the cell is assumed to lie in the Z=0 plane. If this is not the case,
     * computed coordinates may not be correct. Thus, for 3D geometry, only
     * volumetric cells can be correctly represented and 2D cells should be
     * ignored.
     * @param[in] pos Position
     * @param[in] cell Cell Identifier
     * @param[in] ignore2dCells ignore 2D cells. Use this for 3D geometry where only
     * volumetric cells should be considered as containing a point.
     */
    bool compute(const Vec3& pos, const EnsightCellIdentifier* cell,
                 bool ignore2dCells = true);


    /**
     * @brief Checks if object is a valid barycentric coordinate.
     */
    bool isValid() const;


    /**
     * @brief Barycentric interpolation of data
     * @param[in] name Field name to evaluate
     * @return Vector of length m interpolating the data
     */
    Vecx evaluate(const QString& name) const;
    Vecx evaluate(const std::string& name) const;


    /**
     * @brief Barycentric interpolation of data
     * @param data Mxn matrix where n is equal to the number of vertices of the part corresponding to the cell_
     * @return Vector of length m interpolating the data
     */
    Vecx evaluate(const Matx& data) const;


    /**
     * @brief Interpolation of time dependent data.
     *
     * Interpolate data at time tCurrent from given data at time t0 and t1
     * @param[in] localCoord0 Local barycentric coordinates at time t0
     * @param[in] localCoord1 Local barycentric coordinates at time t1
     * @param[in] t0 Time with t0 <= tCurrent
     * @param[in] t1 Time with t1 >= tCurrent
     * @param[in] tCurrent Current time
     * @param[in] name Field name to evaluate
     * @return Vector of length m interpolating the data
     */
    static Vecx evaluateUnsteady(const EnsightBarycentricCoordinates& localCoord0,
                                 const EnsightBarycentricCoordinates& localCoord1,
                                 double t0, double t1, double tCurrent,
                                 const QString& name);
    static Vecx evaluateUnsteady(const EnsightBarycentricCoordinates& localCoord0,
                                 const EnsightBarycentricCoordinates& localCoord1,
                                 double t0, double t1, double tCurrent,
                                 const std::string& name);

    /**
     * @brief Returns the computed barycentric coordinates
     */
    Vecx getCoords() const;

protected:

    /**
     * @brief Checks if the computed barycentric coordinates really interpolate the given position.
     * Merely for debugging, should generally be true, as long as computation was successful
     */
    bool isBarycentricInterpolation();

    /**
     * @brief Vector of barycentric coordinates
     */
    Vecx baryCoords_;


    /**
     * @brief Cell for coordinates
     */
    const EnsightCellIdentifier* cell_ = nullptr;

    /**
     * @brief Position for coordinates, i.e.
     * pos_ = sum baryCoords_(i) * vi, where vi are the cells vertices
     */
    Vec3 pos_;


private:
    /**
     * @brief compute barycentric coordinates for tetra cell.
     * @param[in] pos position of interpolation point
     * @param[in] cell
     * @param[in] iLocalSubNodes ordered local node indices of sub cell with values from [0...n-1], where n is
     * the number of vertices of the of the original cell.
     * Example:
     * For an unsplit hexa cell:
     * iLocalSubNodes = 0, 1, 2, 3, 4, 5, 6, 7
     * For a hexa cell split into wedges:
     * iLocalSubNodes = 3, 1, 2, 7, 5, 6 (one wedge)
     * iLocalSubNodes = 0, 1, 3, 4, 5, 7 (other wedge)
     */
    void computeTetraCell(const Vec3& pos, const EnsightCellIdentifier* cell,
                          const Veci& iLocalSubNodes);

    /**
     * @brief compute barycentric coordinates for pyramid cell.
     * @param[in] pos position of interpolation point
     * @param[in] cell
     * @param[in] iLocalSubNodes ordered local node indices of sub cell with values from [0...n-1], where n is
     * the number of vertices of the of the original cell.
     */
    void computePyramidCell(const Vec3& pos, const EnsightCellIdentifier* cell,
                            const Veci& iLocalSubNodes);

    /**
     * @brief compute barycentric coordinates for wedge cell.
     * @param[in] pos position of interpolation point
     * @param[in] cell
     * @param[in] iLocalSubNodes ordered local node indices of sub cell with values from [0...n-1], where n is
     * the number of vertices of the of the original cell.
     */
    void computeWedgeCell(const Vec3& pos, const EnsightCellIdentifier* cell,
                          const Veci& iLocalSubNodes);

    /**
     * @brief compute barycentric coordinates for hexa cell.
     * @param[in] pos position of interpolation point
     * @param[in] cell
     * @param[in] iLocalSubNodes ordered local node indices of sub cell with values from [0...n-1], where n is
     * the number of vertices of the of the original cell.
     */
    void computeHexaCell(const Vec3& pos, const EnsightCellIdentifier* cell,
                         const Veci& iLocalSubNodes);

    /**
     * @brief compute barycentric coordinates for triangle cell.
     * @param[in] pos position of interpolation point
     * @param[in] cell
     * @param[in] iLocalSubNodes ordered local node indices of sub cell with values from [0...n-1], where n is
     * the number of vertices of the of the original cell.
     */
    void computeTriCell(const Vec3& pos, const EnsightCellIdentifier* cell,
                        const Veci& iLocalSubNodes);

    /**
     * @brief compute barycentric coordinates for quadrangle cell.
     * @param[in] pos position of interpolation point
     * @param[in] cell
     * @param[in] iLocalSubNodes ordered local node indices of sub cell with values from [0...n-1], where n is
     * the number of vertices of the of the original cell.
     */
    void computeQuadCell(const Vec3& pos, const EnsightCellIdentifier* cell,
                         const Veci& iLocalSubNodes);

    /**
     * @brief check if pos is above split plane
     * @param[in] pos position of interpolation point
     * @param[in] iNodeSideSpecifier local index to a node which lies on the specified side.
     * This is necessary because in cells can be inverted due to symmetry flipping.
     * @param[in] cell
     * @param[in] iSplitNodes nodes of the split plane
     */
    bool isOnSameSideOfSplitPlane(const Vec3& pos, int iNodeSideSpecifier,
                                  const EnsightCellIdentifier* cell,
                                  const Vec3i& iSplitNodes);

    bool isOnSameSideOfSplitLine(const Vec3& pos, int iNodeSideSpecifier,
                                 const EnsightCellIdentifier* cell,
                                 const Vec2i& iSplitNodes);

    /**
     * @brief compute if point lies inside cell, i.e. all barycentric coordinates are positive.
     */
    bool computeIsInside();

private:
    /**
     * @brief isComputationSuccessful_ flag indicating that barycentric coordinates where computed successfully
     * otherwise evaluation of cell is not possible
     */
    bool isComputationSuccessful_ = false;

    /**
     * @brief numeric tolerance for inside cell check.
     * To guarantee that every point finds a cell this tolerance should not be too small.
     */
    const double insideCellTolerance_ = 5e-2;
};

#endif // ENSIGHTBARYCENTRICCOORDINATES_H
