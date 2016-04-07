#pragma once

#ifndef _STRSTREAM_
#include <sstream>
#endif
#ifndef _IOSTREAM_
#include <iostream> 
#endif
#ifndef _IOSFWD_
#include <iosfwd>
#endif
#ifndef __wtypes_h__
#include <wtypes.h>
#endif

#define WRITE_SIMPLE_LOGGER

namespace SIMPLE_LOGGER
{

#ifdef WRITE_SIMPLE_LOGGER

  #define LOGGER_WRITE(Value) { \
	sl_ostringstream logstream; logstream << Value; sl_string logstring = logstream.str(); \
	SIMPLE_LOGGER::S_Write(logstring.c_str(), __FILE__, __LINE__); }

  #define LOGGER_WRITE2(Value) { \
	sl_ostringstream logstream; logstream << Value; sl_string logstring = logstream.str(); \
	SIMPLE_LOGGER::S_Write(logstring.c_str(), __FILE__, __LINE__, TRUE); }

  #define LOGGER_WRITE_A(Value) { \
	sl_ostringstream_a logstream; logstream << Value; sl_string_a logstring = logstream.str(); \
	SIMPLE_LOGGER::S_WriteA(logstring.c_str(), __FILE__, __LINE__); }

  #define LOGGER_WRITE2_A(Value) { \
	sl_ostringstream_a logstream; logstream << Value; sl_string_a logstring = logstream.str(); \
	SIMPLE_LOGGER::S_WriteA(logstring.c_str(), __FILE__, __LINE__, TRUE); }
#else

  #define LOGGER_WRITE(Value)    NULL;
  #define LOGGER_WRITE2(Value)   NULL;
  #define LOGGER_WRITE_A(Value)  NULL;
  #define LOGGER_WRITE2_A(Value) NULL;
#endif

#ifdef UNICODE
  #define sl_ostringstream   std::wostringstream
  #define sl_string          std::wstring
  #define sl_ostringstream_a std::ostringstream
  #define sl_string_a        std::string
#else
  #define sl_ostringstream   std::ostringstream
  #define sl_string          std::string
  #define sl_ostringstream_a std::ostringstream
  #define sl_string_a        std::string
#endif

	void S_Write(const TCHAR *Value, const CHAR* File, int Line, BOOL bFlush = FALSE);
	void S_WriteA(const CHAR *Value, const CHAR* File, int Line, BOOL bFlush = FALSE);
};
