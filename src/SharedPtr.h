// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
template<class T>
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
