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


#ifndef ENSIGHTREADER_H
#define ENSIGHTREADER_H

#include <string>

#include <QList>
#include <QString>
#include <QStringList>

#include "eigentypes.h"
#include "ensightdef.h"

class EnsightObj;

class EnsightCase
{
public:
    EnsightCase();
    EnsightCase(const QString& fileName);
    EnsightCase(const std::string& fileName);

    bool readCaseFile();
    void print() const;
    void setMasterFileName(const QString& fileName);
    void setMasterFileName(const std::string& fileName);

    QString masterFileName;
    QString modelFilename;
    int modelTimeset;

    // filenames and timesets of all variables
    QStringList variableNames;
    QStringList variableFilenames;
    QList<int> variableTimesets;
    QList<Ensight::VarTypes> variableTypes;

    // names and values of all constants
    QStringList constantsNames;
    QList<double> constantsValues;

    // Timestep values
    int timesetId;
    int timesetSteps;
    int timesetFilenameStart;
    int timesetFilenameIncrement;
    Vecx timesteps;
};

/**
 * @namespace Ensight
 * @brief Namespace Ensight
 */
namespace Ensight
{
/**
 * @namespace Ensight::Reader
 * @brief Namespace Reader
 */
namespace Reader
{

/**
 * @brief This method reads and parses an Ensight Gold *.case file.
 *
 * Then the referred geometry and variable files are also read and
 * an EnsightObj is returned;
 *
 * In case of any error NULL is returned. See EnsightObj::ERROR_STR for
 * more details in this case.
 *
 * The reader has some limitations:
 * - FILE: This section is not supported
 * - VAR: Only constant per case, scalar per node and vector per node
 * - TIME: Only on time set. This time set must have ID 1
 * - GEOMETRY: Only <model> is supported
 *
 * @param filename The case file to read
 * @param timestep A specific time step to read or -1 to read all time steps
 * @return a pointer to an EnsightObj or NULL in case of an error.
 *
 */
EnsightObj* read(const QString& filename, int timestep);
EnsightObj* read(const std::string& filename, int timestep);


namespace detail
{
    enum class IdMode { Off, Given, Assign, Ignore, Invalid };
    enum class FileType { Ascii, C_Binary, Fortran_Binary };

    FileType getEnsightFileType(const QString& filename);


    /**
     * @brief Checks if the specified filename has a correct definition of wildcards and if enough wildcards are available for timestep.
     * @param filename
     * @param timestep
     */
    bool checkWildcards(const QString& filename, int timestep);

    bool readSectionFormat(QStringList& data);
    bool readSectionGeometry(QStringList& data, QString& modelFilename, int& modelTimeset);
    bool readSectionVariables(QStringList& data, QStringList& variableNames,
                              QStringList& variableFilenames, QList<int>& variableTimesets,
                              QList<Ensight::VarTypes>& variableTypes,
                              QStringList& constantsNames, QList<double>& constantsValues);
    bool readSectionTimeset(QStringList& data, int& timesetId, int& timesetSteps,
                            int& timesetFilenameStart, int& timesetFilenameIncrement, Vecx& timesteps);
    Ensight::Section getSection(const QString& line);
    Ensight::VarTypes getVarType(const QString& line);
} // namespace detail

} // namespace Writer
} // namespace Ensight

#endif // ENSIGHTREADER_H
