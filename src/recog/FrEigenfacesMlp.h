// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_RECOG_FREIGENFACESMLP_H
#define LOSEFACE_RECOG_FREIGENFACESMLP_H

#include "recog/Eigenfaces.h"
#include "recog/FrSystem.h"
#include "Ann.h"

namespace recog {

  class FrEigenfacesMlp : public FrSystem
  {
  public:
    typedef Mlp<double, Logsig<double>, Logsig<double> > Net;
    typedef Pattern<double>::Set PatternSet;

  private:
    EigenfacesPtr eigenfaces;
    Net net;
    Backpropagation<Net> bp;
    double threshold;
    bool normalize;
    Vector<double> normalMin;
    Vector<double> normalMax;

  public:
    FrEigenfacesMlp(SubjectsListPtr subjects,
		    EigenfacesPtr eigenfaces);
    virtual ~FrEigenfacesMlp();

    Net& getNet() { return net; }
    Backpropagation<Net>& getBp() { return bp; }
    
    void configure(int hiddens, double threshold);

    void setLearningRate(double eta);
    void setMomentum(double momentum);

    void trainEpochs(int epochs, const PatternSet& trainingSet);

    virtual int recognize(const Image* faceImage) const;

    void convertToPatterns(SubjectsListPtr subjectsList, PatternSet& patterns) const;
    void calcBoundsToNormalize(PatternSet& patterns);
    void normalizePatterns(PatternSet& patterns) const;

    void savePatterns(const char* filename, PatternSet& patterns) const;
    void loadPatterns(const char* filename, PatternSet& patterns) const;

  protected:

    void normalizePoint(const Vector<double>& eigenspacePoint, Vector<double>& x) const;
    int calcMinDistance(const Vector<double>& eigenspacePoint) const;
    int interpretOutput(const Vector<double>& z) const;
    Pattern<double> makePattern(const Vector<double>& omega, int s) const;
  };

} // namespace recog

#endif // LOSEFACE_RECOG_FREIGENFACESMLP_H
