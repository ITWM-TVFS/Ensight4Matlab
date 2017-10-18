#include "ensightreadertests.h"

#include "ensightpart.h"

#include <ensightvariable.h>

void EnsightReaderTests::init()
{
    testEnsightCase = EnsightCase();
    EnsightObj::ERROR_STR = "";
    testEnsightCase.masterFileName = "";
}



void EnsightReaderTests::DefaultConstructor_StaticValueAssignment_CorrectValuesAssigned()
{
    QString expectedMasterFileName{};
    QString expectedModelFileName{};
    int expectedModelTimeset{-2};
    int expectedModelFileset{-1};
    int expectedTimesetID{-1};
    int expectedTimesetSteps{-1};
    int expectedTimesetFilenameStart{-1};
    int expectedTimesetFilenameIncrement{-1};
    Vecx expectedTimesteps{Vecx::Zero(0)};
    int expectedFilesetId{-1};
    int expectedFilesetSteps{-1};

    QCOMPARE(testEnsightCase.masterFileName, expectedMasterFileName);
    QCOMPARE(testEnsightCase.modelFilename, expectedModelFileName);
    QCOMPARE(testEnsightCase.modelTimeset, expectedModelTimeset);
    QCOMPARE(testEnsightCase.modelFileSet, expectedModelFileset);
    QCOMPARE(testEnsightCase.timesetId, expectedTimesetID);
    QCOMPARE(testEnsightCase.timesetSteps, expectedTimesetSteps);
    QCOMPARE(testEnsightCase.timesetFilenameStart, expectedTimesetFilenameStart);
    QCOMPARE(testEnsightCase.timesetFilenameIncrement, expectedTimesetFilenameIncrement);
    QCOMPARE(testEnsightCase.timesteps, expectedTimesteps);
    QCOMPARE(testEnsightCase.filesetId, expectedFilesetId);
    QCOMPARE(testEnsightCase.filesetSteps, expectedFilesetSteps);
}

void EnsightReaderTests::QStringArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigend()
{
    QString argString{relativeValidFlowFilePath_};

    testEnsightCase = EnsightCase(argString);

    QString absoluteValidFlowFilesPath =
        QFileInfo(relativeValidFlowFilePath_).absoluteFilePath();

    QString expectedMasterFileName{absoluteValidFlowFilesPath};

    QCOMPARE(testEnsightCase.masterFileName, expectedMasterFileName);
}

void EnsightReaderTests::StdStringArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigend()
{
    QString absoluteValidFlowFilesPath =
        QFileInfo(relativeValidFlowFilePath_).absoluteFilePath();

    std::string argString = relativeValidFlowFilePath_.toStdString();

    testEnsightCase = EnsightCase(argString);

    QString expectedMasterFileName{absoluteValidFlowFilesPath};

    QCOMPARE(testEnsightCase.masterFileName, expectedMasterFileName);
}

void EnsightReaderTests::ReadCaseFile_SuccessfulCaseFileRead_CaseFileCorrectlyParsed()
{
    QString argString = relativeValidFlowFilePath_;

    testEnsightCase.setMasterFileName(argString);

    bool successfulRead = testEnsightCase.readCaseFile();

    QVERIFY(successfulRead);

    QString expectedModelFilename{"jet.geo"};
    QStringList expectedVariableNames
        = {"density", "temperature", "turb_kinetic_energy",
           "turb_diss_rate", "viscosity_lam", "velocity"};
    QStringList expectedVariableFilenames
        = {"jet.scl1", "jet.scl2", "jet.scl3",
           "jet.scl4", "jet.scl5", "jet.vel"};
    QList<int> expectedVariableTimeSets
        = {-1, -1, -1, -1, -1, -1};
    QList<int> expectedVariableFileSets
        = {-1, -1, -1, -1, -1, -1};
    QList<Ensight::VarTypes> expectedVariableTypes
        = {Ensight::ScalarPerNode, Ensight::ScalarPerNode, Ensight::ScalarPerNode,
           Ensight::ScalarPerNode, Ensight::ScalarPerNode, Ensight::VectorPerNode};

    QCOMPARE(testEnsightCase.modelFilename, expectedModelFilename);
    QCOMPARE(testEnsightCase.variableNames, expectedVariableNames);
    QCOMPARE(testEnsightCase.variableFilenames, expectedVariableFilenames);
    QCOMPARE(testEnsightCase.variableTimeSets, expectedVariableTimeSets);
    QCOMPARE(testEnsightCase.variableFileSets, expectedVariableFileSets);
    QCOMPARE(testEnsightCase.variableTypes, expectedVariableTypes);
}

void EnsightReaderTests::ReadCaseFile_BadFilePath_ReturnsFalse()
{
    QString argString = "testForBadFilePath01";

    testEnsightCase.setMasterFileName(argString);

    QCOMPARE(testEnsightCase.masterFileName, argString);

    bool returnedValue = testEnsightCase.readCaseFile();
    bool expectedValue{false};

    QCOMPARE(returnedValue, expectedValue);
}

void EnsightReaderTests::ReadCaseFile_BadFilePath_SetsCorrectErrorString()
{
    QString argString = "testForBadFilePath02";

    testEnsightCase.setMasterFileName(argString);

    bool successfulRead = testEnsightCase.readCaseFile();

    QVERIFY(!successfulRead);

    QString setErrorString = EnsightObj::ERROR_STR;
    QString expectedErrorString =
            "[EnsightReader::read()] Cannot read from file  " +
            testEnsightCase.masterFileName + ".";

    QCOMPARE(setErrorString, expectedErrorString);
}

/*
    Corresponding test file: readCaseFile_unequalTimesteps.case

    Testfile contains differing values for the "file set" and
    "number of sets" properties of the FILE section. This leads
    to data members filesetSteps and timesetSteps being unequal,
    triggering the UnequalTimesteps error.
*/
void EnsightReaderTests::ReadCaseFile_UnequalTimesteps_ReturnsFalse()
{
    std::string absoluteInvalidTestFilePath =
            QFileInfo(relativeInvalidFilesDirPath_).absoluteFilePath().toStdString()
            + "readCaseFile/readCaseFile_unequalTimesteps.case";

    testEnsightCase.setMasterFileName(absoluteInvalidTestFilePath);

    bool returnedValue = testEnsightCase.readCaseFile();
    bool expectedValue{false};

    QCOMPARE(returnedValue, expectedValue);
}

/*
    Corresponding test file: readCaseFile_unequalTimesteps.case

    Testfile contains differing values for the "file set" and
    "number of sets" properties of the FILE section. This leads
    to data members filesetSteps and timesetSteps being unequal,
    triggering the UnequalTimesteps error.
*/
void EnsightReaderTests::ReadCaseFile_UnequalTimesteps_SetsCorrectErrorString()
{
    std::string absoluteInvalidTestFilePath =
            QFileInfo(relativeInvalidFilesDirPath_).absoluteFilePath().toStdString()
            + "readCaseFile/readCaseFile_unequalTimesteps.case";

    testEnsightCase.setMasterFileName(absoluteInvalidTestFilePath);

    bool successfulRead = testEnsightCase.readCaseFile();

    QVERIFY(!successfulRead);

    QString setErrorString = EnsightObj::ERROR_STR;
    QString expectedErrorString =
            "[EnsightReader::read()] Number of "
            "steps in file set must be equal to "
            "number of steps in time set.";

    QCOMPARE(setErrorString, expectedErrorString);
}

/*
    Corresponding test file: readCaseFile_missingGeometrySection.case

    Testfile omits the GEOMETRY section of the case file format. This
    leads to the data member modelFilename being an empty string, triggering
    the MissingGeometry error.
*/
void EnsightReaderTests::ReadCaseFile_MissingGeometrySection_ReturnsFalse()
{
    std::string absoluteInvalidTestFilePath =
            QFileInfo(relativeInvalidFilesDirPath_).absoluteFilePath().toStdString()
            + "readCaseFile/readCaseFile_missingGeometrySection.case";

    testEnsightCase.setMasterFileName(absoluteInvalidTestFilePath);

    bool returnedValue = testEnsightCase.readCaseFile();
    bool expectedValue{false};

    QCOMPARE(returnedValue, expectedValue);
}

/*
    Corresponding test file: readCaseFile_missingGeometrySection.case

    Testfile omits the GEOMETRY section of the case file format. This
    leads to the data member modelFilename being an empty string, triggering
    the MissingGeometry error.
*/
void EnsightReaderTests::ReadCaseFile_MissingGeometrySection_SetsCorrectErrorString()
{
    std::string absoluteInvalidTestFilePath =
            QFileInfo(relativeInvalidFilesDirPath_).absoluteFilePath().toStdString()
            + "readCaseFile/readCaseFile_missingGeometrySection.case";

    testEnsightCase.setMasterFileName(absoluteInvalidTestFilePath);

    bool successfulRead = testEnsightCase.readCaseFile();

    QVERIFY(!successfulRead);

    QString setErrorString = EnsightObj::ERROR_STR;
    QString expectedErrorString =
            "[EnsightReader::read()] SECTION GEOMETRY  not found.";

    QCOMPARE(setErrorString, expectedErrorString);
}

void EnsightReaderTests::QStringSetMasterFileName_ValueMutator_CorrectMasterFileNameAssigned()
{
    testEnsightCase = EnsightCase();

    QString argString = relativeValidFlowFilePath_;

    testEnsightCase.setMasterFileName(argString);

    QCOMPARE(testEnsightCase.masterFileName, argString);
}

void EnsightReaderTests::StdStringSetMasterFileName_ValueMutator_CorrectMasterFileNameAssigned()
{
    testEnsightCase = EnsightCase();

    std::string argString = relativeValidFlowFilePath_.toStdString();

    testEnsightCase.setMasterFileName(argString);

    QCOMPARE(testEnsightCase.masterFileName.toStdString(), argString);
}

void EnsightReaderTests::GetFileNumberForStep_Predicade_ReturnsCorrectly()
{
    QFETCH(int, argValue);
    QFETCH(int, setTimesetFilenameStart);
    QFETCH(int, setTimesetFilenameIncrement);
    QFETCH(int, expectedReturnValue);

    testEnsightCase.timesetFilenameStart = setTimesetFilenameStart;
    testEnsightCase.timesetFilenameIncrement = setTimesetFilenameIncrement;

    int returnedValue = testEnsightCase.getFileNumberForStep(argValue);

    QCOMPARE(returnedValue, expectedReturnValue);
}

/*
    Tested Logic:

    (step is the parameter -> argValue)

    if (timesetFilenameStart < 0 || timesetFilenameIncrement <= 0)
        return step;

    return timesetFilenameStart + step * timesetFilenameIncrement;
*/
void EnsightReaderTests::GetFileNumberForStep_Predicade_ReturnsCorrectly_data()
{
    QTest::addColumn<int>("argValue");
    QTest::addColumn<int>("setTimesetFilenameStart");
    QTest::addColumn<int>("setTimesetFilenameIncrement");
    QTest::addColumn<int>("expectedReturnValue");

    QTest::newRow("FirstSubPredicateTrue")
            << 1
            << -1
            << 2
            << 1;

    QTest::newRow("SecondSubPredicateTrue")
            << 1
            << 2
            << 0
            << 1;

    QTest::newRow("AllSubPredicatesTrue")
            << 1
            << -1
            << 0
            << 1;

    QTest::newRow("AllSubPredicatesFalse")
            << 2
            << 0
            << 5
            << 10; //0 + 2 * 5 = 10
}

void EnsightReaderTests::QStringRead_SuccessfulCaseFileRead_CaseFileCorrectylParsed()
{
    QString argString = relativeValidFlowFilePath_;

    std::unique_ptr<EnsightObj> createdEnsightObj{Ensight::Reader::read(argString,0)};

    QVERIFY((bool) createdEnsightObj);

    Vecx expectedTimestepsVector = Vecx::Zero(1);

    QCOMPARE(createdEnsightObj->getTimesteps(), expectedTimestepsVector);

    QStringList expectedVariableNames
        = {"density", "temperature", "turb_kinetic_energy",
           "turb_diss_rate", "viscosity_lam", "velocity"};
    QList<Ensight::VarTypes> expectedVariableTypes
        = {Ensight::ScalarPerNode, Ensight::ScalarPerNode, Ensight::ScalarPerNode,
           Ensight::ScalarPerNode, Ensight::ScalarPerNode, Ensight::VectorPerNode};

    //Check correct creation of variables

    int numOfVariables = createdEnsightObj->getNumberOfVariables();

    EnsightVariableIdentifier varID;

    QString returnedVarName;
    int returnedVarDim;
    Ensight::VarTypes returnedVarType;

    QString expectedVarName;
    int expectedVarDim;
    Ensight::VarTypes expectedVarType;

    for(int i = 0; i < numOfVariables; ++i)
    {
        varID = createdEnsightObj->getVariable(i);

        returnedVarName = varID.getName();
        returnedVarDim = varID.getDim();
        returnedVarType = varID.getType();

        expectedVarName = expectedVariableNames.at(i);
        expectedVarDim =
            Ensight::varTypeDims[static_cast<int>(expectedVariableTypes.at(i))];
        expectedVarType = expectedVariableTypes.at(i);

        QCOMPARE(returnedVarName, expectedVarName);
        QCOMPARE(returnedVarDim, expectedVarDim);
        QCOMPARE(returnedVarType, expectedVarType);
    }

}

void EnsightReaderTests::QStringRead_UnsuccessfulCaseFileRead_ReturnsNullptr()
{
    QString argString = "testForUnsuccessfulCaseFileRead";

    std::unique_ptr<EnsightObj>
            returnedPointer{Ensight::Reader::read(argString, 10)};

    std::unique_ptr<EnsightObj> expectedPointer{nullptr};

    QCOMPARE(returnedPointer, expectedPointer);
}


void EnsightReaderTests::QStringRead_OutOfBoundsTimestepRequest_ReturnsNullptr()
{
    QFETCH(QString, argFilename);
    QFETCH(int, argReadTimesteps);

    std::unique_ptr<EnsightObj>
            returnedPointer{Ensight::Reader::read(argFilename, argReadTimesteps)};

    std::unique_ptr<EnsightObj> expectedReturnPointer{nullptr};

    QCOMPARE(returnedPointer, expectedReturnPointer);
}

/*
    Corresponding test files:   QStringRead_outOfBoundsTimestep01.case
                                QStringRead_outOfBoundsTimestep02.case


    Testfile 01 sets the value of the "number of steps" property to 1 and
    consequently only assigns a single value to the "time values" property.
    During the reading process, this leads to the isTransient variable
    turning false.

    Testfile 02 sets the value of the "number of steps" property to 2 and
    assigns two values to the "time values" property. Reading this file sets
    the isTransient variable to true.


    Tested Logic:

    readTimeStep >= caseFile.timesteps.rows() &&
    ( isTransient || readTimeStep > 0 )


    Note:

    It is impossible for the combination of

        readTimeStep >= caseFile.timesteps.rows() -> true
    and
        readTimeStep > 0                          -> false

    to occur. For this to happen, readTimeStep would have to be
    greater or equal to 1 (smallest possible value for vector row count)
    and lesser or equal to 0 at the same time:

    readTimeStep >= 1 && readTimeStep <= 0  # can never be true

    Therefore only the possible values of the isTransient variable are
    tested.
*/
void EnsightReaderTests::QStringRead_OutOfBoundsTimestepRequest_ReturnsNullptr_data()
{

    QTest::addColumn<QString>("argFilename");
    QTest::addColumn<int>("argReadTimesteps");


    QTest::newRow("SecondSubPredicateIsTransientFalse")
            << relativeInvalidFilesDirPath_ +
                "QStringRead/QStringRead_outOfBoundsTimestep01.case"
            << 1;

    QTest::newRow("SecondSubPredicateIsTransientTrue")
            << relativeInvalidFilesDirPath_ +
               "QStringRead/QStringRead_outOfBoundsTimestep02.case"
            << 2;
}

void EnsightReaderTests::QStringRead_OutOfBoundsTimestepRequest_SetsCorrectErrorString()
{
    QFETCH(QString, argFilename);
    QFETCH(int, argReadTimesteps);

    auto successfulRead = Ensight::Reader::read(argFilename, argReadTimesteps);

    QVERIFY(!successfulRead);

    QString setErrorString = EnsightObj::ERROR_STR;

    QString expectedErrorString =
            QString("In [EnsightReader::read()] requested time step %1 "
                    "(index %2) but file contains only %3 time steps.")
                .arg(argReadTimesteps + 1)
                .arg(argReadTimesteps)
                .arg(argReadTimesteps);

    QCOMPARE(setErrorString, expectedErrorString);
}

void EnsightReaderTests::QStringRead_OutOfBoundsTimestepRequest_SetsCorrectErrorString_data()
{
    //Previous data set it used due to matching error patterns
    QStringRead_OutOfBoundsTimestepRequest_ReturnsNullptr_data();
}

void EnsightReaderTests::QStringRead_FaultyWildcards_ReturnsNullptr()
{
    QString argFileName
        {relativeInvalidFilesDirPath_ +
                "QStringRead/QStringRead_FaultyWildcards.case"};

    /*  Parameter readTimeStep is set to 1
        -> readTransientAsStatic == true
        -> step = readTimeStep
        -> fileNumber = readTimeStep

        Allows control over second argument of checkWildcards()
        through the readTimeStep parameter
    */
    int argReadTimestep{1};

    std::unique_ptr<EnsightObj> returnedPointer
        {Ensight::Reader::read(argFileName, argReadTimestep)};

    std::unique_ptr<EnsightObj> expectedPointer{nullptr};

    QCOMPARE(returnedPointer, expectedPointer);
}

void EnsightReaderTests::QStringRead_FaultyWildcards_SetsCorrectErrorString()
{
    QString argFileName
        {relativeInvalidFilesDirPath_ +
                "QStringRead/QStringRead_FaultyWildcards.case"};

    /*
        Parameter readTimeStep is set to 1
        -> readTransientAsStatic == true
        -> step = readTimeStep
        -> fileNumber = readTimeStep

        Allows control over second argument of checkWildcards()
        through the readTimeStep parameter
    */
    int argReadTimestep{1};

    std::unique_ptr<EnsightObj> successfulRead
        {Ensight::Reader::read(argFileName, argReadTimestep)};

    QVERIFY(!successfulRead);


    QString setErrorString = EnsightObj::ERROR_STR;

    QString expectedErrorString =
        "In [EnsightReader::read()] for timestep = " +
        QString::number(argReadTimestep) + ". Something wrong with the "
        "wildcards or the corresponding time step for geometry model.";

    QCOMPARE(setErrorString, expectedErrorString);
}

/*
    Corresponding test file: QStringRead_UnsupportedFortranFormat.case
                             FORTRAN.geo

    Testfile links to the FORTRAN.geo file trough the "model" property of the
    GEOMETRY section. On the first line of the FORTRAN.geo file, the fortran
    format is specified. During the reading of the case file, this is caught
    using the getEnsightFileType function. The value FileType::Fortran_Binary
    is assigned to the variable model, triggering the UnsupportedFortranFormat
    error.
*/
void EnsightReaderTests::QStringRead_UnsupportedFortranFormat_ReturnsNullptr()
{
    QString argFileName
        {relativeInvalidFilesDirPath_ +
                "QStringRead/QStringRead_UnsupportedFortranFormat.case"};

    int argReadTimestep{1};

    std::unique_ptr<EnsightObj> returnedPointer
        {Ensight::Reader::read(argFileName, argReadTimestep)};

    std::unique_ptr<EnsightObj> expectedPointer{nullptr};

    QCOMPARE(returnedPointer, expectedPointer);
}

/*
    Corresponding test file: QStringRead_UnsupportedFortranFormat.case
                             FORTRAN.geo

    Testfile links to the FORTRAN.geo file trough the "model" property of the
    GEOMETRY section. On the first line of the FORTRAN.geo file, the fortran
    format is specified. During the reading of the case file, this is caught
    using the getEnsightFileType function. The value FileType::Fortran_Binary
    is assigned to the variable model, triggering the UnsupportedFortranFormat
    error.
*/
void EnsightReaderTests::QStringRead_UnsupportedFortranFormat_SetsCorrectErrorString()
{
    QString argFileName
        {relativeInvalidFilesDirPath_ +
                "QStringRead/QStringRead_UnsupportedFortranFormat.case"};

    int argReadTimestep{1};

    std::unique_ptr<EnsightObj> successfulRead
        {Ensight::Reader::read(argFileName, argReadTimestep)};

    QVERIFY(!successfulRead);

    QString setErrorString = EnsightObj::ERROR_STR;

    QString expectedErrorString =
            "In [EnsightReader::read()] Format "
            "\"Fortran Binary\" is not supported.";

    QCOMPARE(setErrorString, expectedErrorString);
}
/*
    Corresponding test files: QStringRead_UnsuccessfulBinaryGeometryRead.case
                              CBINARY.geo

    Testfile links to the CBINARY.geo file trough the "model" property of the
    GEOMETRY section. On the first line of the CBINARY.geo file, the C binary
    format is specified. During the reading of the case file, the
    readBinaryGeometry() function is called unsuccessfully, returning false and,
    through the negation of the if predicate, triggering the
    UnsuccessfulBinaryGeometryRead error.
*/
void EnsightReaderTests::QStringRead_UnsuccessfulBinaryGeometryRead_ReturnsNullptr()
{
    QString argFileName
        {relativeInvalidFilesDirPath_ +
                "QStringRead/QStringRead_UnsuccessfulBinaryGeometryRead.case"};

    int argReadTimestep{2};

    std::unique_ptr<EnsightObj> returnedPointer
        {Ensight::Reader::read(argFileName, argReadTimestep)};

    std::unique_ptr<EnsightObj> expectedPointer{nullptr};

    QCOMPARE(returnedPointer, expectedPointer);
}

/*
    Corresponding test files: QStringRead_UnsuccessfulASCIIGeometryRead.case
                              ASCII.geo

    Testfile links to the ASCII.geo file trough the "model" property of the
    GEOMETRY section. On the first line of the ASCII.geo file, the ASCII
    format is specified. During the reading of the case file, the
    readAsciiGeometry() function is called unsuccessfully, returning false and,
    through the negation of the if predicate, triggering the
    UnsuccessfulAsciiGeometryRead error.
*/
void EnsightReaderTests::QStringRead_UnsuccesfulAsciiGeomteryRead_ReturnsNullptr()
{
    QString argFileName
        {relativeInvalidFilesDirPath_ +
                "QStringRead/QStringRead_UnsuccessfulASCIIGeometryRead.case"};

    int argReadTimestep{2};

    std::unique_ptr<EnsightObj> returnedPointer
        {Ensight::Reader::read(argFileName, argReadTimestep)};

    std::unique_ptr<EnsightObj> expectedPointer{nullptr};

    QCOMPARE(returnedPointer, expectedPointer);
}

void EnsightReaderTests::QStringRead_UnableToCreateVariable_ReturnsNullptr()
{
    QString argFileName
        {relativeInvalidFilesDirPath_ +
              "QStringRead/QStringRead_UnableToCreateVariable.case"};

    int argTimestep{2};

    std::unique_ptr<EnsightObj>returnedPointer
        {Ensight::Reader::read(argFileName, argTimestep)};

    std::unique_ptr<EnsightObj>expectedPointer{nullptr};

    QCOMPARE(returnedPointer, expectedPointer);
}

void EnsightReaderTests::QStringRead_UnableToCreateVariable_SetsCorrectErrorString()
{
    QString argFileName
        {relativeInvalidFilesDirPath_ +
              "QStringRead/QStringRead_UnableToCreateVariable.case"};

    int argTimestep{0};

    std::unique_ptr<EnsightObj>successfulReadPointer
        {Ensight::Reader::read(argFileName, argTimestep)};

    QVERIFY(!successfulReadPointer);

    QString setErrorString{EnsightObj::ERROR_STR};

    QString expectedErrorString01
            =   "In [EnsightAsciiReader::readGeometry()] Invalid file header.";

    QString expectedErrorString02
            =   "In [EnsightReader::read()] for timestep = " +
                QString::number(0) +
                ". Cannot create variable with name density";

    QVERIFY(setErrorString.compare(expectedErrorString01) == 0 ||
            setErrorString.compare(expectedErrorString02) == 0);

}

void EnsightReaderTests::CheckWildcards_Predicates_ReturnsCorrectly()
{
    QFETCH(QString, argFilename);
    QFETCH(int, argTimestep);
    QFETCH(bool, expectedReturnValue);

    bool returnedValue =
            Ensight::Reader::detail::checkWildcards(argFilename, argTimestep);

    QCOMPARE(returnedValue, expectedReturnValue);
}

/*
    Tested Logic:

    ( filename is QString parameter -> argFilename,
      timestep is int parameter -> argTimestep )

    int numWildcards = filename.count("*");
    int firstWildCard = filename.indexOf("*");
    int lastWildCard = filename.indexOf("*", firstWildCard + numWildcards - 1);

    if ((lastWildCard - firstWildCard + 1) != numWildcards)
        return false;

    int numRequiredWildcards = timestep > 0 ? log10(double(timestep)) + 1 : 0;
    if (timestep == 0)
        numRequiredWildcards = 1;

    if ((numWildcards > 0 && timestep < 0) ||
        (numWildcards == 0 && timestep >= 0) ||
        (numRequiredWildcards > numWildcards))
        return false;

    return true;
*/
void EnsightReaderTests::CheckWildcards_Predicates_ReturnsCorrectly_data()
{
    QTest::addColumn<QString>("argFilename");
    QTest::addColumn<int>("argTimestep");
    QTest::addColumn<bool>("expectedReturnValue");

    /*
        numWildcards = 2
        firstWildCard = 0
        lastWildcard = 4

        (4 - 0 + 1) != 2
             5      != 2   -> First Predicate true -> returns false
    */
    QTest::newRow("FirstPredicateTrue")
            << "*___*"
            << 0
            << false;


    /*
        numWildcards = 3
        firstWildCard = 0
        lastWildCard = 4

        (4 - 0 + 1) == 3
            3       == 3    -> First Predicate true -> function body continues

        (3 > 0 && timestep <0) -> First Sub Predicate true -> returns false
    */
    QTest::newRow("ThirdPredicateFirstSubPredicateTrue")
            << "__*_*"
            << -1
            << false;

    QTest::newRow("ThirdPredicateSecondSubPredicateTrue")
            << "_____"
            << 1
            << false;

    QTest::newRow("ThirdPredicateThirdSubPredicateTrue")
            << "__*_*"
            << 10
            << false;
}





