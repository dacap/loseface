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

#ifndef LOSEFACE_RECOG_SUBJECTSFILTER_H
#define LOSEFACE_RECOG_SUBJECTSFILTER_H

#include <cstdio>
#include <cstdlib>
#include <vector>
#include "recog/SubjectsList.h"

namespace recog {

  /// Class to filter images from a list of subjects.
  ///
  /// @see SubjectsList
  /// 
  class SubjectsFilter
  {
    struct SubjectMask
    {
      int id;			// 0 is all subjects
      std::vector<bool> mask;	// included images
    };
    std::vector<SubjectMask> m_masks;

  public:

    SubjectsFilter()
    {
      SubjectMask mask = { -1, std::vector<bool>() };
      m_masks.push_back(mask);
    }

    void includeRange(unsigned subjectId, unsigned firstIncluded, unsigned lastIncluded)
    {
      //std::cout << "includeRange("<<subjectId<<", "<<firstIncluded<<", "<<lastIncluded<<")\n";

      if (firstIncluded > lastIncluded)
	throw std::invalid_argument("firstIncluded cannot be greater than lastIncluded");

      if (subjectId >= m_masks.size())
	m_masks.resize(subjectId+1);

      std::vector<bool>& mask(m_masks[subjectId].mask);

      if (lastIncluded >= mask.size())
	mask.resize(lastIncluded+1, false);

      for (unsigned i=firstIncluded; i<=lastIncluded; ++i)
	mask[i] = true;
    }

    /// Crea un nuevo filtro por medio de una cadena con un formato
    /// especial.
    /// 
    /// @param filterString
    ///   Imágenes a incluir. El formato es:
    ///   0-2       Incluir las fotos 0, 1 y 2 de cada sujeto.
    ///   1,4       Incluir las fotos 1 y 4 de cada sujeto.
    ///   3,5-8     Incluir las fotos 3, 5, 6, 7 y 8 de cada sujeto.
    ///   0-2,[1]4  Incluir las fotos 0, 1, 2 de todos los sujetos más la 4 del
    ///             primer sujeto.
    /// 
    void parse(const char* filterString)
    {
      unsigned subjectId = 0;

      for (char* p = const_cast<char*>(filterString); p && *p != '\0'; ) {
	if (*p == ',') {
	  ++p;
	  continue;
	}
	// change the subject
	else if (*p == '[') {
	  subjectId = std::strtol(p+1, &p, 10);
	  if (*p != ']')
	    throw std::invalid_argument("Syntax error in SubjectsFilter::parseString()");
	  ++p;
	  continue;
	}

	unsigned n = std::strtol(p, &p, 10);
	if (!p) break;

	switch (*p) {
	  case '\0':
	  case ',':
	  case '[':
	    includeRange(subjectId, n, n);
	    break;
	  case '-': {
	    unsigned m = std::strtol(p+1, &p, 10);
	    includeRange(subjectId, n, m);
	    break;
	  }
	}
      }
    }

    void apply(SubjectsListPtr subjects)
    {
      for (int j=0; j<subjects->size(); ++j) {
	for (int i=(*subjects)[j]->getImageCount()-1; i>=0; --i) {
	  //std::cout << "isIncluded("<<(j+1)<<", "<<i<<") = " << isIncluded(j+1, i) << "\n";

	  ImagePtr image = (*subjects)[j]->getImage(i);
	  if (!isIncluded(j+1, i))
	    (*subjects)[j]->removeImage(image);
	}
      }
    }

  private:

    bool isIncluded(unsigned subjectId, unsigned imageIndex) const
    {
      return
	(imageIndex < m_masks[0].mask.size() && m_masks[0].mask[imageIndex]) ||
	(subjectId < m_masks.size() &&
	 imageIndex < m_masks[subjectId].mask.size() &&
	 m_masks[subjectId].mask[imageIndex]);
    }

  };

} // namespace recog

#endif // LOSEFACE_RECOG_SUBJECTSFILTER_H
