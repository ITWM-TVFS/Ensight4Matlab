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


#ifndef ENSIGHTOBJ_H
#define ENSIGHTOBJ_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "eigentypes.h"
#include "ensightdef.h"
#include "bbox.h"

class EnsightConstant;
class EnsightCellIdentifier;
class EnsightBarycentricCoordinates;
class EnsightSubdivTree;
class EnsightPart;
class EnsightVariableIdentifier;
class QString;
class QStringList;


/**
 * @brief Represents one Ensight File
 * @author Dr. Daniel Burkhart - Fraunhofer ITWM
 * @date 2012-06-28
 *
 * The EnsightObj class represents one Ensight File.
 */


class EnsightObj
{
public:
    /**
     * @brief Ensight constructor
     */
    EnsightObj();

    /**
     * @brief destructor
     */
    ~EnsightObj();


    /**
     * @brief Enables editing
     */
    void beginEdit();

    /**
     * @brief Disables editing and verifies all changes.
     *
     * In detail, this method checks if all parts are defined in all time steps, i.e.
     * all parts must have vertex information and cell information for all timesteps.
     * Both can be empty.
     *
     * All variables must be defined for all parts and all timesteps.
     * All variables must be defined for all vertices in the corresponding part and timestep.
     *
     * Verifies if all indices of all cells refer to a really defined vertex.
     *
     * @returns false in case of any error or true if the definiton of the Ensight Object is correct.
     *
     */
    bool endEdit();

    /**
     * @brief Checks if in edit mode
     */
    bool inEditMode();


    /**
     * @brief Creates a new Ensight part
     * @param[in] name A unique name for the new part
     * @param[in] id A unique id for the new part
     *
     * This creates a new Ensight part with given name and id.
     * If name or id are already in use an exception is thrown.
     */
    EnsightPart* createEnsightPart(const QString& name, int id);
    EnsightPart* createEnsightPart(const std::string& name, int id);


    /**
     * @brief Get the number of parts.
     */
    int getNumberOfParts() const;

    /**
     * @brief Get the largest used part id.
     */
    int getMaxId() const;


    /**
     * @brief Get the i-th part of this Ensight file.
     * @param[in] i Integer smaller than the number of parts
     *
     * Get the i-th part of this Ensight file. i must be in [0, getNumberOfParts()-1];
     */
    EnsightPart* getPart(int i);

    /**
     * @brief Get the EnsightPart with given name, NULL if not exists.
     * @param[in] name Part name
     */
    EnsightPart* getPartByName(const QString& name);
    EnsightPart* getPartByName(const std::string& name);

    /**
     * @brief Get EnsightPart with given ID, NULL if not exists.
     * @param[in] id Part ID
     */
    EnsightPart* getPartById(int id);




    /**
     * @brief Defines that this Ensight Obj is transient, with the N timesteps defined
     * in the Nx1 Vector values.
     * @param[in] values Nx1 vector containing the N timesteps
     */
    bool setTransient(const Vecx & values);

    /**
     * @brief Defines that this is an static Ensight Obj, i.e. timesteps=[0]. One time step with t=0
     */
    bool setStatic();

    /**
     * @brief Returns if this EnsightObj is transient, i.e. size(timesteps)>1
     */
    bool isTransient() const;

    /**
     * @brief Get the number of timesteps
     */
    int getNumberOfTimesteps() const;

    /**
     * @brief Get the timesteps
     */
    const Vecx& getTimesteps() const;


    /**
     * @brief Deletes all data (variables, vertices, cells) for the given timestep
     * @param step Timestep
     */
    void clean(int step);




    /**
     * @brief Sets the vertices for a part and a timestep.
     * @param[in] part Ensight part
     * @param[in] vertices A 3xN matrix
     * @param[in] timestep Timestep
     */
    bool setVertices(EnsightPart* part, const Matx& vertices, int timestep);

    /**
     * @brief Sets the cells for a part and a time step
     * @param[in] part Ensight part
     * @param[in] cells MxN matrix defining N cells with M vertices each
     * @param[in] timestep Timestep
     * @param[in] e Cell Type
     * M and e must be consistent, i.e. if e==bar2,M=2
     */
    bool setCells(EnsightPart* part, const Mati& cells, int timestep, Ensight::Cell e);

    /**
     * @brief Get the geometric bounds of the EnsightObj for a given timestep.
     *
     * Only parts with PartMode != Inactive are recognized.
     * For timestep==-1 the bounds of all timestep is calculated.
     * @param partsToExclude The names of the parts to exclude.
     */
    Bbox getGeometryBounds(int timestep,
                           const QStringList& partsToExclude) const;



    /**
     * @brief Creates a new variable with given name.
     *
     * Name must be unique.
     * @param[in] name Unique variable name
     * @param[in] type Variable type (See Ensight::VarTypes for further information)
     * @return false if the given name already exists.
     */
    bool createVariable(const QString& name, Ensight::VarTypes type);
    bool createVariable(const std::string& name, Ensight::VarTypes type);

    /**
     * @brief Get the number of variables
     */
    int getNumberOfVariables() const;

    /**
     * @brief Get the i-th variable.
     *
     * The first created variable has index zero. The last created variable has index getNumberOfVariables()-1.
     */
    const EnsightVariableIdentifier& getVariable(int i) const;

    /**
     * @brief Get the variable with given name.
     *
     * If no variable with name exists the resulting EnsightVariableIdentifier return true for isNull().
     * @param name Variable name
     */
    const EnsightVariableIdentifier& getVariable(const QString& name) const;
    const EnsightVariableIdentifier& getVariable(const std::string& name) const;

    /**
     * @brief Checks if variable with name exists
     * @param[in] name Variable name
     */
    bool hasVariable(const QString &name) const;
    bool hasVariable(const std::string &name) const;


    /**
     * @brief Sets values for a variable for a timestep with a given name.
     *
     * The matrix values must match the defined vertices of the geometry and the dimension of the variable.
     * @param[in] part Ensight Part
     * @param[in] name Variable name
     * @param[in] values Variable values
     * @param[in] type Variable type (See Ensight::VarTypes)
     * @param[in] timestep Timestep
     */
    bool setVariable(EnsightPart* part, const QString& name, const Matx& values,
                     Ensight::VarTypes type, int timestep);
    bool setVariable(EnsightPart* part, const std::string& name, const Matx& values,
                     Ensight::VarTypes type, int timestep);


    /**
     * @brief Adds a new constant with given name.
     *
     * Name must be unique.
     * @param[in] name Unique variable name
     * @param[in] value Variable value
     */
    bool addConstant(const QString& name, double value);
    bool addConstant(const std::string& name, double value);

    /**
     * @brief Removes a constant by its name.
     * @param[in] name Variable name
     */
    bool removeConstant(const QString& name);
    bool removeConstant(const std::string& name);

    /**
     * @brief Checks if a constant with given name exists.
     * @param[in] name Constant name
     */
    bool hasConstant(const QString& name) const;
    bool hasConstant(const std::string& name) const;


    /**
     * @brief Get a constant by its name.
     * @param[in] name Constant name
     */
    const EnsightConstant& getConstant(const QString& name) const;
    const EnsightConstant& getConstant(const std::string& name) const;

    /**
     * @brief getNumberOfConstants
     */
    int getNumberOfConstants() const;

    /**
     * @brief Get the i-th constant.
     */
    const EnsightConstant& getConstant(int i) const;

    /**
     * @brief Get the value of a constant constant by its name.
     * @param[in] name Constant name
     */
    double getConstantValue(const QString& name) const;
    double getConstantValue(const std::string& name) const;


    /**
     * @brief Calculates the max and min values for the variable given by its name and for the given timestep for all parts.
     *
     * If timestep=-1 the min and max value for all timesteps are calculated.
     * The result is a Nx2 matrix. In the first colums the min and in the second column the max value.
     * N is 1 for 1d variables and 4 for 3d variables. for 3d variables the 4 values are (x,y,z,norm(x,y,z))
     * @param[in] name Variable name
     * @param[in] timestep Timestep
     * @return Nx2 matrix
     */
    Matx getVariableBounds(const QString& name, int timestep) const;
    Matx getVariableBounds(const std::string& name, int timestep) const;


    /**
     * @brief Creates the SubdivTree.
     *
     * If the input is in 2D a Quadtree is created. Otherwise, an Octree is created.
     * @param[in] maxDepth Maximum depth of the SubdivTree
     * @param[in] maxElements Maximum number of SubdivTree elements
     * @param[in] partsToExclude The names of the parts to exclude
     * @param[in] sizeOffset Increases the cell boundaries.
     */
    bool createSubdivTree(int maxDepth, int maxElements,
                          const QStringList& partsToExclude,
                          double sizeOffset = 0.0);


    /**
     * @brief Get the SubdivTree.
     *
     * Returns NULL if SubdivTree doesn't exist.
     */
    EnsightSubdivTree* getSubdivTree();


    /**
     * @brief Interpolates CFD data
     * @param[in] pos Position
     */
    EnsightCellIdentifier* interpolate(const Vec3 & pos);


    /**
     * @brief Interpolates CFD data
     * @param[in] pos Position
     * @param[out] baryCoordOut Barycentric coordinates are used to check if pos is inside cell and can be retrieved here.
     */
    EnsightCellIdentifier* interpolate(const Vec3& pos, EnsightBarycentricCoordinates &baryCoordOut);

    /**
     * @brief Prints the whole information defining this Ensight object to given
     * stream
     */
    void print(std::ostream& out) const;


public:
    /** Whenever something goes wrong the error message is here. */
    static QString ERROR_STR;


private:

    /** The timesteps; If static this is 1x1
     *
     * vector with value zero, else it is an Nx1 vector with N timesteps */
    Vecx timesteps_;

    /** Flag indicating edit mode; Adding parts, variables, etc is only possible in edit mode. */
    bool edit_;

    /** List of parts */
    std::vector<std::unique_ptr<EnsightPart>> parts_;

    /** List of constants */
    std::vector<EnsightConstant> constants_;

    /** List of Variables, variables are tuples <name, dim> */
    std::vector<EnsightVariableIdentifier> variables_;

    /** Object representing the SubdivTree, see createSubdivTree() */
    std::unique_ptr<EnsightSubdivTree> subdivTree_;
};

#endif // ENSIGHTOBJ_H
