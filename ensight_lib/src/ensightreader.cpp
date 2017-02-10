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


#include "../include/ensightreader.h"

#include <memory>
#include <QDebug>
#include <QFileInfo>
#include "../include/ensightasciireader.h"
#include "../include/ensightbinaryreader.h"
#include "../include/ensightobj.h"

using namespace Ensight::Reader::detail;


EnsightCase::EnsightCase() :
    masterFileName(),
    modelFilename(),
    modelTimeset(-2),
    timesetId(-1),
    timesetSteps(-1),
    timesetFilenameStart(-1),
    timesetFilenameIncrement(-1),
    timesteps(Vecx::Zero(0))
{
}

EnsightCase::EnsightCase(const QString& fileName) :
    modelFilename(),
    modelTimeset(-2),
    timesetId(-1),
    timesetSteps(-1),
    timesetFilenameStart(-1),
    timesetFilenameIncrement(-1),
    timesteps(Vecx::Zero(0))
{
    masterFileName = QFileInfo(fileName).absoluteFilePath();
}

EnsightCase::EnsightCase(const std::string &fileName) :
    modelFilename(),
    modelTimeset(-2),
    timesetId(-1),
    timesetSteps(-1),
    timesetFilenameStart(-1),
    timesetFilenameIncrement(-1),
    timesteps(Vecx::Zero(0))
{
    masterFileName = QFileInfo(QString::fromStdString(fileName)).absoluteFilePath();
}


void EnsightCase::setMasterFileName(const QString& fileName)
{
    this->masterFileName = fileName;
}

void EnsightCase::setMasterFileName(const std::string &fileName)
{
    this->masterFileName = QString::fromStdString(fileName);
}

bool EnsightCase::readCaseFile()
{
    QFile file(this->masterFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        EnsightObj::ERROR_STR = "[EnsightReader::read()] Cannot read from file  " + this->masterFileName + ".";
        qDebug() << "### EnsightCase::readCaseFile => " << EnsightObj::ERROR_STR;
        return false;
    }

    QTextStream in(&file);
    QStringList data;
    // Read file line by line to data, remove blank lines and comments
    while (!in.atEnd())
    {
        QString line = in.readLine();
        line = line.left(line.indexOf("#")).trimmed();
        if (line.isEmpty())
            continue;
        data << line;
    }
    file.close();

    while (!data.isEmpty())
    {
        QString line = data.takeFirst();
        Ensight::Section section = getSection(line);

        //evaluate and read section
        switch (section)
        {
        case Ensight::SectionUnknown:
            break;

        case Ensight::Format:
            if (!readSectionFormat(data))
            {
                qDebug() << "### EnsightCase::readCaseFile => " << EnsightObj::ERROR_STR;
                return false;
            }
            break;

        case Ensight::Geometry:
            if (!readSectionGeometry(data, modelFilename, modelTimeset))
            {
                qDebug() << "### EnsightCase::readCaseFile => " << EnsightObj::ERROR_STR;
                return false;
            }
            break;

        case Ensight::Variable:
            if (!readSectionVariables(data, variableNames, variableFilenames, variableTimesets, variableTypes,
                                      constantsNames, constantsValues))
            {
                qDebug() << "### EnsightCase::readCaseFile => " << EnsightObj::ERROR_STR;
                return false;
            }
            break;

        case Ensight::Time:
            if (!readSectionTimeset(data, timesetId, timesetSteps, timesetFilenameStart, timesetFilenameIncrement,
                                    timesteps))
            {
                qDebug() << "### EnsightCase::readCaseFile => " << EnsightObj::ERROR_STR;
                return false;
            }
            break;

        case Ensight::File:
            EnsightObj::ERROR_STR = "[EnsightReader::read()] SECTION FILE is not supported in this version of EnsightLib. Maybe in later versions";
            qDebug() << "### EnsightCase::readCaseFile => " << EnsightObj::ERROR_STR;
            return false;

        default:
            break;
        }
    }


    if (modelFilename.isEmpty())
    {
        EnsightObj::ERROR_STR = "[EnsightReader::read()] SECTION GEOMETRY  not found.";
        qDebug() << "### EnsightCase::readCaseFile => " << EnsightObj::ERROR_STR;
        return false;
    }

    return true;
}


void EnsightCase::print() const
{
    QString tmpText;
    int tmpInteger;
    Ensight::VarTypes tmpTypes;
    double tmpDouble;

    qDebug() << "masterFileName: " << masterFileName;
    qDebug() << "modelFileName: " << modelFilename;
    qDebug() << "modelTimeset: " << modelTimeset;
    qDebug() << "variableNames: ";
    foreach (tmpText, variableNames)
        qDebug() << tmpText;
    qDebug() << "variableTimesets: ";
    foreach (tmpText, variableFilenames)
        qDebug() << tmpText;
    qDebug() << "variableTimesets: ";
    foreach (tmpInteger, variableTimesets)
        qDebug() << tmpInteger;
    qDebug() << "variableTypes: ";
    foreach (tmpTypes, variableTypes)
        qDebug() << tmpTypes;
    qDebug() << "constantsNames: ";
    foreach (tmpText, constantsNames)
        qDebug() << tmpText;
    qDebug() << "constantValues: ";
    foreach (tmpDouble, constantsValues)
        qDebug() << tmpDouble;
    qDebug() << "timesetId: " << timesetId;
    qDebug() << "timesetSteps: " << timesetSteps;
    qDebug() << "timesetFilenameStart: " << timesetFilenameStart;
    qDebug() << "timesetFilenameIncrement: " << timesetFilenameIncrement;
    qDebug() << "timesteps: ";
    for (int i = 0; i < timesteps.rows(); i++)
        qDebug() << timesteps(i);
}

namespace Ensight
{
namespace Reader
{

EnsightObj* read(const QString& filename, int readTimeStep)
{
    //read the casefile
    EnsightCase caseFile(filename);
    bool caseFileReadSuccess = caseFile.readCaseFile();
    if (!caseFileReadSuccess)
        return nullptr;

    QString path = QFileInfo(filename).absolutePath();

    // Create Ensight object
    std::unique_ptr<EnsightObj> ensight(new EnsightObj());
    ensight->beginEdit();

    // Time steps
    if (readTimeStep >= caseFile.timesteps.rows())
    {

        EnsightObj::ERROR_STR = QString("In [EnsightReader::read()] requested time step %1 "
                                        "(index %2) but file contains only %3 time steps.")
                                .arg(readTimeStep+1)
                                .arg(readTimeStep)
                                .arg(caseFile.timesteps.rows());
        return nullptr;
    }
    bool isTransient = caseFile.timesetId == 1 && caseFile.timesteps.rows() > 1;
    bool readTransientAsStatic = isTransient && readTimeStep >= 0;
    if (isTransient && !readTransientAsStatic)
        ensight->setTransient(caseFile.timesteps);
    else
        ensight->setStatic();

    // Constants
    for (int i = 0; i < caseFile.constantsNames.size(); i++)
        ensight->addConstant(caseFile.constantsNames.at(i), caseFile.constantsValues.at(i));

    // Ascii or binary format. Will be determined once in the next loop
    bool binary = false;

    // Geometry
    for (int timestep = 0; timestep < ensight->getNumberOfTimesteps(); timestep++)
    {
        QString geometryFile = caseFile.modelFilename;
        if (isTransient)
        {
            int fileNameStep = readTransientAsStatic ? readTimeStep : timestep;
            if (!checkWildcards(geometryFile, fileNameStep))
            {
                EnsightObj::ERROR_STR = "In [EnsightReader::read()] for timestep = " + QString::number(timestep) + "." +
                                        " Something wrong with the wildcards or the according time step for geometry model. ";
                return nullptr;
            }

            int numWildcards = geometryFile.count("*");
            int firstWildCard = geometryFile.indexOf("*");

            geometryFile.replace(firstWildCard, numWildcards,
                                 QString("%0").arg(fileNameStep, numWildcards, 10, QLatin1Char('0')));
        }
        geometryFile.prepend(path + "/");

        if (timestep == 0)
        {
            FileType type = getEnsightFileType(geometryFile);
            if (type == FileType::Fortran_Binary)
            {
                EnsightObj::ERROR_STR = "In [EnsightReader::read()] Format \"Fortran Binary\" is not supported.";
                return nullptr;
            }
            binary = type == FileType::C_Binary;
        }

        if (binary)
        {
            if (!(readBinaryGeometry(*ensight, geometryFile, timestep)))
                return nullptr;
        }
        else
        {
            if (!(readAsciiGeometry(*ensight, geometryFile, timestep)))
                return nullptr;
        }
    }

    // Variables
    for (int timestep = 0; timestep < ensight->getNumberOfTimesteps(); timestep++)
    {
        for (int j = 0; j < caseFile.variableNames.size(); j++)
        {
            QString varName = caseFile.variableNames.at(j);
            QString varFile = caseFile.variableFilenames.at(j);
            Ensight::VarTypes varType = caseFile.variableTypes.at(j);

            if (timestep == 0)
            {
                if (!ensight->createVariable(varName, varType))
                {
                    EnsightObj::ERROR_STR = "In [EnsightReader::read()] for timestep = " + QString::number(timestep) + "." +
                                            " Cannot create Variable with name " + varName;
                    return nullptr;
                }
            }

            int dim = Ensight::varTypeDims[varType];

            if (isTransient)
            {
                int fileNameStep = readTransientAsStatic ? readTimeStep : timestep;
                if (!checkWildcards(varFile, fileNameStep))
                {
                    EnsightObj::ERROR_STR = "In [EnsightReader::read()] for timestep = " + QString::number(timestep) + "." +
                                            " Something wrong with the wildcards or the according time step for variable " + varName;
                    return nullptr;
                }

                int numWildcards = varFile.count("*");
                int firstWildCard = varFile.indexOf("*");

                varFile.replace(firstWildCard, numWildcards,
                                QString("%0").arg(fileNameStep, numWildcards, 10, QLatin1Char('0')));
            }

            varFile.prepend(path + "/");

            if (binary)
            {
                if (!(readBinaryVariable(*ensight, varFile, varName, timestep, varType, dim)))
                    return nullptr;
            }
            else
            {
                if (!(readAsciiVariable(*ensight, varFile, varName, timestep, varType, dim)))
                    return nullptr;
            }
        }
    }

    if (!ensight->endEdit())
    {
        return nullptr;
    }

    return ensight.release();
}

EnsightObj* read(const std::string &filename, int timestep)
{
    return read(QString::fromStdString(filename), timestep);
}

namespace detail
{

bool readSectionFormat(QStringList& data)
{
    QString line = data.takeFirst();

    if (!line.startsWith("type"))
    {
        EnsightObj::ERROR_STR = "[EnsightReader::read()] Unknown keyword in FORMAT section <" + line + ">.";
        return false;
    }

    line = line.right(line.length() - line.indexOf(":") - 1).trimmed();


    if (line != "ensight gold")
    {
        EnsightObj::ERROR_STR = "[EnsightReader::read()] Unknown format in line <" + line + ">. Only format <ensight gold> is supported by this libary.";
        return false;
    }
    return true;
}


bool readSectionGeometry(QStringList& data, QString& modelFilename, int& modelTimeset)
{
    QString line = data.takeFirst();
    if (!line.startsWith("model"))
    {
        EnsightObj::ERROR_STR = "[EnsightReader::read()] Unknown keyword in GEOMETRY section<" + line + ">. Only the keyword <model> is supported by this library";
        return false;
    }

    //extract model-names into list
    line = line.right(line.length() - line.indexOf(":") - 1);
    QStringList model = line.split(" ", QString::SkipEmptyParts);

    //determine if static or transient geometry is used
    switch (model.size())
    {
    case 1:  //static
        modelFilename = model.first();
        modelTimeset = -1;
        return true;

    case 2:  //transient
        modelTimeset = model.at(0).toInt();
        modelFilename = model.at(1);
        return true;

    default:
        break;
    }

    EnsightObj::ERROR_STR = "[EnsightReader::read()] Unsupported format in GEOMETRY section in line <" + line + ">. " +
                            "Note that the keyword [change_coords_only] is not supported and filesets are not supported.";
    return false;
}

bool readSectionVariables(QStringList& data, QStringList& variableNames, QStringList& variableFilenames,
                          QList<int>& variableTimesets, QList<Ensight::VarTypes>& variableTypes,
                          QStringList& constantsNames, QList<double>& constantsValues)
{
    while (data.size() > 0)
    {
        QString line = data.first();
        if (getSection(line) != Ensight::SectionUnknown)
            break;
        data.removeFirst();
        Ensight::VarTypes varType = getVarType(line);
        if (varType == Ensight::VarTypeUnknown)
        {
            EnsightObj::ERROR_STR = "[EnsightReader::read()] In VARIABLE section in line <" + line + ">. Unkown keyword or delimiter <:> not found.";
            return false;
        }

        line = line.right(line.length() - line.indexOf(":") - 1);
        QStringList varSplit = line.split(" ", QString::SkipEmptyParts);
        if (varSplit.size() < 2)
        {
            EnsightObj::ERROR_STR = "[EnsightReader::read()] In VARIABLE section in line <" + line + ">. At least variable type, name and value expected.";
            return false;
        }

        if (varType == Ensight::ConstantPerCase)
        {
            int offset = 0;
            if (varSplit.size() > 2)  // transient
                offset = 1;

            constantsNames.append(varSplit.at(offset));
            constantsValues.append(varSplit.at(offset+1).toDouble());
        }
        else if (varType == Ensight::ScalarPerNode || varType == Ensight::VectorPerNode)
        {
            int offset = 0;
            if (varSplit.size() == 3)  // transient
                offset = 1;
            else if (varSplit.size() != 2 && varSplit.size() != 3)  // transient and file sets
            {
                EnsightObj::ERROR_STR = "[EnsightReader::read()] In VARIABLE section in line <" + line + ">. Expected fields after delimiter is 2 or 3. FILE SETS are not supported.";
                return false;
            }

            if (varSplit.size() == 3)
                variableTimesets << varSplit.at(0).toInt();
            else
                variableTimesets << -1;

            variableNames << varSplit.at(0 + offset);
            variableFilenames << varSplit.at(1 + offset);
            variableTypes << varType;
        }
    }
    return true;
}


bool readSectionTimeset(QStringList& data, int& timesetId,
                        int& timesetSteps, int& timesetFilenameStart,
                        int& timesetFilenameIncrement, Vecx& timesteps)
{
    for (int i = 0; i < Ensight::numTimeSetDef; i++)
    {
        QString line = data.takeFirst();
        if (!line.startsWith(Ensight::strTimeSetDef[i]))
        {
            EnsightObj::ERROR_STR = "[EnsightReader::read()] In TIME section in line <" + line + ">. " +
                                    "Unkown keyword or delimiter <:> not found. " + QString("%0 expected").arg(Ensight::strTimeSetDef[i]);
            return false;
        }

        line = line.right(line.length() - line.indexOf(":") - 1).trimmed();
        bool ok = true;
        if (i == 0)
        {
            timesetId = line.toInt(&ok);
            if (timesetId != 1 || !ok)
            {
                EnsightObj::ERROR_STR = "[EnsightReader::read()] In line <" + line + ">. " +
                                        "This version of EnsightLib supports only TIME SET. " +
                                        "This <time set> must have ID equal to 1. Don't use a description.";
                return false;
            }
        }
        else if (i == 1)
        {
            timesetSteps = line.toInt(&ok);
        }
        else if (i == 2)
        {
            timesetFilenameStart = line.toInt(&ok);
        }
        else if (i == 3)
        {
            timesetFilenameIncrement = line.toInt(&ok);
            if (!ok)
            {
                EnsightObj::ERROR_STR = "[EnsightReader::read()] In line <" + line + ">. " +
                                        "Invalid <filename increment>.";
                return false;
            }
        }
        else if (i == 4)
        {
            QStringList timesetValuesStr = line.split(" ", QString::SkipEmptyParts);
            timesteps = Vecx(timesetSteps);
            int j = 0;
            while (true)
            {
                for (int k = 0; k < timesetValuesStr.size(); k++)
                {
                    if (j >= timesetSteps)
                    {
                        EnsightObj::ERROR_STR = "[EnsightReader::read()] Reading TIME SET values at line <" + line + ">. " +
                                                "More values found than expected";
                        return false;
                    }

                    bool ok;
                    timesteps[j++] = timesetValuesStr[k].toDouble(&ok);
                    if (!ok)
                    {
                        EnsightObj::ERROR_STR = "[EnsightReader::read()] Reading TIME SET values at line <" + line + ">. " +
                                                "Floating point values expected";
                        return false;
                    }
                }

                if (j == timesetSteps)
                    break;
                else if (data.size() == 0)
                {
                    EnsightObj::ERROR_STR = "[EnsightReader::read()] Reading TIME SET. End of CASE file reached but more floating point values expected as time steps.";
                    return false;
                }

                line = data.takeFirst();
                timesetValuesStr = line.split(" ", QString::SkipEmptyParts);
            }
        }
    }
    return true;
}


Ensight::VarTypes getVarType(const QString& line)
{
    Ensight::VarTypes varType = Ensight::VarTypeUnknown;
    for (int i = 0; i < Ensight::numVarTypes; i++)
    {
        if (line.startsWith(Ensight::strVarTypes[i]))
        {
            varType = static_cast<Ensight::VarTypes>(i);
            break;
        }
    }

    return varType;
}


Ensight::Section getSection(const QString& line)
{
    Ensight::Section section = Ensight::SectionUnknown;
    for (int i = 0; i < Ensight::numSections; i++)
    {
        if (line == Ensight::strSections[i])
        {
            section = static_cast<Ensight::Section>(i);
            break;
        }
    }

    return section;
}


bool checkWildcards(const QString& filename, int timestep)
{
    int numWildcards = filename.count("*");
    int firstWildCard = filename.indexOf("*");
    int lastWildCard = filename.indexOf("*", firstWildCard + numWildcards - 1);

    if ((lastWildCard - firstWildCard + 1) != numWildcards)
        return false;

    int numRequiredWildcards = timestep > 0 ? log10(double(timestep)) + 1 : 0;
    if (timestep == 0)
        numRequiredWildcards = 1;

    if ((numWildcards > 0 && timestep < 0) || (numWildcards == 0 && timestep >= 0) ||
        (numRequiredWildcards > numWildcards))
        return false;

    return true;
}

}

// namespace detail

} // namespace Reader
} // namespace Ensight
