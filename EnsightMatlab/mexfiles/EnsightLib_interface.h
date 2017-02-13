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

#ifndef ENSIGHT_MATLAB_INTERFACE
#define ENSIGHT_MATLAB_INTERFACE

#ifdef MEX_INCLUDE
#include "ensightlib.h"
#include "ensightobj.h"
#include "ensightpart.h"
#include "ensightsubdivtree.h"
#include "ensightcell.h"
#include "ensightconstant.h"
#include "ensightvariable.h"
#include "ensightbarycentriccoordinates.h"

#include <Eigen/Dense>

#include "qstring.h"
#else

#include "../../../libs/ensight_lib/include/ensightlib.h"
#include "../../../libs/ensight_lib/include/ensightobj.h"
#include "../../../libs/ensight_lib/include/ensightpart.h"
#include "../../../libs/ensight_lib/include/ensightsubdivtree.h"
#include "../../../libs/ensight_lib/include/ensightcell.h"
#include "../../../libs/ensight_lib/include/ensightconstant.h"
#include "../../../libs/ensight_lib/include/ensightvariable.h"
#include "../../../libs/ensight_lib/include/ensightbarycentriccoordinates.h"
#include <QString>

#include "/m/soft/Mathworks/matlab/R2015b/extern/include/mex.h"
#include "../../../libs/3dparty/eigen_v3_2_1/Eigen/Eigen";
#endif


#include "class_handle.hpp"
#include <string>
#include <exception>
#include <math.h>

namespace EnsightMatlab
{
/* Matlab pointer conversion */
EnsightObj*  getEnsightObject ( const mxArray* prhs[] );
EnsightPart* getEnsightPart   ( EnsightObj* object, const mxArray* prhs[] );

/* EnsightLib interface methods */
void createEnsight ( mxArray* plhs[] );
void deleteEnsight ( const mxArray* prhs[] );
void readEnsight   ( const mxArray* prhs[], mxArray* plhs[] );

/* EnsightObject class methods */
void writeEnsight            ( EnsightObj* object, const mxArray* prhs[] );
void beginEdit               ( EnsightObj* object );
void endEdit                 ( EnsightObj* object );
void createEnsightPart       ( EnsightObj* object, const mxArray *prhs[] );
void cleanObject             ( EnsightObj* object, const mxArray *prhs[]);
void setStatic               ( EnsightObj* object );
void setTransient            ( EnsightObj* object, const mxArray* prhs[] );
void getObjectGeometryBounds ( EnsightObj* object, const mxArray* prhs[], mxArray* plhs[] );
void createVariable          ( EnsightObj* object, const mxArray* prhs[] );
void addConstant             ( EnsightObj* object, const mxArray* prhs[] );
void replaceConstants        ( EnsightObj* object, const mxArray* prhs[] );
void getVariableBounds       ( EnsightObj* object, const mxArray* prhs[], mxArray* plhs[] );

/* EnsightPart class methods */
void cleanPart               ( EnsightPart* part, const mxArray* prhs[]);
void getPartGeometryBounds   ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void setVerticesOfPart       ( EnsightPart* part, const mxArray* prhs[] );
void getVerticesOfPart       ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void setCellsOfPart          ( EnsightPart* part, const mxArray* prhs[] );
void getCellValuesOfPart     ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void getCellBoundsOfPart     ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void getVertexCountOfPart    ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void setVariableForPart      ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void getVariableBoundsOfPart ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void getVariableValuesOfPart ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void partHasCelltype         ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void partHasVariable         ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );
void getCellListOfPart       ( EnsightPart* part, const mxArray* prhs[], mxArray* plhs[] );

/* Interpolation and search method */
void createSubdivTree    ( EnsightObj* object, const mxArray* prhs[]);
void interpolate         ( EnsightObj* object, const mxArray* prhs[], mxArray* plhs[] );
void findCell            ( EnsightObj* object, const mxArray* prhs[], mxArray* plhs[] );
void getSubdivTreeBounds ( EnsightObj* object, mxArray* plhs[] );
}

namespace MexTools
{


// Internal code used by the typecheck parser
enum Type { TNumeric  = 0,
            TString   = 1,
            TBool     = 2,
            TStruct   = 3,
            TCell     = 4,
            TAbstract = 5
          };

/* Represents the plhs*[] memory offset level
 *  0: EnsightLib interface methods (read,create,delete)
 *  1: Requires EnsightObj* pointer
 *  2: Also requires EnsightPart* pointer
*/
enum MethodType { EInterface = 0,
                  EObject    = 1,
                  EPart      = 2
                };

void mapCommand      ( const mxArray* prhs[], int nrhs, char* command, int cmdSize, int &mID );
void checkInputParam ( const mxArray* prhs[], int nrhs, int nlhs );

const Mati getIntegerMatrix ( const mxArray* prhs[], int slot );
const Matx getDoubleMatrix  ( const mxArray* prhs[], int slot );
const Vecx getDoubleVector  ( const mxArray* prhs[], int slot );
const Veci getIntegerVector ( const mxArray* prhs[], int slot );
const double getDoubleScalar( const mxArray* prhs[], int slot );
const int  getIntegerScalar ( const mxArray* prhs[], int slot );

void mexAllocateAndCopyMatrix ( const Eigen::MatrixXd& Matrix, mxArray *plhs[] , int slot );
void mexAllocateAndCopyMatrix ( const Eigen::MatrixXi& Matrix, mxArray *plhs[] , int slot );
void mexAllocateAndCopyMatrix ( const Eigen::VectorXd& Vector, mxArray *plhs[] , int slot );
void mexAllocateAndCopyMatrix ( const Eigen::VectorXi& Vector, mxArray *plhs[] , int slot );

}

#endif
