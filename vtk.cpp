#include "vtk.h"

//std
#include <map>
#include <set>
#include <utility>

// OpenCASCADE
#include <gp_Pnt.hxx>
#include <Poly_Triangle.hxx>
#include <TColgp_Array1OfPnt.hxx>

//  VTK
#include <vtkCellArray.h>
#include <vtkTriangle.h>


//DEBUG
#include <iostream>

struct vertex {
  double x;
  double y;
  double z;

  vertex(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {
  }

  bool operator<(const vertex& other) const
  {
    if (x != other.x)
      return (x < other.x);

   if (y != other.y)
       return (y < other.y);

   return (z < other.z);
  }
};


/**
* @brief Convert Cascade triangulation to vtk triangulation
*
**/
vtkSmartPointer<vtkPolyData> cascade_to_vtk(const std::vector<Handle(Poly_Triangulation)>& cascade_data) {
  vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();

  int point_counter = 0; // fix this
  int face_counter = 0;
  
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

  std::map<vertex, int> point_map; 
  std::map<int, std::vector< vtkSmartPointer<vtkTriangle> > > faces;
  std::cout << cascade_data.size() << std::endl;
  for(std::vector<Handle(Poly_Triangulation)>::const_iterator it = cascade_data.begin();
    it != cascade_data.end();
    ++it) {
    std::vector< vtkSmartPointer<vtkTriangle> > triangles;

    //Nodes are 1 indexed WTF?! why?
    for(int i = 1; i <= (*it)->NbNodes(); i++) {
      gp_Pnt point = (*it)->Nodes().Value(i);
      vertex p(point.X(), point.Y(), point.Z());
      if(!point_map.count(p)) {
        point_map.insert(std::make_pair(p, point_counter));
        point_counter++;
        points->InsertNextPoint(p.x, p.y, p.z);
      }
    }


    //as are triangles
    for(int i = 1; i <= (*it)->NbTriangles(); i++) {
      vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
      Poly_Triangle pt = (*it)->Triangles().Value(i);
      int index0, index1, index2;
      pt.Get(index0, index1, index2);
      
      gp_Pnt point0 = (*it)->Nodes().Value(index0);
      vertex p0(point0.X(), point0.Y(), point0.Z());
      index0 = point_map[p0];
      triangle->GetPointIds()->SetId(0, index0);
      
      gp_Pnt point1 = (*it)->Nodes().Value(index1);
      vertex p1(point1.X(), point1.Y(), point1.Z());
      index1 = point_map[p1];
      triangle->GetPointIds()->SetId(1, index1);

      gp_Pnt point2 = (*it)->Nodes().Value(index2);
      vertex p2(point2.X(), point2.Y(), point2.Z());
      index2 = point_map[p2];
      triangle->GetPointIds()->SetId(2, index2);
      triangles.push_back(triangle);
      cells->InsertNextCell(triangle);
    }
    faces.insert(std::make_pair(face_counter, triangles));
    face_counter++;  
  }
  poly_data->SetPoints(points);
  poly_data->SetPolys(cells);
  return poly_data;
}

void normalise_vtk_actor(vtkSmartPointer<vtkActor> actor) {
  double* bounds;
  bounds = actor->GetBounds();
  double largest = 0;
  for(int i = 0; i < 3; i++) {
    double size = fabs(bounds[i*2 + 1] - bounds[i * 2]);
    largest = std::max(largest, size);
  }
  actor->SetScale(1/largest);
}