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


#include "../include/ensightobj.h"

#include <iostream>
#include <QString>
#include <QStringList>
#include "../include/ensightbarycentriccoordinates.h"
#include "../include/ensightcell.h"
#include "../include/ensightconstant.h"
#include "../include/ensightsubdivtree.h"
#include "../include/ensightsubdivtreeimpl.h"
#include "../include/ensightpart.h"
#include "../include/ensightvariable.h"


QString EnsightObj::ERROR_STR = "";

EnsightObj::EnsightObj() : edit_(false), subdivTree_()
{
    EnsightObj::ERROR_STR.clear();
}

EnsightObj::~EnsightObj() = default;

void EnsightObj::beginEdit()
{
    edit_ = true;
}

bool EnsightObj::endEdit()
{
    for (auto& part : parts_)
    {
        // Verify if the indices of all cells are correct
        for (int timestep = 0; timestep < getNumberOfTimesteps(); timestep++)
        {
            Matx vertices = part->getVertices(timestep);
            QList<EnsightCellList*> cells = part->getCells(timestep);
            for (auto cell : cells)
            {
                Ensight::Cell type = cell->getType();
                Mati values = cell->getValues();
                for (int j = 0; j < values.rows(); j++)
                {
                    for (int k = 0; k < values.cols(); k++)
                    {
                        if (values(j, k) < 0 || values(j, k) >= vertices.cols())
                        {
                            EnsightObj::ERROR_STR = "Referencing vertex with index "
                                                    + QString::number(values(j, k)) +
                                                    "in Part <" + part->getName() +
                                                    "at timestep " + QString::number(timestep) +
                                                    "for cell " + Ensight::strCell[type];
                            return false;
                        }
                    }
                }
            }

            // Verify if all variables are defined for either all vertices or for none
            for (int j = 0; j < getNumberOfVariables(); j++)
            {
                EnsightVariableIdentifier varIdent = getVariable(j);
                if (!part->hasVariable(varIdent.getName(), timestep))
                    continue;
                Matx varValues = part->getVariableValues(varIdent.getName(), timestep);
                if (varValues.cols() != vertices.cols() && varValues.cols() != 0)
                {
                    EnsightObj::ERROR_STR = "Error of variable definition of variable <" + varIdent.getName() +
                                            "> in Part <" + part->getName() +
                                            "> at timestep " + QString::number(timestep) + ". " +
                                            QString("Number of Vertices: %0, Defined variables: %1")
                                                .arg(vertices.cols())
                                                .arg(varValues.cols());
                    return false;
                }
            }
        }
    }

    edit_ = false;
    return true;
}

bool EnsightObj::inEditMode()
{
    return edit_;
}


bool EnsightObj::createVariable(const QString& name, Ensight::VarTypes type)
{
    if (!edit_)
    {
        EnsightObj::ERROR_STR = "Not in Edit Mode, call beginEdit() first";
        return false;
    }

    if (hasVariable(name))
    {
        EnsightObj::ERROR_STR = "Variable already exists";
        return false;
    }

    EnsightVariableIdentifier var(name, type);
    variables_.push_back(var);
    return true;
}

bool EnsightObj::createVariable(const std::string &name, Ensight::VarTypes type)
{
    return createVariable(QString::fromStdString(name), type);
}

EnsightPart* EnsightObj::createEnsightPart(const QString& name, int id)
{
    if (!edit_)
    {
        EnsightObj::ERROR_STR = "Not in Edit Mode, call beginEdit() first";
        return nullptr;
    }
    if (getPartByName(name) != nullptr)
    {
        EnsightObj::ERROR_STR = ("Part with name " + name + " already exists.");
        return nullptr;
    }

    if (getPartById(id) != nullptr)
    {
        EnsightObj::ERROR_STR = ("Part with id " + QString::number(id) + " already exists.");
        return nullptr;
    }
    std::unique_ptr<EnsightPart> newPart(new EnsightPart(name, id, timesteps_.rows()));
    EnsightPart* result = newPart.get();
    parts_.push_back(std::move(newPart));
    return result;
}

EnsightPart *EnsightObj::createEnsightPart(const std::string &name, int id)
{
    return createEnsightPart(QString::fromStdString(name), id);
}

EnsightPart* EnsightObj::getPart(int i)
{
    return parts_[i].get();
}

EnsightPart* EnsightObj::getPartByName(const QString& name)
{
    for (auto& part : parts_)
        if (part->getName() == name)
            return part.get();

    return nullptr;
}

EnsightPart *EnsightObj::getPartByName(const std::string &name)
{
    return getPartByName(QString::fromStdString(name));
}

EnsightPart* EnsightObj::getPartById(int id)
{
    for (auto& part : parts_)
        if (part->getId() == id)
            return part.get();

    return nullptr;
}

int EnsightObj::getNumberOfParts() const
{
    return static_cast<int>(parts_.size());
}

int EnsightObj::getMaxId() const
{
    int maxId = -1;
    for (const auto& part : parts_)
    {
        int id = part->getId();
        if (id > maxId)
            maxId = id;
    }
    return maxId;
}

bool EnsightObj::setTransient(const Vecx& values)
{
    if (!edit_)
    {
        EnsightObj::ERROR_STR = ("Not in Edit Mode, call beginEdit() first");
        return false;
    }

    timesteps_ = values;
    return true;
}

bool EnsightObj::setStatic()
{
    if (!edit_)
    {
        EnsightObj::ERROR_STR = ("Not in Edit Mode, call beginEdit() first");
        return false;
    }

    timesteps_ = Vecx::Zero(1);
    return true;
}

bool EnsightObj::isTransient() const
{
    return getNumberOfTimesteps() > 1;
}


int EnsightObj::getNumberOfTimesteps() const
{
    return timesteps_.rows();
}

const Vecx& EnsightObj::getTimesteps() const
{
    return timesteps_;
}

void EnsightObj::clean(int step)
{
    for (auto& part : parts_)
    {
        part->clean(step);
    }
}

bool EnsightObj::setVertices(EnsightPart* part, const Matx& vertices, int timestep)
{
    if (!edit_)
    {
        EnsightObj::ERROR_STR = "In [setVertices()]; Not in Edit Mode, call beginEdit() first";
        return false;
    }

    // Check if timestep is correct
    if (timestep < 0)
    {
        EnsightObj::ERROR_STR = "In [setVertices()]; Timestep must be larger or equal to zero. For static data use timestep=0.";
        return false;
    }

    if (timestep >= getNumberOfTimesteps())
    {
        EnsightObj::ERROR_STR = QString("In [setVertices()]; Timestep %0 is not defined for Part %1.").arg(timestep).arg(part->getName());
        return false;
    }

    if (vertices.rows() != 3 || vertices.cols() == 0)
    {
        EnsightObj::ERROR_STR = "In [setVertices()]; The matrix must be a 3xN matrix, N>0";
        return false;
    }

    part->setVertices(vertices, timestep);
    return true;
}

bool EnsightObj::setCells(EnsightPart* part, const Mati& cells, int timestep, Ensight::Cell e)
{
    if (!edit_)
    {
        EnsightObj::ERROR_STR = ("In [setCells()]; Not in Edit Mode, call beginEdit() first");
        return false;
    }
    if (timestep < 0)
    {
        EnsightObj::ERROR_STR = ("In [setCells()]; Timestep must be larger or equal to zero. For static data use timestep=0.");
        return false;
    }

    if (timestep >= getNumberOfTimesteps())
    {
        EnsightObj::ERROR_STR = QString("In [setCells()]; Timestep %0 is not defined for Part %1.").arg(timestep).arg(part->getName());
        return false;
    }

    if (part->hasCellType(timestep, e))
    {
        EnsightObj::ERROR_STR = ("In [setCells()]; Part " + part->getName() + " has cell types " + Ensight::strCell[e] +
                                 " already defined for timestep " + QString::number(timestep));
        return false;
    }

    part->setCells(cells, timestep, e);
    return true;
}

Bbox EnsightObj::getGeometryBounds(int timestep,
                                   const QStringList& partsToExclude) const
{
    Bbox b;
    for (auto& part : parts_)
    {
        if (partsToExclude.contains(part->getName()))
            continue;
        else
            b.extend(part->getGeometryBounds(timestep));
    }
    return b;
}

int EnsightObj::getNumberOfVariables() const
{
    return static_cast<int>(variables_.size());
}

const EnsightVariableIdentifier& EnsightObj::getVariable(int i) const
{
    return variables_[i];
}

const EnsightVariableIdentifier& EnsightObj::getVariable(const QString& name) const
{
    for (const auto& variable : variables_)
        if (variable.getName() == name)
            return variable;
    assert(false && "Variable not defined");
}

const EnsightVariableIdentifier &EnsightObj::getVariable(const std::string &name) const
{
    return getVariable(QString::fromStdString(name));
}

bool EnsightObj::hasVariable(const QString& name) const
{
    for (const auto& variable : variables_)
        if (variable.getName() == name)
            return true;
    return false;
}

bool EnsightObj::hasVariable(const std::string &name) const
{
    return hasVariable(QString::fromStdString(name));
}

bool EnsightObj::addConstant(const QString& name, double value)
{
    if (!edit_)
    {
        EnsightObj::ERROR_STR = ("Not in Edit Mode, call beginEdit() first");
        return false;
    }

    if (hasConstant(name))
    {
        EnsightObj::ERROR_STR = ("ERROR in [addConstant()] Constant with name " + name + " already defined.");
        return false;
    }

    constants_.push_back(EnsightConstant(name, value));
    return true;
}

bool EnsightObj::addConstant(const std::string &name, double value)
{
    return addConstant(QString::fromStdString(name), value);
}

bool EnsightObj::removeConstant(const QString& name)
{
    if (!edit_)
    {
        EnsightObj::ERROR_STR = "Not in Edit Mode, call beginEdit() first";
        return false;
    }

    if (!hasConstant(name))
    {
        EnsightObj::ERROR_STR = "ERROR in [addConstant()] Constant with name " + name + " not defined.";
        return false;
    }

    constants_.erase(std::remove_if(constants_.begin(), constants_.end(),
                                   [&](const EnsightConstant& c) { return c.getName() == name; }),
                    constants_.end());
    return true;
}

bool EnsightObj::removeConstant(const std::string &name)
{
    return removeConstant(QString::fromStdString(name));
}

bool EnsightObj::hasConstant(const QString& name) const
{
    for (const auto& constant : constants_)
        if (constant.getName() == name)
            return true;

    return false;
}

bool EnsightObj::hasConstant(const std::string &name) const
{
    return hasConstant(QString::fromStdString(name));
}

const EnsightConstant& EnsightObj::getConstant(const QString& name) const
{
    for (const auto& constant : constants_)
        if (constant.getName() == name)
            return constant;

    assert(false && "Constant not defined");
}

const EnsightConstant &EnsightObj::getConstant(const std::string &name) const
{
    return getConstant(QString::fromStdString(name));
}

const EnsightConstant& EnsightObj::getConstant(int i) const
{
    return constants_[i];
}

double EnsightObj::getConstantValue(const QString& name) const
{
    return getConstant(name).getValue();
}

double EnsightObj::getConstantValue(const std::string &name) const
{
    return getConstant(name).getValue();
}

Matx EnsightObj::getVariableBounds(const QString& name, int timestep) const
{
    // Calculate the min and max values for this variable over all parts
    Matx result;
    for (auto& part : parts_)
    {
        // bounds contains the min and max values for the current part for this variable
        // the minimum value is in the first col, the maximum value is in the second col
        // See EnsightPart->setVariable and EnsightVariable->setValue
        if (!part->hasVariable(name, timestep))
            continue;
        Matx bounds = part->getVariableBounds(name, timestep);
        if (result.cols() == 0)
        {
            result = bounds;
        }
        else
        {
            for (int j = 0; j < bounds.rows(); j++)
            {
                result(j, 0) = std::min(result(j, 0), bounds(j, 0));
                result(j, 1) = std::max(result(j, 1), bounds(j, 1));
            }
        }
    }
    return result;
}

Matx EnsightObj::getVariableBounds(const std::string &name, int timestep) const
{
    return getVariableBounds(QString::fromStdString(name), timestep);
}

bool EnsightObj::createSubdivTree(int maxDepth, int maxElements,
                                  const QStringList& partsToExclude,
                                  double sizeOffset)
{
    // TODO: for the moment only static data supported
    // Later one Octree for each timestep !?
    int timestep = 0;

    Bbox bounds = getGeometryBounds(-1, partsToExclude);
    double zlen = fabs(bounds.maxCorner()[2] - bounds.minCorner()[2]);
    if (zlen <= std::numeric_limits<double>::epsilon())
        // create quadtree
        subdivTree_.reset(new EnsightQuadtree(bounds, maxDepth, maxElements));
    else
        // create octree
        subdivTree_.reset(new EnsightOctree(bounds, maxDepth, maxElements));
    for (int i = 0; i < getNumberOfParts(); i++)
    {
        EnsightPart* part = getPart(i);
        if (partsToExclude.contains(part->getName()))
            continue;
        bool hasTris = part->hasCellType(timestep, Ensight::Triangle);
        bool hasBars = part->hasCellType(timestep, Ensight::Bar);
        bool extendSize = hasTris || hasBars;
        subdivTree_->insert(part, timestep, extendSize ? sizeOffset : 0.0);
    }

    return true;
}

EnsightSubdivTree *EnsightObj::getSubdivTree()
{
    return subdivTree_.get();
}

EnsightCellIdentifier* EnsightObj::interpolate(const Vec3& pos)
{
    EnsightBarycentricCoordinates baryCoord;
    EnsightCellIdentifier* cell = interpolate(pos, baryCoord);
    return cell;
}

EnsightCellIdentifier* EnsightObj::interpolate(const Vec3& pos, EnsightBarycentricCoordinates& baryCoordOut)
{
    if (!subdivTree_)
        return nullptr;
    return subdivTree_->search(pos, baryCoordOut);
}

bool EnsightObj::setVariable(EnsightPart* part, const QString& name, const Matx& values, Ensight::VarTypes type, int timestep)
{
    if (!edit_)
    {
        EnsightObj::ERROR_STR = ("In [EnsightObj::setVariable()] Not in Edit Mode, call beginEdit() first");
        return false;
    }

    // Save global
    if (this->hasVariable(name))
    {
        EnsightVariableIdentifier var = this->getVariable(name);
        if (var.getType() != type)
        {
            EnsightObj::ERROR_STR = QString("In [EnsightObj::setVariable()] Variable with name <%0> was defined with different type.").arg(name);
            return false;
        }
    }
    else
    {
        EnsightObj::ERROR_STR = QString("In [EnsightObj::setVariable()] Variable with name <%0> is not yet defined. Call EnsightObj::createVariable() first.").arg(name);
        return false;
    }


    if (timestep < 0)
    {
        EnsightObj::ERROR_STR = ("In [EnsightObj::setVariable()] Timestep must be larger or equal to zero. For static data use timestep=0.");
        return false;
    }

    if (timestep >= timesteps_.rows())
    {
        EnsightObj::ERROR_STR = QString("In [EnsightObj::setVariable()] Timestep must be smaller %0").arg(timesteps_.rows());
        return false;
    }

    if (values.rows() != 1 && type == Ensight::ScalarPerNode)
    {
        EnsightObj::ERROR_STR = ("For Variables of type ScalarPerNode a 1xN matrix,N>0, must be defined");
        return false;
    }

    if (values.rows() != 3 && type == Ensight::VectorPerNode)
    {
        EnsightObj::ERROR_STR = ("For Variables of type VectorPerNode a 3xN matrix,N>0, must be defined");
        return false;
    }

    if (part->hasVariable(name, timestep))
    {
        EnsightObj::ERROR_STR = ("Part " + part->getName() + " already has variable " + name +
                                 " defined for timestep " + QString::number(timestep));
        return false;
    }

    if (type == Ensight::VarTypeUnknown)
    {
        EnsightObj::ERROR_STR = "Cannot add Variable " + name + "to Part " + part->getName();
        return false;
    }

    if (Ensight::varTypeDims[static_cast<int>(type)] != values.rows())
    {
        EnsightObj::ERROR_STR = "Cannot add Variable " + name + "to Part " + part->getName() +
                                QString(". %0 values per node expected")
                                .arg(Ensight::varTypeDims[static_cast<int>(type)]);
        return false;
    }

    // Save local at part
    part->setVariable(name, values, type, timestep);
    return true;
}

bool EnsightObj::setVariable(EnsightPart *part, const std::string &name, const Matx &values, Ensight::VarTypes type, int timestep)
{
    return setVariable(part, QString::fromStdString(name), values, type, timestep);
}


int EnsightObj::getNumberOfConstants() const
{
    return static_cast<int>(constants_.size());
}


void EnsightObj::print(std::ostream& out) const
{
    if (edit_)
    {
        out << "Printing is not supported in Edit mode. Call endEdit() first."
            << std::endl;
        return;
    }

    out << "--------------------------------------------------\n";

    out << "-------------------- TIMESTEPS -------------------\n";
    out << timesteps_.transpose() << std::endl;
    out << std::endl;

    out << "-------------------- CONSTANTS -------------------\n";
    for (const auto& c : constants_)
    {
        out << c.getName().toStdString() << " = " << c.getValue() << std::endl;
    }
    out << std::endl;
    out << "-------------------- VARIABLES -------------------\n";
    for (int i = 0; i < getNumberOfVariables(); i++)
    {
        EnsightVariableIdentifier varId = getVariable(i);
        out << varId.getName().toStdString() << " (dim=" << varId.getDim() << ")\n";
    }

    out << std::endl;
    out << "---------------------- PARTS ---------------------\n";
    for (const auto& part : parts_)
        part->print(out);

    out << "--------------------------------------------------\n";
    out << "--------------------------------------------------" << std::endl;
}
