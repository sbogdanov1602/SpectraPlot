#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CSVLOADER_LIB)
#  define CSVLOADER_EXPORT Q_DECL_EXPORT
# else
#  define CSVLOADER_EXPORT Q_DECL_IMPORT
# endif
#else
# define CSVLOADER_EXPORT
#endif
