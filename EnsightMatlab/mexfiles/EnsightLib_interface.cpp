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


#include "EnsightLib_interface.h"

#include <QStringList>

// ********** EnSight-MATLAB interface function ****//
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{

    try
    {
        if (nrhs == 0)
            throw std::runtime_error("Expected at least one argument, got none.");

        // Load command string
        char command[64];
        MexTools::MethodType methodType;
        int mID;

        MexTools::mapCommand(prhs, nrhs, command, sizeof(command), mID);

        methodType = MexTools::MethodType(mID);
        // Verify datatypes given by MATLAB
        MexTools::checkInputParam(prhs, nrhs, nlhs);

        if (methodType == MexTools::EInterface)
        {
            if (strcmp(command, "create") == 0)
                EnsightMatlab::createEnsight(plhs);
            else if (strcmp(command, "delete") == 0)
                EnsightMatlab::deleteEnsight(prhs);
            else if (strcmp(command, "read") == 0)
                EnsightMatlab::readEnsight(prhs, plhs);
            else
                throw std::runtime_error("Unknown method.");
        }
        else if (methodType == MexTools::EObject)
        {
            EnsightObj* object = EnsightMatlab::getEnsightObject(prhs);
            if (strcmp(command, "write") == 0)
                EnsightMatlab::writeEnsight(object, prhs);
            else if (strcmp(command, "beginEdit") == 0)
                EnsightMatlab::beginEdit(object);
            else if (strcmp(command, "endEdit") == 0)
                EnsightMatlab::endEdit(object);
            else if (strcmp(command, "createPart") == 0)
                EnsightMatlab::createEnsightPart(object, prhs);
            else if (strcmp(command, "clean") == 0)
                EnsightMatlab::cleanObject(object, prhs);
            else if (strcmp(command, "setStatic") == 0)
                EnsightMatlab::setStatic(object);
            else if (strcmp(command, "setTransient") == 0)
                EnsightMatlab::setTransient(object, prhs);
            else if (strcmp(command, "getGeometryBounds") == 0)
                EnsightMatlab::getObjectGeometryBounds(object, prhs, plhs);
            else if (strcmp(command, "createVariable") == 0)
                EnsightMatlab::createVariable(object, prhs);
            else if (strcmp(command, "addConstant") == 0)
                EnsightMatlab::addConstant(object, prhs);
            else if (strcmp(command, "replaceConstants") == 0)
                EnsightMatlab::replaceConstants(object, prhs);
            else if (strcmp(command, "getVariableBounds") == 0)
                EnsightMatlab::getVariableBounds(object, prhs, plhs);
            else if (strcmp(command, "createSubdivTree") == 0)
                EnsightMatlab::createSubdivTree(object, prhs);
            else if (strcmp(command, "interpolate") == 0)
                EnsightMatlab::interpolate(object, prhs, plhs);
            else if (strcmp(command, "findCell") == 0)
                EnsightMatlab::findCell(object, prhs, plhs);
            else if (strcmp(command, "getSubdivTreeBounds") == 0)
                EnsightMatlab::getSubdivTreeBounds(object, plhs);
            else
                throw std::runtime_error("Unknown method for EnsightObj.");
        }
        else if (methodType == MexTools::EPart)
        {
            EnsightObj* object = EnsightMatlab::getEnsightObject(prhs);
            EnsightPart* part = EnsightMatlab::getEnsightPart(object, prhs);

            if (strcmp(command, "clean") == 0)
                EnsightMatlab::cleanPart(part, prhs);
            else if (strcmp(command, "getGeometryBounds") == 0)
                EnsightMatlab::getPartGeometryBounds(part, prhs, plhs);
            else if (strcmp(command, "setVertices") == 0)
                EnsightMatlab::setVerticesOfPart(part, prhs);
            else if (strcmp(command, "getVertices") == 0)
                EnsightMatlab::getVerticesOfPart(part, prhs, plhs);
            else if (strcmp(command, "setCells") == 0)
                EnsightMatlab::setCellsOfPart(part, prhs);
            else if (strcmp(command, "getCellValues") == 0)
                EnsightMatlab::getCellValuesOfPart(part, prhs, plhs);
            else if (strcmp(command, "getCellBounds") == 0)
                EnsightMatlab::getCellBoundsOfPart(part, prhs, plhs);
            else if (strcmp(command, "getVertexCount") == 0)
                EnsightMatlab::getVertexCountOfPart(part, prhs, plhs);
            else if (strcmp(command, "setVariable") == 0)
                EnsightMatlab::setVariableForPart(part, prhs, plhs);
            else if (strcmp(command, "getVariableBounds") == 0)
                EnsightMatlab::getVariableBoundsOfPart(part, prhs, plhs);
            else if (strcmp(command, "getVariableValues") == 0)
                EnsightMatlab::getVariableValuesOfPart(part, prhs, plhs);
            else if (strcmp(command, "hasCellType") == 0)
                EnsightMatlab::partHasCelltype(part, prhs, plhs);
            else if (strcmp(command, "hasVariable") == 0)
                EnsightMatlab::partHasVariable(part, prhs, plhs);
            else if (strcmp(command, "getCellList") == 0)
                EnsightMatlab::getCellListOfPart(part, prhs, plhs);
            else
                throw std::runtime_error("Unknown method for EnsightPart.");
        }
    }
    catch (const std::runtime_error& e)
    {
        mexErrMsgIdAndTxt("EnsightMatlab:interfaceException", e.what());
    }
    catch (const std::bad_alloc& e)
    {
        mexErrMsgIdAndTxt("EnsightMatlab:badAllocException", e.what());
    }
    catch (const std::exception& e)
    {
        mexErrMsgIdAndTxt("EnsightMatlab:exception", e.what());
    }
    catch (...)
    {
        mexErrMsgIdAndTxt("EnsightMatlab:unknownException", "An unknown error occured.");
    }

    return;
}


// ********** MEX helper functions ****//
void MexTools::mapCommand(const mxArray* prhs[], int nrhs, char* command, int cmdSize, int& mID)
{
    if (nrhs < 2)
        throw std::runtime_error("Expected at least two arguments. ");

    char level[64];
    if (mxGetString(prhs[0], level, sizeof(level)))
        throw std::runtime_error("Input[0]: Could not read command string.");

    if (mxGetString(prhs[1], command, cmdSize))
        throw std::runtime_error("Input[1]: Could not read command string.");

    if (strcmp(level, "obj") == 0)
        mID = 1;
    else if (strcmp(level, "part") == 0)
        mID = 2;
    else if (strcmp(level, "interface") == 0)
        mID = 0;
    else
        throw std::runtime_error("Invalid method type identifier.");
}

void MexTools::checkInputParam(const mxArray* prhs[], int nrhs, int nlhs)
{
    /*
     * EnsightLib_interface function call Layout
     * prhs[0]            : (string) method level
     * prhs[1]            : (string) method name
     * prhs[2]            : (ptr) EnsightObj pointer
     * prhs[3]            : (numeric) [nArguments, ArgTypes ... ]
     * prhs[4]            :   ... first parameter
     * prhs[3+nArguments] :   ... last parameter
     */
    if (nrhs < 3)
        throw std::runtime_error("Expected at least three arguments. ");

    double* typeList = mxGetPr(prhs[2]);
    const mwSize* dim = mxGetDimensions(prhs[2]);
    int n = std::max(static_cast<int>(dim[0]), static_cast<int>(dim[1]));

    int nIn = static_cast<int>(typeList[0]);
    int nOut = static_cast<int>(typeList[1]);
    if (nrhs < nIn + 3)
    {
        char err[64];
        sprintf(err, "Invalid number of input arguments. Got: %d , Expected: %d", nrhs - 3, nIn);
        throw std::runtime_error(err);
    }
    if (nlhs != nOut)
    {
        char err[64];
        sprintf(err, "Invalid number of output arguments. Got: %d , Expected: %d", nlhs, nOut);
        throw std::runtime_error(err);
    }
    if (n < nIn + 2)
        throw std::runtime_error("Invalid valueTypeArray.");

    MexTools::Type type;
    for (int i = 0; i < nIn; i++)
    {
        if (((int)typeList[i + 2] < 0) || ((int)typeList[i + 2] > 5))
        {
            char err[64];
            sprintf(err, "Input[%d]: Invalid type identifier. ID:%d", i, (int)typeList[i + 2]);
            throw std::runtime_error(err);
        }

        type = MexTools::Type(static_cast<int>(typeList[i + 2]));
        if (type == MexTools::TAbstract)
        {
        }
        else if ((type == MexTools::TNumeric) && (!mxIsNumeric(prhs[i + 3])))
        {
            char err[64];
            sprintf(err, "Input[%d]: Numeric value expected.", i);
            throw std::runtime_error(err);
        }
        else if ((type == MexTools::TStruct) && (!mxIsStruct(prhs[i + 3])))
        {
            char err[64];
            sprintf(err, "Input[%d]: Struct expected.", i);
            throw std::runtime_error(err);
        }
        else if ((type == MexTools::TCell) && (!mxIsCell(prhs[i + 3])))
        {
            char err[64];
            sprintf(err, "Input[%d]: Cell array expected.", i);
            throw std::runtime_error(err);
        }
        else if ((type == MexTools::TString) && (!mxIsChar(prhs[i + 3])))
        {
            char err[64];
            sprintf(err, "Input[%d]: String expected.", i);
            throw std::runtime_error(err);
        }
        else if ((type == MexTools::TBool) && (!mxIsLogical(prhs[i + 3])))
        {
            char err[64];
            sprintf(err, "Input[%d]: Boolean value expected.", i);
            throw std::runtime_error(err);
        }
    }
}

const Mati MexTools::getIntegerMatrix(const mxArray* prhs[], int slot)
{
    const mwSize* dim = mxGetDimensions(prhs[slot]);
    double* input = mxGetPr(prhs[slot]);
    int m = static_cast<int>(dim[0]);
    int n = static_cast<int>(dim[1]);

    Eigen::MatrixXi Matrix(m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            Matrix(i, j) = static_cast<int>(input[i + m * j]);

    return Matrix;
}

const Matx MexTools::getDoubleMatrix(const mxArray* prhs[], int slot)
{
    const mwSize* dim = mxGetDimensions(prhs[slot]);
    double* input = mxGetPr(prhs[slot]);
    int m = static_cast<int>(dim[0]);
    int n = static_cast<int>(dim[1]);

    //auto mat = std::allocate_shared<Matx>( Matx(m,n) );

    Matx matrix(m,n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            matrix(i,j) = static_cast<double>(input[i + j * m]);

    return matrix;
}

const Vecx MexTools::getDoubleVector(const mxArray* prhs[], int slot)
{
    const mwSize* dim = mxGetDimensions(prhs[slot]);
    double* input = mxGetPr(prhs[slot]);
    int m = static_cast<int>(dim[0]);
    int n = static_cast<int>(dim[1]);

    if (m > 1 && n > 1)
        throw std::runtime_error("Invalid dimensions.");

    int N = std::max(m, n);
    Eigen::VectorXd Vector(N);
    for (int i = 0; i < N; i++)
        Vector(i) = static_cast<double>(input[i]);

    return Vector;
}

const Veci MexTools::getIntegerVector(const mxArray* prhs[], int slot)
{
    const mwSize* dim = mxGetDimensions(prhs[slot]);
    double* input = mxGetPr(prhs[slot]);
    int m = static_cast<int>(dim[0]);
    int n = static_cast<int>(dim[1]);

    if (m > 1 && n > 1)
        throw std::runtime_error("Invalid dimensions.");

    int N = std::max(m, n);
    Eigen::VectorXi Vector(N);
    for (int i = 0; i < N; i++)
        Vector(i) = static_cast<int>(input[i]);

    return Vector;
}

const double MexTools::getDoubleScalar(const mxArray* prhs[], int slot)
{
    const mwSize* dim = mxGetDimensions(prhs[slot]);
    int m = static_cast<int>(dim[0]);
    int n = static_cast<int>(dim[1]);

    if (m > 1 || n > 1)
        throw std::runtime_error("Expected scalar, but got Vector.");

    double scalar = static_cast<double>(mxGetPr(prhs[slot])[0]);
    return scalar;
}

const int MexTools::getIntegerScalar(const mxArray* prhs[], int slot)
{
    const mwSize* dim = mxGetDimensions(prhs[slot]);
    int m = static_cast<int>(dim[0]);
    int n = static_cast<int>(dim[1]);

    if (m > 1 || n > 1)
        throw std::runtime_error("Expected scalar, but got Vector.");

    int scalar = static_cast<int>(mxGetPr(prhs[slot])[0]);
    return scalar;
}

void MexTools::mexAllocateAndCopyMatrix(const Eigen::MatrixXd& Matrix, mxArray* plhs[], int slot)
{
    int m = Matrix.rows();
    int n = Matrix.cols();

    plhs[slot] = mxCreateDoubleMatrix(m, n, mxREAL);
    double* output = mxGetPr(plhs[slot]);

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            output[i + j * m] = Matrix(i, j);
}

void MexTools::mexAllocateAndCopyMatrix(const Eigen::MatrixXi& Matrix, mxArray* plhs[], int slot)
{
    int m = Matrix.rows();
    int n = Matrix.cols();

    plhs[slot] = mxCreateDoubleMatrix(m, n, mxREAL);
    double* output = mxGetPr(plhs[slot]);

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            output[i + j * m] = Matrix(i, j);
}

void MexTools::mexAllocateAndCopyMatrix(const Eigen::VectorXd& Vector, mxArray* plhs[], int slot)
{

    int n = Vector.cols();

    plhs[slot] = mxCreateDoubleMatrix(n, 1, mxREAL);

    double* output = mxGetPr(plhs[slot]);

    for (int i = 0; i < n; i++)
        output[i] = static_cast<double>(Vector(i));
}

void MexTools::mexAllocateAndCopyMatrix(const Eigen::VectorXi& Vector, mxArray* plhs[], int slot)
{
    int n = Vector.cols();

    plhs[slot] = mxCreateDoubleMatrix(n, 1, mxREAL);
    double* output = mxGetPr(plhs[slot]);

    for (int i = 0; i < n; i++)
        output[i] = static_cast<double>(Vector(i));
}

// ********** EnSightLib interface methods ****//
EnsightObj* EnsightMatlab::getEnsightObject(const mxArray* prhs[])
{
    EnsightObj* object = convertMat2Ptr<EnsightObj>(prhs[3]);
    if (object == 0)
        throw std::runtime_error("Pointer to EnsightObject is invalid.");

    return object;
}

EnsightPart* EnsightMatlab::getEnsightPart(EnsightObj* object, const mxArray* prhs[])
{
    int partID = static_cast<int>(mxGetPr(prhs[4])[0]);
    EnsightPart* part = object->getPartById(partID);
    if (part == 0)
        throw std::runtime_error("Invalid partID.");
    return part;
}

void EnsightMatlab::createEnsight(mxArray* plhs[])
{
    // Return a handle to a new C++ instance of EnsightObj
    EnsightObj* object = EnsightLib::createEnsight();
    if (object == 0)
        throw std::runtime_error("Could not create EnSight object.");

    plhs[0] = convertPtr2Mat<EnsightObj>(object);
}

void EnsightMatlab::deleteEnsight(const mxArray* prhs[])
{
    destroyObject<EnsightLib>(prhs[3]);
}

void EnsightMatlab::readEnsight(const mxArray* prhs[], mxArray* plhs[])
{
    char filename[64];
    double* outArray;
    mxArray* valueArray;

    mxGetString(prhs[3], filename, sizeof(filename));
    // Return a handle to a new C++ instance of EnsightObj

    EnsightObj* object = EnsightLib::readEnsight(QString(filename));
    if (object == 0)
        throw std::runtime_error(std::string("Could not read file. EnsightObj::ERROR_STR='")
        + EnsightObj::ERROR_STR.toStdString() + "'");

    int NTstep = object->getNumberOfTimesteps();
    int NVar = object->getNumberOfVariables();
    int NParts = object->getNumberOfParts();
    int NConst = object->getNumberOfConstants();

    // Object Handle
    plhs[0] = convertPtr2Mat<EnsightObj>(object);

    // Create NParts_x_2 CellArray {part_id, part_name}  representing the underlying parts
    plhs[1] = mxCreateCellMatrix(NParts, 2);
    for (int i = 0; i < NParts; i++)
    {
        EnsightPart* part = object->getPart(i);
        if (part != 0)
        {
            // Part ID
            valueArray = mxCreateDoubleScalar(part->getId());
            mxSetCell(plhs[1], i, valueArray);
            mxSetCell(plhs[1], i + NParts, mxCreateString(part->getName().toLatin1().data()));
        }
    }

    // Create NConst_x_2 CellArray {const_name, const_value} representing the constants
    plhs[2] = mxCreateCellMatrix(NConst, 2);

    // Members of QHash can be easily accessed using an iterator
    for (int i = 0; i < NConst; ++i)
    {
        // mxCreateString only takes C-style char[]-Arrays, no std::String !
        // qstring.toLatin1().data() converts a QString variable to char[]
        const EnsightConstant cnst = object->getConstant(i);
        mxSetCell(plhs[2], i, mxCreateString(cnst.getName().toLatin1().data()));

        // Insert constant value (double)
        valueArray = mxCreateDoubleScalar(cnst.getValue());
        mxSetCell(plhs[2], i + NConst, valueArray);
    }


    // Create NVar_x_4 CellArray {variable_name, type, dimensions} representing the variables
    plhs[3] = mxCreateCellMatrix(NVar, 3);
    for (int i = 0; i < NVar; i++)
    {
        EnsightVariableIdentifier var = object->getVariable(i);

        // mxCreateString only takes C-style char[]-Arrays, no std::String !
        // qstring.toLatin1().data() converts a QString variable to char[]
        valueArray = mxCreateString(var.getName().toLatin1().data());
        mxSetCell(plhs[3], i, valueArray);

        // Variable Type
        mxSetCell(plhs[3], i + NVar, mxCreateDoubleScalar(static_cast<int>(var.getType())));

        // dimension of variable(i)
        valueArray = mxCreateDoubleScalar(static_cast<double>(var.getDim()));
        mxSetCell(plhs[3], i + 2 * NVar, valueArray);
    }

    // SubdivTree struct-array with five fields (is not saved in .case files)
    const char* field_names[] = {"created",
                                 "maxLevel",
                                 "maxElements",
                                 "offset",
                                 "bounds"};
    mwSize dims[2] = {1, 1};

    // Create Struct
    plhs[4] = mxCreateStructArray(2, dims, 5, field_names);

    // 0: created->false
    valueArray = mxCreateLogicalScalar(false);
    mxSetFieldByNumber(plhs[4], 0, 0, valueArray);

    // 1: maxLevel->0
    valueArray = mxCreateDoubleScalar(0);
    mxSetFieldByNumber(plhs[4], 0, 1, valueArray);

    // 2: maxLevel->0
    valueArray = mxCreateDoubleScalar(0);
    mxSetFieldByNumber(plhs[4], 0, 2, valueArray);

    // 3: offset->0.0
    valueArray = mxCreateDoubleScalar(0.0);
    mxSetFieldByNumber(plhs[4], 0, 3, valueArray);

    // 4: bounds->[0 0 0; 0 0 0]
    valueArray = mxCreateDoubleMatrix(3, 2, mxREAL);
    mxSetFieldByNumber(plhs[4], 0, 4, valueArray);
    double* values = mxGetPr(valueArray);
    for (int i = 0; i < 6; i++)
        values[i] = 0.0;

    // return timesteps
    plhs[5] = mxCreateDoubleMatrix(1, NTstep, mxREAL);
    outArray = mxGetPr(plhs[5]);
    if (NTstep == 1)
        outArray[0] = 0.0;
    else
    {
        Vecx timesteps = object->getTimesteps();
        for (int i = 0; i < NTstep; i++)
            outArray[i] = timesteps(i);
    }
}

void EnsightMatlab::writeEnsight(EnsightObj* object, const mxArray* prhs[])
{
    char filename[512];

    if (mxGetString(prhs[4], filename, sizeof(filename)))
        throw std::runtime_error("Second input (filename) should be a string less than 512 characters long.");

    const bool mode = static_cast<bool>(MexTools::getIntegerScalar(prhs, 5));
    const double timestep = MexTools::getIntegerScalar(prhs, 6);

    bool success = EnsightLib::writeEnsight(object, QString(filename), mode, timestep);

    if (!success)
        throw std::runtime_error(object->ERROR_STR.toLatin1().data());
}


// ********** EnsightObj methods ***********//
void EnsightMatlab::beginEdit(EnsightObj* object)
{
    object->beginEdit();
}

void EnsightMatlab::endEdit(EnsightObj* object)
{
    bool success = object->endEdit();
    if (!success)
        throw std::runtime_error(object->ERROR_STR.toLatin1().data());
}

void EnsightMatlab::createEnsightPart(EnsightObj* object, const mxArray* prhs[])
{
    char partname[64];

    // Get parameters
    mxGetString(prhs[4], partname, sizeof(partname));
    const int id = MexTools::getIntegerScalar(prhs, 5);

    bool success = object->createEnsightPart(QString(partname), id);

    if (!success)
        throw std::runtime_error(object->ERROR_STR.toLatin1().data());
}

void EnsightMatlab::setStatic(EnsightObj* object)
{
    bool success = object->setStatic();
    if (!success)
        throw std::runtime_error(object->ERROR_STR.toLatin1().data());
}

void EnsightMatlab::setTransient(EnsightObj* object, const mxArray* prhs[])
{
    // read MATLAB input
    const Vecx timesteps = MexTools::getDoubleVector(prhs, 4);

    bool success = object->setTransient(timesteps);
    if (!success)
        throw std::runtime_error(object->ERROR_STR.toLatin1().data());
}

void EnsightMatlab::cleanObject(EnsightObj* object, const mxArray* prhs[])
{
    const int timestep = MexTools::getIntegerScalar(prhs, 4);
    object->clean(timestep);
}

void EnsightMatlab::getObjectGeometryBounds(EnsightObj* object, const mxArray* prhs[], mxArray* plhs[])
{
    // Access first input
    const int timestep = MexTools::getIntegerScalar(prhs, 4);

    // Get bounding box from C++ object
    Bbox bound;

    for (int i = 0; i < object->getNumberOfParts(); i++)
        bound.extend(object->getPart(i)->getGeometryBounds(timestep));

    Eigen::MatrixXd output(3, 2);
    output.block(0, 0, 3, 1) = bound.minCorner();
    output.block(0, 1, 3, 1) = bound.maxCorner();

    MexTools::mexAllocateAndCopyMatrix(output, plhs, 0);
}

void EnsightMatlab::createVariable(EnsightObj* object, const mxArray* prhs[])
{
    char name[64];

    mxGetString(prhs[4], name, sizeof(name));
    const int type = MexTools::getIntegerScalar(prhs, 5);

    bool success = object->createVariable(QString(name), Ensight::VarTypes(type));
    if (!success)
        throw std::runtime_error(object->ERROR_STR.toLatin1().data());
}

void EnsightMatlab::addConstant(EnsightObj* object, const mxArray* prhs[])
{
    char name[64];

    mxGetString(prhs[4], name, sizeof(name));

    const double value = MexTools::getDoubleScalar(prhs, 5);
    const QString qName(name);
    bool success = object->addConstant(qName, value);
    if (!success)
        throw std::runtime_error(object->ERROR_STR.toLatin1().data());
}


void EnsightMatlab::replaceConstants(EnsightObj* object, const mxArray* prhs[])
{
    char name[64];
    mxArray* cellPointer;

    const mwSize* dims = mxGetDimensions(prhs[2]);
    int m = static_cast<int>(dims[0]);

    for (int i = 0; i < m; i++)
    {
        cellPointer = mxGetCell(prhs[2], i);
        mxGetString(cellPointer, name, sizeof(name));

        cellPointer = mxGetCell(prhs[2], i + m);

        if (object->hasConstant(QString(name)))
        {
            double value = static_cast<double>(mxGetPr(cellPointer)[0]);
            const QString qName(name);
            bool success = object->removeConstant(qName);
            success &= object->addConstant(qName, value);
            if (!success)
                throw std::runtime_error(object->ERROR_STR.toLatin1().data());
        }
        else
        {
            char warnmsg[64];
            sprintf(warnmsg, "obj_replaceConstants - invalid constant name '%c'\n", name);
            mexWarnMsgTxt(warnmsg);
        }
    }
}

void EnsightMatlab::getVariableBounds(EnsightObj* object, const mxArray* prhs[], mxArray* plhs[])
{
    // Access parameters
    char name[64];
    mxGetString(prhs[4], name, sizeof(name));
    const int timestep = MexTools::getIntegerScalar(prhs, 5);

    // Get matrix with variable bounds
    Matx bounds = object->getVariableBounds(QString(name), timestep);

    MexTools::mexAllocateAndCopyMatrix(bounds, plhs, 0);
}

void EnsightMatlab::getSubdivTreeBounds(EnsightObj* object, mxArray* plhs[])
{
    EnsightSubdivTree* subdivTree = object->getSubdivTree();

    Eigen::MatrixXd output = Eigen::MatrixXd::Zero(3, 2);

    if (subdivTree == 0)
        mexWarnMsgTxt("Searchtree has not been initialized yet.");
    else
    {
        Bbox bounds = subdivTree->getBounds();
        output.block(0, 0, 3, 1) = bounds.minCorner();
        output.block(0, 1, 3, 1) = bounds.maxCorner();
    }

    MexTools::mexAllocateAndCopyMatrix(output, plhs, 0);
}

// *************** EnsightPart methods ******************* //

void EnsightMatlab::cleanPart(EnsightPart* part, const mxArray* prhs[])
{
    const int timestep = MexTools::getIntegerScalar(prhs, 5);
    part->clean(timestep);
}


void EnsightMatlab::getPartGeometryBounds(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    // Access parameters
    const int timestep = MexTools::getIntegerScalar(prhs, 5);

    Eigen::MatrixXd output = Eigen::MatrixXd::Zero(3, 2);

    Bbox bounds = part->getGeometryBounds(timestep);
    output.block(0, 0, 3, 1) = bounds.minCorner();
    output.block(0, 1, 3, 1) = bounds.maxCorner();

    MexTools::mexAllocateAndCopyMatrix(output, plhs, 0);
}

void EnsightMatlab::setVerticesOfPart(EnsightPart* part, const mxArray* prhs[])
{
    // Access parameters
    const Matx vertices = MexTools::getDoubleMatrix(prhs, 5);
    const int timestep = MexTools::getIntegerScalar(prhs, 6);

    part->setVertices( vertices, timestep);
}

void EnsightMatlab::getVerticesOfPart(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    // Access first parameter
    const int timestep = MexTools::getIntegerScalar(prhs, 5);

    // Call C++ method
    Matx vertices = part->getVertices(timestep);
    MexTools::mexAllocateAndCopyMatrix(vertices, plhs, 0);
}

void EnsightMatlab::partHasCelltype(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    const int timestep = MexTools::getIntegerScalar(prhs, 5);
    const int celltype = MexTools::getIntegerScalar(prhs, 6);

    bool result = part->hasCellType(timestep, Ensight::Cell(celltype));
    plhs[0] = mxCreateLogicalScalar(result);
}

void EnsightMatlab::setCellsOfPart(EnsightPart* part, const mxArray* prhs[])
{
    // Access parameters
    const Mati nodes = MexTools::getIntegerMatrix(prhs, 5);
    const int timestep = MexTools::getIntegerScalar(prhs, 6);
    const int celltype = MexTools::getIntegerScalar(prhs, 7);

    part->setCells(nodes, timestep, Ensight::Cell(celltype));
}

void EnsightMatlab::getCellListOfPart(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    const int timestep = MexTools::getIntegerScalar(prhs, 5);

    Eigen::MatrixXd output = Eigen::MatrixXd::Zero(1, 8);
    for (int i = 0; i < 8; i++)
    {
        EnsightCellList* cellist = part->getCells(timestep, Ensight::Cell(i));
        if (cellist != 0)
            output(i) = cellist->getValues().cols();
    }

    MexTools::mexAllocateAndCopyMatrix(output, plhs, 0);
}

void EnsightMatlab::getCellValuesOfPart(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    const int celltype = MexTools::getIntegerScalar(prhs, 5);
    const int timestep = MexTools::getIntegerScalar(prhs, 6);

    EnsightCellList* cellList = part->getCells(timestep, Ensight::Cell(celltype));

    if (cellList == 0)
        throw std::runtime_error("Cells of this type do not exist.");

    Eigen::MatrixXi values = cellList->getValues();
    MexTools::mexAllocateAndCopyMatrix(values, plhs, 0);
}

void EnsightMatlab::getCellBoundsOfPart(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    const Veci cell = MexTools::getIntegerVector(prhs, 5);
    const int timestep = MexTools::getIntegerScalar(prhs, 6);

    Eigen::MatrixXd output = Eigen::MatrixXd::Zero(3, 2);

    Bbox bounds = part->getCellBounds(cell, timestep);
    output.block(0, 0, 3, 1) = bounds.minCorner();
    output.block(0, 1, 3, 1) = bounds.maxCorner();

    MexTools::mexAllocateAndCopyMatrix(output, plhs, 0);
}

void EnsightMatlab::getVertexCountOfPart(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    const int timestep = MexTools::getIntegerScalar(prhs, 5);

    int vertexCount = part->getVertexCount(timestep);
    plhs[0] = mxCreateDoubleScalar(vertexCount);
}

void EnsightMatlab::setVariableForPart(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    // Access parameters
    const Matx variableValues = MexTools::getDoubleMatrix(prhs, 6);
    char variableName[64];
    mxGetString(prhs[5], variableName, sizeof(variableName));
    const int variableType = MexTools::getIntegerScalar(prhs, 7);
    const int timestep = MexTools::getIntegerScalar(prhs, 8);

    part->setVariable(QString(variableName), variableValues, Ensight::VarTypes(variableType), timestep);
}

void EnsightMatlab::getVariableBoundsOfPart(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    // Access parameters
    char variableName[64];
    mxGetString(prhs[5], variableName, sizeof(variableName));
    const int timestep = MexTools::getIntegerScalar(prhs, 6);

    // Get matrix with variable bounds
    const Matx bounds = part->getVariableBounds(QString(variableName), timestep);
    MexTools::mexAllocateAndCopyMatrix(bounds, plhs, 0);
}

void EnsightMatlab::getVariableValuesOfPart(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    // Access parameters
    char variableName[64];
    mxGetString(prhs[5], variableName, sizeof(variableName));
    const int timestep = MexTools::getIntegerScalar(prhs, 6);

    const Matx values = part->getVariableValues(QString(variableName), timestep);
    MexTools::mexAllocateAndCopyMatrix(values, plhs, 0);
}

void EnsightMatlab::partHasVariable(EnsightPart* part, const mxArray* prhs[], mxArray* plhs[])
{
    // Access parameters
    char variableName[64];
    mxGetString(prhs[5], variableName, sizeof(variableName));
    const int timestep = MexTools::getIntegerScalar(prhs, 6);


    // Call C++ routine
    bool result = part->hasVariable(QString(variableName), timestep);
    plhs[0] = mxCreateLogicalScalar(result);
}

// ********** EnsightSubdivTree and interpolation methods ***********//
void EnsightMatlab::createSubdivTree(EnsightObj* object, const mxArray* prhs[])
{
    // Access input parameters
    const int maxDepth = MexTools::getIntegerScalar(prhs, 4);
    const int maxElements = MexTools::getIntegerScalar(prhs, 5);
    const double sizeOffset = MexTools::getDoubleScalar(prhs, 6);

    QStringList partsToExclude;
    bool success = object->createSubdivTree(maxDepth, maxElements,
                                            partsToExclude, sizeOffset);
    if (!success)
        throw std::runtime_error(object->ERROR_STR.toLatin1().data());
}

void EnsightMatlab::interpolate(EnsightObj* object, const mxArray* prhs[], mxArray* plhs[])
{
    char variableName[64];
    mxGetString(prhs[5], variableName, sizeof(variableName));
    const Vec3 pos = MexTools::getDoubleVector(prhs, 4);
    if (!object->hasVariable(QString(variableName)))
        throw std::runtime_error("Variable does not exist.");

    if (object->getSubdivTree() == 0)
    {
        QStringList partsToExclude;
        object->createSubdivTree(7, 50, partsToExclude, 0.0);
    }

    // Search SubdivTree
    EnsightBarycentricCoordinates barycoords;
    EnsightCellIdentifier* cell = object->interpolate(pos, barycoords);
    if (cell == NULL)
    {
        plhs[0] = mxCreateDoubleScalar(0.0);
        plhs[1] = mxCreateLogicalScalar(false);
    }
    else
    {
        try
        {
            const Eigen::MatrixXd values = cell->getValues(QString(variableName));
            Eigen::VectorXd result = barycoords.evaluate(values);

            MexTools::mexAllocateAndCopyMatrix(result, plhs, 0);
            plhs[1] = mxCreateLogicalScalar(true);
        }
        catch (...)
        {
            throw std::runtime_error(object->ERROR_STR.toLatin1().data());
        }
    }
}

void EnsightMatlab::findCell(EnsightObj* object, const mxArray* prhs[], mxArray* plhs[])
{
    int optLength = 0;
    bool ok_volume = false, ok_centroid = false;

    if (object->getSubdivTree() == 0)
    {
        QStringList partsToExclude;
        object->createSubdivTree(7, 50, partsToExclude, 0.0);
    }

    // Access parameters
    const Matx lineSegments = MexTools::getDoubleMatrix(prhs, 4);
    char options[64];
    mxGetString(prhs[5], options, sizeof(options));

    // Specify further options
    optLength = options[0] - '0';
    for (int i = 1; i <= optLength; ++i)
        if (options[i] == 'v')
            ok_volume = true;
        else if (options[i] == 'c')
            ok_centroid = true;


    // Get pointer to EnsightSubdivTree
    EnsightSubdivTree* subdivTree = object->getSubdivTree();
    if (subdivTree == 0)
    {
        mexWarnMsgTxt("SubdivTree has not been created. Attempting to create it.");
        QStringList partsToExclude;
        bool success = object->createSubdivTree(7, 50, partsToExclude, 0.0);
        if (!success)
            throw std::runtime_error("Could not create SubdivTree.");
    }

    EnsightBarycentricCoordinates bary;
    int numberOfPoints = lineSegments.cols();

    // Save handle_pointers in cell array
    if (ok_volume && ok_centroid)
    {
        Eigen::MatrixXd output(7, numberOfPoints);
        EnsightCellIdentifier* cell;

        // Run subdiv tree search for each point
        for (int i = 0; i < numberOfPoints; i++)
        {
            cell = subdivTree->search( lineSegments.col(i), bary);
            // Cell found
            // Compute a unique triplet (cell_id, type_id, part_id) representing the cell
            if (cell != 0)
            {
                // Compute centroid
                Veci vId = cell->getCell();
                Vec3 centroid = Eigen::Vector3d::Zero();
                double scale = 1.0 / vId.rows();

                for (int c = 0; c < vId.rows(); c++)
                    centroid += scale * cell->getVertex(c);

                // Save results
                output(0, i) = cell->getIndex();
                output(1, i) = cell->getCellList()->getType();
                output(2, i) = object->getPartByName(cell->getCellList()->getPartName())->getId();
                output(3, i) = cell->computeVolume();
                output(4, i) = centroid(0);
                output(5, i) = centroid(1);
                output(6, i) = centroid(2);
            }
            else
            {
                // Convention: Set result to (-1,-1,-1,0,0,0,0) if result is empty
                output(0, i) = -1;
                output(1, i) = -1;
                output(2, i) = -1;
                output(3, i) = 0.0;
                output(4, i) = 0.0;
                output(5, i) = 0.0;
                output(6, i) = 0.0;
            }
        }

        MexTools::mexAllocateAndCopyMatrix(output, plhs, 0);
    }
    else if (ok_volume)
    {
        Eigen::MatrixXd output(4, numberOfPoints);
        EnsightCellIdentifier* cell;

        // Run subdiv tree search for each point
        for (int i = 0; i < numberOfPoints; i++)
        {
            cell = subdivTree->search(lineSegments.col(i), bary);
            // Cell found
            // Compute a unique triplet (cell_id, type_id, part_id) representing the cell
            if (cell != 0)
            {
                // Save results
                output(0, i) = cell->getIndex();
                output(1, i) = cell->getCellList()->getType();
                output(2, i) = object->getPartByName(cell->getCellList()->getPartName())->getId();
                output(3, i) = cell->computeVolume();
            }
            else
            {
                // Convention: Set result to (-1,-1,-1,0) if result is empty
                output(0, i) = -1;
                output(1, i) = -1;
                output(2, i) = -1;
                output(3, i) = 0.0;
            }
        }

        MexTools::mexAllocateAndCopyMatrix(output, plhs, 0);
    }
    else if (ok_centroid)
    {
        Eigen::MatrixXd output(6, numberOfPoints);
        EnsightCellIdentifier* cell;

        // Run subdiv tree search for each point
        for (int i = 0; i < numberOfPoints; i++)
        {
            cell = subdivTree->search(lineSegments.col(i), bary);
            // Cell found
            // Compute a unique triplet (cell_id, type_id, part_id) representing the cell
            if (cell != 0)
            {
                // Compute centroid
                Veci vId = cell->getCell();
                Vec3 centroid = Eigen::Vector3d::Zero();
                double scale = 1.0 / vId.rows();

                for (int c = 0; c < vId.rows(); c++)
                    centroid += scale * cell->getVertex(c);

                // Save results
                output(0, i) = cell->getIndex();
                output(1, i) = cell->getCellList()->getType();
                output(2, i) = object->getPartByName(cell->getCellList()->getPartName())->getId();
                output(3, i) = centroid(0);
                output(4, i) = centroid(1);
                output(5, i) = centroid(2);
            }
            else
            {
                // Convention: Set result to (-1,-1,-1,0,0,0,0) if result is empty
                output(0, i) = -1;
                output(1, i) = -1;
                output(2, i) = -1;
                output(3, i) = 0.0;
                output(4, i) = 0.0;
                output(5, i) = 0.0;
            }
        }

        MexTools::mexAllocateAndCopyMatrix(output, plhs, 0);
    }
    else
    {
        Eigen::MatrixXd output(3, numberOfPoints);
        EnsightCellIdentifier* cell;

        // Run subdiv tree search for each point
        for (int i = 0; i < numberOfPoints; i++)
        {
            cell = subdivTree->search(lineSegments.col(i), bary);
            // Cell found
            // Compute a unique triplet (cell_id, type_id, part_id) representing the cell
            if (cell != 0)
            {
                // Save results
                output(0, i) = cell->getIndex();
                output(1, i) = cell->getCellList()->getType();
                output(2, i) = object->getPartByName(cell->getCellList()->getPartName())->getId();
            }
            else
            {
                // Convention: Set result to (-1,-1,-1) if result is empty
                output(0, i) = -1;
                output(1, i) = -1;
                output(2, i) = -1;
            }
        }

        MexTools::mexAllocateAndCopyMatrix(output, plhs, 0);
    }
}
