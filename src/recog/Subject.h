// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
