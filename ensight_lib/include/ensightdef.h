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


#ifndef ENSIGHTDEF_H
#define ENSIGHTDEF_H

/**
 * The namespace Ensight summarizes the structural key words of the EnSight Gold format.
 */
namespace Ensight {

/**
 * @brief The Section enum contains the five sections used in the EnSight Gold case format:
 * (FORMAT, GEOMETRY, VARIABLE, TIME, FILE).
 */
enum Section {SectionUnknown=-1, Format=0, Geometry=1, Variable=2, Time=3, File=4};
extern const char *strSections[];
const int numSections = 5;

/**
 * @brief The Cell enum addresses the section GEOMETRY and contains all supported EnSight Gold Elements
 * (for details see "EnSight 7 User Manual" page 11-4).
 */
enum Cell{Unknown=-1, Point=0, Bar=1, Triangle=2, Quadrangle=3, Tetradhedron=4, Pyramid=5, Wedge=6, Hexahedron=7};
extern const char *strCell[];
extern const int numCellNodes[];
extern const int numCellFaces[];
const int maxNumNodesPerFace = 4;
extern const int cellFaces[8][6][4];
const int numCellTypes = 8;

/**
 * @brief The VarTypes enum addresses the section VARIABLE and contains the supported variable type:
 * - constants per case,
 * - scalars per node,
 * - vectors per node.
 *
 * Not supported variables types are, e.g., tensors, complex scalars and vectors, and variables per element.
 */
enum VarTypes {VarTypeUnknown=-1, ConstantPerCase=0, ScalarPerNode=1, VectorPerNode=2};
extern const char *strVarTypes[];
extern const int varTypeDims[];
const int numVarTypes = 3;

/**
 * @brief strTimeSetDef addresses the section TIME.
 */
extern const char *strTimeSetDef[];
const int numTimeSetDef = 5;

} // namespace Ensight

#endif // ENSIGHTDEF_H
