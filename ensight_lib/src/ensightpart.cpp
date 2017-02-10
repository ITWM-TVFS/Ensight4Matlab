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


#include "../include/ensightpart.h"

#include <iostream>
#include <utility>
#include "../include/bbox.h"
#include "../include/ensightcell.h"
#include "../include/ensightvariable.h"

using std::unique_ptr;
using std::make_pair;

EnsightPart::EnsightPart(QString name, int id, int timesteps) :
    name_(name), id_(id), timesteps_(timesteps)
{
    vertices_.resize(timesteps);
    bounds_.resize(timesteps);
}

EnsightPart::EnsightPart(std::string name, int id, int timesteps) :
    name_(QString::fromStdString(name)), id_(id), timesteps_(timesteps)
{
    vertices_.resize(timesteps);
    bounds_.resize(timesteps);
}

EnsightPart::~EnsightPart() = default;

QString EnsightPart::getName() const
{
    return name_;
}

int EnsightPart::getId() const
{
    return id_;
}

void EnsightPart::clean(int step)
{
    if (vertices_.size() > step)
        vertices_[step].resize(0, 0);
    variables_.erase(step);
    cells_.erase(step);
}

void EnsightPart::setTimeSteps(int timesteps)
{
    timesteps_ = timesteps;
    vertices_.resize(timesteps);
    bounds_.resize(timesteps);
}

int EnsightPart::getNumberOfTimesteps() const
{
    return timesteps_;
}

void EnsightPart::setVertices(const Matx& vertices, int timestep)
{
    this->vertices_[timestep] = vertices;
    this->bounds_[timestep] = Bbox(vertices.rowwise().minCoeff(),
                                   vertices.rowwise().maxCoeff());
}

int EnsightPart::getVertexCount(int timestep) const
{
    return vertices_[timestep].cols();
}

const Matx& EnsightPart::getVertices(int timestep) const
{
    return vertices_[timestep];
}

bool EnsightPart::hasCellType(int timestep, Ensight::Cell c) const
{
    auto range = timestep >= 0 ? cells_.equal_range(timestep)
                               : make_pair(cells_.begin(), cells_.end());

    using vt = decltype(cells_)::value_type;
    auto iter = std::find_if(range.first, range.second, [=](const vt& v) {
        return v.second->getType() == c; });
    return iter != range.second;
}

void EnsightPart::setCells(const Mati& values, int timestep, Ensight::Cell type)
{
    unique_ptr<EnsightCellList> cl(new EnsightCellList(type, values, name_));
    cells_.insert(make_pair(timestep, std::move(cl)));
}

QList<EnsightCellList*> EnsightPart::getCells(int timestep)
{
    QList<EnsightCellList*> result;
    auto range = cells_.equal_range(timestep);
    for (auto it = range.first; it != range.second; ++it)
        result.push_back(it->second.get());
    return result;
}

EnsightCellList* EnsightPart::getCells(int timestep, Ensight::Cell type)
{
    auto range = cells_.equal_range(timestep);
    for (auto it = range.first; it != range.second; ++it)
        if (it->second->getType() == type)
            return it->second.get();

    return nullptr;
}

Bbox EnsightPart::getCellBounds(const Veci& cell, int timestep) const
{
    Bbox result;
    for (int i = 0; i < cell.rows(); i++)
        result.extend(vertices_[timestep].col(cell[i]));
    return result;
}


void EnsightPart::setVariable(const QString& name, const Matx& values,
                              Ensight::VarTypes type, int timestep)
{
    if (values.cols() > 0)
    {
        unique_ptr<EnsightVariable> variable(new EnsightVariable(name, type));
        variable->setValues(values);
        variables_.insert(make_pair(timestep, std::move(variable)));
    }
}

void EnsightPart::setVariable(const std::string &name, const Matx &values, Ensight::VarTypes type, int timestep)
{
    setVariable(QString::fromStdString(name), values, type, timestep);
}

bool EnsightPart::hasVariable(const QString& name, int timestep) const
{
    auto range = timestep >= 0 ? variables_.equal_range(timestep)
                               : make_pair(variables_.begin(), variables_.end());

    using vt = decltype(variables_)::value_type;
    auto iter = std::find_if(range.first, range.second, [&](const vt& v) {
        return v.second->getName() == name; });
    return iter != range.second;
}

bool EnsightPart::hasVariable(const std::string &name, int timestep) const
{
    return hasVariable(QString::fromStdString(name), timestep);
}

const Matx& EnsightPart::getVariableValues(const QString& name, int timestep)
{
    EnsightVariable* var = getVariable(name, timestep);
    assert(var != nullptr && "Variable not defined");

    return var->getValues();
}

const Matx &EnsightPart::getVariableValues(const std::string &name, int timestep)
{
    return getVariableValues(QString::fromStdString(name), timestep);
}

EnsightVariable* EnsightPart::getVariable(const QString& name, int timestep)
{
    using vt = decltype(variables_)::value_type;
    auto range = variables_.equal_range(timestep);
    auto iter = std::find_if(range.first, range.second, [&](const vt& v) {
        return v.second->getName() == name; });
    return iter != range.second ? iter->second.get() : nullptr;
}

EnsightVariable *EnsightPart::getVariable(const std::string &name, int timestep)
{
    return getVariable(QString::fromStdString(name), timestep);
}


Matx EnsightPart::getVariableBounds(const QString& name, int timestep)
{
    if (timestep >= 0)
    {
        // return bounds for one time step
        return getVariable(name, timestep)->getBounds();
    }

    // return combined bounds over all time steps
    Matx result = getVariable(name, 0)->getBounds();
    for (int i = 1; i < getNumberOfTimesteps(); i++)
    {
        Matx temp = getVariable(name, i)->getBounds();
        for (int j = 0; j < temp.rows(); j++)
        {
            result(j, 0) = std::min(result(j, 0), temp(j, 0));
            result(j, 1) = std::max(result(j, 1), temp(j, 1));
        }
    }
    return result;
}

Matx EnsightPart::getVariableBounds(const std::string &name, int timestep)
{
    return getVariableBounds(QString::fromStdString(name), timestep);
}

Bbox EnsightPart::getGeometryBounds(int timestep) const
{
    if (timestep >= 0)
        return bounds_[timestep];

    Bbox result = bounds_[0];
    for (int i = 1; i < bounds_.size(); i++)
        result.extend(bounds_[i]);
    return result;
}


void EnsightPart::print() const
{
    std::cout << "Part " << name_.toStdString() << ",  id=" << id_ << std::endl;
    std::cout << ":::::::::::::::::::: VERTICES ::::::::::::::::::::" << std::endl;
    for (int i = 0; i < vertices_.size(); i++)
        std::cout << "::::::::::::::::::::   T=" << i << "    ::::::::::::::::::::\n"
                  << vertices_[i] << std::endl;
    std::cout << "::::::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
    std::cout << ":::::::::::::::::::::: CELLS :::::::::::::::::::::" << std::endl;
    for (int i = 0; i < getNumberOfTimesteps(); i++)
    {
        std::cout << "::::::::::::::::::::   T=" << i << "    ::::::::::::::::::::" << std::endl;
        auto crange = cells_.equal_range(i);
        for (auto it = crange.first; it != crange.second; ++it)
        {
            std::cout << "Cells: " << Ensight::strCell[it->second->getType()] << "\n"
                      << it->second->getValues() << std::endl;
        }

        auto vrange = variables_.equal_range(i);
        for (auto it = vrange.first; it != vrange.second; ++it)
        {
            const EnsightVariable& var = *it->second;
            std::cout << "Variables: " << var.getName().toStdString() << "\n"
                      << var.getValues() << std::endl;
        }
    }
    std::cout << "::::::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
}
