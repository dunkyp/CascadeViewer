#ifndef STEPLOADER_H_
#define STEPLOADER_H_

#include <vector>

#include <QString>

#include <Poly_Triangulation.hxx>

#include <TopoDS_Shape.hxx>

/**
 * @brief The StepLoader class
 * Loads a step file and returns a TopoDS_Shape
 */
class StepLoader
{
public:
  StepLoader(QString);
  const QString filename;
  TopoDS_Shape get_shape();
  std::vector<Handle(Poly_Triangulation)> get_triangulated_shape();
};

#endif // STEPLOADER_H_
