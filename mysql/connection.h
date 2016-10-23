#ifndef CPPDB_MYSQL_CONNECTION_H
#define CPPDB_MYSQL_CONNECTION_H

#include <my_global.h>
#include <mysql.h>
#include "../connection.h"
#include "../exception.h"

namespace cppdb {
	class mysql_connection: public connection {
		MYSQL* conn;
		std::string dbname;
	public:
		mysql_connection(const std::string& url, const std::string& u, const std::string& p):
		conn(mysql_init(NULL)), dbname("") {
			if(mysql_real_connect(conn, url.c_str(), u.c_str(),
			  p.c_str(), NULL, 0, NULL, 0) == NULL) {
				std::string err = "Could not connect to database: ";
				err += mysql_error(conn);
				mysql_close(conn);
				throw connect_error(err);
			}
		}

		virtual ~mysql_connection() { mysql_close(conn); }
		
		virtual void select_database(const char* dbnme) {
			if(mysql_select_db(conn, dbnme) != 0) {
				std::string err = "Could not select database: ";
				err += mysql_error(conn);
				throw connect_error(err);
			}
			dbname = dbnme;
		}

		virtual void select_database(const std::string& dbnme) {
			return select_database(dbnme.c_str());
		}
		
		virtual const char* database_name() const {
			return dbname.c_str();	
		}
		virtual statement* make_statement() {
			return nullptr;
		}
		//virtual prepared_statement* make_prepared_statement() = 0;
	};
	
	template<> connection* connect<mysql>(const std::string& url, const std::string& user, const std::string& pass) {
		return new mysql_connection(url, user, pass);	
	}
} // namespace cppdb

#endif // CPPDB_MYSQL_CONNECTION_H
