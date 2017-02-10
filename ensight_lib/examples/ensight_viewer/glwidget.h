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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QDebug>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPoint>
#include <Eigen/Dense>
#include "GL/glu.h"
#include <cmath>

using Vecx = Eigen::VectorXd;
using Vec3 = Eigen::Vector3d;
using Vec2 = Eigen::Vector2d;
using Matx = Eigen::MatrixXd;
using Mati = Eigen::MatrixXi;

/**
 * @brief The GLWidget class is an extension of the Qt QGLWidget.
 * @author Daniel Burkhart, Andre Schmeißer - Fraunhofer ITWM
 *
 * The GLWidget class enhances the QT QGLWidget:
 * - Mouse interaction (Trackball rotation, zooming and translation)
 * - Basic drawing methods (grids, cylinders)
 * - Alignement of the scene (e.g. to an specific orthonormal system)
 *
 * This widget uses Qt, Eigen, and GLU
 */
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    /* Sets the center and size of the whole scene.
       The _center is used as fixpoint for rotations and for adjusting
       the camera/viewer (see view_all()). */
    void set_scene_pos( const Vec3& _center, float _radius );

    /**
     * @brief view_all View the whole scene: the eye point is moved far enough from the
     *   center so that the whole scene is visible.
     *
     */
    void view_all();

    /**
     * @brief radius Get the radius of the scene
     * @return
     */
    float radius() const { return radius_; }

    /**
     * @brief center Get the Scene center
     * @return
     */
    const Vec3& center() const { return center_; }

    /**
     * @brief set_up_vector Align the world coordinate system with the orthonormal system defined
     * by the axis right up and forward
     * @param up
     */
    void align_with_axis();

    /**
     * @brief is_orthonormal Check if the coordinate system with axis right up and forward is orthonormal
     * @param right
     * @param up
     * @param forward
     * @return
     */
    bool is_orthonormal(const Vec3& right, const Vec3 &up, const Vec3 &forward);


    // rotate the scene (around its center) and update modelview matrix
    void rotate(const Vec3& _axis, float _angle);


    /**
     * @brief drawGrid Draw a 2d grid
     * @param p
     * @param v1
     * @param v2
     * @param subdiv1
     * @param subdiv2
     */
    void drawGrid(const Vec3 & p, const Vec3 & v1, const Vec3 & v2, int subdiv1, int subdiv2);

    /**
     * @brief drawAxis Draw coordinate axis at the scene center
     * @param length
     */
    void drawAxis(double length);

    /**
     * @brief drawCylinder Draw a cylinder with radius r from p1 to p2
     * @param p1
     * @param p2
     * @param r
     */
    void drawCylinder(const Vec3 &p1, const Vec3 &p2, double r, int slices = 32,
                      int stacks = 1);



    void drawSphere(const Vec3 &p, double r, int slices = 32, int stacks = 32);

    /**
     * @brief drawGrid draw 3d axis aligned bounding box
     * @param min
     * @param max
     *
     */
    void drawBounds(const Vec3 & mi, const Vec3 & ma);

    /**
     * @brief drawPointCloud  Draw a list of 3d points
     * @param points a 3xN matrix containing N 3d vertices
     */
    void drawPointCloud(const Matx & points);



    /**
     * @brief drawCells Draw a list of cells
     * @param points 3xN matrix containing N 3d vertices
     * @param cells (2/3/4)xM matrix containing M cells, either bar2 tria3 or quad4
     */
    void drawCells(const Matx & points, const Mati & cells);


    void drawCells(const Matx & points, const Mati & cells, const Vecx &var, double min,
                   double max);

    /**
     * @brief pick world coordinates for the given window coordinates, using the
     *        Z-buffer of the currently drawn scene.
     * @param windowX the X window coordinate
     * @param windowY the Y window coordinate
     * @param centerIfNoHit If no object is hit at the given screen position, project
     *        the view ray to the current center
     * @param someHit [optional] Returns if something was hit at the given position
     */
    Vec3 pickCoordinates(int windowX, int windowY, bool centerIfNoHit = true,
                         bool *someHit = NULL);

signals:
    /**
     * @brief draw Connect to this Signal for drawing stuff
     */
    void draw();


    /**
     * @brief init Connect to this Signal to initialize OpenGL
     */
    void init();

    /**
     * @brief key Connect to this Signal to react on key signals
     */
    void key(int);

    void clicked(QMouseEvent*);

protected:
    // OpenGL events
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    // Qt mouse events
    virtual void mousePressEvent( QMouseEvent* e);
    virtual void mouseReleaseEvent( QMouseEvent* e);
    virtual void mouseMoveEvent( QMouseEvent* e);
    virtual void wheelEvent( QWheelEvent* e);
    virtual void keyPressEvent( QKeyEvent* e);

private:

    // Init material
    void setDefaultMaterial(void);

    // Init light
    void setDefaultLight(void);

    // updates projection matrix
    void update_projection_matrix();

    // translate the scene and update modelview matrix
    void translate(const Vec3& _trans);


    // virtual trackball: map 2D screen point to unit sphere
    bool map_to_sphere(const Vec2& _v2D, Vec3& _v3D);

    // The scene center
    Vec3    center_;

    // The scene radius
    float   radius_;

    // OpenGL matrices
    GLdouble    projection_matrix_[16], modelview_matrix_[16];

    // Points for mouse interaction
    Vec2     last_point_2D_;
    Vec3     last_point_3D_;
    bool     last_point_ok_;

    GLUquadric *cylinders;
    GLUquadric *spheres;
};

#endif
