#ifndef ENSIGHTREADERTESTS_H
#define ENSIGHTREADERTESTS_H

#include <QtTest/QtTest>
#include <QMetaType>
#include <QFileInfo>

#include <iostream>

#include <fstream>
#include <cstdio>
#include <memory>

#include "ensightreader.h"
#include "ensightobj.h"
#include "ensightpart.h"
#include "ensightcell.h"

#include "bbox.h"

/*
    Unit Tests for EnsightLib >> EnsightReader

    Since the EnsightReader class is used to read files of the Ensight Case
    Gold Format, multiple faulty test files are used during testing to simulate
    and trigger specific error events. These files, if necessary, can contain
    additional information and clarification of the underlying mechanisms to
    trigger certain error events.

    These files are located in the following location
    (see also EnsightReaderTests::relativeInvalidFilesDirPath_ member):

        ./customFiles/testFlowFiles/invalidFiles/EnsightReader

*/
class EnsightReaderTests : public QObject
{
    Q_OBJECT

private slots:

    void init();

    //class EnsightCase

    void DefaultConstructor_StaticValueAssignment_CorrectValuesAssigned();

    void QStringArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigend();

    void StdStringArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigend();

    void ReadCaseFile_SuccessfulCaseFileRead_CaseFileCorrectlyParsed();
    void ReadCaseFile_BadFilePath_ReturnsFalse();
    void ReadCaseFile_BadFilePath_SetsCorrectErrorString();
    void ReadCaseFile_UnequalTimesteps_ReturnsFalse();
    void ReadCaseFile_UnequalTimesteps_SetsCorrectErrorString();
    void ReadCaseFile_MissingGeometrySection_ReturnsFalse();
    void ReadCaseFile_MissingGeometrySection_SetsCorrectErrorString();

    void QStringSetMasterFileName_ValueMutator_CorrectMasterFileNameAssigned();
    void StdStringSetMasterFileName_ValueMutator_CorrectMasterFileNameAssigned();

    void GetFileNumberForStep_Predicade_ReturnsCorrectly();
    void GetFileNumberForStep_Predicade_ReturnsCorrectly_data();

    //namespace Ensight::Reader

    void QStringRead_SuccessfulCaseFileRead_CaseFileCorrectylParsed();
    void QStringRead_UnsuccessfulCaseFileRead_ReturnsNullptr();
    void QStringRead_OutOfBoundsTimestepRequest_ReturnsNullptr();
    void QStringRead_OutOfBoundsTimestepRequest_ReturnsNullptr_data();
    void QStringRead_OutOfBoundsTimestepRequest_SetsCorrectErrorString();
    void QStringRead_OutOfBoundsTimestepRequest_SetsCorrectErrorString_data();
    void QStringRead_FaultyWildcards_ReturnsNullptr();
    void QStringRead_FaultyWildcards_SetsCorrectErrorString();
    void QStringRead_UnsupportedFortranFormat_ReturnsNullptr();
    void QStringRead_UnsupportedFortranFormat_SetsCorrectErrorString();
    void QStringRead_UnsuccessfulBinaryGeometryRead_ReturnsNullptr();
    void QStringRead_UnsuccesfulAsciiGeomteryRead_ReturnsNullptr();
    void QStringRead_UnableToCreateVariable_ReturnsNullptr();
    void QStringRead_UnableToCreateVariable_SetsCorrectErrorString();

    //namespace Ensight::Reader::detail

    void CheckWildcards_Predicates_ReturnsCorrectly();
    void CheckWildcards_Predicates_ReturnsCorrectly_data();

private:

    EnsightCase testEnsightCase;

    QString relativeValidFlowFilePath_
    {"customFiles/testFlowFiles/validFiles/jet.case"};

    QString relativeInvalidFilesDirPath_
    {"customFiles/testFlowFiles/invalidFiles/EnsightReader/"};

};

#endif // ENSIGHTREADERTESTS_H
