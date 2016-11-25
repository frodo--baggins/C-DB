#ifndef CPPDB_MYSQL_CONNECTION_H
#define CPPDB_MYSQL_CONNECTION_H

#include <my_global.h>
#include <mysql.h>
#include "../connection.h"
#include "../exception.h"

namespace cppdb {
	class DBLock;
	enum class LockType;
	
	class MySQLConnection: public DBConnection {
		MYSQL* conn;
		std::string dbname;
		//std::string cat();
	public:
		MySQLConnection(const std::string& url, const std::string& u, const std::string& p);

		virtual ~MySQLConnection() { mysql_close(conn); }
		
		virtual void select_database(const char* dbnme);

		virtual void select_database(const std::string& dbnme) {
			return select_database(dbnme.c_str());
		}
		
		virtual const char* database_name() const {
			return dbname.c_str();	
		}
		virtual Statement* make_statement() {
			return nullptr;
		}
		
		
		//virtual DBLock lock_tables(LockType ltype, const std::string& first, const std::string... tables);
		//virtual prepared_statement* make_prepared_statement() = 0;
	};
	
	template<> Connection* connect<mysql>(const std::string& url, const std::string& user, const std::string& pass) {
		return new mysql_connection(url, user, pass);	
	}
} // namespace cppdb

#endif // CPPDB_MYSQL_CONNECTION_H
