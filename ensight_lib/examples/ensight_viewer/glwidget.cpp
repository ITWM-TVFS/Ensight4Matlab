/*
 * This file contains code to display and interact with a 3d scene. Some parts
 * of the file are adapted from code originally found in OpenMesh, which is
 * under the LGPL license. Thus, this file is covered by the LGPL.
 *
 * Additionally, the changes from the original code can be used under the less
 * restrictive MIT license, if desired, as stated by the copyright notice below.
 */


/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2015 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/


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

#include "glwidget.h"

#ifndef M_PI
#  define M_PI 3.1415926535897932
#endif

const double TRACKBALL_RADIUS = 0.6;

GLWidget::GLWidget(QWidget *parent):QGLWidget(parent)
{
    cylinders = 0;
    spheres = 0;
}


GLWidget::~GLWidget()
{
	gluDeleteQuadric(spheres);
	gluDeleteQuadric(cylinders);
}

void GLWidget::set_scene_pos(const Vec3 &_c, float _r)
{
    center_ = _c;
    radius_ = _r;

    update_projection_matrix();
    view_all();
}

void GLWidget::view_all()
{
    translate( Vec3( -(modelview_matrix_[0]*center_[0] +
                       modelview_matrix_[4]*center_[1] +
                       modelview_matrix_[8]*center_[2] +
                       modelview_matrix_[12]),
                     -(modelview_matrix_[1]*center_[0] +
                       modelview_matrix_[5]*center_[1] +
                       modelview_matrix_[9]*center_[2] +
                       modelview_matrix_[13]),
                     -(modelview_matrix_[2]*center_[0] +
                       modelview_matrix_[6]*center_[1] +
                       modelview_matrix_[10]*center_[2] +
                       modelview_matrix_[14] +
                       3.0*radius_) ) );
}


void GLWidget::align_with_axis()
{

    // The 3 vectors must build an orthonormal system
    //     +y
    //     |
    //     |
    //     |
    //     |________  +x
    //     /
    //    /
    //   / +z

    // Check if orthonormal
    // x
    modelview_matrix_[0]=1;
    modelview_matrix_[1]=0;
    modelview_matrix_[2]=0;


    // y
    modelview_matrix_[4]=0;
    modelview_matrix_[5]=1;
    modelview_matrix_[6]=0;

    // z
    modelview_matrix_[8]=0;
    modelview_matrix_[9]=0;
    modelview_matrix_[10]=1;
    updateGL();


}

bool GLWidget::is_orthonormal(const Vec3 &right, const Vec3 &up, const Vec3 &forward)
{
    double a = right.norm();
    double b = right.norm();
    double c = right.norm();
    double d = up.dot(right);
    double e = right.dot(forward);
    double f = up.dot(forward);
    return a==1 && b==1 && c==1 && d==0 && e==0 && f==0;
}


void GLWidget::drawGrid(const Vec3 &p, const Vec3 &v1, const Vec3 &v2, int subdiv1, int subdiv2)
{

    glBegin(GL_LINES);
    // Lines p-v1
    for(int i=0;i<=subdiv2;i++)
    {
        double d = double(i)/double(subdiv2);
        Vec3 q = p + d*v2;
        glVertex3d(q[0],q[1],q[2]);
        glVertex3d(q[0]+v1[0],q[1]+v1[1],q[2]+v1[2]);
    }

    // Lines p-v2
    for(int i=0;i<=subdiv1;i++)
    {
        double d = double(i)/double(subdiv1);
        Vec3 q = p + d*v1;
        glVertex3d(q[0],q[1],q[2]);
        glVertex3d(q[0]+v2[0],q[1]+v2[1],q[2]+v2[2]);
    }
    glEnd();
}




void GLWidget::drawAxis(double length)
{
    glBegin(GL_LINES);
    glColor3d(1,0,0);
    glVertex3d(center_[0], center_[1], center_[2]);
    glVertex3d(center_[0]+length, center_[1], center_[2]);

    glColor3d(0,1,0);
    glVertex3d(center_[0], center_[1], center_[2]);
    glVertex3d(center_[0], center_[1]+length, center_[2]);


    glColor3d(0,0,1);
    glVertex3d(center_[0], center_[1], center_[2]);
    glVertex3d(center_[0], center_[1], center_[2]+length);
    glEnd();

    QFont font("Times", 14, QFont::Bold);
    glColor3d(1,0,0);
    renderText(center_[0]+1.2*length, center_[1], center_[2], QString("x"), font);
    glColor3d(0,1,0);
    renderText(center_[0], center_[1]+1.2*length, center_[2], QString("y"), font);
    glColor3d(0,0,1);
    renderText(center_[0], center_[1], center_[2]+1.2*length, QString("z"), font);
}

void GLWidget::drawCylinder(const Vec3& p1, const Vec3& p2, double r,
                            int slices, int stacks)
{
    Vec3 v = p2-p1;
    double vnorm = v.norm();
    if (vnorm < 1e-16)
        return;
    v /= vnorm;

    // gluCylinder draws a cylinder along the z-axis.
    // Compute a rotation axis and angle to align p2-p1 along the z-axis.
    // The rotation axis is given given by e_z x v = (-v[1], v[0], 0)
    double angleInDegrees = 180.0 / M_PI * acos(v[2]);
    double rx = -v[1];
    double ry = v[0];

    glPushMatrix();

    //draw the cylinder body
    glTranslatef(p1[0], p1[1], p1[2]);
    // If v is already parallel to the z-axis, the rotation angle is 0
    // and the rotation axis undefined. Just skip rotation in this case
    if (fabs(angleInDegrees) > 1e-3)
        glRotatef(angleInDegrees, rx, ry, 0.0);
    gluQuadricOrientation(cylinders, GLU_OUTSIDE);
    gluCylinder(cylinders, r, r, vnorm, slices, stacks);
    glPopMatrix();
}

void GLWidget::drawSphere(const Vec3& p, double r, int slices, int stacks)
{
    glPushMatrix();
    glTranslatef(p[0], p[1], p[2]);
    gluQuadricOrientation(spheres,GLU_OUTSIDE);
    gluSphere(spheres, r, slices, stacks);
    glPopMatrix();
}

void GLWidget::drawBounds(const Vec3 &mi, const Vec3 &ma)
{
    glBegin(GL_LINES);
    glVertex3d(mi[0], mi[1], mi[2]);
    glVertex3d(mi[0], mi[1], ma[2]);

    glVertex3d(ma[0], mi[1], mi[2]);
    glVertex3d(ma[0], mi[1], ma[2]);

    glVertex3d(ma[0], ma[1], mi[2]);
    glVertex3d(ma[0], ma[1], ma[2]);

    glVertex3d(mi[0], ma[1], mi[2]);
    glVertex3d(mi[0], ma[1], ma[2]);

    glVertex3d(mi[0], mi[1], mi[2]);
    glVertex3d(ma[0], mi[1], mi[2]);

    glVertex3d(mi[0], ma[1], mi[2]);
    glVertex3d(ma[0], ma[1], mi[2]);

    glVertex3d(mi[0], ma[1], ma[2]);
    glVertex3d(ma[0], ma[1], ma[2]);

    glVertex3d(mi[0], mi[1], ma[2]);
    glVertex3d(ma[0], mi[1], ma[2]);


    glVertex3d(mi[0], mi[1], mi[2]);
    glVertex3d(mi[0], ma[1], mi[2]);

    glVertex3d(ma[0], mi[1], mi[2]);
    glVertex3d(ma[0], ma[1], mi[2]);

    glVertex3d(ma[0], mi[1], ma[2]);
    glVertex3d(ma[0], ma[1], ma[2]);

    glVertex3d(mi[0], mi[1], ma[2]);
    glVertex3d(mi[0], ma[1], ma[2]);

    glEnd();
}

void GLWidget::drawPointCloud(const Matx &points)
{
    glBegin(GL_POINTS);
    for(int i=0;i<points.cols();i++)
        glVertex3d(points(0,i), points(1,i), points(2,i));
    glEnd();
}

void GLWidget::drawCells(const Matx &points, const Mati &cells)
{
    if(cells.rows()==2)
        glBegin(GL_LINES);
    else if(cells.rows()==3)
        glBegin(GL_TRIANGLES);
    else if(cells.rows()==4)
        glBegin(GL_QUADS);
    else
        glBegin(GL_POINTS);

    for(int i=0;i<cells.cols();i++)
        for(int j=0;j<cells.rows();j++)
            glVertex3d(points(0, cells(j,i)), points(1, cells(j,i)), points(2, cells(j,i)));

    glEnd();
}


void GLWidget::drawCells(const Matx & points, const Mati & cells, const Vecx & var, double min, double max)
{
    if(cells.rows()==2)
        glBegin(GL_LINES);
    else if(cells.rows()==3)
        glBegin(GL_TRIANGLES);
    else if(cells.rows()==4)
        glBegin(GL_QUADS);
    else
        glBegin(GL_POINTS);

    for(int i=0;i<cells.cols();i++)
    {
        for(int j=0;j<cells.rows();j++)
        {
            double value = var(cells(j,i));
            double col = (value-min)/(max-min);
            QColor hsv = QColor::fromHsv(240 - col*240, 255, 255, 255); //
            QColor rgb = hsv.toRgb();
            //qDebug() << value << " : " << col << "  =>   " <<  rgb.red() << " / " << rgb.green() << " / " << rgb.blue();
            glColor3d(rgb.red()/255.0, rgb.green()/255.0, rgb.blue()/255.0);
            glVertex3d(points(0, cells(j,i)), points(1, cells(j,i)), points(2, cells(j,i)));
        }
    }
    glEnd();
}


void GLWidget::setDefaultMaterial()
{
    GLfloat mat_a[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat mat_d[] = {0.7f, 0.7f, 0.5f, 1.0f};
    GLfloat mat_s[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat shine[] = {10.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

void GLWidget::setDefaultLight()
{

    GLfloat pos1[] = { 0.1f,  0.1f, -0.02f, 0.0f};
    GLfloat pos2[] = {-0.1f,  0.1f, -0.02f, 0.0f};
    GLfloat pos3[] = { 0.0f,  0.0f,  0.1f,  0.0f};
    GLfloat col1[] = { 1.0f,  1.0f,  1.0f,  1.0f};
    GLfloat col2[] = { 1.0f,  1.0f,  1.0f,  1.0f};
    GLfloat col3[] = { 1.0f,  1.0f,  1.0f,  1.0f};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_POSITION, pos1);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,  col1);
    glLightfv(GL_LIGHT0,GL_SPECULAR, col1);

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1,GL_POSITION, pos2);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,  col2);
    glLightfv(GL_LIGHT1,GL_SPECULAR, col2);

    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2,GL_POSITION, pos3);
    glLightfv(GL_LIGHT2,GL_DIFFUSE,  col3);
    glLightfv(GL_LIGHT2,GL_SPECULAR, col3);
}

void GLWidget::initializeGL()
{

    // OpenGL state
    // background color for springer-book: glClearColor(213.0/255, 215.0/255, 217.0/255, 0.0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    // glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );

    // glEnable(GL_LINE_SMOOTH);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    // Material
    setDefaultMaterial();

    // Lighting
    setDefaultLight();

    // scene pos and size
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix_);

    // Init Scene
    set_scene_pos(Vec3(0.0, 0.0, 0.0), 1.0);

    // Init Glu Quadric object
    cylinders = gluNewQuadric();
    gluQuadricNormals(cylinders, GLU_SMOOTH);

    spheres = gluNewQuadric();
    gluQuadricNormals(spheres, GLU_SMOOTH);


    emit init();
}


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_PROJECTION );
    glLoadMatrixd( projection_matrix_ );
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( modelview_matrix_ );
    emit draw();
}

void GLWidget::resizeGL(int _w, int _h)
{
    update_projection_matrix();
    glViewport(0, 0, _w, _h);
    updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent* e)
{

    last_point_ok_ = map_to_sphere( last_point_2D_=Vec2(e->pos().x(), e->pos().y()), last_point_3D_ );
    setFocus();
    emit clicked(e);
}

void GLWidget::mouseReleaseEvent(QMouseEvent*)
{
    last_point_ok_ = false;
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    Vec2 newPoint2D(e->pos().x(), e->pos().y());


    // Left button: rotate around center_
    // Right: translate object
    // Middle button: zoom in/out


    Vec3  newPoint3D;
    bool   newPoint_hitSphere = map_to_sphere( newPoint2D, newPoint3D );

    float dx = newPoint2D[0] - last_point_2D_[0];
    float dy = newPoint2D[1] - last_point_2D_[1];

    float w  = width();
    float h  = height();



    // enable GL context
    makeCurrent();


    // move in z direction
    if ( (e->buttons() == (Qt::MidButton)) ||
         (e->buttons() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier))
    {
        float value_y = radius_ * dy * 3.0 / h;
        translate(Vec3(0.0, 0.0, value_y));
    }
    // move in x,y direction
    else if ( (e->buttons() == Qt::RightButton) ||
              (e->buttons() == Qt::LeftButton && e->modifiers() == Qt::AltModifier) )
    {
        float z = - (modelview_matrix_[ 2]*center_[0] +
                     modelview_matrix_[ 6]*center_[1] +
                     modelview_matrix_[10]*center_[2] +
                     modelview_matrix_[14]) /
                (modelview_matrix_[ 3]*center_[0] +
                 modelview_matrix_[ 7]*center_[1] +
                 modelview_matrix_[11]*center_[2] +
                 modelview_matrix_[15]);

        float aspect     = w / h;
        float near_plane = 0.01 * radius_;
        float top        = tan(45.0/2.0*M_PI/180.0) * near_plane;
        float right      = aspect*top;

        translate(Vec3( 2.0*dx/w*right/near_plane*z,
                        -2.0*dy/h*top/near_plane*z,
                        0.0f));
    }
    // rotate
    else if (e->buttons() == Qt::LeftButton) {

        if (last_point_ok_) {
            if ((newPoint_hitSphere = map_to_sphere(newPoint2D, newPoint3D))) {

                // TODO check if cross or dot
                Vec3 axis = last_point_3D_.cross(newPoint3D);
                if (axis.squaredNorm() < 1e-7) {
                    axis = Vec3(1, 0, 0);
                } else {
                    axis = axis / axis.norm();
                }
                // find the amount of rotation
                Vec3 d = last_point_3D_ - newPoint3D;
                float t = 0.5 * d.norm() / TRACKBALL_RADIUS;
                if (t < -1.0)
                    t = -1.0;
                else if (t > 1.0)
                    t = 1.0;
                float phi = 2.0 * asin(t);
                float angle = phi * 180.0 / M_PI;
                rotate(axis, angle);
            }
        }

    }


    // remember this point
    last_point_2D_ = newPoint2D;
    last_point_3D_ = newPoint3D;
    last_point_ok_ = newPoint_hitSphere;

    // trigger redraw
    updateGL();
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    // Use the mouse wheel to zoom in/out

    float d = -(float)e->delta() / 120.0 * 0.2 * radius_;
    translate(Vec3(0.0, 0.0, d));
    updateGL();
    e->accept();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    switch( e->key() )
    {
    case Qt::Key_C:

        break;

    case Qt::Key_F:

        break;

    case Qt::Key_I:
        align_with_axis();
        break;

    case Qt::Key_A:
        view_all();
        updateGL();
        break;
    }
    emit key(e->key());
    e->ignore();


}


void GLWidget::update_projection_matrix()
{
    makeCurrent();
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat) width() / (GLfloat) height(),
                   0.01*radius_, 100.0*radius_);
    glGetDoublev( GL_PROJECTION_MATRIX, projection_matrix_);
    glMatrixMode( GL_MODELVIEW );
}

void GLWidget::translate(const Vec3 &_trans)
{
    // Translate the object by _trans
    // Update modelview_matrix_
    makeCurrent();
    glLoadIdentity();
    glTranslated( _trans[0], _trans[1], _trans[2] );
    glMultMatrixd( modelview_matrix_ );
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview_matrix_);
}

void GLWidget::rotate(const Vec3 &_axis, float _angle)
{
    // Rotate around center center_, axis _axis, by angle _angle
    // Update modelview_matrix_

    Vec3 t( modelview_matrix_[0]*center_[0] +
            modelview_matrix_[4]*center_[1] +
            modelview_matrix_[8]*center_[2] +
            modelview_matrix_[12],
            modelview_matrix_[1]*center_[0] +
            modelview_matrix_[5]*center_[1] +
            modelview_matrix_[9]*center_[2] +
            modelview_matrix_[13],
            modelview_matrix_[2]*center_[0] +
            modelview_matrix_[6]*center_[1] +
            modelview_matrix_[10]*center_[2] +
            modelview_matrix_[14] );

    makeCurrent();
    glLoadIdentity();
    glTranslatef(t[0], t[1], t[2]);
    glRotated( _angle, _axis[0], _axis[1], _axis[2]);
    glTranslatef(-t[0], -t[1], -t[2]);
    glMultMatrixd(modelview_matrix_);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix_);
}

bool GLWidget::map_to_sphere(const Vec2 &_v2D, Vec3 &_v3D)
{
    // This is actually doing the Sphere/Hyperbolic sheet hybrid thing,
    // based on Ken Shoemake's ArcBall in Graphics Gems IV, 1993.
    double x =  (2.0*_v2D[0] - width())/width();
    double y = -(2.0*_v2D[1] - height())/height();
    double xval = x;
    double yval = y;
    double x2y2 = xval*xval + yval*yval;

    const double rsqr = TRACKBALL_RADIUS*TRACKBALL_RADIUS;
    _v3D[0]=xval;
    _v3D[1]=yval;
    if (x2y2 < 0.5*rsqr) {
        _v3D[2] = sqrt(rsqr - x2y2);
    } else {
        _v3D[2] = 0.5*rsqr/sqrt(x2y2);
    }

    return true;
}

Vec3 GLWidget::pickCoordinates(int windowX, int windowY, bool centerIfNoHit, bool *someHit)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX,winY;
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    // read the Z position from Z buffer (in range 0 - 1)
    GLfloat depth;
    GLdouble x, y, z;
    winX = (GLfloat) windowX;
    winY = viewport[3] - (GLfloat) windowY;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    // if nothing was hit here, try the neighboring pixels
    if (depth == 1)
    {
        GLfloat depthValues[9];
        glReadPixels(winX-1, winY-1, 3, 3, GL_DEPTH_COMPONENT, GL_FLOAT, &depthValues);
        for (int i = 0; i < 9; i++)
        {
            // reads beyond the borders could be zero, ignore those too
            if (depthValues[i] != 1 && depthValues[i] != 0)
            {
                depth = depthValues[i];
                break;
            }
        }
    }

    // if nothing was hit, project to center coordinates
    bool noHit = depth == 1;
    if (someHit)
        *someHit = !noHit;
    if (centerIfNoHit && noHit)
    {
        // compute the view ray for the given coordinates
        gluUnProject(winX, winY, 0, modelview, projection, viewport, &x, &y, &z);
        Vec3 minDepth = Vec3(x, y, z);
        gluUnProject(winX, winY, 1, modelview, projection, viewport, &x, &y, &z);
        Vec3 maxDepth = Vec3(x, y, z);
        Vec3 normal = (maxDepth - minDepth).normalized();

        // select point on view ray closest to original center
        double d = normal.dot(center() - minDepth);
        return minDepth + d*normal;
    }

    // compute world coordinates
    gluUnProject(winX, winY, depth, modelview, projection, viewport, &x, &y, &z);
    return Vec3(x, y, z);
}



