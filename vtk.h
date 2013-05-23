#include <vector>

#include <Poly_Triangulation.hxx>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

vtkSmartPointer<vtkPolyData> cascade_to_vtk(const std::vector<Handle(Poly_Triangulation)>& cascade_data);
void normalise_vtk_actor(vtkSmartPointer<vtkActor> actor);