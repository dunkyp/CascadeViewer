#include "steploader.h"
#include "triangulate.h"

#include <BRepTools.hxx>

#include <STEPControl_Reader.hxx>

StepLoader::StepLoader(QString _filename) : filename(_filename) {
}

TopoDS_Shape StepLoader::get_shape() {
  TopoDS_Shape shape;
  STEPControl_Reader reader;
  reader.ReadFile(filename.toLocal8Bit());
  reader.TransferRoots();
  shape = reader.OneShape();
  BRepTools::Clean(shape);
  return shape;
}

std::vector<Handle(Poly_Triangulation)> StepLoader::get_triangulated_shape() {
  return triangulate(get_shape());
}
