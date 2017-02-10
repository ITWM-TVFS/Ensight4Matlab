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


#include "../include/ensightvariable.h"


EnsightVariableIdentifier::EnsightVariableIdentifier() :
    name_(), varType_(), dim_(0)
{
}

EnsightVariableIdentifier::EnsightVariableIdentifier(const QString& name, Ensight::VarTypes varType)
{
    name_ = name;
    varType_ = varType;
    dim_ = Ensight::varTypeDims[static_cast<int>(varType)];
}

EnsightVariableIdentifier::EnsightVariableIdentifier(const std::string &name, Ensight::VarTypes varType)
{
    name_ = QString::fromStdString(name);
    varType_ = varType;
    dim_ = Ensight::varTypeDims[static_cast<int>(varType)];
}

const QString& EnsightVariableIdentifier::getName() const { return name_; }
int EnsightVariableIdentifier::getDim() const { return dim_; }
Ensight::VarTypes EnsightVariableIdentifier::getType() const { return varType_; }
bool EnsightVariableIdentifier::isNull() const { return dim_ == 0 || name_.isEmpty(); }


EnsightVariable::EnsightVariable(const QString& name, Ensight::VarTypes varType):
    EnsightVariableIdentifier(name, varType)
{
}

EnsightVariable::EnsightVariable(const std::string &name, Ensight::VarTypes varType):
    EnsightVariableIdentifier(name, varType)
{
}

void EnsightVariable::setValues(const Matx& newValues)
{
    if (newValues.rows() == 3)  // 3d variable
    {
        values_ = Matx(4, newValues.cols());
        values_.block(0, 0, 3, newValues.cols()) = newValues;

        // In the 4th row the magnitude is stored
        for (int i = 0; i < values_.cols(); i++)
            values_(3, i) = newValues.col(i).norm();

        bounds_ = Matx(4, 2);
        bounds_.col(0) = values_.rowwise().minCoeff();
        bounds_.col(1) = values_.rowwise().maxCoeff();
    }
    else if (newValues.rows() == 1)  // 1d variable
    {
        values_ = Matx(1, newValues.cols());
        values_.block(0, 0, 1, newValues.cols()) = newValues;
        bounds_ = Matx(1, 2);
        bounds_.col(0) = values_.rowwise().minCoeff();
        bounds_.col(1) = values_.rowwise().maxCoeff();
    }
}

const Matx& EnsightVariable::getValues() const
{
    return values_;
}

const Matx& EnsightVariable::getBounds() const
{
    return bounds_;
}
