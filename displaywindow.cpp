#include "displaywindow.h"
#include "steploader.h"
#include "triangulate.h"
#include "vtk.h"

//QT
#include <qcolor.h>
#include <qcolordialog.h>
#include <qfiledialog.h>
#include <qfileinfo.h>

//VTK
#include <QVTKInteractor.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

DisplayWindow::DisplayWindow(QWidget *parent) : QMainWindow(parent)
{
  actor_count = 0;
  showMaximized();
  setup_vtk();
  setup_layout();
  setup_actions();
  setup_menus();
}

void DisplayWindow::setup_layout() {
  widget = new QWidget();
  layout = new QGridLayout();
  widget->setLayout(layout);
  setCentralWidget(widget);
  list = new QListWidget();
  layout->addWidget(vtk_widget, 0, 0, 1, 1);
  layout->addWidget(list, 0, 1, 1, 1);
  layout->setColumnStretch(0, 3);
}

void DisplayWindow::setup_menus() {
  menu = new QMenuBar();
  
  file_menu = menu->addMenu(tr("&File"));
  file_menu->addAction(load_action);
  file_menu->addAction(quit_action);

  options_menu = menu->addMenu(tr("&Options"));
  options_menu->addAction(background_action);

  setMenuBar(menu);
}

void DisplayWindow::setup_actions() {
  load_action = new QAction(tr("&Load"), this);
  load_action->setShortcuts(QKeySequence::Open);
  load_action->setStatusTip(tr("Load a model"));
  connect(load_action, SIGNAL(triggered()), this, SLOT(load_file()));

  quit_action = new QAction(tr("&Quit"), this);
  quit_action->setShortcuts(QKeySequence::Close);
  quit_action->setStatusTip(tr("Exit Program"));
  connect(quit_action, SIGNAL(triggered()), this, SLOT(close()));

  background_action = new QAction(tr("&Set Background Colour"), this);
  background_action->setStatusTip(tr("What do you think it might do??"));
  connect(background_action, SIGNAL(triggered()), this, SLOT(set_background_colour()));
}

void DisplayWindow::setup_vtk() {
  vtk_widget = new QVTKWidget();
  renderer = vtkSmartPointer<vtkRenderer>::New();
  renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<QVTKInteractor> interactor = vtk_widget->GetInteractor();
  vtkSmartPointer<vtkInteractorStyleTrackballActor> style = vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();
  interactor->SetInteractorStyle(style);
  interactor->SetRenderWindow(renderWindow);


  vtk_widget->SetRenderWindow(renderWindow);
  vtk_widget->show();
  vtk_widget->update();
}

void DisplayWindow::load_file() {
  display_model(load_model());
}

TopoDS_Shape DisplayWindow::load_model() {
  QString filename;
  while(filename.isEmpty()) {
    filename = QFileDialog::getOpenFileName(this, tr("Open Model"), "", tr("STEP (*.stp *.step);; STL (*.stl);; WILD(*.*)"));
  }
  StepLoader loader(filename);
  QFileInfo info(filename);
  QString name = info.fileName();
  list->addItem(name);
  return loader.get_shape();
}

void DisplayWindow::display_model(const TopoDS_Shape& shape) {
  VTK_Helper helper;
  vtkSmartPointer<vtkPolyData> polydata = helper.cascade_to_vtk(triangulate(shape));
  helper.colour_original_faces();
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(polydata);
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->SetPosition(2 * (actor_count % 5), - floor(actor_count / 5), 0);
  normalise_vtk_actor(actor);
  renderer->AddActor(actor);
  actor_count++;
  renderer->ResetCamera();
  vtk_widget->SetRenderWindow(renderWindow);
  vtk_widget->show();
  vtk_widget->update();
}

void DisplayWindow::set_background_colour() {
  QColor colour = QColorDialog::getColor();
  renderer->SetBackground(colour.redF(), colour.greenF(), colour.blueF());
}