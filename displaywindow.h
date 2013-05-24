#include "TopoDS_Shape.hxx"

//std
#include <utility>

//QT
#include <QHBoxLayout>
#include <qgridlayout.h>
#include <qlistwidget.h>
#include <qmainwindow.h>
#include <qmap.h>
#include <qmenubar.h>
#include <qpushbutton.h>

//VTK
#include <QVTKWidget.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

struct model {
  model(QString _name, TopoDS_Shape _shape) : name(_name), shape(_shape) {
  }
  QString name;
  TopoDS_Shape shape;
};

class DisplayWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit DisplayWindow(QWidget *parent = 0);
  ~DisplayWindow();
  model load_model();
  void add_model(model& shape);
  vtkSmartPointer<vtkActor> display_model(const model& shape);
  vtkSmartPointer<vtkActor> display_polydata(const vtkSmartPointer<vtkPolyData> polydata);

private slots:
  void load_file();
  void set_background_colour();
  void copy_model();
  void delete_model();

private:
  void setup_menus();
  void setup_actions();
  void setup_vtk();
  void setup_layout();
  void setup_buttons();
  QString new_name(QString name);
  
  //Menu
  QMenuBar* menu;
  //File
  QMenu* file_menu;
  QAction* load_action;
  QAction* quit_action;
  //Options
  QMenu* options_menu;
  QAction* background_action;

  QWidget* widget;
  QGridLayout* layout;
  QListWidget* list;

  //Visualisation
  int actor_count;
  QVTKWidget* vtk_widget;
  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkRenderWindow> renderWindow;

  //Object Management
  QWidget* button_container;
  QHBoxLayout* button_layout;
  QPushButton* copy_button;
  QPushButton* delete_button;
  QMap< QString, vtkSmartPointer<vtkActor> > name_actor_map;
};