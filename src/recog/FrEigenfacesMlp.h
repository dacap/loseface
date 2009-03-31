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
    StdBp<Net> bp;
    double threshold;
    bool normalize;
    Vector<double> normalMin;
    Vector<double> normalMax;

  public:
    FrEigenfacesMlp(SubjectsListPtr subjects,
		    EigenfacesPtr eigenfaces);
    virtual ~FrEigenfacesMlp();

    Net& getNet() { return net; }
    StdBp<Net>& getBp() { return bp; }
    
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
