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

#ifndef LOSEFACE_RECOG_FRSYSTEM_H
#define LOSEFACE_RECOG_FRSYSTEM_H

#include "recog/SubjectsList.h"
#include "Vector.h"

namespace recog {

  /// Facial recognition system.
  ///
  class FrSystem
  {
    SubjectsListPtr m_subjects;

  public:

    FrSystem(SubjectsListPtr subjects)
      : m_subjects(subjects) {
    }

    virtual ~FrSystem() {
    }

    inline SubjectsListPtr getSubjects() const {
      return m_subjects;
    }

    inline const SubjectPtr getSubject(int i) const {
      return (*m_subjects)[i];
    }

    inline const ImagePtr getImage(int i) const {
      return m_subjects->getImage(i);
    }

    inline int getSubjectCount() const {
      return m_subjects->size();
    }

    inline int getImageCount() const {
      return m_subjects->getImageCount();
    }

    inline int getPixelsPerImage() const {
      return (m_subjects->getImageWidth() *
	      m_subjects->getImageHeight());
    }

    virtual int recognize(const Image* faceImage) const = 0;

  };

} // namespace recog

#endif // LOSEFACE_RECOG_FRSYSTEM_H
