// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LOSEFACE_RECOG_EIGENFACES_H
#define LOSEFACE_RECOG_EIGENFACES_H

#include <cmath>
#include "recog/SubjectsList.h"
#include "recog/Image.h"
#include "Vector.h"
#include "Matrix.h"

namespace recog {

  /// Se encarga del cálculo de las eigenfaces.
  ///
  class Eigenfaces
  {
    /// Cantidad de pixeles por foto. La cantidad de dimensiones del
    /// espacio original.
    ///
    int m_pixelsPerImage;		// N

    /// Cantidad de rostros de "entrenamiento".
    ///
    /// Cantidad de rostros disponibles para calcular los
    /// eigenvalores/vectores, esto equivale a la cantidad de columnas
    /// del @ref dataSet.
    ///
    int m_imageCount;			// M

    /// Cantidad de componentes de eigenfaces.
    ///
    int m_eigenfaceComponents;	 	// M'

    Vector<double> m_eigenvalues;

    Matrix<double> m_eigenvectors;

    /// Conjunto de datos con media nula.
    ///
    Matrix<double> m_dataSet;

    /// Promedio de todos los rostros (rostro medio).
    ///
    Vector<double> m_meanFace;		// psi

    /// Conjunto de eigenfaces, cada columna es un eigenface.
    ///
    /// Esta matriz tiene @ref eigenspaceComponents columnas y
    /// @ref pixelsPerImage filas.
    ///
    Matrix<double> m_eigenfaces;

  public:

    Eigenfaces(SubjectsList& subjects);
    virtual ~Eigenfaces();

    inline int getImageCount() const {
      return m_imageCount;
    }

    inline int getEigenfaceComponents() const {
      return m_eigenfaceComponents;
    }

    inline int getEigenvaluesCount() const {
      return m_eigenvalues.size();
    }

    void save(const char* filename) const;
    void load(const char* filename);

    bool calculateEigenvalues();
    bool calculateEigenfaces(int components);

    int getNumComponentsFor(double levelOfInfo) const;

    Vector<double> projectInEigenspace(Vector<double>& faceImage) const;
  };

  typedef SharedPtr<Eigenfaces> EigenfacesPtr;

  //////////////////////////////////////////////////////////////////////

} // namespace recog

#endif // LOSEFACE_RECOG_EIGENFACES_H
