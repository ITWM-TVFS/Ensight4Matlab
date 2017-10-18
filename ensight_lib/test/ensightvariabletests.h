#ifndef ENSIGHTVARIABLETESTS_H
#define ENSIGHTVARIABLETESTS_H

#include <QtTest/QtTest>
#include <QMetaType>

#include <iostream>

#include "ensightvariable.h"

Q_DECLARE_METATYPE(Ensight::VarTypes)


/*
    Unit Tests for EnsightLib >> EnsightVariableIdentifier
               and EnsightLib >> EnsightVariable

    (Share one source file)
*/
class EnsightVariableTests : public QObject
{
    Q_OBJECT

private slots:

    //EnsightVariableIdentifier

    void EnsightVariableIdentifierDefaultConstructor_StaticValueAssignment_CorrectValuesAssigned();

    void EnsightVariableIdentifierQStringArgumentConstructor_ParameterAssignment_CorrectValuesAssigned();

    void EnsightVariableIdentifierStdStringArgumentConstructor_ParameterAssignment_CorrectValuesAssigned();

    void EnsightVariableIdentifierGetName_ValueObservator_CorrectNameReturned();

    void EnsightVariableIdentifierGetType_ValueObservator_CorrectTypeReturned();

    void EnsightVariableIdentifierGetDim_ValueObservator_CorrectDimensionReturned();

    void EnsightVariableIdentifierIsNull_Predicate_ReturnsCorrectly();


    //EnsightVariable

    void EnsightVariableQStringArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigned();

    void EnsightVariableStdStringArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigned();

    void EnsightVariableSetValues_ValueMutator_CorrectMatricesAssignedFor3DVariable();
    void EnsightVariableSetValues_ValueMutator_CorrectMatrixAssignedFor1DVariable();

    void EnsightVariableGetValues_ValueObservator_CorrectMatrixReturned();

    void EnsightVariableGetBounds_ValueObservator_CorrectMatrixReturned();
};

#endif // ENSIGHTVARIABLETESTS_H
