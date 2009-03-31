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

#ifndef LOSEFACE_RECOG_SUBJECTSLIST_H
#define LOSEFACE_RECOG_SUBJECTSLIST_H

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include "recog/Subject.h"
#include "recog/SubjectsReader.h"
#include "recog/recog_util.h"

namespace recog {

  class SubjectsList
  {
    std::vector<SubjectPtr> m_subjects;
    int m_imageWidth, m_imageHeight;

  public:

    /// Construye una nueva lista de sujetos a reconocer.
    /// 
    /// @param reader
    ///   Lector de imágenes a utilizar para obtener la lista de sujetos
    ///   y rostros conocidos.
    /// 
    SubjectsList(SubjectsReader& reader)
      : m_imageWidth(reader.getImageWidth())
      , m_imageHeight(reader.getImageHeight())
    {
      int c, N = reader.getSubjectCount();
      for (c=0; c<N; ++c) {
	SubjectPtr subject = reader.readSubject(c);
	assert(subject != NULL);
	add(subject);
      }
    }

    void add(SubjectPtr subject) {
      m_subjects.push_back(subject);
    }

    void remove(int index) {
      assert(index >= 0 && index < m_subjects.size());
      m_subjects.erase(m_subjects.begin()+index);
    }

    void remove(SubjectPtr s) {
      std::vector<SubjectPtr>::iterator
	it = std::find(m_subjects.begin(),
		       m_subjects.end(), s);

      if (it == m_subjects.end())
	throw std::invalid_argument("Specified subject doesn't exist in this list");

      remove((int)(it - m_subjects.begin()));
    }

    inline SubjectPtr operator[](int i) {
      assert(i >= 0 && i < m_subjects.size());
      return m_subjects[i];
    }

    inline const SubjectPtr operator[](int i) const {
      assert(i >= 0 && i < m_subjects.size());
      return m_subjects[i];
    }

    inline int size() const { return m_subjects.size(); }
    inline int getImageWidth() const { return m_imageWidth; }
    inline int getImageHeight() const { return m_imageHeight; }

    int getImageCount() const {
      int count = 0;
      for (int i=0; i<size(); ++i)
      	count += m_subjects[i]->getImageCount();
      return count;
    }

    ImagePtr getImage(int index) const {
      SubjectPtr subject;

      for (int i=0; i<size(); ++i) {
	subject = m_subjects[i];

	if (index < subject->getImageCount())
	  return subject->getImage(index);
	else
	  index -= subject->getImageCount();
      }
      return ImagePtr();
    }

  }; // class SubjectsList

  typedef SharedPtr<SubjectsList> SubjectsListPtr;

} // namespace recog

#endif // LOSEFACE_RECOG_SUBJECTSLIST_H
