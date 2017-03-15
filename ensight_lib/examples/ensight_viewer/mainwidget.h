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



#ifndef WIDGET_H
#define WIDGET_H

#include <memory>
#include <QMainWindow>
#include <QMessageBox>
#include <QStringList>
#include <QTime>
#include "ensightobj.h"
#include "ensightsubdivtreeimpl.h"
#include "viewer.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWidget(QWidget* parent = 0);
    ~MainWidget();

private:
    void loadEnsightFile(const QString& filename, int timeStep);
    const QString boundsToString(const Matx& bounds, int dim) const;
    const QString boundsToString(const Bbox& bounds, int dim) const;
    void fillTable();
    void closeEvent(QCloseEvent*) override;

    void drawOctree(const OctreeNode& node);
    void drawQuadtree(const QuadtreeNode& node);

    int selectTimeStep(const QString& fileName, bool& cancel);
    void openFile(bool singleTimeStep);

    QStringList inactiveParts();

private slots:
    void partTypeChanged(int);


    void init();
    void draw();
    void key(int);
    void on_timestep_valueChanged(int i);
    void on_representation_currentIndexChanged(int index);
    void on_subdivtreeupdate_clicked();
    void on_colorvar_currentIndexChanged(int index);
    void on_colorcomponent_currentIndexChanged(int index);

    void on_actionOpen_triggered();
    void on_actionImport_single_time_step_triggered();
    void on_actionViewAll_triggered();
    void on_actionAlign_triggered();
    void on_actionDisplayAxis_toggled();
    void on_actionDisplaySubdivTree_toggled();


private:
    enum RepresentationModes { Outline = 0, Wireframe, Surface, Volume, numRepresentationModes };
    enum PartModes { Active = 0, Inactive, numPartModes };
    static QStringList strRepresentationModes;
    static QStringList strPartModes;

    QMap<QString, PartModes> partMode;
    bool show_axis;
    bool show_space_partioning;
    Ui::MainWidget* ui;
    std::unique_ptr<EnsightObj> ensight;
    std::unique_ptr<Viewer> viewer;

};

#endif // WIDGET_H
