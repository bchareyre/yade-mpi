// 2006-2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
/*
 * This file defines various useful logging-related macros - userspace stuff is
 * - LOG_* for actual logging,
 * - DECLARE_LOGGER; that should be used in class header to create separate logger for that class,
 * - CREATE_LOGGER(className); that must be used in class implementation file to create the static variable.
 *
 * Note that the latter 2 may change their name to something like LOG_DECLARE and LOG_CREATE, to be consistent.
 * Some other macros will be very likely added, to allow for easy variable tracing etc. Suggestions welcome.
 *
 *
 * Yade has the logging config file by default in ~/.yade-$VERSION/logging.conf.
 *
 */

#include <iostream>

#	define _POOR_MANS_LOG(level,msg) {std::cerr<<level " "<<_LOG_HEAD<<msg<<std::endl;}
#	define _LOG_HEAD __FILE__ ":"<<__LINE__<<" "<<__FUNCTION__<<": "

#ifdef YADE_DEBUG
	# define LOG_TRACE(msg) _POOR_MANS_LOG("TRACE",msg)
	# define LOG_INFO(msg)  _POOR_MANS_LOG("INFO ",msg)
	# define LOG_DEBUG(msg) _POOR_MANS_LOG("DEBUG",msg)
#else
	# define LOG_TRACE(msg)
	# define LOG_INFO(msg)
	# define LOG_DEBUG(msg)
#endif


#define LOG_WARN(msg)  _POOR_MANS_LOG("WARN ",msg)
#define LOG_ERROR(msg) _POOR_MANS_LOG("ERROR",msg)
#define LOG_FATAL(msg) _POOR_MANS_LOG("FATAL",msg)

#define DECLARE_LOGGER
#define CREATE_LOGGER(classname)


// these macros are temporary
#define TRACE LOG_TRACE("Been here")
#define _TRVHEAD cerr<<__FILE__<<":"<<__LINE__<<":"<<__FUNCTION__<<": "
#define _TRV(x) #x"="<<x<<"; "
#define TRVAR1(a) LOG_TRACE( _TRV(a) );
#define TRVAR2(a,b) LOG_TRACE( _TRV(a) << _TRV(b) );
#define TRVAR3(a,b,c) LOG_TRACE( _TRV(a) << _TRV(b) << _TRV(c) );
#define TRVAR4(a,b,c,d) LOG_TRACE( _TRV(a) << _TRV(b) << _TRV(c) << _TRV(d) );
