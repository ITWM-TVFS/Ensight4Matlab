#include <QTest>

#include "bboxtests.h"
#include "ensightconstanttests.h"
#include "ensightreadertests.h"
#include "ensightvariabletests.h"

int main(int argc, char** argv)
{
    int returnStatus = 0;

    auto runTest =
            [&returnStatus, argc, argv](QObject&& testedClass)
                {
                    returnStatus |= QTest::qExec(&testedClass, argc, argv);
                };

    runTest(BboxTests());
    runTest(EnsightConstantTests());
    runTest(EnsightReaderTests());
    runTest(EnsightVariableTests());

    return returnStatus;
}
