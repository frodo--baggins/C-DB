#ifndef _CPPDB_SUPPORT_MYSQL_QUERY_H
#define _CPPDB_SUPPORT_MYSQL_QUERY_H

/*
 * query.h
 * 
 * Provides the function query_mysql_update, a variant on mysql_query that uses an initializer list 
 * (rather than a string), and takes care of exceptions, etc.
 *
 * THIS IS A SUPPORT FILE. This file, along with all other files in the support directory, should NOT be
 * included directly by library clients! All support files are used to provide necessary classes,
 * functions, etc. for use by the library headers and implementation files and are included automatically
 * by those aforementioned headers and files. If a client directly includes a support file, the behavior
 * is undefined.
 *
 */

#include <my_global.h>
#include <mysql.h>
#include <initializer_list>

namespace cppdb {
	namespace Support {
		void query_mysql_update(MYSQL* conn, std::initializer_list<std::string> il);
	}
}

#endif // _CPPDB_SUPPORT_MYSQL_QUERY_H
