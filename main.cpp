#include "displaywindow.h"
#include <QApplication>


#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkImageViewer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkJPEGReader.h>

int main(int argc, char** argv) {
  QApplication a(argc, argv);
	DisplayWindow w;
  w.show();
  return a.exec();
}