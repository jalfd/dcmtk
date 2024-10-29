/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: OFGlobal is a template class intended for the declaration
 *           of global objects, access to which is protected by a Mutex
 *           for multi-thread applications.
 *           class T must have copy constructor and assignment operator.
 *
 *
 */


#ifndef OFGLOBAL_H
#define OFGLOBAL_H


#if defined(HAVE_CXX11_ATOMICS) && defined(WITH_THREADS)
#include <atomic>
#endif
#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofthread.h"  /* for class OFBool */

/** Template class which allows to declare global objects that are
 *  protected by a Mutex if used in multi-thread applications.
 *  Must be compiled with -DWITH_THREADS for multi-thread-operation.
 *  Template class T must have copy constructor and assignment operator.
 */
template <class T> class OFGlobal
{
public:

  /** constructor.
   *  @param arg value to which this object is initialised
   */
  OFGlobal(const T &arg)
  : val(arg)
#ifdef WITH_THREADS
  , theMutex()
#endif
  {
  }

  /** destructor.
   */
  virtual ~OFGlobal() { }

  /** assigns new value to this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Mutex.
   *  @param arg new value
   */
  void set(const T& arg)
  {
#ifdef WITH_THREADS
    theMutex.wrlock();
#endif
    val = arg;
#ifdef WITH_THREADS
    theMutex.wrunlock();
#endif
  }

  /** returns the value of this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Mutex. The result
   *  is returned by value, not by reference.
   *  @return value of this object.
   */
  T get()
  {
#ifdef WITH_THREADS
    theMutex.rdlock();
#endif
    T result(val);
#ifdef WITH_THREADS
    theMutex.rdunlock();
#endif
    return result;
  }

private:

  /** value of this object
   */
  T val;

#ifdef WITH_THREADS
  /** if compiled for multi-thread operation, the Mutex protecting
   *  access to the value of this object.
   */
  OFReadWriteLock theMutex;
#endif

  /** unimplemented private default constructor */
  OFGlobal();

  /** unimplemented private copy constructor */
  OFGlobal(const OFGlobal<T>& arg);

  /** unimplemented private assignment operator */
  const OFGlobal<T>& operator=(const OFGlobal<T>& arg);

};

template <> class OFGlobal<OFBool>
{
public:

  /** constructor.
   *  @param arg value to which this object is initialised
   */
  OFGlobal(const OFBool &arg)
  : val(arg)
  {
  }

  /** destructor.
   */
  virtual ~OFGlobal() { }

  /** assigns new value to this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Mutex.
   *  @param arg new value
   */
  void set(const OFBool& arg)
  {
    val = arg;
  }

  /** gets the value of this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Mutex.
   *  @param arg return value is assigned to this parameter.
   */
  void xget(OFBool& arg)
  {
    arg = val;
  }

  /** returns the value of this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Mutex. The result
   *  is returned by value, not by reference.
   *  @return value of this object.
   */
  OFBool get()
  {
    OFBool result(val);
    return result;
  }

private:

  /** value of this object
   */
  std::atomic<OFBool> val;


  /** unimplemented private default constructor */
  OFGlobal();

  /** unimplemented private copy constructor */
  OFGlobal(const OFGlobal<OFBool>& arg);

  /** unimplemented private assignment operator */
  const OFGlobal<OFBool>& operator=(const OFGlobal<OFBool>& arg);

};

#endif
