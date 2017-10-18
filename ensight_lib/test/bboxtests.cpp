#include "bboxtests.h"


void BboxTests::init()
{
    testBbox = Bbox();
}


void BboxTests::DefaultConstructor_StaticValueAssignment_CorrectValuesAssigned()
{
    Vec3 min = testBbox.minCorner();
    Vec3 max = testBbox.maxCorner();

    QCOMPARE(Vec3::Constant(1e12).eval(), min);
    QCOMPARE(Vec3::Constant(-1e12).eval(), max);
}

void BboxTests::ArgumentConstructor_ParameterValueAssignment_CorrectValuesAssigend()
{
    Vec3 min{1, 2, 3};
    Vec3 max{4, 5, 6};

    testBbox = Bbox(min, max);

    QCOMPARE(testBbox.minCorner(), min);
    QCOMPARE(testBbox.maxCorner(), max);
}


void BboxTests::IsEmpty_Predicate_ReturnsCorrectly()
{
    QFETCH(Vec3, minVector);
    QFETCH(Vec3, maxVector);
    QFETCH(bool, expectedResult);

    testBbox = Bbox(minVector, maxVector);

    QCOMPARE(testBbox.isEmpty(), expectedResult);
}

void BboxTests::IsEmpty_Predicate_ReturnsCorrectly_data()
{
    /* Tested Logic:

        min_[0] > max_[0] || min_[1] > max_[1] || min_[2] > max_[2];

    */

    Vec3 max{2, 2, 2};

    QTest::addColumn<Vec3>("minVector");
    QTest::addColumn<Vec3>("maxVector");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("FirstSubPredicateTrue")
            << Vec3{3, 1, 1}
            << max
            << true;
    QTest::newRow("SecondSubPredicateTrue")
            << Vec3{1, 3, 1}
            << max
            << true;
    QTest::newRow("ThirdSubPredicateTrue")
            << Vec3{1, 1, 3}
            << max
            << true;
    QTest::newRow("AllSubPredicatesTrue")
            << Vec3{3, 3, 3}
            << max
            << true;

    QTest::newRow("FirstSubPredicateFalse")
            << Vec3{1, 3, 3}
            << max
            << true;
    QTest::newRow("SecondSubPredicateFalse")
            << Vec3{3, 1, 3}
            << max
            << true;
    QTest::newRow("ThirdSubPredicateFalse")
            << Vec3{3, 3, 1}
            << max
            << true;
    QTest::newRow("AllSubPredicatesFalse")
            << Vec3{1, 1, 1}
            << max
            << false;
}


void BboxTests::Contains_Predicate_ReturnsCorrectly()
{
    QFETCH(Vec3, minVector);
    QFETCH(Vec3, maxVector);
    QFETCH(Vec3, argVector);
    QFETCH(bool, expectedResult);

    testBbox = Bbox(minVector, maxVector);
    bool localResult = testBbox.contains(argVector);

    QCOMPARE(localResult, expectedResult);
}

void BboxTests::Contains_Predicate_ReturnsCorrectly_data()
{
    /* Tested Logic:

          pos[0] >= min_[0] &&
          pos[1] >= min_[1] &&
          pos[2] >= min_[2] &&
          pos[0] <= max_[0] &&
          pos[1] <= max_[1] &&
          pos[2] <= max_[2];

    */

    Vec3 min{2, 2, 2};
    Vec3 max{4, 4, 4};


    QTest::addColumn<Vec3>("minVector");
    QTest::addColumn<Vec3>("maxVector");
    QTest::addColumn<Vec3>("argVector");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("FirstIndexTrue")
            << min
            << max
            << Vec3{3, 1, 1} << false;
    QTest::newRow("SecondIndexTrue")
            << min
            << max
            << Vec3{1, 3, 1} << false;
    QTest::newRow("ThirdIndexTrue")
            << min
            << max
            << Vec3{1, 1, 3} << false;
    QTest::newRow("AllIndicesTrue")
            << min
            << max
            << Vec3{3, 3, 3} << true;

    QTest::newRow("FirstIndexFalse")
            << min
            << max
            << Vec3{1, 3, 3} << false;
    QTest::newRow("SecondIndexFalse")
            << min
            << max
            << Vec3{3, 1, 3} << false;
    QTest::newRow("ThirdIndexFalse")
            << min
            << max
            << Vec3{3, 3, 1} << false;
    QTest::newRow("AllIndicesFalse")
            << min
            << max
            << Vec3{1, 1, 1} << false;
}


void BboxTests::Intersects_Predicate_ReturnsCorrectly()
{
    QFETCH(Bbox, localBox);
    QFETCH(Bbox, argBox);
    QFETCH(bool, expectedResult);

    testBbox = localBox;
    bool localResult = testBbox.intersects(argBox);

    QVERIFY(localResult == expectedResult);
}

void BboxTests::Intersects_Predicate_ReturnsCorrectly_data()
{
    /* Tested Logic:

          minCorner()[0] <= other.maxCorner()[0] &&
          maxCorner()[0] >= other.minCorner()[0] &&
          minCorner()[1] <= other.maxCorner()[1] &&
          maxCorner()[1] >= other.minCorner()[1] &&
          minCorner()[2] <= other.maxCorner()[2] &&
          maxCorner()[2] >= other.minCorner()[2]

    */

    QTest::addColumn<Bbox>("localBox");
    QTest::addColumn<Bbox>("argBox");
    QTest::addColumn<bool>("expectedResult");

    Bbox localBox{Vec3{2, 2, 2}, Vec3{4, 4, 4}};

    QTest::newRow("FirstIndexTrue")
            << localBox
            << Bbox{Vec3{3, 5, 5}, Vec3{3, 1, 1}}
            << false;
    QTest::newRow("SecondIndexTrue")
            << localBox
            << Bbox{Vec3{5, 3, 5}, Vec3{1, 3, 1}}
            << false;
    QTest::newRow("ThirdIndexTrue")
            << localBox
            << Bbox{Vec3{5, 5, 3}, Vec3{1, 1, 3}}
            << false;
    QTest::newRow("AllIndicesTrue")
            << localBox
            << Bbox{Vec3{3, 3, 3}, Vec3{3, 3, 3}}
            << true;

    QTest::newRow("FirstIndexFalse")
            << localBox
            << Bbox{Vec3{5, 3, 3}, Vec3{1, 3, 3}}
            << false;
    QTest::newRow("SecondIndexFalse")
            << localBox
            << Bbox{Vec3{3, 5, 3}, Vec3{3, 1, 3}}
            << false;
    QTest::newRow("ThirdIndexFalse")
            << localBox
            << Bbox{Vec3{3, 3, 5}, Vec3{3, 3, 1}}
            << false;
    QTest::newRow("AllIndicesFalse")
            << localBox
            << Bbox{Vec3{5, 5, 5}, Vec3{1, 1, 1}}
            << false;
}


void BboxTests::Center_Calculation_CorrectCalculation()
{
    Vec3 min{20, 0, 15};
    Vec3 max{40, 50, 45};

    testBbox = Bbox{min, max};
    Vec3 resultVector = testBbox.center();
    Vec3 correctResultVector = {30, 25, 30};

    QVERIFY(resultVector == correctResultVector);
}


void BboxTests::Diagonal_Calculation_CorrectCalculation()
{
    Vec3 min{20, 30, 40};
    Vec3 max{40, 55, 67};

    testBbox = Bbox{min, max};
    Vec3 resultVector = testBbox.diagonal();
    Vec3 correctResultVector = {20, 25, 27};

    QVERIFY(resultVector == correctResultVector);
}


void BboxTests::ExtendVec_EmptyBox_SetValuesFromParameter()
{
    Vec3 arg = {10, 22, 17};

    testBbox = Bbox{};

    QVERIFY(testBbox.isEmpty());

    testBbox.extend(arg);

    QVERIFY(testBbox.minCorner() == arg &&
            testBbox.maxCorner() == arg);
}

void BboxTests::ExtendVec_Extending_ExtendByVector()
{
    QFETCH(Bbox, localBox);
    QFETCH(Vec3, argVector);
    QFETCH(Bbox, expectedResultBox);

    localBox.extend(argVector);

    QVERIFY(localBox.minCorner() == expectedResultBox.minCorner() &&
            localBox.maxCorner() == expectedResultBox.maxCorner());
}

void BboxTests::ExtendVec_Extending_ExtendByVector_data()
{
    /* Tested Logic:

         min_[0] = std::min(min_[0], v[0]);
         min_[1] = std::min(min_[1], v[1]);
         min_[2] = std::min(min_[2], v[2]);

         max_[0] = std::max(max_[0], v[0]);
         max_[1] = std::max(max_[1], v[1]);
         max_[2] = std::max(max_[2], v[2]);

    */

    QTest::addColumn<Bbox>("localBox");
    QTest::addColumn<Vec3>("argVector");
    QTest::addColumn<Bbox>("expectedResultBox");

    Bbox localBox{Vec3{20, 22, 24}, Vec3{40, 43, 45}};

    QTest::newRow("localFirstIndexIsMinimum")
            << localBox
            << Vec3{30, 10, 10}
            << Bbox{Vec3{20, 10, 10}, Vec3{40, 43, 45}};
    QTest::newRow("localSecondIndexIsMinimum")
            << localBox
            << Vec3{10, 30, 10}
            << Bbox{Vec3{10, 22, 10}, Vec3{40, 43, 45}};
    QTest::newRow("localThirdIndexIsMinimum")
            << localBox
            << Vec3{10, 10, 30}
            << Bbox{Vec3{10, 10, 24}, Vec3{40, 43, 45}};

    QTest::newRow("localFirstIndexIsMaximum")
            << localBox
            << Vec3{30, 50, 50}
            << Bbox{Vec3{20, 22, 24}, Vec3{40, 50, 50}};
    QTest::newRow("localSecondIndexIsMaximum")
            << localBox
            << Vec3{50, 30, 50}
            << Bbox{Vec3{20, 22, 24}, Vec3{50, 43, 50}};
    QTest::newRow("localThirdIndexIsMaximum")
            << localBox
            << Vec3{50, 50, 30}
            << Bbox{Vec3{20, 22, 24}, Vec3{50, 50, 45}};

    QTest::newRow("argFirstIndexIsMinimum")
            << localBox
            << Vec3{10, 30, 30}
            << Bbox{Vec3{10, 22, 24}, Vec3{40, 43, 45}};
    QTest::newRow("argSecondIndexIsMinimum")
            << localBox
            << Vec3{30, 10, 30}
            << Bbox{Vec3{20, 10, 24}, Vec3{40, 43, 45}};
    QTest::newRow("argThirdIndexIsMinimum")
            << localBox
            << Vec3{30, 30, 10}
            << Bbox{Vec3{20, 22, 10}, Vec3{40, 43, 45}};

    QTest::newRow("argFirstIndexIsMaximum")
            << localBox
            << Vec3{50, 30, 30}
            << Bbox{Vec3{20, 22, 24}, Vec3{50, 43, 45}};
    QTest::newRow("argSecondIndexIsMaximum")
            << localBox
            << Vec3{30, 50, 30}
            << Bbox{Vec3{20, 22, 24}, Vec3{40, 50, 45}};
    QTest::newRow("argThirdIndexIsMaximum")
            << localBox
            << Vec3{30, 30, 50}
            << Bbox{Vec3{20, 22, 24}, Vec3{40, 43, 50}};
}


void BboxTests::ExtendDouble_EmptyBox_SetValuesFromParameter()
{
    Vec3 expectedResultVector = {4.0, 0.0, 0.0};
    double extendValue = 4.0;
    testBbox = Bbox{};

    QVERIFY(testBbox.isEmpty());

    testBbox.extend(extendValue);

    QVERIFY(testBbox.minCorner() == expectedResultVector &&
            testBbox.maxCorner() == expectedResultVector) ;
}

void BboxTests::ExtendDouble_Extending_ExtendByValue()
{
    QFETCH(Bbox, localBox);
    QFETCH(double, argValue);
    QFETCH(Bbox, expectedResultBox);

    QVERIFY(!localBox.isEmpty());

    localBox.extend(argValue);

    QVERIFY(localBox.minCorner() == expectedResultBox.minCorner() &&
            localBox.maxCorner() == expectedResultBox.maxCorner());
}

void BboxTests::ExtendDouble_Extending_ExtendByValue_data()
{
    QTest::addColumn<Bbox>("localBox");
    QTest::addColumn<double>("argValue");
    QTest::addColumn<Bbox>("expectedResultBox");

    Bbox localBox{Vec3{2, 2, 2}, Vec3{4, 4, 4}};

    QTest::newRow("localIndicesMinAndMax")
            << localBox
            << 3.0
            << localBox;
    QTest::newRow("argValueIsMinimum")
            << localBox
            << 1.0
            << Bbox{Vec3{1, 2, 2}, Vec3{4, 4, 4}};
    QTest::newRow("argValueIsMaximum")
            << localBox
            << 5.0
            << Bbox{Vec3{2, 2, 2}, Vec3{5, 4, 4}};
}


void BboxTests::ExtendBox_EmptyBox_SetValuesFromParameter()
{
    Bbox localBox{};
    Bbox argBox{Vec3{1, 2, 3}, Vec3{4, 5, 6}};

    QVERIFY(localBox.isEmpty());

    localBox.extend(argBox);

    QVERIFY(localBox.minCorner() == argBox.minCorner() &&
            localBox.maxCorner() == argBox.maxCorner());

}

void BboxTests::ExtendBox_Extending_ExtendByBox()
{
    QFETCH(Bbox, localBox);
    QFETCH(Bbox, argBox);
    QFETCH(Bbox, expectedResultBox);

    testBbox = localBox;

    QVERIFY(!testBbox.isEmpty());

    testBbox.extend(argBox);

    QVERIFY(testBbox.minCorner() == expectedResultBox.minCorner() &&
            testBbox.maxCorner() == expectedResultBox.maxCorner());

}

void BboxTests::ExtendBox_Extending_ExtendByBox_data()
{
    QTest::addColumn<Bbox>("localBox");
    QTest::addColumn<Bbox>("argBox");
    QTest::addColumn<Bbox>("expectedResultBox");

    Bbox localBox{Vec3{2, 2, 2}, Vec3{4, 4, 4}};

    QTest::newRow("localFirstIndexIsMinimum")
            << localBox
            << Bbox{Vec3{3, 1, 1}, Vec3{3, 3, 3}}
            << Bbox{Vec3{2, 1, 1}, Vec3{4, 4, 4}};
    QTest::newRow("localSecondIndexIsMinimum")
            << localBox
            << Bbox{Vec3{1, 3, 1}, Vec3{3, 3, 3}}
            << Bbox{Vec3{1, 2, 1}, Vec3{4, 4, 4}};
    QTest::newRow("localThirdIndexIsMinimum")
            << localBox
            << Bbox{Vec3{1, 1, 3}, Vec3{3, 3, 3}}
            << Bbox{Vec3{1, 1, 2}, Vec3{4, 4, 4}};

    QTest::newRow("localFirstIndexIsMaximum")
            << localBox
            << Bbox{Vec3{3, 3, 3}, Vec3{3, 5, 5}}
            << Bbox{Vec3{2, 2, 2}, Vec3{4, 5, 5}};
    QTest::newRow("localSecondIndexIsMaximum")
            << localBox
            << Bbox{Vec3{3, 3, 3}, Vec3{5, 3, 5}}
            << Bbox{Vec3{2, 2, 2}, Vec3{5, 4, 5}};
    QTest::newRow("localThirdIndexIsMaximum")
            << localBox
            << Bbox{Vec3{3, 3, 3}, Vec3{5, 5, 3}}
            << Bbox{Vec3{2, 2, 2}, Vec3{5, 5, 4}};

    QTest::newRow("argFirstIndexIsMinimum")
            << localBox
            << Bbox{Vec3{1, 3, 3}, Vec3{3, 3, 3}}
            << Bbox{Vec3{1, 2, 2}, Vec3{4, 4, 4}};
    QTest::newRow("argSecondIndexIsMinimum")
            << localBox
            << Bbox{Vec3{3, 1, 3}, Vec3{3, 3, 3}}
            << Bbox{Vec3{2, 1, 2}, Vec3{4, 4, 4}};
    QTest::newRow("argThirdIndexIsMinimum")
            << localBox
            << Bbox{Vec3{3, 3, 1}, Vec3{3, 3, 3}}
            << Bbox{Vec3{2, 2, 1}, Vec3{4, 4, 4}};

    QTest::newRow("argFirstIndexIsMaximum")
            << localBox
            << Bbox{Vec3{3, 3, 3}, Vec3{5, 3, 3}}
            << Bbox{Vec3{2, 2, 2}, Vec3{5, 4, 4}};
    QTest::newRow("argSecondIndexIsMaximum")
            << localBox
            << Bbox{Vec3{3, 3, 3}, Vec3{3, 5, 3}}
            << Bbox{Vec3{2, 2, 2}, Vec3{4, 5, 4}};
    QTest::newRow("argThirdIndexIsMaximum")
            << localBox
            << Bbox{Vec3{3, 3, 3}, Vec3{3, 3, 5}}
            << Bbox{Vec3{2, 2, 2}, Vec3{4, 4, 5}};
}


void BboxTests::IncreaseBy_CalculateNewValues_CorrectCalculation()
{
    testBbox = Bbox{Vec3{10, 12, 14}, Vec3{21, 23, 25}};

    QVERIFY(!testBbox.isEmpty());

    testBbox.increaseBy(5.0);

    Vec3 expectedMin{5, 7, 9};
    Vec3 expectedMax{26, 28, 30};

    QVERIFY(testBbox.minCorner() == expectedMin &&
            testBbox.maxCorner() == expectedMax);
}


void BboxTests::MinCorner_ValueObservator_ReturnCorrectVector()
{
    Vec3 expectedMinVector{1, 2, 3};

    testBbox = Bbox{expectedMinVector, Vec3{4, 5, 6}};

    QVERIFY(testBbox.minCorner() == expectedMinVector);
}

void BboxTests::MaxCorner_ValueObservator_ReturnCorrectVector()
{
    Vec3 expectedMaxVector{4, 5, 6};

    testBbox = Bbox{Vec3{1, 2, 3}, expectedMaxVector};

    QVERIFY(testBbox.maxCorner() == expectedMaxVector);
}


void BboxTests::Print_PrintedString_PrintCorrectString()
{
    testBbox = Bbox{Vec3{10, 10, 10}, Vec3{20, 20, 20}};

    std::ostringstream strStream;

    //Printing method output to local stringsream
    testBbox.print(strStream);

    //Testing for correct string printed
    std::string expectedText{ "10 10 10    /    20 20 20\n" };
    std::string printedText{ strStream.str() };

    QCOMPARE(printedText, expectedText);
}



