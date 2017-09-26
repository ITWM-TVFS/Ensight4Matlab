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


#ifndef ENSIGHTVARIABLE_H
#define ENSIGHTVARIABLE_H

#include <string>

#include <QString>

#include "eigentypes.h"
#include "ensightdef.h"

/**
 * @brief Identifies a variable name with an variable type.
 */
class EnsightVariableIdentifier
{
public:
    EnsightVariableIdentifier();
    /**
     * @brief Creates an EnsightVariableIdentifier.
     * @param[in] name Variable name
     * @param[in] varType Variable type (See Ensight::VarTypes)
     */
    EnsightVariableIdentifier(const QString& name, Ensight::VarTypes varType);
    EnsightVariableIdentifier(const std::string& name, Ensight::VarTypes varType);
    /**
     * @brief Get variable name.
     */
    const QString& getName() const;
    /**
     * @brief Get variable dimension.
     */
    int getDim() const;
    /**
     * @brief Get variable type.
     */
    Ensight::VarTypes getType() const;
    bool isNull() const;

protected:
    QString name_;
    Ensight::VarTypes varType_;
    int dim_;
};

/**
 * @brief  Represents an Ensight variable.
 *
 * This class inherits from EnsightVariableIdentifier.
 * Additionally, using this class, you can set the values of the
 * created EnsightVariable with setValues(...).
 */
class EnsightVariable : public EnsightVariableIdentifier
{
public:
    /**
     * @brief Creates an EnsightVariable.
     * @param[in] name Variable name
     * @param[in] varType Variable type (See Ensight::VarTypes)
     */
    EnsightVariable(const QString& name, Ensight::VarTypes varType);
    EnsightVariable(const std::string& name, Ensight::VarTypes varType);
    /**
     * @brief Set the variable values.
     *
     * For 3D variables the input is a 4xN matrix where N is the number of nodes.
     * The four values are x,y,z and norm of (x,y,z).
     * For 1D variables the input is a 1xN mattrix.
     * @param[in] newValues Value matrix
     */
    void setValues(const Matx& newValues);
    const Matx& getValues() const;
    const Matx& getBounds() const;


private:
    /**
     * @brief values for 3d variables this is a 4xN matrix, where N is the number of nodes in this part
     * The 4 values are x,y,z and norm of the vector((x,y,z))
     *
     * For 1d variables this is a 1xN matrix
     */
    Matx values_;

    /**
     * @brief minValue Either a 1x2 or a 4x2 vector, see definition of values.
     * First column contains the minimal values, second column contains the maximum
     * value
     */
    Matx bounds_;
};

#endif // ENSIGHTVARIABLE_H
