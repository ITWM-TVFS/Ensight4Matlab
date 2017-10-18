#include "ensightvariabletests.h"

void EnsightVariableTests::EnsightVariableIdentifierDefaultConstructor_StaticValueAssignment_CorrectValuesAssigned()
{
    auto testEnsightVariableId = EnsightVariableIdentifier();

    QString testName;
    Ensight::VarTypes testVarType{Ensight::VarTypes::ConstantPerCase};
    int testDim{0};

    QCOMPARE(testEnsightVariableId.getName(), testName);
    QCOMPARE(testEnsightVariableId.getType(), testVarType);
    QCOMPARE(testEnsightVariableId.getDim(), testDim);
}

void EnsightVariableTests::EnsightVariableIdentifierQStringArgumentConstructor_ParameterAssignment_CorrectValuesAssigned()
{
    QString argString{"Test01"};
    Ensight::VarTypes argType = Ensight::VarTypes::ScalarPerNode;

    auto testEnsightVariableId = EnsightVariableIdentifier(argString, argType);

    QCOMPARE(testEnsightVariableId.getName(), argString);
    QCOMPARE(testEnsightVariableId.getType(), argType);
}

void EnsightVariableTests::EnsightVariableIdentifierStdStringArgumentConstructor_ParameterAssignment_CorrectValuesAssigned()
{
    std::string argString{"Test02"};
    Ensight::VarTypes argType = Ensight::VarTypes::ScalarPerNode;

    auto testEnsightVariableId = EnsightVariableIdentifier(argString, argType);

    QCOMPARE(testEnsightVariableId.getName().toStdString(), argString);
    QCOMPARE(testEnsightVariableId.getType(), argType);
}

void EnsightVariableTests::EnsightVariableIdentifierGetName_ValueObservator_CorrectNameReturned()
{
    QString argString{"Test03"};
    Ensight::VarTypes argType = Ensight::VarTypes::ScalarPerNode;

    auto testEnsightVariableId = EnsightVariableIdentifier(argString, argType);

    QCOMPARE(testEnsightVariableId.getName(), argString);
}

void EnsightVariableTests::EnsightVariableIdentifierGetType_ValueObservator_CorrectTypeReturned()
{
    QString argString{"Test04"};
    Ensight::VarTypes argType = Ensight::VarTypes::VarTypeUnknown;

    auto testEnsightVariableId = EnsightVariableIdentifier(argString, argType);

    QCOMPARE(testEnsightVariableId.getType(), argType);
}

void EnsightVariableTests::EnsightVariableIdentifierGetDim_ValueObservator_CorrectDimensionReturned()
{
    QString argString{"Test05"};
    Ensight::VarTypes argType = Ensight::VarTypes::VectorPerNode;

    auto testEnsightVariableId = EnsightVariableIdentifier(argString, argType);
    int expectedDimension = Ensight::varTypeDims[static_cast<int>(argType)];

    QCOMPARE(testEnsightVariableId.getDim(), expectedDimension);
}

void EnsightVariableTests::EnsightVariableIdentifierIsNull_Predicate_ReturnsCorrectly()
{
    auto testEnsightVariableId = EnsightVariableIdentifier();

    QVERIFY(testEnsightVariableId.isNull());
}



void EnsightVariableTests::EnsightVariableQStringArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigned()
{
    QString argString{"Test06"};
    auto argType = Ensight::VarTypes::ConstantPerCase;

    auto testEnsightVariable = EnsightVariable(argString, argType);

    QCOMPARE(testEnsightVariable.getName(), argString);
    QCOMPARE(testEnsightVariable.getType(), argType);
}

void EnsightVariableTests::EnsightVariableStdStringArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigned()
{
    std::string argString{"Test07"};
    auto argType = Ensight::VarTypes::ConstantPerCase;

    auto testEnsightVariable = EnsightVariable(argString, argType);

    QCOMPARE(testEnsightVariable.getName().toStdString(), argString);
    QCOMPARE(testEnsightVariable.getType(), argType);
}

void EnsightVariableTests::EnsightVariableSetValues_ValueMutator_CorrectMatricesAssignedFor3DVariable()
{
   QString argString{"Test07"};
   auto argType = Ensight::VarTypes::ConstantPerCase;

   auto testEnsightVariable = EnsightVariable(argString, argType);

   Matx test3DVariable{3, 3};

   test3DVariable << 1, 3, 5,
                     2, 4, 12,
                     2, 12, 84;

    testEnsightVariable.setValues(test3DVariable);


    //Testing Values Matrix

    Matx expectedValuesMatrix{4, 3};

    // norm(1, 2, 2) = sqrt(1*1 + 2*2 + 2*2) = sqrt(1 + 4 + 4) = sqrt(9) = 3
    // norm(3, 4, 12) = sqrt(3*3 + 4*4 + 12*12) = sqrt(9 + 16 + 144) = sqrt(169) = 13
    // norm(5, 12, 84) = sqrt(5*5 + 12*12 + 84*84) = sqrt(25 + 144 + 7056) = sqrt(7225) = 85

    expectedValuesMatrix << 1, 3, 5,
                            2, 4, 12,
                            2, 12, 84,
                            3, 13, 85;

    Matx returnedValues = testEnsightVariable.getValues();
    size_t returnedNumberOfRows = returnedValues.rows();
    size_t returnedNumberOfCols = returnedValues.cols();

    QVERIFY(returnedNumberOfRows == 4 && returnedNumberOfCols == 3);

    for(size_t i = 0; i < returnedNumberOfRows; ++i)
        for(size_t j = 0; j < returnedNumberOfCols; ++j)
            QCOMPARE(returnedValues(i,j), expectedValuesMatrix(i, j));


    //Testing Bounds Matrix

    Matx expectedBoundsMatrix{4, 2};

    expectedBoundsMatrix << 1, 5,
                            2, 12,
                            2, 84,
                            3, 85;

    Matx returnedBounds = testEnsightVariable.getBounds();
    returnedNumberOfRows = returnedBounds.rows();
    returnedNumberOfCols = returnedBounds.cols();

    QVERIFY(returnedNumberOfRows == 4 && returnedNumberOfCols == 2);

    for(size_t i = 0; i < returnedNumberOfRows; ++i)
        for(size_t j = 0; j < returnedNumberOfCols; ++j)
            QCOMPARE(returnedBounds(i,j), expectedBoundsMatrix(i,j));
}

void EnsightVariableTests::EnsightVariableSetValues_ValueMutator_CorrectMatrixAssignedFor1DVariable()
{
    QString argString{"Test08"};
    auto argType = Ensight::VarTypes::ConstantPerCase;

    auto testEnsightVariable = EnsightVariable(argString, argType);

    Matx test1DVariable{1, 3};

    test1DVariable << 1, 2, 3;

    testEnsightVariable.setValues(test1DVariable);

    //Testing Values Matrix

    Matx returnedValues = testEnsightVariable.getValues();
    size_t returnedNumberOfRows = returnedValues.rows();
    size_t returnedNumberOfCols = returnedValues.cols();

    QVERIFY(returnedNumberOfRows == 1 && returnedNumberOfCols == 3);

    for(size_t i = 0; i < returnedNumberOfCols; ++i)
        QCOMPARE(returnedValues(0,i), test1DVariable(0,i));


    //Testing Bounds Matrix

    Matx expectedBoundsMatrix{1,2};

    expectedBoundsMatrix << 1, 3;

    Matx returnedBounds = testEnsightVariable.getBounds();
    returnedNumberOfCols = returnedBounds.cols();

    for(size_t i = 0; i < returnedNumberOfCols; ++i)
        QCOMPARE(returnedBounds(0,i), expectedBoundsMatrix(0,i));
}

void EnsightVariableTests::EnsightVariableGetValues_ValueObservator_CorrectMatrixReturned()
{
    QString argString{"Test09"};
    auto argType = Ensight::VarTypes::ConstantPerCase;


    auto testEnsightVariable = EnsightVariable(argString, argType);


    Matx expectedValuesMatrix{1, 3};

    expectedValuesMatrix << 1, 2, 3;


    testEnsightVariable.setValues(expectedValuesMatrix);


    Matx returnedValuesMatrix = testEnsightVariable.getValues();
    size_t returnedNumberOfCols = returnedValuesMatrix.cols();

    QVERIFY(returnedNumberOfCols == 3);

    for(size_t i = 0; i < returnedNumberOfCols; ++i)
        QCOMPARE(returnedValuesMatrix(0,i), expectedValuesMatrix(0,i));
}

void EnsightVariableTests::EnsightVariableGetBounds_ValueObservator_CorrectMatrixReturned()
{
    QString argString{"Test10"};
    auto argType = Ensight::VarTypes::ConstantPerCase;

    auto testEnsightVariable = EnsightVariable(argString, argType);


    Matx argMatrix{1, 3};

    argMatrix << 2, 4, 8;


    testEnsightVariable.setValues(argMatrix);


    Matx returnedBoundsMatrix = testEnsightVariable.getBounds();
    size_t returnedNumberOfCols = returnedBoundsMatrix.cols();

    QVERIFY(returnedNumberOfCols == 2);

    Matx expectedBoundsMatrix{1, 2};

    expectedBoundsMatrix << 2, 8;

    for(size_t i = 0; i < returnedNumberOfCols; ++i)
        QCOMPARE(returnedBoundsMatrix(0,i), expectedBoundsMatrix(0,i));


}
