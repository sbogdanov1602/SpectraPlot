#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(SMPLOADER_LIB)
#  define SMPLOADER_EXPORT Q_DECL_EXPORT
# else
#  define SMPLOADER_EXPORT Q_DECL_IMPORT
# endif
#else
# define SMPLOADER_EXPORT
#endif
