#ifndef GRAPH_H_
#define GRAPH_H_

#include <utility>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

class Graph {
public:
  Graph(const TopoDS_Shape _shape) : shape(_shape) {
    populate_faces();
  }
    
  void compute_face_codes();
private:
  const TopoDS_Shape shape;
  TopTools_IndexedMapOfShape* faces;

  int compute_face_type(const TopoDS_Face&);
  int compute_edge_type(const TopoDS_Edge&);
  std::pair<double, double> face_function(const TopoDS_Face&);
  std::pair<double, double> edge_function(TopoDS_Edge&, TopoDS_Face&, TopoDS_Face&);
  double random_float(double, double);
  void populate_faces();
  
  
  template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
  }
};

#endif // GRAPH_H_