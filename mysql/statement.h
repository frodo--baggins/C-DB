#ifndef CPPDB_MYSQL_STATEMENT_H
#define CPPDB_MYSQL_STATEMENT_H

#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include "../statement.h"
#include "../exception.h"
#include "result_set.h"

//#ifndef CPPDB_MYSQL_RESULT_SET_H
//#error "<cppdb/mysql/result_set.h> must be included before <cppdb/mysql/statement.h>"
//#endif

namespace cppdb {
	
	class mysql_statement: public statement {
		friend class mysql_connection; // Allow make_statement to call private constructor
		sql::Connection* conn;
		sql::Statement* stmt;
		
		explicit mysql_statement(sql::Connection* c, sql::Statement* st) : conn(c), stmt(st) {}
		
	public:
		
		virtual ~mysql_statement() { delete stmt; }
		virtual result_set* query(const std::string& qry) {
			bool has_result_set = stmt->execute(qry);
			if(has_result_set) {
				sql::ResultSet* res = stmt->getResultSet();
				return new mysql_result_set(conn, res);
			} else {
				return NULL;
			}
		}
		
		virtual multi_query_t multi_query(const std::string& qrys) {
			std::vector<std::string>* query_arr = _Impl::split(qrys, ';'); // Split the query string into individual statements,
																								// separated by semicolons
			size_t sz = query_arr->size();
			multi_query_t mqt(sz);
			for(size_t i = 0; i < sz; ++i) {
				mqt[i] = query((*query_arr)[i]); // Pass each statement to query() and push back the results
			}
			delete query_arr;
			return mqt;
		}
	}; // end class mysql_statement
}

#endif // CPPDB_MYSQL_STATEMENT_H
