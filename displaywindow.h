#include "TopoDS_Shape.hxx"

//QT
#include <qgridlayout.h>
#include <qlistwidget.h>
#include <QMainWindow>
#include <qmenubar.h>

//VTK
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

class DisplayWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit DisplayWindow(QWidget *parent = 0);
  TopoDS_Shape load_model();
  void display_model(const TopoDS_Shape& shape);

private slots:
  void load_file();
  void set_background_colour();

private:
  void setup_menus();
  void setup_actions();
  void setup_vtk();
  void setup_layout();
  
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
};