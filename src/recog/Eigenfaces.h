// Lose Face - An open source face recognition project
// Copyright (C) 2008-2009 David Capello
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the authors nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

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
