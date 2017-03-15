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
#include <QFileInfo>
#include <QTextStream>
#include "../include/ensightasciireader.h"
#include "../include/ensightbinaryreader.h"
#include "../include/ensightobj.h"

using namespace Ensight::Reader::detail;


EnsightCase::EnsightCase()
    : masterFileName(), modelFilename(), modelTimeset(-2), modelFileSet(-1),
      timesetId(-1), timesetSteps(-1), timesetFilenameStart(-1),
      timesetFilenameIncrement(-1), timesteps(Vecx::Zero(0)), filesetId(-1),
      filesetSteps(-1)
{
}

EnsightCase::EnsightCase(const QString& fileName)
    : EnsightCase()
{
    masterFileName = QFileInfo(fileName).absoluteFilePath();
}

EnsightCase::EnsightCase(const std::string& fileName)
    : EnsightCase(QString::fromStdString(fileName))
{
}


void EnsightCase::setMasterFileName(const QString& fileName)
{
    this->masterFileName = fileName;
}

void EnsightCase::setMasterFileName(const std::string& fileName)
{
    this->masterFileName = QString::fromStdString(fileName);
}

bool EnsightCase::readCaseFile()
{
    QFile file(this->masterFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        EnsightObj::ERROR_STR =
            "[EnsightReader::read()] Cannot read from file  " +
            this->masterFileName + ".";
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

        // evaluate and read section
        switch (section)
        {
        case Ensight::SectionUnknown:
            break;

        case Ensight::Format:
            if (!readSectionFormat(data))
                return false;
            break;

        case Ensight::Geometry:
            if (!readSectionGeometry(data, modelFilename, modelTimeset,
                                     modelFileSet))
                return false;
            break;

        case Ensight::Variable:
            if (!readSectionVariables(data, variableNames, variableFilenames,
                                      variableTimeSets, variableFileSets,
                                      variableTypes, constantsNames,
                                      constantsValues))
                return false;
            break;

        case Ensight::Time:
            if (!readSectionTime(data, timesetId, timesetSteps,
                                 timesetFilenameStart, timesetFilenameIncrement,
                                 timesteps))
                return false;
            break;

        case Ensight::File:
            if (!readSectionFile(data, filesetId, filesetSteps))
                return false;
            if (filesetSteps != timesetSteps)
            {
                EnsightObj::ERROR_STR = "[EnsightReader::read()] Number of "
                                        "steps in file set must be equal to "
                                        "number of steps in time set.";
                return false;
            }
            break;

        default:
            break;
        }
    }


    if (modelFilename.isEmpty())
    {
        EnsightObj::ERROR_STR =
            "[EnsightReader::read()] SECTION GEOMETRY  not found.";
        return false;
    }

    return true;
}

int EnsightCase::getFileNumberForStep(int step) const
{
    if (timesetFilenameStart < 0 || timesetFilenameIncrement <= 0)
        return step;
    return timesetFilenameStart + step * timesetFilenameIncrement;
}


namespace Ensight
{
namespace Reader
{

EnsightObj* read(const QString& filename, int readTimeStep)
{
    // read the casefile
    EnsightCase caseFile(filename);
    bool caseFileReadSuccess = caseFile.readCaseFile();
    if (!caseFileReadSuccess)
        return nullptr;

    QString path = QFileInfo(filename).absolutePath();

    // Create Ensight object
    std::unique_ptr<EnsightObj> ensight(new EnsightObj());
    ensight->beginEdit();

    // Time steps
    bool isTransient = caseFile.timesetId == 1 && caseFile.timesteps.rows() > 1;
    bool readTransientAsStatic = isTransient && readTimeStep >= 0;
    if (isTransient && !readTransientAsStatic)
        ensight->setTransient(caseFile.timesteps);
    else
        ensight->setStatic();
    bool isTransientSingleFile = isTransient && caseFile.filesetId >= 0;

    if (readTimeStep >= caseFile.timesteps.rows() &&
        (isTransient || readTimeStep > 0))
    {

        EnsightObj::ERROR_STR =
            QString("In [EnsightReader::read()] requested time step %1 "
                    "(index %2) but file contains only %3 time steps.")
                .arg(readTimeStep + 1)
                .arg(readTimeStep)
                .arg(caseFile.timesteps.rows());
        return nullptr;
    }

    // Constants
    for (int i = 0; i < caseFile.constantsNames.size(); i++)
        ensight->addConstant(caseFile.constantsNames.at(i),
                             caseFile.constantsValues.at(i));

    // Ascii or binary format. Will be determined once in the next loop
    bool binary = false;

    // Geometry
    int numFiles = isTransientSingleFile ? 1 : ensight->getNumberOfTimesteps();
    for (int timestep = 0; timestep < numFiles; timestep++)
    {
        QString geometryFile = caseFile.modelFilename;
        if (isTransient && !isTransientSingleFile)
        {
            int step = readTransientAsStatic ? readTimeStep : timestep;
            int fileNumber = caseFile.getFileNumberForStep(step);
            if (!checkWildcards(geometryFile, fileNumber))
            {
                EnsightObj::ERROR_STR =
                    "In [EnsightReader::read()] for timestep = " +
                    QString::number(step) + ". Something wrong with the "
                    "wildcards or the corresponding time step for geometry model.";
                return nullptr;
            }

            int numWildcards = geometryFile.count("*");
            int firstWildCard = geometryFile.indexOf("*");

            geometryFile.replace(firstWildCard, numWildcards,
                                 QString("%0").arg(fileNumber, numWildcards,
                                                   10, QLatin1Char('0')));
        }
        geometryFile.prepend(path + "/");

        if (timestep == 0)
        {
            FileType type = getEnsightFileType(geometryFile);
            if (type == FileType::Fortran_Binary)
            {
                EnsightObj::ERROR_STR = "In [EnsightReader::read()] Format "
                                        "\"Fortran Binary\" is not supported.";
                return nullptr;
            }
            binary = type == FileType::C_Binary;
        }

        if (binary)
        {
            if (!(readBinaryGeometry(*ensight, geometryFile, timestep,
                                     readTimeStep, isTransientSingleFile)))
                return nullptr;
        }
        else
        {
            if (!(readAsciiGeometry(*ensight, geometryFile, timestep,
                                    readTimeStep, isTransientSingleFile)))
                return nullptr;
        }
    }

    // Variables
    for (int timestep = 0; timestep < numFiles; timestep++)
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
                    EnsightObj::ERROR_STR =
                        "In [EnsightReader::read()] for timestep = " +
                        QString::number(timestep) +
                        ". Cannot create variable with name " + varName;
                    return nullptr;
                }
            }

            int dim = Ensight::varTypeDims[varType];

            if (isTransient && !isTransientSingleFile)
            {
                int step = readTransientAsStatic ? readTimeStep : timestep;
                int fileNumber = caseFile.getFileNumberForStep(step);
                if (!checkWildcards(varFile, fileNumber))
                {
                    EnsightObj::ERROR_STR =
                        "In [EnsightReader::read()] for timestep = " +
                        QString::number(step) +
                        ". Something wrong with the wildcards or the "
                        "corresponding time step for variable " +
                        varName;
                    return nullptr;
                }

                int numWildcards = varFile.count("*");
                int firstWildCard = varFile.indexOf("*");

                varFile.replace(firstWildCard, numWildcards,
                                QString("%0").arg(fileNumber, numWildcards,
                                                  10, QLatin1Char('0')));
            }

            varFile.prepend(path + "/");

            if (binary)
            {
                if (!(readBinaryVariable(*ensight, varFile, varName, timestep,
                                         readTimeStep, isTransientSingleFile,
                                         varType, dim)))
                    return nullptr;
            }
            else
            {
                if (!(readAsciiVariable(*ensight, varFile, varName, timestep,
                                        readTimeStep, isTransientSingleFile,
                                        varType, dim)))
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

EnsightObj* read(const std::string& filename, int timestep)
{
    return read(QString::fromStdString(filename), timestep);
}

namespace detail
{

// returns the first element of the list or an empty string if list is empty
QString first(const QStringList& list)
{
    return list.value(0, QString());
}

bool readSectionFormat(QStringList& data)
{
    QString line = data.takeFirst();

    if (!line.startsWith("type"))
    {
        EnsightObj::ERROR_STR =
            "[EnsightReader::read()] Unknown keyword in FORMAT section <" +
            line + ">.";
        return false;
    }

    line = line.right(line.length() - line.indexOf(":") - 1).trimmed();


    if (line != "ensight gold")
    {
        EnsightObj::ERROR_STR =
            "[EnsightReader::read()] Unknown format in line <" + line +
            ">. Only format <ensight gold> is supported by this libary.";
        return false;
    }
    return true;
}


// Parse the optional time set and file set parts of a line into the output
// parameters. If these parts exists and are correctly parsed, the corresponding
// tokens are removed from the list. I.e., after this function has been called,
// the remaining items in the list are the required parts.
void parseOptionalTimeFileSet(QStringList& lineTokens, int& timeSet,
                              int& fileSet)
{
    // try to parse time set
    QString token = first(lineTokens);
    bool conversionOk;
    timeSet = token.toInt(&conversionOk);
    if (conversionOk)
    {
        // take next token
        lineTokens.removeFirst();
        token = first(lineTokens);

        // try to read file set only if time set could be read
        fileSet = token.toInt(&conversionOk);
        if (conversionOk)
            lineTokens.removeFirst();
        else
            fileSet = -1;
    }
    else
    {
        timeSet = -1;
        fileSet = -1;
    }
}


bool readSectionGeometry(QStringList& data, QString& modelFilename,
                         int& modelTimeset, int& modelFileSet)
{
    QString line = data.takeFirst();
    if (!line.startsWith("model"))
    {
        EnsightObj::ERROR_STR =
            "[EnsightReader::read()] Unknown keyword in GEOMETRY section<" +
            line + ">. Only the keyword <model> is supported by this library";
        return false;
    }

    // extract model-names into list
    line = line.right(line.length() - line.indexOf(":") - 1);
    QStringList modelTokens = line.split(" ", QString::SkipEmptyParts);

    // try to parse time set
    parseOptionalTimeFileSet(modelTokens, modelTimeset, modelFileSet);

    // The next token should be the geometry file name. The line is valid only
    // if this is the last token.
    if (modelTokens.size() != 1)
    {
        EnsightObj::ERROR_STR =
            "[EnsightReader::read()] Unsupported format in GEOMETRY section in "
            "line <" +
            line + ">. (Note that the keyword [change_coords_only] "
                   "or filenames containing spaces are not supported.)";
        return false;
    }

    modelFilename = modelTokens.front();
    return true;
}

bool readSectionVariables(QStringList& data, QStringList& variableNames,
                          QStringList& variableFileNames,
                          QList<int>& variableTimeSets,
                          QList<int>& variableFileSets,
                          QList<Ensight::VarTypes>& variableTypes,
                          QStringList& constantsNames,
                          QList<double>& constantsValues)
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
            EnsightObj::ERROR_STR =
                "[EnsightReader::read()] In VARIABLE section in line <" + line +
                ">. Unkown keyword or delimiter <:> not found.";
            return false;
        }

        line = line.right(line.length() - line.indexOf(":") - 1);
        QStringList varSplit = line.split(" ", QString::SkipEmptyParts);
        if (varSplit.size() < 2)
        {
            EnsightObj::ERROR_STR =
                "[EnsightReader::read()] In VARIABLE section in line <" + line +
                ">. At least variable type, name and value expected.";
            return false;
        }

        if (varType == Ensight::ConstantPerCase)
        {
            if (varSplit.size() > 2) // transient
            {
                EnsightObj::ERROR_STR =
                    "[EnsightReader::read()] In VARIABLE section in line <" +
                    line + ">. Transient constants are not supported.";
                return false;
            }

            constantsNames.append(varSplit.at(0));
            constantsValues.append(varSplit.at(1).toDouble());
        }
        else if (varType == Ensight::ScalarPerNode ||
                 varType == Ensight::VectorPerNode)
        {
            int timeSet, fileSet;
            parseOptionalTimeFileSet(varSplit, timeSet, fileSet);

            if (varSplit.size() != 2)
            {
                EnsightObj::ERROR_STR =
                    "[EnsightReader::read()] In VARIABLE section in line <" +
                    line + ">. Expected variable description and file name.";
                return false;
            }

            variableTimeSets << timeSet;
            variableFileSets << fileSet;
            variableNames << varSplit.at(0);
            variableFileNames << varSplit.at(1);
            variableTypes << varType;
        }
    }
    return true;
}


bool readSectionTime(QStringList& data, int& timesetId, int& timesetSteps,
                     int& timesetFilenameStart, int& timesetFilenameIncrement,
                     Vecx& timesteps)
{
    timesetId = -1;
    timesetSteps = -1;
    timesetFilenameStart = 1;
    timesetFilenameIncrement = 1;

    while (data.size() > 0)
    {
        QString line = data.first();
        if (getSection(line) != Ensight::SectionUnknown)
            break;
        data.removeFirst();

        QString keyword = line.left(line.indexOf(":")).trimmed();
        line = line.right(line.length() - line.indexOf(":") - 1).trimmed();
        QStringList tokens = line.split(" ", QString::SkipEmptyParts);
        bool ok = true;

        if (keyword == "time set")
        {
            timesetId = first(tokens).toInt(&ok);
            if (timesetId != 1 || !ok)
            {
                EnsightObj::ERROR_STR =
                    "[EnsightReader::read()] In line <" + line +
                    ">. Only one <time set> is supported, which must have ID 1.";
                return false;
            }
        }
        else if (keyword == "number of steps")
        {
            timesetSteps = line.toInt(&ok);
            if (timesetSteps <= 0 || !ok)
            {
                EnsightObj::ERROR_STR =
                    "[EnsightReader::read()] In line <" + line +
                    ">: Invalid number of time steps";
                return false;
            }
        }
        else if (keyword == "filename start number")
        {
            timesetFilenameStart = line.toInt(&ok);
            if (!ok)
            {
                EnsightObj::ERROR_STR = "[EnsightReader::read()] In line <" +
                                        line + ">: Invalid number";
                return false;
            }
        }
        else if (keyword == "filename increment")
        {
            timesetFilenameIncrement = line.toInt(&ok);
            if (!ok)
            {
                EnsightObj::ERROR_STR = "[EnsightReader::read()] In line <" +
                                        line + ">. Invalid <filename increment>.";
                return false;
            }
        }
        else if (keyword == "time values")
        {
            if (timesetSteps <= 0)
            {
                EnsightObj::ERROR_STR = "[EnsightReader::read()] In line <" +
                                        line + ">: Number of time steps must "
                                        "be declared before time values.";
                return false;
            }

            QStringList timesetValuesStr = tokens;
            timesteps = Vecx(timesetSteps);
            int j = 0;
            while (true)
            {
                for (auto strValue : timesetValuesStr)
                {
                    if (j >= timesetSteps)
                    {
                        EnsightObj::ERROR_STR =
                            "[EnsightReader::read()] Reading TIME SET values "
                            "at line <" +
                            line + ">. " + "More values found than expected";
                        return false;
                    }

                    bool ok;
                    timesteps[j++] = strValue.toDouble(&ok);
                    if (!ok)
                    {
                        EnsightObj::ERROR_STR =
                            "[EnsightReader::read()] Reading time set values "
                            "at line <" +
                            line + ">. Floating point values expected";
                        return false;
                    }
                }

                if (j == timesetSteps)
                    break;
                else if (data.size() == 0)
                {
                    EnsightObj::ERROR_STR = "[EnsightReader::read()] Reading "
                                            "TIME SET. End of CASE file "
                                            "reached but more floating point "
                                            "values expected as time steps.";
                    return false;
                }

                line = data.takeFirst();
                timesetValuesStr = line.split(" ", QString::SkipEmptyParts);
            }
        }
    }

    // check that required fiels have been read
    if (timesetId <= 0 || timesetSteps <= 0)
    {
        EnsightObj::ERROR_STR =
            "[EnsightReader::read()]: Section TIME is incomplete.";
        return false;
    }

    return true;
}

bool readSectionFile(QStringList& data, int& filesetId, int &filesetSteps)
{
    filesetId = -1;
    filesetSteps = -1;

    while (data.size() > 0)
    {
        QString line = data.first();
        if (getSection(line) != Ensight::SectionUnknown)
            break;
        data.removeFirst();

        QString keyword = line.left(line.indexOf(":")).trimmed();
        line = line.right(line.length() - line.indexOf(":") - 1).trimmed();
        QStringList tokens = line.split(" ", QString::SkipEmptyParts);
        bool ok = true;

        if (keyword == "file set")
        {
            filesetId = first(tokens).toInt(&ok);
            if (filesetId != 1 || !ok)
            {
                EnsightObj::ERROR_STR =
                    "[EnsightReader::read()] In line <" + line +
                    ">. Only one <file set> is supported, which must have ID 1.";
                return false;
            }
        }
        else if (keyword == "number of steps")
        {
            filesetSteps = line.toInt(&ok);
            if (filesetSteps <= 0 || !ok)
            {
                EnsightObj::ERROR_STR =
                    "[EnsightReader::read()] In line <" + line +
                    ">: Invalid number of time steps in file set.";
                return false;
            }
        }
        else if (keyword == "filename index")
        {
            EnsightObj::ERROR_STR = "[EnsightReader::read()] In line <" +
                                    line + ">: filename index is not supported";
            return false;
        }
    }

    // check that required fiels have been read
    if (filesetId <= 0 || filesetSteps <= 0)
    {
        EnsightObj::ERROR_STR =
            "[EnsightReader::read()]: Section FILE is incomplete.";
        return false;
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

    if ((numWildcards > 0 && timestep < 0) ||
        (numWildcards == 0 && timestep >= 0) ||
        (numRequiredWildcards > numWildcards))
        return false;

    return true;
}

} // namespace detail

} // namespace Reader
} // namespace Ensight
