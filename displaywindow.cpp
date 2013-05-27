#include "displaywindow.h"
#include "steploader.h"
#include "triangulate.h"
#include "vtk.h"
#include "graph.h"

//QT
#include <qcolor.h>
#include <qcolordialog.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qpoint.h>

//VTK
#include <QVTKInteractor.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

#include <iostream>

DisplayWindow::DisplayWindow(QWidget *parent) : QMainWindow(parent)
{
  actor_count = 0;
  showMaximized();
  setup_vtk();
  setup_layout();
  setup_actions();
  setup_menus();
  setup_buttons();
}

DisplayWindow::~DisplayWindow() {
  //Actions
  delete load_action;
  delete background_action;
  //Menus
  delete file_menu;
  delete options_menu;
  delete menu;
  //Buttons
  delete copy_button;
  delete delete_button;
  //Layouts
  delete layout;
  delete button_layout;
  //Widgets
  delete button_container;
  delete list;
  delete widget;
}

void DisplayWindow::setup_buttons() {
  connect(copy_button, SIGNAL(clicked()), this, SLOT(copy_model()));
  connect(delete_button, SIGNAL(clicked()), this, SLOT(delete_model()));
}

void DisplayWindow::setup_layout() {
  widget = new QWidget();
  layout = new QGridLayout();
  button_container = new QWidget();
  button_layout = new QHBoxLayout();
  widget->setLayout(layout);
  button_container->setLayout(button_layout);
  setCentralWidget(widget);
  list = new QListWidget();
  layout->addWidget(vtk_widget, 0, 0, 2, 1);
  layout->addWidget(button_container, 0, 1, 1, 1);
  layout->addWidget(list, 1, 1, 1, 1);
  layout->setColumnStretch(0, 3);

  copy_button = new QPushButton(tr("Copy"));
  delete_button = new QPushButton(tr("Delete"));
  button_layout->addWidget(copy_button);
  button_layout->addWidget(delete_button);
}

void DisplayWindow::setup_menus() {
  menu = new QMenuBar();
  
  file_menu = menu->addMenu(tr("&File"));
  file_menu->addAction(load_action);
  file_menu->addAction(quit_action);

  options_menu = menu->addMenu(tr("&Options"));
  options_menu->addAction(background_action);

  analysis_menu = menu->addMenu(tr("&Analysis"));
  analysis_menu->addAction(graph_action);

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

  graph_action = new QAction(tr("&Graph shape"), this);
  graph_action->setStatusTip(tr("Convert shape to attributed face adjacency graph"));
  connect(graph_action, SIGNAL(triggered()), this, SLOT(create_graph()));
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
  add_model(load_model());
}

model DisplayWindow::load_model() {
  QString filename;
  while(filename.isEmpty()) {
    filename = QFileDialog::getOpenFileName(this, tr("Open Model"), "", tr("STEP (*.stp *.step);; STL (*.stl);; WILD(*.*)"));
  }
  StepLoader loader(filename);
  QFileInfo info(filename);
  QString name = info.fileName();
  return model(name, loader.get_shape());
}

void DisplayWindow::add_model(model& shape) {
  list->addItem(shape.name);
  vtkSmartPointer<vtkActor> actor = display_model(shape);
  if(!name_actor_map.contains(shape.name)) {
    name_actor_map.insert(shape.name, std::make_pair(actor, shape.shape));
  }
  else {
    shape.name = new_name(shape.name);
    name_actor_map.insert(shape.name, std::make_pair(actor, shape.shape));
  }
}

vtkSmartPointer<vtkActor> DisplayWindow::display_model(const model& shape) {
  VTK_Helper helper;
  vtkSmartPointer<vtkPolyData> polydata = helper.cascade_to_vtk(triangulate(shape.shape));
  helper.colour_original_faces();
  return display_polydata(polydata);
}

vtkSmartPointer<vtkActor> DisplayWindow::display_polydata(const vtkSmartPointer<vtkPolyData> polydata) {
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
  return actor;
}

void DisplayWindow::set_background_colour() {
  QColor colour = QColorDialog::getColor();
  renderer->SetBackground(colour.redF(), colour.greenF(), colour.blueF());
}

void DisplayWindow::copy_model() {
  if(list->currentItem()) {
    QListWidgetItem* item = list->currentItem();
    vtkSmartPointer<vtkActor> actor = name_actor_map[item->text()].first;
    vtkSmartPointer<vtkPolyData> polydata = vtkPolyData::SafeDownCast(actor->GetMapper()->GetInput());
    vtkSmartPointer<vtkPolyData> polydata_copy = vtkSmartPointer<vtkPolyData>::New();
    polydata_copy->DeepCopy(polydata);
    vtkSmartPointer<vtkActor> new_actor = display_polydata(polydata_copy);
    QString copy_name = new_name(item->text());
    list->addItem(copy_name);
    name_actor_map.insert(copy_name, std::make_pair(new_actor, name_actor_map[item->text()].second));
  }
}

void DisplayWindow::delete_model() {
  if(list->currentItem()) {
    QListWidgetItem* item = list->currentItem();
    vtkSmartPointer<vtkActor> actor = name_actor_map[item->text()].first;
    renderer->RemoveActor(actor);
    vtk_widget->update();
    delete item;
  }
}

QString DisplayWindow::new_name(QString name) {
  //This will probably be changed as it could be a little misleading
  if(name_actor_map.contains(name))
    return new_name(name.append("_copy"));
  return name;
}

void DisplayWindow::create_graph() {
  if(list->currentItem()) {
    QListWidgetItem* item = list->currentItem();
    TopoDS_Shape shape = name_actor_map[item->text()].second;
    Graph graph(shape);
    graph.compute_face_codes();
  }
}