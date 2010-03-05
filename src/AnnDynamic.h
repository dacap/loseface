// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_ANNDYNAMIC_H
#define LOSEFACE_ANNDYNAMIC_H

#include "Ann.h"

template<class T>
class DynamicFunctionWrapperBase
{
public:
  virtual T  f(T x)      const = 0;
  virtual T df(T x, T s) const = 0;
  virtual DynamicFunctionWrapperBase<T>* clone() const = 0;
};

template<class T, class Function>
class DynamicFunctionWrapper : public DynamicFunctionWrapperBase<T>
{
public:
  virtual T  f(T x)      const { return typename Function::f(x); }
  virtual T df(T x, T s) const { return typename Function::df(x, s); }
  virtual DynamicFunctionWrapper<T, Function>* clone() const {
    return new DynamicFunctionWrapper<T, Function>();
  }
};

// Activation function that can be changed in runtime
template<class T>
class DynamicFunction
{
  DynamicFunctionWrapperBase<T>* func;

public:

  DynamicFunction()
    : func(new DynamicFunctionWrapper<T, Logsig<T> >())
  {
  }

  DynamicFunction(const DynamicFunction<T>& other) 
    : func(other.func->clone())
  {
  }

  virtual ~DynamicFunction() 
  {
    delete func;
  }

  DynamicFunction& operator=(const DynamicFunction<T>& other)
  {
    delete func;
    func = other.func->clone();
  }

  T f(T x)       const { return func->f(x); }
  T df(T x, T s) const { return func->df(x, s); }

  template<template<class T> class Function>
  void setFunction()
  {
    delete func;
    func = new DynamicFunctionWrapper<T, Function<T> >();
  }

};

// An MLP where you can change activation functions in runtime
template<class T>
class DynamicMlp : public Mlp<T, DynamicFunction<T>, DynamicFunction<T> >
{
public:

  DynamicMlp()
    : Mlp()
  {
  }

  DynamicMlp(size_t inputs, size_t hiddens, size_t outputs)
    : Mlp(inputs, hiddens, outputs)
  {
  }

  DynamicMlp(const DynamicMlp& mlp)
    : Mlp(mlp)
  {
  }

};

#endif // LOSEFACE_ANNDYNAMIC_H
