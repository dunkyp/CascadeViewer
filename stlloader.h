#ifndef STEPLOADER_H_
#define STEPLOADER_H_

#include <vector>

#include <QString>

#include <Poly_Triangulation.hxx>

/**
 * @brief The STLLoader class
 * Loads a step file and returns a Triangulation
 */
class STLLoader
{
public:
  STLLoader(QString);
  const QString filename;
  std::vector<Handle(Poly_Triangulation)> get_triangulated_shape();
};

#endif // STEPLOADER_H_