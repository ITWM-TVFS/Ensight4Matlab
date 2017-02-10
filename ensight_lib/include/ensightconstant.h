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


#ifndef ENSIGHTCONSTANT_H
#define ENSIGHTCONSTANT_H

#include <string>

#include <QString>


/**
 * @brief Represents a single constant.
 *
 * Constants in the Ensight Gold format are defined per case file, i.e.
 * Constants are stored global (not local in parts like variables).
 *
 * A constant is always a scalar value and can be different for each time step.
 *
 */
class EnsightConstant
{
public:
    EnsightConstant();
    /**
     * @brief Creates an EnsightConstant with given name and value.
     * @param name Constant name
     * @param value Constant value
     */
    EnsightConstant(const QString& name, double value);
    EnsightConstant(const std::string& name, double value);
    /**
     * @brief Get constant name.
     */
    const QString& getName() const;
    /**
     * @brief Get constant value.
     */
    double getValue() const;
    /**
     * @brief Checks if constant name is empty.
     */
    bool isNull() const;

private:
    QString name;
    double value;
};

#endif // ENSIGHTCONSTANT_H
