#ifndef BBOXTESTS_H
#define BBOXTESTS_H

#include <QtTest/QtTest>
#include <QMetaType>

#include <iostream>

#include "bbox.h"

Q_DECLARE_METATYPE(Vec3)
Q_DECLARE_METATYPE(Bbox)

/*
    Unit Tests for EnsightLib >> Bbox
*/
class BboxTests : public QObject
{
    Q_OBJECT

private slots:

    void init();

    void DefaultConstructor_StaticValueAssignment_CorrectValuesAssigned();

    void ArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigend();

    void IsEmpty_Predicate_ReturnsCorrectly();
    void IsEmpty_Predicate_ReturnsCorrectly_data();

    void Contains_Predicate_ReturnsCorrectly();
    void Contains_Predicate_ReturnsCorrectly_data();

    void Intersects_Predicate_ReturnsCorrectly();
    void Intersects_Predicate_ReturnsCorrectly_data();

    void Center_Calculation_CorrectCalculation();

    void Diagonal_Calculation_CorrectCalculation();

    void ExtendVec_EmptyBox_SetValuesFromParameter();
    void ExtendVec_Extending_ExtendByVector();
    void ExtendVec_Extending_ExtendByVector_data();

    void ExtendDouble_EmptyBox_SetValuesFromParameter();
    void ExtendDouble_Extending_ExtendByValue();
    void ExtendDouble_Extending_ExtendByValue_data();

    void ExtendBox_EmptyBox_SetValuesFromParameter();
    void ExtendBox_Extending_ExtendByBox();
    void ExtendBox_Extending_ExtendByBox_data();

    void IncreaseBy_CalculateNewValues_CorrectCalculation();

    void MinCorner_ValueObservator_ReturnCorrectVector();
    void MaxCorner_ValueObservator_ReturnCorrectVector();

    void Print_PrintedString_PrintCorrectString();

private:

    Bbox testBbox;
};



#endif // BBOXTESTS_H
