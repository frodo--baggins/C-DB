#ifndef CPPDB_MYSQL_STATEMENT_H
#define CPPDB_MYSQL_STATEMENT_H

#include <my_global.h>
#include <mysql.h>
#include <string>
#include "../statement.h"
#include "../exception.h"
#include "resultset.h"

namespace cppdb {
	
	class MySQLStatement: public Statement {
		MYSQL* conn;
	public:
		MySQLStatement(MYSQL* c): conn(c) {}
		virtual ~MySQLStatement() {}

		ResultSet* query(const std::string& qry) {
			// Send a query to the MySQL server
			// If successful, return a result_set pointer if the query yields a result set
			// If the query does not yield a result set, return nullptr
			// If error, throw QueryFailureException() using the error() function (see
			// exception.h)
			
			using Support::error; // For convenience
			
			if(mysql_query(conn, qry.c_str()) != 0) { // Error in performing query
				error<QueryFailureException>("Could not perform query: ",
					mysql_error(conn));
			} else {
				MYSQL_RES* result = mysql_store_result(conn);
				if(result != nullptr) {
					return new MySQLResultSet(conn, result);
				} else {
					if(mysql_errno(conn)) { // Error in retrieving the result set
						error<QueryFailureException>("Could not perform query: ",
							mysql_error(conn));
					} else {
						// Query was a SELECT statement or something similar
						return nullptr;
					}
				}
			}
		}
	};
}

#endif // CPPDB_MYSQL_STATEMENT_H
