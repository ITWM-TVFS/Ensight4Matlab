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


#ifndef BBOX_H
#define BBOX_H

#include <iosfwd>
#include "eigentypes.h"

/**
 * @brief Represents a bounding box.
 *
 * This class is used to represent geometric bounds which is required for the Octree.
 *
 * This class is also used to represent bounds of variables.
 * For scalar variables the x-component of min_ and max_ are used and the y-,
 * and z-component are ignored.
 */
class Bbox
{
public:
    /**
     * @brief Creates an empty Bbox. See isEmpty()
     */
    Bbox();

    /**
     * @brief Creates a Bbox with min and max
     * @param[in] min Minimum value of Bbox / bottom-left-front
     * @param[in] max Maximum value of Bbox / top-right-rear
     */
    Bbox(const Vec3& min, const Vec3& max);

    /**
     * @brief Checks if Bbox is empty. This is true if any min-value is larger
     * than corresponding max value
     */
    bool isEmpty() const;

    /**
     * @brief Checks if a point is inside the Bbox
     */
    bool contains(const Vec3& pos) const;

    /**
     * @brief Checks if this Bbox and Bbox 'other' intersect
     */
    bool intersects(const Bbox& other) const;

    /**
     * @brief Returns the center of the Bbox = (min + max)/2
     */
    const Vec3 center() const;

    /**
     * @brief Returns the diagonal of the Bbox = max - min
     */
    const Vec3 diagonal() const;

    /**
     * @brief Extends min and max so that v is inside of the Bbox.
     * If v is already inside the box it is left unchagned. After calling this
     * method, contains(v) returns true.
     */
    void extend(const Vec3& v);

    /**
     * @brief Extends min and max for scalar variables.
     * The x coordinate of the box is adapted such that it contains d, i.e.
     * min_x <= d <= max_x, or left unchanged if this is already the case.
     */
    void extend(double d);

    /**
     * @brief Unity of this Bbox and Bbox 'b'
     */
    void extend(const Bbox& b);

    /**
     * @brief Increases the extents of the bounding box (only for non-empty Bboxes)
     * @param[in] d Size by which each bound is increased (away from the center)
     * For Bbox coordinates of
     *   [min_x, min_y, min_z] x [max_x, max_y, max_z]
     * the resulting coordinates are
     *   [min_x-d, min_y-d, min_z-d] x [max_x+d, max_y+d, max_z+d]
     */
    void increaseBy(double d);

    /**
     * @brief The minimum value of the Bbox
     */
    const Vec3& minCorner() const;

    /**
     * @brief The maximum value of the Bbox
     */
    const Vec3& maxCorner() const;

    /**
     * @brief Prints min and max to given stream
     */
    void print(std::ostream& out) const;


private:
    /**
     * @brief min_ Minimum value of Bbox / bottom-left-front
     */
    Vec3 min_;

    /**
     * @brief max_ Maximum value of Bbox / top-right-rear
     */
    Vec3 max_;
};

#endif // BBOX_H
