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

#include <iostream>
#include <fstream>

#include "recog/Eigenfaces.h"
#include "recog/recog_util.h"

using namespace recog;

//////////////////////////////////////////////////////////////////////

Eigenfaces::Eigenfaces(SubjectsList& subjects)
  : m_pixelsPerImage(subjects.getImageWidth() *
		     subjects.getImageHeight())
  , m_imageCount(subjects.getImageCount())
  , m_dataSet(m_pixelsPerImage,	// rows
	      m_imageCount)	// columns
{
  // colocamos los niveles de grises de cada imagen en dataSet
  for (int j=0; j<m_imageCount; j++) {
    Vector<double> faceImage = image2vector(subjects.getImage(j).get());
    m_dataSet.setCol(j, faceImage);
  }

  // calculamos la media de todas las caras
  m_dataSet.meanCol(m_meanFace);

  // quitamos la media a cada pixel (dataSet termina siendo los datos
  // de las imágenes pero con media nula)
  for (int j=0; j<m_imageCount; ++j) {
    for (int i=0; i<m_pixelsPerImage; ++i)
      m_dataSet(i, j) -= m_meanFace(i);
  }
}

Eigenfaces::~Eigenfaces()
{
}

void Eigenfaces::save(const char* filename) const
{
  using namespace std;

  unsigned eigenvalues_size = m_eigenvalues.size();
  unsigned meanFace_size = m_meanFace.size();
  unsigned eigenvectors_rows = m_eigenvectors.rows();
  unsigned eigenvectors_cols = m_eigenvectors.cols();
  unsigned eigenfaces_rows = m_eigenfaces.rows();
  unsigned eigenfaces_cols = m_eigenfaces.cols();

  ofstream f(filename, ios::binary);

  f.write((char*)&eigenvalues_size, sizeof(unsigned));
  f.write((char*)m_eigenvalues.getRaw(), sizeof(double)*eigenvalues_size);

  f.write((char*)&meanFace_size, sizeof(unsigned));
  f.write((char*)m_meanFace.getRaw(), sizeof(double)*meanFace_size);

  f.write((char*)&eigenvectors_rows, sizeof(unsigned));
  f.write((char*)&eigenvectors_cols, sizeof(unsigned));
  f.write((char*)m_eigenvectors.getRaw(), sizeof(double)*eigenvectors_rows*eigenvectors_cols);

  f.write((char*)&eigenfaces_rows, sizeof(unsigned));
  f.write((char*)&eigenfaces_cols, sizeof(unsigned));
  f.write((char*)m_eigenfaces.getRaw(), sizeof(double)*eigenfaces_rows*eigenfaces_cols);
}

void Eigenfaces::load(const char* filename)
{
  using namespace std;

  ifstream f(filename, ios::binary);

  unsigned eigenvalues_size;
  unsigned meanFace_size;
  unsigned eigenvectors_rows;
  unsigned eigenvectors_cols;
  unsigned eigenfaces_rows;
  unsigned eigenfaces_cols;

  f.read((char*)&eigenvalues_size, sizeof(unsigned));
  m_eigenvalues.resize(eigenvalues_size);
  f.read((char*)m_eigenvalues.getRaw(), sizeof(double)*eigenvalues_size);

  f.read((char*)&meanFace_size, sizeof(unsigned));
  m_meanFace.resize(meanFace_size);
  f.read((char*)m_meanFace.getRaw(), sizeof(double)*meanFace_size);

  f.read((char*)&eigenvectors_rows, sizeof(unsigned));
  f.read((char*)&eigenvectors_cols, sizeof(unsigned));
  m_eigenvectors.resize(eigenvectors_rows, eigenvectors_cols);
  f.read((char*)m_eigenvectors.getRaw(), sizeof(double)*eigenvectors_rows*eigenvectors_cols);

  f.read((char*)&eigenfaces_rows, sizeof(unsigned));
  f.read((char*)&eigenfaces_cols, sizeof(unsigned));
  m_eigenfaces.resize(eigenfaces_rows, eigenfaces_cols);
  f.read((char*)m_eigenfaces.getRaw(), sizeof(double)*eigenfaces_rows*eigenfaces_cols);

  m_eigenfaceComponents = eigenfaces_cols;
}

/// Calculate eivenvalues and sort them in descendant order.
/// 
bool Eigenfaces::calculateEigenvalues()
{
  // Obtener la matriz de covarianza de MxM para calcular sus
  // eigenvectores (donde M es la cantidad de fotos de
  // entrenamiento). Así evitamos calcular los N eigenvectores de la
  // matriz de covarianza real (una matriz de NxN, A*At, donde N es la
  // cantidad de pixeles)

  Matrix<double> covarianceMatrix;
  m_dataSet.getTranspose(covarianceMatrix);
  covarianceMatrix *= m_dataSet;

  std::cout << "covarianceMatrix = " << covarianceMatrix.rows() << " x " << covarianceMatrix.cols() << "\n";

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
  	  Vector<double> aux = m_eigenvectors.getCol(j);
  	  m_eigenvectors.setCol(j, m_eigenvectors.getCol(i));
  	  m_eigenvectors.setCol(i, aux);
  	}
      }
    }
  }

#if 1
  std::cout << "----------------------------------------------------------------------\n";
  std::cout << "Eigenvalores (" << m_eigenvalues.size() << "):\n";
  double accum = 0.0, total = 0.0;
  for (int i=0; i<m_eigenvalues.size(); ++i)
    total += m_eigenvalues(i);

  std::cout << 0 << "\t" << 0 << "\n";

  for (int i=0; i<m_eigenvalues.size(); ++i) {
    accum += m_eigenvalues(i);
    std::cout << (i+1) << "\t" << (accum/total) << "\t" << "\n";
  }
  std::cout << "----------------------------------------------------------------------\n";
#endif
}

/// Calculates the eigenfaces.
/// 
/// @warning You have to call #calculateEigenvalues before.
/// 
bool Eigenfaces::calculateEigenfaces(int components)
{
  assert(components > 0 && components <= m_eigenvalues.size());
  m_eigenfaceComponents = components;

  // Calculate eigenfaces...
  m_eigenfaces.resize(m_pixelsPerImage,	      // rows
		      m_eigenfaceComponents); // columns

  Vector<double> eigenface(m_pixelsPerImage);
  for (int i=0; i<m_eigenfaceComponents; ++i) {
    eigenface.zero();

    for (int j=0; j<m_imageCount; ++j)
      eigenface += m_eigenvectors(i, j) * m_dataSet.getCol(j);

    m_eigenfaces.setCol(i, eigenface);
  }

  return true;
}

/// Devuelve la cantidad de componentes de eigenfaces que deberían ser
/// necesarios para la cantidad de información que se desea representar.
/// 
/// @param levelOfInfo
///   1.0 significa toda la información, 0.8 un 80% de la información, etc.
/// @return
///   La cantidad de componentes para las eigenfaces. Supuestamente
///   usted luego debería llamar a #calculateEigenfaces con el valor
///   retornado.
/// 
int Eigenfaces::getNumComponentsFor(double levelOfInfo) const
{
  double total = 0.0;
  for (int i=0; i<m_eigenvalues.size(); ++i)
    total += m_eigenvalues(i);

  double accum = 0.0;
  for (int i=0; i<m_eigenvalues.size(); ++i) {
    accum += m_eigenvalues(i);
    if (accum/total >= levelOfInfo)
      return i+1;
  }

  return m_eigenvalues.size();
}

/// Proyecta la imagen @a faceImage en el eigenspace, devolviendo su
/// correspondiente "facespacePoint".
/// 
Vector<double> Eigenfaces::projectInEigenspace(Vector<double>& faceImage) const
{
  Vector<double> eigenspacePoint(m_eigenfaceComponents);

  for (int k=0; k<m_eigenfaceComponents; ++k)
    eigenspacePoint(k) = m_eigenfaces.getCol(k) * (faceImage - m_meanFace);

  return eigenspacePoint;
}
