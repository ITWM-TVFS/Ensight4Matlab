#include <iostream>
#include <memory>
#include <string>

#include "ensightlib.h"
#include "ensightobj.h"
#include "ensightpart.h"
#include "ensightvariable.h"

using std::cout;
using Vec4i = Eigen::Vector4i;

/**
 * This example demonstrates how to write a simple data set.
 *
 * It creates
 *   - an Ensight object with a single time step (i.e. static) and part
 *   - a rectangular grid of vertices
 *   - rectangular cells (quads) connecting the vertices
 *   - a variable containing the result of an example function
 * and writes the data set to a file. You can use the `ensight_viewer` GUI
 * example to inspect the resulting data set.
 *
 * You can verify that the results were written correctly by diff'ing the
 * resulting files `write_example*` with the corresponding reference files
 * in `../data`.
 */
int main(void)
{
    // Name of case file for output data
    std::string fileNameAscii = "write_example.case";
    std::string fileNameBinary = "write_example_binary.case";

    // Create object
    auto data = std::unique_ptr<EnsightObj>(EnsightLib::createEnsight());
    data->beginEdit();
    // Static data set, i.e. single time step
    data->setStatic();


    // Create a single part
    auto part = data->createEnsightPart(std::string("Part"), 0);

    // Create a grid of vertices
    const int nx = 100, ny = 100;
    Matx vertices(3, nx * ny);
    // The vertices make up a geometrically 2D grid but are stored in a
    // contiguous 1D array. Map 2D index to linear index.
    auto idx = [=](int i, int j) { return i + j * nx; };
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            vertices.col(idx(i, j)) = Vec3((double)i / nx, (double)j / ny, 0);

    const int timestep = 0;
    data->setVertices(part, vertices, timestep);

    // Create rectangular cells covering the grid
    Mati cells(4, (nx - 1) * (ny - 1));
    for (int i = 0; i < nx - 1; i++)
        for (int j = 0; j < ny - 1; j++)
            cells.col(i + j*(nx-1)) = Vec4i(idx(i, j), idx(i + 1, j),
                                            idx(i + 1, j + 1), idx(i, j + 1));
    data->setCells(part, cells, timestep, Ensight::Quadrangle);

    // Create a variable
    auto varData = [](double x, double y) { return sin(7*x)*sin(11*y); };
    Matx values(1, nx*ny);
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            values(0, idx(i, j)) = varData((double)i / nx, (double)j / ny);
    std::string varName = "wave";
    data->createVariable(varName, Ensight::ScalarPerNode);
    data->setVariable(part, varName, values, Ensight::ScalarPerNode, timestep);


    // Finish editing, validate data
    data->endEdit();

    // Write data set to file in both ASCII and binary format
    std::cout << "Writing file: " << fileNameAscii << std::endl;
    if (!EnsightLib::writeEnsight(data.get(), fileNameAscii, false))
    {
        std::cerr << "Error: Could not write file!" << std::endl;
        return 1;
    }
    std::cout << "Writing file: " << fileNameBinary << std::endl;
    if (!EnsightLib::writeEnsight(data.get(), fileNameBinary, false))
    {
        std::cerr << "Error: Could not write file!" << std::endl;
        return 1;
    }

    return 0;
}
