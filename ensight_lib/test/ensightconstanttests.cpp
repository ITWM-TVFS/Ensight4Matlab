#include "ensightconstanttests.h"

void EnsightConstantTests::initTestCase()
{
    testEnsightConstant = EnsightConstant();
}

void EnsightConstantTests::DefaultConstructor_DefaultInit_MembersDefaultInitialized()
{
    QString defaultString{""};
    double defaultDouble{0.0};

    QCOMPARE(testEnsightConstant.getName().toStdString(), defaultString.toStdString());
    QCOMPARE(testEnsightConstant.getValue(), defaultDouble);
}

void EnsightConstantTests::ArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigned()
{
    std::string argString{"test"};
    double argDouble{1.0};

    testEnsightConstant = EnsightConstant(argString, argDouble);

    QCOMPARE(testEnsightConstant.getName().toStdString(), argString);
    QCOMPARE(testEnsightConstant.getValue(), argDouble);
}

void EnsightConstantTests::IsNull_EmptyName_ReturnsTrue()
{
    std::string argString{};

    testEnsightConstant = EnsightConstant{argString, 4.0};

    QVERIFY(testEnsightConstant.isNull());
}

