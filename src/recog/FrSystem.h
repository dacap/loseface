// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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
