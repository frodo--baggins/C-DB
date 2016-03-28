#ifndef _CPPDB_CONNECTION_H
#define _CPPDB_CONNECTION_H

#include <string>

namespace cppdb {
	class statement;
	class prepared_statement;
	
	class connection {
	public:
		virtual ~connection() {}
		virtual void select_database(const char* nme) = 0;
		virtual void select_database(const std::string& nme) = 0;
		virtual const char* database_name() const = 0;
		virtual statement* make_statement() = 0;
		// virtual prepared_statement* make_prepared_statement() = 0;
	};
	
	enum db_token { mysql, oracle, mssql };
	
	template<db_token DB>
	connection* connect(const std::string& url, const std::string& user, const std::string& password);
}

#endif // _CPPDB_CONNECTION_H
