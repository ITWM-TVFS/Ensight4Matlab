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


#include "../include/bbox.h"
#include <cassert>
#include <iostream>

Bbox::Bbox()
{
    min_ = Vec3::Constant(1e12);
    max_ = Vec3::Constant(-1e12);
}

Bbox::Bbox(const Vec3& min, const Vec3& max)
{
    this->min_ = min;
    this->max_ = max;
}


bool Bbox::isEmpty() const
{
    return min_[0] > max_[0] || min_[1] > max_[1] || min_[2] > max_[2];
}

bool Bbox::contains(const Vec3& pos) const
{
    return pos[0] >= min_[0] &&
           pos[1] >= min_[1] &&
           pos[2] >= min_[2] &&
           pos[0] <= max_[0] &&
           pos[1] <= max_[1] &&
           pos[2] <= max_[2];
}

bool Bbox::intersects(const Bbox& other) const
{
    bool result = minCorner()[0] <= other.maxCorner()[0] &&
                  maxCorner()[0] >= other.minCorner()[0] &&
                  minCorner()[1] <= other.maxCorner()[1] &&
                  maxCorner()[1] >= other.minCorner()[1] &&
                  minCorner()[2] <= other.maxCorner()[2] &&
                  maxCorner()[2] >= other.minCorner()[2];
    return result;
}

const Vec3 Bbox::center() const
{
    return (min_+max_)/2.;
}

const Vec3 Bbox::diagonal() const
{
    return (max_-min_);
}

void Bbox::extend(const Vec3& v)
{
    if (isEmpty())
    {
        min_ = v;
        max_ = v;
    }
    else
    {
        min_[0] = std::min(min_[0], v[0]);
        min_[1] = std::min(min_[1], v[1]);
        min_[2] = std::min(min_[2], v[2]);

        max_[0] = std::max(max_[0], v[0]);
        max_[1] = std::max(max_[1], v[1]);
        max_[2] = std::max(max_[2], v[2]);
    }
}

void Bbox::extend(double d)
{
    if (isEmpty())
    {
        min_[0] = d;
        min_[1] = 0;
        min_[2] = 0;
        max_[0] = d;
        max_[1] = 0;
        max_[2] = 0;
    }
    else
    {
        min_[0] = std::min(min_[0], d);
        max_[0] = std::max(max_[0], d);
    }
}


void Bbox::extend(const Bbox& b)
{
    if (isEmpty())
    {
        min_ = b.minCorner();
        max_ = b.maxCorner();
    }
    else
    {
        min_[0] = std::min(b.minCorner()[0], min_[0]);
        min_[1] = std::min(b.minCorner()[1], min_[1]);
        min_[2] = std::min(b.minCorner()[2], min_[2]);

        max_[0] = std::max(b.maxCorner()[0], max_[0]);
        max_[1] = std::max(b.maxCorner()[1], max_[1]);
        max_[2] = std::max(b.maxCorner()[2], max_[2]);
    }
}

void Bbox::increaseBy(double d)
{
    // For empty boxes we could set this to [-d,-d,-d]x[d,d,d] but this
    // doesn't make sense since the BB's center is not defined originally.
    assert(!isEmpty());

    min_[0] -= d;
    min_[1] -= d;
    min_[2] -= d;
    max_[0] += d;
    max_[1] += d;
    max_[2] += d;
}

const Vec3& Bbox::minCorner() const
{
    return min_;
}

const Vec3& Bbox::maxCorner() const
{
    return max_;
}

void Bbox::print() const
{
    std::cout << min_.transpose() << "    /    " << max_.transpose() << std::endl;
}
