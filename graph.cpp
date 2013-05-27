#include "graph.h"

#include <cmath>

#include <BRepAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>


#include <iostream>

int Graph::compute_face_type(const TopoDS_Face& face) {
  Handle(Geom_Surface) surface =  BRep_Tool::Surface(face);

  if(surface->IsKind(STANDARD_TYPE(Geom_Plane)))
    return 1;
  if(surface->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
    return 2;
  if(surface->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
    return 3;
  if(surface->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
    return 4;
  return 5;
}

int Graph::compute_edge_type(const TopoDS_Edge& edge) {
  BRepAdaptor_Curve curve(edge);
  GeomAbs_CurveType type = curve.GetType();
  switch(type) {
  case GeomAbs_Line:
    return 1;
  case GeomAbs_Circle:
    return 2;
  default:
    return 3;
  }
}

double Graph::random_float(double start, double stop) {
  double random = ((double) rand()) / (double) RAND_MAX;
  double diff = stop - start;
  double r = random * diff;
  return start + r;
}

std::pair<double, double> Graph::face_function(const TopoDS_Face& face) {
  const int sample_count = 4;
  Standard_Real umin, umax, vmin, vmax;
  BRepTools::UVBounds(face, umin, umax, vmin, vmax);
  Handle(Geom_Surface) surface = BRep_Tool::Surface(face);
  std::pair<double, double> min_max = std::make_pair<double, double>(0,0);
  for(int i = 0; i < sample_count; i++) {
    double u = random_float(umin, umax);
    double v = random_float(vmin, vmax);
    GeomLProp_SLProps props = GeomLProp_SLProps(surface, u, v, 2, 0.01);
    double min = props.MinCurvature();
    double max = props.MaxCurvature();
    if(face.Orientation() != TopAbs_FORWARD) {
      min *= -1;
      max *= -1;
    }
    min_max.first += (min / sample_count);
    min_max.second += (max / sample_count);
  }
  return min_max;
}

void Graph::populate_faces() {
  faces = new TopTools_IndexedMapOfShape();
  TopExp::MapShapes(shape, TopAbs_FACE, *faces);
}

std::pair<double, double> Graph::edge_function(TopoDS_Edge& edge, TopoDS_Face& face1, TopoDS_Face& face2) {
  double angle;
  std::pair<double, double> rvalue = std::pair<double, double>(0.,0.);
  int sample_count = 3;
  Standard_Real start, end;
  Standard_Real length;
  Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, start, end);
  length = start - end;
  for(int i = 0; i < sample_count; i++) {
    double point = random_float(start, end);
    gp_Pnt a = curve->Value(point);
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface(face1);
    GeomAPI_ProjectPointOnSurf pos(a, surface1);
    Standard_Real u, v;
    pos.Parameters(1, u, v);
    GeomLProp_SLProps props1 = GeomLProp_SLProps(surface1, u, v, 1, 0.01);
    gp_Vec normal1 = props1.Normal();
    Handle(Geom_Surface) surface2 = BRep_Tool::Surface(face2);
    GeomAPI_ProjectPointOnSurf pos2(a, surface2);
    pos2.Parameters(1, u, v);
    GeomLProp_SLProps props2 = GeomLProp_SLProps(surface2, u, v, 1, 0.01);
    gp_Vec normal2 = props2.Normal();
    angle = atan2(normal1.CrossMagnitude(normal2), normal1.Dot(normal2));
    if(angle == M_PI) {
      rvalue.first += 0;
      rvalue.second += 0;  
    }
    if(angle < M_PI) {
      rvalue.first += 1 / sample_count;
    }
    if(angle > M_PI) {
      rvalue.second += 1 / sample_count;
    }
  }
  return rvalue;
}

void Graph::compute_face_codes() {
  TopTools_IndexedDataMapOfShapeListOfShape ef_map;
  TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, ef_map);
  for(int i = 1; i < faces->Extent(); i++) {
    double e= 0;
    TopoDS_Face face = TopoDS::Face(faces->FindKey(i));
    std::pair<double, double> face_code = face_function(face);
    double angle = 0;
    TopoDS_Wire wire = BRepTools::OuterWire(face);
    BRepTools_WireExplorer explorer;
    double x, y;
    x = y = 0;
    for(explorer.Init(wire); explorer.More(); explorer.Next()) {
      std::pair<double, double> edge_code = std::make_pair(0,0);
      TopoDS_Edge edge = explorer.Current();
      const TopTools_ListOfShape& m_faces = ef_map.FindFromKey(edge);
      TopoDS_Face face1 = TopoDS::Face(m_faces.First());
      TopoDS_Face face2 = TopoDS::Face(m_faces.Last());
      if(face1 == face2)
        continue; // revolved surface
      int face_type = (compute_face_type(face1) + 1) * (compute_face_type(face2) + 1);
      edge_code = edge_function(edge, face1, face2);
      Standard_Real start, end;
      Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, start, end);
      gp_Pnt a = curve->Value(end - start / 2);
      Handle(Geom_Surface) surface1 = BRep_Tool::Surface(face1);
      GeomAPI_ProjectPointOnSurf pos(a, surface1);
      Standard_Real u, v;
      pos.Parameters(1, u, v);
      GeomLProp_SLProps props1 = GeomLProp_SLProps(surface1, u, v, 1, 0.01);
      gp_Vec normal1 = props1.Normal();
      Handle(Geom_Surface) surface2 = BRep_Tool::Surface(face2);
      GeomAPI_ProjectPointOnSurf pos2(a, surface2);
      pos2.Parameters(1, u, v);
      GeomLProp_SLProps props2 = GeomLProp_SLProps(surface2, u, v, 1, 0.01);
      gp_Vec normal2 = props2.Normal();
      angle = atan2(normal1.CrossMagnitude(normal2), normal1.Dot(normal2));
      if(angle == 0) {
        e = 0;
      } else {
        e = face_type / angle;
      }
      x += e * face_code.first + edge_code.first;
      y += e * face_code.second + edge_code.second;
    }
    std::cout << x << ", " << y << std::endl;
  }
}