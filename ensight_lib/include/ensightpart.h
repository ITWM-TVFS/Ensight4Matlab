/*
 * Copyright (c) 2016 Fraunhofer ITWM
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef ENSIGHTPART_H
#define ENSIGHTPART_H

#include <map>
#include <memory>
#include <string>
#include <QString>
#include <QVector>

#include "eigentypes.h"
#include "ensightdef.h"

class Bbox;
class EnsightCell;
class EnsightCellList;
class EnsightVariable;

/**
 * @brief The EnsightPart class
 */
class EnsightPart
{
public:
    EnsightPart(QString name, int id, int timesteps);
    EnsightPart(std::string name, int id, int timesteps);
    ~EnsightPart();


    QString getName() const;
    int getId() const;

    /**
     * @brief Deletes data for the given step
     * @param step Timestep
     */
    void clean(int step);
    /**
     * @brief Set number of timesteps.
     * @param timesteps
     */
    void setTimeSteps(int timesteps);
    /**
     * @brief Get the number of timesteps
     */
    int getNumberOfTimesteps() const;
    /**
     * @brief Sets the vertices and boundaries at a given timestep
     * @param vertices A 3xN matrix containing N 3D vertices
     * @param timestep Timestep
     */
    void setVertices(const Matx& vertices, int timestep);
    void print() const;
    /**
     * @brief Get the number of vertices at a given timestep
     * @param timestep Timestep
     */
    int getVertexCount(int timestep) const;
    /**
     * @brief Get all vertices at a given timestep
     * @param timestep Timestep
     * @return a 3xN matrix containing N 3D vertices.
     */
    const Matx& getVertices(int timestep) const;

    /**
     * @brief Get the geometric bounds of this part.
     * @param timestep The timestep for which the bounds are determined
     * @return the geometric bounds as a BBox.
     *
     * For timestep==-1 the geometric bounds of all timesteps are computed.
     */
    Bbox getGeometryBounds(int timestep) const;

    /**
     * @brief Checks if the Ensight part contains the given cell type at timestep
     * @param timestep Timestep
     * @param c Cell Type (See Ensight::Cell)
     */
    bool hasCellType(int timestep, Ensight::Cell c) const;
    /**
     * @brief Set cells of cell type 'type' at timestep
     * @param values Cell values
     * @param timestep Timestep
     * @param type Cell Type (See Ensight::Cell)
     */
    void setCells(const Mati& values, int timestep, Ensight::Cell type);
    /**
     * @brief Get cells (of all types) at a given timestep
     * @param timestep Timestep
     * @return a QList of EnsightCellLists containing all cells of the same cell type.
     */
    QList<EnsightCellList*> getCells(int timestep);
    /**
     * @brief Get cells (of a certain type) at a given timestep
     * @param timestep Timestep
     * @param type Cell type (See Ensight::Cell)
     * @return a EnsightCellList containing all cell of a certain cell type.
     */
    EnsightCellList* getCells(int timestep, Ensight::Cell type);
    /**
     * @brief Get the BBox of cell at a given timestep
     * @param cell Cell
     * @param timestep Timestep
     */
    Bbox getCellBounds(const Veci& cell, int timestep) const;
    /**
     * @brief Sets the values of a variable at a given timestep
     * @param name Variable name
     * @param values Value matrix
     * @param type Variable type (See Ensight::VarTypes)
     * @param timestep Timestep
     */
    void setVariable(const QString& name, const Matx& values, Ensight::VarTypes type, int timestep);
    void setVariable(const std::string& name, const Matx& values, Ensight::VarTypes type, int timestep);
    /**
     * @brief Checks if the Ensight part contains a variable at a given timestep
     * @param name Variable name
     * @param timestep Timestep
     */
    bool hasVariable(const QString& name, int timestep) const;
    bool hasVariable(const std::string& name, int timestep) const;
    /**
     * @brief Get the value matrix of a variable at a given timestep
     * @param name Variable name
     * @param timestep Timestep
     */
    const Matx& getVariableValues(const QString& name, int timestep);
    const Matx& getVariableValues(const std::string& name, int timestep);
    /**
     * @brief Get the EnsightVariable class of a variable by its name
     * @param name Variable name
     * @param timestep Timestep
     */
    EnsightVariable* getVariable(const QString& name, int timestep);
    EnsightVariable* getVariable(const std::string& name, int timestep);
    /**
     * @brief Get the boundaries of a variable.
     *
     * If timestep is <0 the function will return the combined boundaries over
     * all timesteps.
     * By setting timestep to 0 or >0, you can get the boundaries of a certain timestep.
     * @param name Variable name
     * @param timestep Timestep
     */
    Matx getVariableBounds(const QString& name, int timestep);
    Matx getVariableBounds(const std::string& name, int timestep);

private:
    /**
     * @brief name_ The name of this part, e.g. the name specified in *.case file
     */
    QString name_;

    /**
     * @brief id The ID of the part, e.g. the ID specified in *.case file
     */
    int id_;

    /**
     * @brief timesteps The number of time steps of this part.
     */
    int timesteps_;

    /**
     * @brief bounds For each timestep a axes aligned bounding box.
     */
    QVector<Bbox> bounds_;

    /**
     * @brief vertices For each timestep i a 3xN_i matrix containing N_i 3d vertices
     */
    QVector<Matx> vertices_;

    /**
     * @brief variables Set of variables.
     * For each timestep we store multiple variables
     */
    std::multimap<int, std::unique_ptr<EnsightVariable>> variables_;

    /**
     * @brief cells set of cells.
     * For each time step we store multiple cells.
     */
    std::multimap<int, std::unique_ptr<EnsightCellList>> cells_;
};

#endif // ENSIGHTPART_H
