#include <iostream>
#include <memory>
#include <string>

#include "ensightlib.h"
#include "ensightobj.h"
#include "ensightpart.h"
#include "ensightvariable.h"

using std::cout;

int main(void)
{
    // Relative path to example data, make sure this exists.
    std::string fileName = "../../../data/jet.encas";
    cout << "Reading file: " << fileName << std::endl;

    // Read file.
    auto data = std::unique_ptr<EnsightObj>(EnsightLib::readEnsight(fileName));
    if (!data)
    {
        std::cerr << "Error: could not read file." << std::endl;
        return 1;
    }

    // Print some information.
    cout << "Number of time steps: " << data->getNumberOfTimesteps() << "\n";
    cout << "Number of parts: " << data->getNumberOfParts() << "\n";
    for (int i = 0; i < data->getNumberOfParts(); i++)
    {
        const auto* part = data->getPart(i);
        cout << "Part " << i + 1 << ": " << part->getName().toStdString()
             << "\n";
    }
    cout << "Number of variables: " << data->getNumberOfVariables() << "\n";
    for (int i = 0; i < data->getNumberOfVariables(); i++)
    {
        auto var = data->getVariable(i);
        cout << "Variable " << i + 1 << ": " << var.getName().toStdString()
             << "\n";
    }

    return 0;
}
