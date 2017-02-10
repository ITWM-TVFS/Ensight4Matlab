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


#include "../include/ensightwriter.h"

#include <QDir>
#include <QFileInfo>
#include <QTextStream>

#include "../include/ensightasciiwriter.h"
#include "../include/ensightbinarywriter.h"
#include "../include/ensightconstant.h"
#include "../include/ensightobj.h"
#include "../include/ensightvariable.h"

namespace Ensight
{
namespace Writer
{

bool writeCase(EnsightObj* ensight, const QString& name, const QString& filename);

bool write(EnsightObj* ensight, const QString& filename, bool binary, int timestep)
{
    // Extract path and name from filename
    QFileInfo file(filename);

    QString path = file.absolutePath();
    QString name = file.completeBaseName();

    QDir dir(path);

    if (!(dir.exists() || QDir::root().mkdir(path)))
    {
        EnsightObj::ERROR_STR = ("ERROR IN [write()] Unable to create directory <" + path + ">");
        return false;
    }
    // Case file is only written in if timestep==0 or timestep==-1
    // timestep==0  => write first time step
    // timestep==-1 => write all time steps
    if (timestep == 0 || timestep == -1)
        if (!writeCase(ensight, name, filename))
            return false;

    if (binary)
        return EnsightBinaryWriter::writeBinary(ensight, name, path, timestep);
    return EnsightAsciiWriter::writeAscii(ensight, name, path, timestep);
}

bool write(EnsightObj *ensight, const std::string &filename, bool binary, int timestep)
{
    return write(ensight, QString::fromStdString(filename), binary, timestep);
}

bool writeCase(EnsightObj* ensight, const QString& name, const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        EnsightObj::ERROR_STR = "ERROR IN [writeAsciiCase()] Unable to open file <" + file.fileName() + ">";
        return false;
    }

    QTextStream out(&file);

    out << "FORMAT\n";
    out << "type:      ensight gold\n\n";

    out << "GEOMETRY\n";
    int wildcards = ensight->isTransient() ? log10(double(ensight->getNumberOfTimesteps())) + 1 : 0;

    out << QString("model:     %0%1.geo%2\n\n").
           arg(ensight->isTransient()?"1      ":"      ").
           arg(name).
           arg(QString("*").repeated(wildcards));

    out << "VARIABLE\n";
    for (int i = 0; i < ensight->getNumberOfConstants(); i++)
    {
        EnsightConstant c = ensight->getConstant(i);
        out << QString("%0:%1%2%3\n")
               .arg(Ensight::strVarTypes[Ensight::ConstantPerCase])
               .arg(" ", 20, QLatin1Char(' '))
               .arg(c.getName(), -20, QLatin1Char(' '))
               .arg(c.getValue());
    }
    for (int i = 0; i < ensight->getNumberOfVariables(); i++)
    {
        EnsightVariableIdentifier var = ensight->getVariable(i);
        out << QString("%0:%1%2%3.%4%5\n")
               .arg(Ensight::strVarTypes[var.getType()])
               .arg(ensight->isTransient()?"       1":" ", -22, QLatin1Char(' '))
               .arg(var.getName(), -20, QLatin1Char(' '))
               .arg(name)
               .arg(var.getName())
               .arg(QString("*").repeated(wildcards));

    }
    out << "\n";

    if (ensight->isTransient())
    {
        out << "TIME\n";
        out << "time set:              1\n";
        out << "number of steps:       " << ensight->getNumberOfTimesteps() << "\n";
        out << "filename start number: 0\n";
        out << "filename increment:    1\n";
        out << "time values:\n";
        Vecx timesteps = ensight->getTimesteps();
        for (int i = 0; i < timesteps.rows(); i++)
            out << QString("%0").arg(timesteps(i), 12, 'e', 5, ' ') << "\n";
        out << "\n";
    }

    file.close();
    return true;
}


} // namespace Writer
} // namespace Ensight
