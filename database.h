/*#ifndef _CPPDB_DATABASE_H
#define _CPPDB_DATABASE_H

#include "connection.h"
#include "db_type.h"

namespace cppdb {
	class result_set;
	class statement;
	
	class database {
	public:
		virtual const char* database_name() = 0;
		virtual statement* make_statement(const char* qry) = 0;
		virtual statement* make_prepared_statement(const char* qry) = 0;
	};
}

#endif // _CPPDB_DATABASE_H*/
