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

#ifndef LOSEFACE_SHAREDPTR_H
#define LOSEFACE_SHAREDPTR_H

#include <cassert>

/// Smart pointer that automatically destroys an object when no more
/// references exist to it.
///
/// This smart pointer class is not intrusive, it means that the
/// references counter is embedded in the same SharedPtr and not in
/// the pointed object.
///
template<typename T>
class SharedPtr
{
  T* m_ptr;
  unsigned* m_refCount;

public:

  /// Creates a new shared pointer pointing to NULL.
  ///
  SharedPtr()
  {
    m_ptr = NULL;
    m_refCount = NULL;
  }

  /// Creates a new shared pointer for the specified raw pointer.
  ///
  /// @warning This constructor must be explicitily specified because
  ///          the raw pointer is deleted automatically.
  ///
  explicit SharedPtr(T* p)
  {
    if (p != NULL) {
      m_ptr = p;
      m_refCount = new unsigned(0);
      ref();
    }
    else {
      m_ptr = NULL;
      m_refCount = NULL;
    }
  }

  /// Creates a copy of the specified shared pointer.
  ///
  SharedPtr(const SharedPtr& other)
  {
    m_ptr = other.m_ptr;
    m_refCount = other.m_refCount;
    ref();
  }

  /// Unreferences the packed pointer.
  ///
  /// If reference counter goes to zero the pointer is deleted automatically.
  ///
  virtual ~SharedPtr()
  {
    unref();
  }

  void reset(T* p = NULL)
  {
    if (m_ptr != p) {
      unref();
      if (p != NULL) {
	m_ptr = p;
	m_refCount = new unsigned(0);
	ref();
      }
    }
  }

  SharedPtr& operator=(const SharedPtr& other)
  {
    if (m_ptr != other.m_ptr) {
      unref();
      if (other.m_ptr != NULL) {
	m_ptr = other.m_ptr;
	m_refCount = other.m_refCount;
	ref();
      }
    }
    return *this;
  }

  inline T* get() const
  {
    return m_ptr;
  }

  inline T& operator*() const
  {
    assert(m_ptr != NULL);
    return *m_ptr;
  }

  inline T* operator->() const
  {
    assert(m_ptr != NULL);
    return m_ptr;
  }

  inline operator bool() const
  {
    return m_ptr != NULL ? true: false;
  }

private:

  void ref()
  {
    if (m_ptr) {
      ++(*m_refCount);
    }
  }

  void unref()
  {
    if (m_ptr) {
      if (--(*m_refCount) == 0) {
	delete m_ptr;
	delete m_refCount;
      }
      m_ptr = NULL;
      m_refCount = NULL;
    }
  }
};

template<class T>
bool operator==(const SharedPtr<T>& ptr1, const SharedPtr<T>& ptr2)
{
  return ptr1.get() == ptr2.get();
}

template<class T>
bool operator!=(const SharedPtr<T>& ptr1, const SharedPtr<T>& ptr2)
{
  return ptr1.get() != ptr2.get();
}

#endif // LOSEFACE_SHAREDPTR_H
