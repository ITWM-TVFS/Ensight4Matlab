#ifndef ENSIGHTCONSTANTTESTS_H
#define ENSIGHTCONSTANTTESTS_H

#include <QtTest/QtTest>
#include <QMetaType>

#include <iostream>

#include "ensightconstant.h"

/*
    Unit Tests for EnsightLib >> EnsightConstant
*/
class EnsightConstantTests : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    void DefaultConstructor_DefaultInit_MembersDefaultInitialized();
    void ArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigned();
    void IsNull_EmptyName_ReturnsTrue();

private:

    EnsightConstant testEnsightConstant;
};

#endif // ENSIGHTCONSTANTTESTS_H
