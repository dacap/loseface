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

#ifndef LOSEFACE_EIGENFACES_H
#define LOSEFACE_EIGENFACES_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>

#include "Vector.h"
#include "Matrix.h"

/// Calculates eigenfaces from a set of images (vectors really).
///
template<typename T>
class Eigenfaces
{
  /// Cantidad de pixeles por foto. La cantidad de dimensiones del
  /// espacio original.
  /// 
  size_t m_pixelsPerImage;		// N

  /// Cantidad de componentes de eigenfaces.
  /// 
  size_t m_eigenfaceComponents;	 	// M'

  Vector<T> m_eigenvalues;

  Matrix<T> m_eigenvectors;

  /// Set of training images (each column is an image).
  /// 
  Matrix<T> m_dataSet;

  /// Set of training images with zero mean.
  ///
  /// @see m_dataSet, m_meanFace
  /// 
  Matrix<T> m_dataSetZeroMean;

  /// Average between all faces.
  /// 
  Vector<T> m_meanFace;			// Psi

  /// Set of eigenfaces, each column is a eigenface.
  /// 
  /// This matrix has @ref eigenspaceComponents columns and
  /// @ref pixelsPerImage rows.
  /// 
  Matrix<T> m_eigenfaces;

public:

  Eigenfaces()
  {
    m_pixelsPerImage = 0;
  }

  void addImage(const Vector<T>& faceImage)
  {
    // If this is the first added image...
    if (m_pixelsPerImage == 0) {
      // Setup the quantity of pixels per image
      m_pixelsPerImage = faceImage.size();

      // Rows(pixels) x columns(images)
      m_dataSet.resize(m_pixelsPerImage, 1);

      // It is the first image in the data set
      m_dataSet.setCol(0, faceImage);
    }
    else {
      // You cannot add differentes images in the same Eigenfaces structure
      assert(m_pixelsPerImage == faceImage.size());

      // Add a column in the data set (each column is an image)
      m_dataSet.addCol(m_dataSet.cols(), faceImage);
    }
  }

  /// Returns the number of training images available to calculate
  /// eigenfaces.
  ///
  inline size_t getImageCount() const {
    return (m_pixelsPerImage == 0) ? 0: m_dataSet.cols();
  }

  /// Returns the number of pixels per image.
  ///
  inline size_t getPixelsPerImage() {
    return m_pixelsPerImage;
  }

  inline int getEigenfaceComponents() const {
    return m_eigenfaceComponents;
  }

  inline int getEigenvaluesCount() const {
    return m_eigenvalues.size();
  }

  /// Calculate eivenvalues and sort them in descendant order.
  /// 
  bool calculateEigenvalues()
  {
    // Calculate the mean of all faces
    m_dataSet.meanCol(m_meanFace);

    // Substract the mean from all faces
    m_dataSetZeroMean.resize(m_dataSet.rows(),
			     m_dataSet.cols());
    for (int j=0; j<m_dataSet.cols(); ++j)
      m_dataSetZeroMean.setCol(j, m_dataSet.getCol(j) - m_meanFace);

    // Obtener la matriz de covarianza de MxM para calcular sus
    // eigenvectores (donde M es la cantidad de fotos de
    // entrenamiento). Así evitamos calcular los N eigenvectores de la
    // matriz de covarianza real (una matriz de NxN, A*At, donde N es la
    // cantidad de pixeles)

    Matrix<T> covarianceMatrix;
    m_dataSetZeroMean.getTranspose(covarianceMatrix);
    covarianceMatrix *= m_dataSetZeroMean;

    //std::cout << "covarianceMatrix = " << covarianceMatrix.rows() << " x " << covarianceMatrix.cols() << "\n";

    // calcular los eigenvectores que sirven para formar las eigenfaces
    // por medio de una combinación lineal
    try {
      covarianceMatrix.eig_sym(m_eigenvalues,
			       m_eigenvectors);
    }
    catch (std::exception& e) {
      return false;
    }

    // ordenar los eigenvectores según su correspondiente eigenvalor
    // (un eigenvalor más grande es más significante)
    for (int i=0; i<m_eigenvalues.size(); ++i) { // TODO reemplazar con un qsort?
      for (int j=i+1; j<m_eigenvalues.size(); ++j) {
	if (std::fabs(m_eigenvalues(j)) > std::fabs(m_eigenvalues(i))) {
	  // intercambiar eigenvalores
	  std::swap(m_eigenvalues(i), m_eigenvalues(j));
	  // intercambiar eigenvectores
	  {
	    Vector<T> aux = m_eigenvectors.getCol(j);
	    m_eigenvectors.setCol(j, m_eigenvectors.getCol(i));
	    m_eigenvectors.setCol(i, aux);
	  }
	}
      }
    }

#if 0
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "Eigenvalores (" << m_eigenvalues.size() << "):\n";
    T accum = 0.0, total = 0.0;
    for (int i=0; i<m_eigenvalues.size(); ++i)
      total += m_eigenvalues(i);

    std::cout << 0 << "\t" << 0 << "\n";

    for (int i=0; i<m_eigenvalues.size(); ++i) {
      accum += m_eigenvalues(i);
      std::cout << (i+1) << "\t" << (accum/total) << "\t" << "\n";
    }
    std::cout << "----------------------------------------------------------------------\n";
#endif

    return true;
  }
  
  /// Calculates the eigenfaces.
  /// 
  /// @warning You have to call #calculateEigenvalues before.
  /// 
  void calculateEigenfaces(size_t components)
  {
    assert(components > 0 && components <= m_eigenvalues.size());
    m_eigenfaceComponents = components;

    // Calculate eigenfaces...
    m_eigenfaces.resize(m_pixelsPerImage,	// Rows
			m_eigenfaceComponents); // Columns

    Vector<T> eigenface(m_pixelsPerImage);
    for (size_t i=0; i<m_eigenfaceComponents; ++i) {
      eigenface.zero();
      for (size_t j=0; j<m_dataSetZeroMean.cols(); ++j)
	eigenface += m_eigenvectors(i, j) * m_dataSetZeroMean.getCol(j);

      m_eigenfaces.setCol(i, eigenface);
    }
  }

  /// Returns the number of eigenfaces required to represent the
  /// specified level of variance.
  /// 
  /// @param variance
  ///   1.0 means all the variance, 0.8 is 80%, etc.
  /// @return
  ///   The number of components for eigenfaces. Then you should call
  ///   #calculateEigenfaces with this returned value.
  /// 
  size_t getNumComponentsFor(T variance) const
  {
    T total = 0.0;
    for (size_t i=0; i<m_eigenvalues.size(); ++i)
      total += m_eigenvalues(i);

    T accum = 0.0;
    for (size_t i=0; i<m_eigenvalues.size(); ++i) {
      accum += m_eigenvalues(i);
      if (accum/total >= variance)
	return i+1;
    }

    return m_eigenvalues.size();
  }

  /// Projects the image specified @a faceImage into the eigenspace,
  /// returning its "facespacePoint".
  ///
  /// @param faceImage
  ///   Face to be projected in the eigenspace.
  ///
  /// @param eigenspacePoint
  ///   Resulting point in the eigenspace.
  /// 
  void projectInEigenspace(const Vector<T>& faceImage, Vector<T>& eigenspacePoint) const
  {
    eigenspacePoint.resize(m_eigenfaceComponents);
    for (size_t k=0; k<m_eigenfaceComponents; ++k)
      eigenspacePoint(k) = m_eigenfaces.getCol(k) * (faceImage - m_meanFace);
  }

  //////////////////////////////////////////////////////////////////////
  // Binary I/O
  //////////////////////////////////////////////////////////////////////

  void save(const char* filename) const
  {
    std::ofstream f(filename, std::ios::binary);
    write(f);
  }

  void load(const char* filename)
  {
    std::ifstream f(filename, std::ios::binary);
    read(f);
  }

  void write(std::ostream& s) const
  {
    size_t eigenvalues_size = m_eigenvalues.size();
    size_t meanFace_size = m_meanFace.size();
    size_t eigenvectors_rows = m_eigenvectors.rows();
    size_t eigenvectors_cols = m_eigenvectors.cols();
    size_t eigenfaces_rows = m_eigenfaces.rows();
    size_t eigenfaces_cols = m_eigenfaces.cols();

    s.write((char*)&eigenvalues_size, sizeof(size_t));
    s.write((char*)m_eigenvalues.getRaw(), sizeof(T)*eigenvalues_size);

    s.write((char*)&meanFace_size, sizeof(size_t));
    s.write((char*)m_meanFace.getRaw(), sizeof(T)*meanFace_size);

    s.write((char*)&eigenvectors_rows, sizeof(size_t));
    s.write((char*)&eigenvectors_cols, sizeof(size_t));
    s.write((char*)m_eigenvectors.getRaw(), sizeof(T)*eigenvectors_rows*eigenvectors_cols);

    s.write((char*)&eigenfaces_rows, sizeof(size_t));
    s.write((char*)&eigenfaces_cols, sizeof(size_t));
    s.write((char*)m_eigenfaces.getRaw(), sizeof(T)*eigenfaces_rows*eigenfaces_cols);
  }

  void read(std::istream& s)
  {
    size_t eigenvalues_size;
    size_t meanFace_size;
    size_t eigenvectors_rows;
    size_t eigenvectors_cols;
    size_t eigenfaces_rows;
    size_t eigenfaces_cols;

    s.read((char*)&eigenvalues_size, sizeof(size_t));
    m_eigenvalues.resize(eigenvalues_size);
    s.read((char*)m_eigenvalues.getRaw(), sizeof(T)*eigenvalues_size);

    s.read((char*)&meanFace_size, sizeof(size_t));
    m_meanFace.resize(meanFace_size);
    s.read((char*)m_meanFace.getRaw(), sizeof(T)*meanFace_size);

    s.read((char*)&eigenvectors_rows, sizeof(size_t));
    s.read((char*)&eigenvectors_cols, sizeof(size_t));
    m_eigenvectors.resize(eigenvectors_rows, eigenvectors_cols);
    s.read((char*)m_eigenvectors.getRaw(), sizeof(T)*eigenvectors_rows*eigenvectors_cols);

    s.read((char*)&eigenfaces_rows, sizeof(size_t));
    s.read((char*)&eigenfaces_cols, sizeof(size_t));
    m_eigenfaces.resize(eigenfaces_rows, eigenfaces_cols);
    s.read((char*)m_eigenfaces.getRaw(), sizeof(T)*eigenfaces_rows*eigenfaces_cols);

    m_eigenfaceComponents = eigenfaces_cols;
  }

};

#endif // LOSEFACE_EIGENFACES_H
