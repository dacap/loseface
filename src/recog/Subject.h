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

#ifndef LOSEFACE_RECOG_SUBJECT_H
#define LOSEFACE_RECOG_SUBJECT_H

#include <cstdio>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>
#include "recog/Image.h"
#include "util.h"

namespace recog {

  class Subject
  {
    std::vector<ImagePtr> m_images;
    int m_subjectId;

  public:

    /// Creates a new subject with the specified ID.
    ///
    /// @param id Identifier for this subject.
    ///
    Subject(int id)
      : m_subjectId(id) {
    }

    /// Returns the subject identifier.
    ///
    inline int getId() const {
      return m_subjectId;
    }

    /// Adds a new image to the subject.
    ///
    inline void addImage(ImagePtr image) {
      m_images.push_back(image);
    }

    /// Removes a image from the subject.
    ///
    void removeImage(ImagePtr image) {
      std::vector<ImagePtr>::iterator it =
	std::find(m_images.begin(), m_images.end(), image);

      if (it != m_images.end())
	m_images.erase(it);
    }

    /// Returns a image from the subject.
    ///
    inline ImagePtr getImage(int i) const
    {
      assert(i >= 0 && i < m_images.size());
      return m_images[i];
    }

    /// Returns the entire collection of images from the subject.
    ///
    inline const std::vector<ImagePtr>& getImages() const
    {
      return m_images;
    }

    /// Returns how many images has this subject.
    ///
    inline int getImageCount() const
    {
      return m_images.size();
    }

    inline bool operator==(const Subject& s) const
    {
      return m_subjectId == s.m_subjectId;
    }

    inline bool operator!=(const Subject& s) const
    {
      return m_subjectId != s.m_subjectId;
    }
  
  }; // class Subject

  typedef SharedPtr<Subject> SubjectPtr;

} // namespace recog

#endif // LOSEFACE_RECOG_SUBJECT_H
