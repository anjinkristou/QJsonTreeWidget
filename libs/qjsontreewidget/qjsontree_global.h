#ifndef QJSONTREE_GLOBAL_H
#define QJSONTREE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QJSONTREE_LIBRARY)
# define QJSONTREE_EXPORT Q_DECL_EXPORT
#else
# define QJSONTREE_EXPORT Q_DECL_IMPORT
#endif

#endif // QJSONTREE_GLOBAL_H
