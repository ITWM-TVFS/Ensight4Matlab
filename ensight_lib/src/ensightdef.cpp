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


#include "../include/ensightdef.h"


namespace Ensight
{

// sections of the EnSight Gold case format
const char* strSections[] = {"FORMAT", "GEOMETRY", "VARIABLE", "TIME", "FILE"};

// supported EnSight Gold Elements
const char* strCell[] = {"point",  "bar2",     "tria3",  "quad4",
                         "tetra4", "pyramid5", "penta6", "hexa8"};
const int numCellNodes[] = {1, 2, 3, 4, 4, 5, 6, 8,};
const int numCellFaces[] = {0, 0, 3, 4, 4, 5, 5, 6,};

// Ordering of vertices for each face must be counter-clockwise when looking
// into the direction of the face normal vector.
// For cell elements the normal is assumed to point outside.
const int cellFaces[8][6][4] = {{ {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1} }, //Point
                                { {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1} }, //Bar
                                { { 0, 1,-1,-1}, { 1, 2,-1,-1}, { 2, 0,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1} }, //Triangle
                                { { 0, 1,-1,-1}, { 1, 2,-1,-1}, { 2, 3,-1,-1}, { 3, 0,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1} }, //Quadrangle
                                { { 0, 1, 2,-1}, { 0, 3, 1,-1}, { 2, 3, 0,-1}, { 1, 3, 2,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1} }, //Tetrahedron
                                { { 0, 1, 2, 3}, { 0, 4, 1,-1}, { 1, 4, 2,-1}, { 2, 4, 3,-1}, { 3, 4, 0,-1}, {-1,-1,-1,-1} }, //Pyramid
                                { { 0, 1, 2,-1}, { 3, 5, 4,-1}, { 0, 4, 3, 2}, { 4, 5, 2, 1}, { 5, 3, 0, 2}, {-1,-1,-1,-1} }, //Wedge
                                { { 0, 1, 2, 3}, { 4, 7, 6, 5}, { 0, 4, 5, 1}, { 6, 7, 3, 2}, { 5, 6, 2, 1}, { 7, 4, 0, 3} }};//Hex

// supported variable types
const char* strVarTypes[] = {"constant per case", "scalar per node",
                             "vector per node"};
const int varTypeDims[] = {1, 1, 3};

// supported time formats
const char* strTimeSetDef[] = {"time set", "number of steps",
                               "filename start number", "filename increment",
                               "time values"};

} // namespace Ensight
