// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LOSEFACE_EIGENFACES_H
#define LOSEFACE_EIGENFACES_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>

#include "Vector.h"
#include "Matrix.h"

/// Calculates eigenfaces from a set of images (vectors really).
///
class Eigenfaces
{
  /// Number of pixels per picture. It is the number of dimensions in the
  /// orignal space (face width x height pixels).
  ///
  size_t m_pixelsPerImage;		// N

  /// Number of eigenfaces components.
  ///
  size_t m_eigenfaceComponents;	 	// M'

  Vector m_eigenvalues;

  Matrix m_eigenvectors;

  /// Set of training images (each column is an image).
  ///
  Matrix m_dataSet;

  /// Set of training images with zero mean.
  ///
  /// @see m_dataSet, m_meanFace
  ///
  Matrix m_dataSetZeroMean;

  /// Average between all faces.
  ///
  Vector m_meanFace;			// Psi

  /// Set of eigenfaces, each column is a eigenface.
  ///
  /// This matrix has @ref eigenspaceComponents columns and
  /// @ref pixelsPerImage rows.
  ///
  Matrix m_eigenfaces;

  /// Number of pre-allocated images (columns).
  ///
  size_t m_preallocatedImages;

public:

  Eigenfaces();
  ~Eigenfaces();

  void reserve(size_t numImages);
  void addImage(const Vector& faceImage);

  size_t getImageCount() const;
  size_t getPixelsPerImage() const;
  size_t getEigenfaceComponents() const;
  size_t getEigenvaluesCount() const;

  bool calculateEigenvalues();
  void calculateEigenfaces(size_t components);
  size_t getNumComponentsFor(double variance) const;
  void projectInEigenspace(const Vector& faceImage, Vector& eigenspacePoint) const;

  //////////////////////////////////////////////////////////////////////
  // Binary I/O
  //////////////////////////////////////////////////////////////////////

  void save(const char* filename) const;
  void load(const char* filename);
  void write(std::ostream& s) const;
  void read(std::istream& s);

};

#endif // LOSEFACE_EIGENFACES_H
