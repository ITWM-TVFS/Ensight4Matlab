#include <iostream>
#include <memory>
#include <string>
#include <QStringList>

#include "ensightlib.h"
#include "ensightobj.h"
#include "ensightbarycentriccoordinates.h"
#include "ensightcell.h"

using std::cout;

int main(void)
{
    // Relative path to example data, make sure this exists.
    std::string fileName = "../../../data/jet.encas";
    std::cout << "Reading file: " << fileName << std::endl;

    // Read file.
    auto data = std::unique_ptr<EnsightObj>(EnsightLib::readEnsight(fileName));
    if (!data)
    {
        std::cerr << "Error: could not read file." << std::endl;
        return 1;
    }

    // Create spatial subdivision data structure
    data->createSubdivTree(5, 50, QStringList());

    // Interpolate variable
    Vec3 position(0, 0, 0);
    std::string variable = "temperature";
    EnsightBarycentricCoordinates coords;
    const auto* cell = data->interpolate(position, coords);
    auto result = coords.evaluate(cell->getValues(variable));
    std::cout << "Value of variable '" << variable << "' at position ("
              << position.transpose() << "): " << result << "\n";

    return 0;
}
