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


#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMap>
#include <QSettings>
#include "ensightcell.h"
#include "ensightlib.h"
#include "ensightobj.h"
#include "ensightpart.h"
#include "ensightreader.h"
#include "ensightvariable.h"

QStringList MainWidget::strRepresentationModes = QStringList() << "Outline"
                                                               << "Wireframe"
                                                               << "Surface"
                                                               << "Volume";
QStringList MainWidget::strPartModes = QStringList() << "Active" << "Inactive";


MainWidget::MainWidget(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    setGeometry(100, 100, 300, 600);
    QApplication::setApplicationName("ensight_viewer_demo");
    QApplication::setOrganizationName("ITWM");

    show_axis = true;
    show_space_partioning = false;

    // Viewer
    viewer.reset(new Viewer());
    viewer->setGeometry(800, 100, 600, 600);

    connect(viewer->gl, SIGNAL(init()), this, SLOT(init()));
    connect(viewer->gl, SIGNAL(draw()), this, SLOT(draw()));
    connect(viewer->gl, SIGNAL(key(int)), this, SLOT(key(int)));
    viewer->show();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::closeEvent(QCloseEvent*)
{
    if (viewer)
        viewer->close();
}

void MainWidget::loadEnsightFile(const QString& filename, int timeStep)
{
    QTime t;
    t.start();
    qDebug() << "Load File: " << filename;

    ensight.reset(EnsightLib::readEnsight(filename, timeStep));
    qDebug() << "Time to read File: " << t.elapsed();
    partMode.clear();

    if (!ensight)
    {
        QMessageBox::critical(this, "Error", EnsightObj::ERROR_STR);
        return;
    }

    // Update Time step
    ui->timestep->setMinimum(0);
    ui->timestep->setMaximum(ensight->getNumberOfTimesteps()-1);
    ui->timestep->setValue(0);
    ui->time->setText(QString("%0 / %1").
                      arg(ensight->getTimesteps()[0]).
                      arg(ensight->getTimesteps()[ensight->getNumberOfTimesteps()-1]));

    // Update Representation
    ui->representation->clear();
    for (auto str : strRepresentationModes)
        ui->representation->addItem(str);

    // Update Color by
    ui->colorvar->clear();
    ui->colorvar->addItem("Solid Color");
    for (int i = 0; i < ensight->getNumberOfVariables(); i++)
    {
        EnsightVariableIdentifier var = ensight->getVariable(i);
        ui->colorvar->addItem(var.getName());
    }
    ui->colorvar->setCurrentIndex(0);

    // Table headers
    ui->table->setRowCount(2*ensight->getNumberOfParts()+2);
    ui->table->setColumnCount(ensight->getNumberOfVariables()  + 4);
    ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem("Bounds"));
    ui->table->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
    ui->table->setHorizontalHeaderItem(2, new QTableWidgetItem("Vertices"));
    ui->table->setHorizontalHeaderItem(3, new QTableWidgetItem("Cells"));
    for(int i=0;i<ensight->getNumberOfVariables();i++)
    {
        EnsightVariableIdentifier var = ensight->getVariable(i);
        ui->table->setHorizontalHeaderItem(i+4, new QTableWidgetItem(var.getName()));
    }
    ui->table->setVerticalHeaderItem(0, new QTableWidgetItem("Global"));
    ui->table->setVerticalHeaderItem(1, new QTableWidgetItem("Global [timestep]"));

    for(int i=0;i<ensight->getNumberOfParts();i++)
    {
        EnsightPart *part = ensight->getPart(i);
        ui->table->setVerticalHeaderItem(2*i+2, new QTableWidgetItem(part->getName()));
        ui->table->setVerticalHeaderItem(2*i+3, new QTableWidgetItem(part->getName() + " [timestep]"));
        QComboBox *combo = new QComboBox();
        for (auto str : strPartModes)
            combo->addItem(str);
        ui->table->setCellWidget(2*i+2,1, combo);
        connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(partTypeChanged(int)));
    }
    fillTable();

    // Update Viewer
    Bbox bounds = ensight->getGeometryBounds(-1, QStringList());
    if(!bounds.isEmpty())
    {
        viewer->gl->set_scene_pos(bounds.center(), bounds.diagonal().norm());
        viewer->gl->updateGL();
    }
}

void MainWidget::fillTable()
{
    int timestep = ui->timestep->value();

    // Geometry bounds - global and for current time step - activated only
    ui->table->setItem(0,0, new QTableWidgetItem(boundsToString(ensight->getGeometryBounds(-1, QStringList()), 3)));
    ui->table->setItem(1,0, new QTableWidgetItem(boundsToString(ensight->getGeometryBounds(timestep, QStringList()), 3)));

    for(int i=0;i<ensight->getNumberOfParts();i++)
    {
        EnsightPart *part = ensight->getPart(i);
        ui->table->setItem(2*i+2,0, new QTableWidgetItem(boundsToString(part->getGeometryBounds(-1), 3)));
        ui->table->setItem(2*i+3,0, new QTableWidgetItem(boundsToString(part->getGeometryBounds(timestep), 3)));

        ui->table->setItem(2*i+3,2, new QTableWidgetItem(QString::number(part->getVertexCount(timestep))));

        QList<EnsightCellList*> cells = part->getCells(timestep);
        QString cellStr = "";
        for(int i=0;i<cells.size();i++) cellStr += QString("%0: %1; ").arg(Ensight::strCell[cells[i]->getType()]).arg(cells[i]->getValues().cols());
        ui->table->setItem(2*i+3,3, new QTableWidgetItem(cellStr));
    }


    // Variable bounds - global and for current time step - activated only
    for(int i=0;i<ensight->getNumberOfVariables();i++)
    {
        EnsightVariableIdentifier var = ensight->getVariable(i);
        ui->table->setItem(0,i+4, new QTableWidgetItem(boundsToString(ensight->getVariableBounds(var.getName(), -1), var.getDim())));
        ui->table->setItem(1,i+4, new QTableWidgetItem(boundsToString(ensight->getVariableBounds(var.getName(), timestep), var.getDim())));

        for(int j=0;j<ensight->getNumberOfParts();j++)
        {
            EnsightPart *part = ensight->getPart(j);
            ui->table->setItem(2*j+2,i+4, new QTableWidgetItem(boundsToString(part->getVariableBounds(var.getName(), -1), var.getDim())));
            ui->table->setItem(2*j+3,i+4, new QTableWidgetItem(boundsToString(part->getVariableBounds(var.getName(), timestep), var.getDim())));
        }
    }

#if QT_VERSION >= 0x050000
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    ui->table->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
}

const QString MainWidget::boundsToString(const Bbox & bounds, int dim) const
{
    if(bounds.isEmpty())
        return "[]";
    QString result = "";

    for(int i=0;i<dim;i++)
        result.append(QString("[%0,%1]").arg(bounds.minCorner()[i]).arg(bounds.maxCorner()[i]));
    return result;
}

const QString MainWidget::boundsToString(const Matx& bounds, int dim) const
{
    QString result = "";
    for (int i = 0; i < dim; i++)
        result.append(QString("[%0,%1]").arg(bounds(i, 0)).arg(bounds(i, 1)));
    return result;
}

void MainWidget::partTypeChanged(int)
{
    if (!ensight)
        return;

    for (int j = 0; j < ensight->getNumberOfParts(); j++)
    {
        auto box = static_cast<QComboBox*>(ui->table->cellWidget(2*j+2, 1));
        PartModes mode = (PartModes) box->currentIndex();
        partMode[ensight->getPart(j)->getName()] = mode;
    }

    // Viewer
    Bbox bounds = ensight->getGeometryBounds(-1, inactiveParts());
    if(!bounds.isEmpty())
    {
        viewer->gl->set_scene_pos(bounds.center(), bounds.diagonal().norm());
        viewer->gl->updateGL();
    }

    fillTable();
}

void MainWidget::on_timestep_valueChanged(int i)
{
    if(!ensight)
        return;
    int numTimeSteps = ensight->getNumberOfTimesteps();
    ui->time->setText(QString("%0 / %1")
                          .arg(ensight->getTimesteps()[i])
                          .arg(ensight->getTimesteps()[numTimeSteps - 1]));
    viewer->gl->updateGL();
    fillTable();
}

QStringList MainWidget::inactiveParts()
{
    QStringList inactive;
    for (auto part = partMode.begin(); part != partMode.end(); ++part)
        if (part.value() == Inactive)
            inactive << part.key();
    return inactive;
}


void MainWidget::init()
{
    glClearColor(0, 0, 0, 0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);

    GLfloat ambColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat diffColor[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, diffColor);
}

void MainWidget::draw()
{
    glDisable(GL_LIGHTING);
    if (show_axis)
        viewer->gl->drawAxis(viewer->gl->radius() / 2.);
    if (!ensight)
        return;
    int timestep = ui->timestep->value();

    auto repIdx = ui->representation->currentIndex();
    RepresentationModes representationMode = (RepresentationModes) repIdx;
    // colorMode==0 => Solid Color, colorModel>0 => color by variable <colorMode-1>
    int colorMode = ui->colorvar->currentIndex();
    double colorMin = -1; double colorMax = -1;
    int colorRow = 0;

    EnsightVariableIdentifier colorVar;
    if (colorMode > ensight->getNumberOfVariables())
        colorMode = 0;
    if (colorMode > 0)
    {
        colorVar = ensight->getVariable(colorMode-1);
        if (!colorVar.isNull())
        {
            Matx b = ensight->getVariableBounds(colorVar.getName(), -1);
            if (colorVar.getType() == Ensight::VectorPerNode)
            {
                int selection = ui->colorcomponent->currentIndex();
                if (selection == 0)
                    colorRow = 3;
                else
                    colorRow = selection-1;
            }
            colorMin = b(colorRow, 0);
            colorMax = b(colorRow, 1);
            if (colorMax <= colorMin)
                colorMax = colorMin + 1e-6;
        }
    }

    for (int i = 0; i < ensight->getNumberOfParts(); i++)
    {
        EnsightPart* part = ensight->getPart(i);
        if (partMode[part->getName()] == Inactive)
            continue;
        glPointSize(1);
        QList<EnsightCellList*> cells = part->getCells(timestep);
        for (int j = 0; j < cells.size(); j++)
        {
            if (representationMode == Outline)
            {
                glDisable(GL_LIGHTING);
                Bbox b = part->getGeometryBounds(timestep);
                viewer->gl->drawBounds(b.minCorner(), b.maxCorner());
            }
            else if (representationMode == Wireframe || representationMode == Surface)
            {
                EnsightCellList *c = cells.at(j);
                if (representationMode == Wireframe)
                {
                    glDisable(GL_LIGHTING);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glEnable(GL_LIGHTING);

                }
                if (colorMode <= 0)
                {
                    viewer->gl->drawCells(part->getVertices(timestep), c->getValues());
                }
                else
                {
                    const Vecx& vals = part->getVariableValues(colorVar.getName(), timestep).row(colorRow);
                    viewer->gl->drawCells(part->getVertices(timestep), c->getValues(),
                                          vals, colorMin, colorMax);
                }
            }
            else if (representationMode == Volume)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                EnsightCellList *c = cells.at(j);
                if(c->getType()>=Ensight::Tetradhedron) // volumetric data set
                {
                    glDisable(GL_LIGHTING);
                    glPointSize(5);
                    viewer->gl->drawPointCloud(part->getVertices(timestep));
                }
                else
                {
                    if(c->getType()>Ensight::Bar)
                        glEnable(GL_LIGHTING);
                    else
                        glDisable(GL_LIGHTING);
                    viewer->gl->drawCells(part->getVertices(timestep), c->getValues());
                }
            }
        }
    }

    if (show_space_partioning)
    {
        // For performance reasons, only EnsightSubdivTree has a virtual method
        // interface. The implementation uses static polymorphism for the nodes,
        // since these are implementation details.
        // However, we want to display the individual nodes here, so we have to
        // use a dynamic_cast to dispatch at runtime.
        auto tree = ensight->getSubdivTree();
        auto octree = dynamic_cast<const EnsightOctree*>(tree);
        auto quadtree = dynamic_cast<const EnsightQuadtree*>(tree);
        if (octree)
            drawOctree(octree->getRoot());
        else if (quadtree)
            drawQuadtree(quadtree->getRoot());
    }
}

template <typename N>
void drawTree(Viewer* viewer, const N& node)
{
    const auto& children = node.getChildren();
    if (!children)
    {
        glLineWidth(1);
        glColor3d(0, 0, 1);
        viewer->gl->drawBounds(node.getBounds().minCorner(),
                               node.getBounds().maxCorner());
    }
    else
    {
        for (const auto& child : *children)
            drawTree(viewer, child);
    }
}

void MainWidget::drawOctree(const OctreeNode& node)
{
    drawTree(viewer.get(), node);
}

void MainWidget::drawQuadtree(const QuadtreeNode& node)
{
    drawTree(viewer.get(), node);
}

void MainWidget::key(int k)
{
    switch (k)
    {
    case Qt::Key_F:
        on_actionViewAll_triggered();
        break;

    case Qt::Key_I:
        on_actionAlign_triggered();
        break;

    case Qt::Key_O:
        ui->actionDisplaySubdivTree->toggle();
        break;
    case Qt::Key_A:
        ui->actionDisplayAxis->toggle();
        break;
    }
}

void MainWidget::on_representation_currentIndexChanged(int)
{
    if (!ensight)
        return;
    viewer->gl->updateGL();
}

void MainWidget::on_subdivtreeupdate_clicked()
{
    if (!ensight)
        return;

    QTime t;
    qDebug() << "Creating Octree/Quadtree...";
    t.start();
    ensight->createSubdivTree(ui->subdivtreedepth->value(),
                              ui->subdivtreeelements->value(), inactiveParts());
    qDebug() << "Time to create Octree/Quadtree: " << t.elapsed();
    viewer->gl->updateGL();
}

void MainWidget::on_colorvar_currentIndexChanged(int index)
{
    if (!ensight)
        return;
    if (index > 0)
    {
        EnsightVariableIdentifier var = ensight->getVariable(index - 1);
        ui->colorcomponent->setEnabled(var.getDim() > 1);
    }
    else
    {
        ui->colorcomponent->setEnabled(false);
    }
    viewer->gl->updateGL();
}

void MainWidget::on_colorcomponent_currentIndexChanged(int)
{
    viewer->gl->updateGL();
}

int MainWidget::selectTimeStep(const QString& fileName, bool& cancel)
{
    // read case file to get the number of time steps
    EnsightCase caseFile(fileName);
    if (!caseFile.readCaseFile())
    {
        QMessageBox::critical(this, "Error", EnsightObj::ERROR_STR);
        cancel = true;
        return -1;
    }

    bool ok;
    int nTimeSteps = caseFile.timesteps.rows();
    int timeStep = QInputDialog::getInt(this, "Import EnSight data at time step ...",
                                        "Time Step:", 0, 0, nTimeSteps-1, 1, &ok);

    cancel = !ok;
    return timeStep;
}

void MainWidget::openFile(bool singleTimeStep)
{
    QSettings settings;
    QString lastFile = settings.value("last_file").toString();
    auto filter = "Ensight Files (*.case *.encas);;All Files (*)";
    QString fileName = QFileDialog::getOpenFileName(this, "Load Ensight File",
                                                    lastFile, filter);
    if (!fileName.isEmpty())
    {
        int timeStep = -1;
        if (singleTimeStep)
        {
            bool cancel = false;
            timeStep = selectTimeStep(fileName, cancel);
            if (cancel)
                return;
        }

        settings.setValue("last_file", fileName);
        loadEnsightFile(fileName, timeStep);
    }
}

void MainWidget::on_actionOpen_triggered()
{
    openFile(false);
}

void MainWidget::on_actionImport_single_time_step_triggered()
{
    openFile(true);
}

void MainWidget::on_actionViewAll_triggered()
{
    viewer->gl->view_all();
    viewer->gl->updateGL();
}

void MainWidget::on_actionAlign_triggered()
{
    viewer->gl->align_with_axis();
    viewer->gl->updateGL();
}

void MainWidget::on_actionDisplayAxis_toggled()
{
    show_axis = ui->actionDisplayAxis->isChecked();
    viewer->gl->updateGL();
}

void MainWidget::on_actionDisplaySubdivTree_toggled()
{
    show_space_partioning = ui->actionDisplaySubdivTree->isChecked();
    viewer->gl->updateGL();
}
