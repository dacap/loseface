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

#ifndef NEWFUNCS_H
#define NEWFUNCS_H

// These functions and classes are candidates to be in Vaca

#include <Vaca/String.h>
#include <Vaca/Exception.h>
#include <Vaca/FindFiles.h>

using namespace Vaca;

class FileSystemError : public Exception
{
public:
  FileSystemError() : Exception() { }
  FileSystemError(const String& message) : Exception(message) { }
  virtual ~FileSystemError() throw() { }
};

/**
   Functions to consult information to the file-system. 
 */
namespace FileSystem
{

  static bool isFile(const String& fileName)
  {
    DWORD ret = GetFileAttributes(fileName.c_str());

    if (ret == INVALID_FILE_ATTRIBUTES)
      return false;

    return (ret & FILE_ATTRIBUTE_DIRECTORY) == 0;
  }

  static bool isDirectory(const String& fileName)
  {
    DWORD ret = GetFileAttributes(fileName.c_str());

    if (ret == INVALID_FILE_ATTRIBUTES)
      return false;

    return (ret & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
  }

  static void makeDirectory(const String& pathName)
  {
    if (!CreateDirectory(pathName.c_str(), NULL))
      throw FileSystemError(L"Error creating directory: " + pathName);
  }

};

#endif

