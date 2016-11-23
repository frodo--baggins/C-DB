#ifndef CPPDB_MYSQL_STATEMENT_H
#define CPPDB_MYSQL_STATEMENT_H

#include <my_global.h>
#include <mysql.h>
#include <string>
#include "../statement.h"
#include "../exception.h"

namespace cppdb {
	class result_set;
	
	class mysql_statement: public statement {
		MYSQL* conn;
	public:
		mysql_statement(MYSQL* c): conn(c) {}
		virtual ~mysql_statement() {}

		result_set* query(const std::string& qry) {
			// Send a query to the MySQL server
			// If successful, return a result_set pointer if the query yields a result set
			// If the query does not yield a result set, return NULL
			// If error, throw query_failure() using the error() function (see exception.h)
			
			using _CPPDB_Private::error; // For convenience
			
			if(mysql_query(conn, qry.c_str()) != 0) {
				error<query_failure>("Could not perform query: ", mysql_error(conn));
			} else {
				MYSQL_RES* result = mysql_store_result(conn);
				if(result) {
					unsigned int num_fields = mysql_num_fields(result);
					return NULL; // To be changed to "return new result_set(args);"
				} else {
					if(mysql_errno(conn)) {
						error<query_failure>("Could not perform query: ",
						mysql_error(conn));
					} else {
						// Query was a SELECT statement or something similar
						return NULL;
					}
				}
			}
		}
	};
}

#endif // CPPDB_MYSQL_STATEMENT_H
