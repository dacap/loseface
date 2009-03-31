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

#include "recog/FrEigenfacesMlp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace recog;

FrEigenfacesMlp::FrEigenfacesMlp(SubjectsListPtr subjects,
				 EigenfacesPtr eigenfaces)
  : FrSystem(subjects)
  , eigenfaces(eigenfaces)
  , bp(net)
{
  normalize = false;
}

FrEigenfacesMlp::~FrEigenfacesMlp()
{
}

void FrEigenfacesMlp::configure(int hiddens, double threshold)
{
  this->threshold = threshold;

  net = Net(eigenfaces->getEigenfaceComponents(),
	    hiddens,
	    getSubjectCount());
  net.initRandom(-1.0, 1.0);

  bp.setLearningRate(0.6);
  bp.setMomentum(0.1);
}

int FrEigenfacesMlp::recognize(const Image* faceImage) const
{
  Vector<double> imagePoint = image2vector(faceImage);
  Vector<double> eigenspacePoint = eigenfaces->projectInEigenspace(imagePoint);

  return calcMinDistance(eigenspacePoint);
}

void FrEigenfacesMlp::convertToPatterns(SubjectsListPtr subjectsList,
					PatternSet& patterns) const
{
  patterns.clear();

  for (int s=0; s<getSubjectCount(); ++s) {		    // s = subject
    for (int f=0; f<getSubject(s)->getImageCount(); f++) {  // f = face
      Vector<double> facePoint = image2vector(getSubject(s)->getImage(f).get());
      Vector<double> eigenspacePoint = eigenfaces->projectInEigenspace(facePoint);
      Pattern<double> pattern = makePattern(eigenspacePoint, s);

      patterns.push_back(pattern);
    }
  }
}

/// Calcula los límites mínimos y máximos de los patrones especificados
/// para luego normalizar las entradas a dichos límites.
/// 
/// Debería llamar esta función una única vez y con los patrones de
/// entrenamiento. Luego debería normalizar todos los patrones
/// (entrenamiento y prueba) utilizando #normalizePatterns.
/// 
void FrEigenfacesMlp::calcBoundsToNormalize(PatternSet& patterns)
{
  normalize = false;

  // crear los vectores normalMin y normalMax para normalizar la
  // entrada de los patrones

  for (PatternSet::iterator it=patterns.begin(); it!=patterns.end(); ++it) {
    if (it == patterns.begin()) {
      normalMin = normalMax = it->input;
      normalize = true;
    }
    else {
      for (size_t i=0; i<it->input.size(); ++i) {
	if (it->input(i) < normalMin(i)) normalMin(i) = it->input(i);
	if (it->input(i) > normalMax(i)) normalMax(i) = it->input(i);
      }
    }
  }
}

void FrEigenfacesMlp::normalizePatterns(PatternSet& patterns) const
{
  for (PatternSet::iterator it=patterns.begin(); it!=patterns.end(); ++it)
    normalizePoint(it->input, it->input);
}

void FrEigenfacesMlp::savePatterns(const char* filename, PatternSet& patterns) const
{
  std::ofstream f(filename);

  f.precision(16);

  for (PatternSet::const_iterator
	 it=patterns.begin(); it!=patterns.end(); ++it) {
    const Pattern<double>& pat(*it);

    for (size_t i=0; i<pat.input.size(); ++i)
      f << '\t' << pat.input(i);

    f << '\t' << ((int)pat.output.getMaxPos()+1) << std::endl;
  }
}

void FrEigenfacesMlp::loadPatterns(const char* filename, PatternSet& patterns) const
{
  std::ifstream f(filename);
  char buf[4096*8];

  while (f.getline(buf, sizeof(buf)).good()) {
    std::istringstream str(buf);

    Pattern<double> pat;
    pat.input = net.createInput();
    pat.output = net.createOutput();

    for (size_t c=0; c<pat.input.size(); ++c)
      str >> pat.input(c);

    int subj = 0;
    str >> subj;

    for (size_t c=0; c<pat.output.size(); ++c)
      pat.output(c) = (c == subj-1) ? 1.0: 0.0;

    patterns.push_back(pat);
  }
}

void FrEigenfacesMlp::setLearningRate(double eta)
{
  bp.setLearningRate(eta);
}

void FrEigenfacesMlp::setMomentum(double momentum)
{
  bp.setMomentum(momentum);
}

void FrEigenfacesMlp::trainEpochs(int epochs, const PatternSet& trainingSet)
{
  while (epochs-- > 0)
    bp.train(trainingSet);
}

void FrEigenfacesMlp::normalizePoint(const Vector<double>& eigenspacePoint,
				     Vector<double>& x) const
{
  if (!normalize)
    x = eigenspacePoint;
  else {
    x.resize(eigenspacePoint.size());
    for (size_t i=0; i<eigenspacePoint.size(); ++i) {
      // normalizar cada componente del punto en el eigenspace
#if 0
      x(i) =
	(eigenspacePoint(i) - normalMin(i)) /
	(normalMax(i) - normalMin(i));
#else
      x(i) =
	2.0 * (eigenspacePoint(i) - normalMin(i)) /
	(normalMax(i) - normalMin(i)) - 1.0;
#endif
    }
  }
}

int FrEigenfacesMlp::calcMinDistance(const Vector<double>& eigenspacePoint) const
{
  Vector<double> x, y, z;
  normalizePoint(eigenspacePoint, x);

  net.recall(x, y, z);
  return interpretOutput(z);
}

/// Interpreta la salida de la red neuronal.
/// 
/// @param z
///   Resultado que debió disparar la llamada a @ref Net#recall.
/// 
int FrEigenfacesMlp::interpretOutput(const Vector<double>& z) const
{
#if 0				// using threshold
  int counter = 0;
  int good_k = -1;

  for (int k=0; k<z.size(); k++)
    if (z(k) >= threshold) {
      counter++;
      good_k = k;
    }

  // std::cout << "z = " << z << std::endl;
  // std::cout << "counter=" << counter << ", good_k=" << good_k << std::endl;

  if (counter == 1)
    return good_k;
  else
    return -1;
#else				// get the best
  return z.getMaxPos();
#endif
}

/// Crea una patrón de entrenamiento para el sujeto especificado.
/// 
/// @param eigenspacePoint
///   Representa una foto del sujeto proyectada en el eigenspace.
/// @param s
///   Índice del sujeto al cual pertenece la foto/punto @a eigenspacePoint,
///   y para el cual queremos crear un patrón de entrenamiento.
/// 
Pattern<double> FrEigenfacesMlp::makePattern(const Vector<double>& eigenspacePoint, int s) const
{
  Pattern<double> p;

  p.input = net.createInput();
  normalizePoint(eigenspacePoint, p.input);

  p.output = net.createOutput();
  for (int k=0; k<getSubjectCount(); ++k) // k = output variable
    p.output(k) = (s == k) ? 1.0: 0.0;

  return p;
}
