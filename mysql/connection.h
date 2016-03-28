#ifndef CPPDB_MYSQL_CONNECTION_H
#define CPPDB_MYSQL_CONNECTION_H

#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include "../connection.h"
#include "statement.h"
#include "../exception.h"

namespace cppdb {
	class mysql_connection: public connection {
		sql::Driver* driv;
		sql::Connection* conn;
		const char* dbname;
	public:
		mysql_connection(const std::string& url, const std::string& user, const std::string& pass = "")
		: driv(get_driver_instance()), conn(driv->connect(url, user, pass)), dbname(NULL) {}
		virtual ~mysql_connection() { delete conn; }
		
		virtual void select_database(const char* dbnme) { conn->setSchema(dbnme); dbname = dbnme; }
		virtual void select_database(const std::string& dbnme) { return select_database(dbnme.c_str()); }
		
		virtual const char* database_name() const {
			return dbname;		
		}
		virtual statement* make_statement() {
			return new mysql_statement(conn, conn->createStatement());
		}
		//virtual prepared_statement* make_prepared_statement() = 0;
	};
	
	template<> connection* connect<mysql>(const std::string& url, const std::string& user, const std::string& pass) {
		return new mysql_connection(url, user, pass);	
	}
} // namespace cppdb

#endif // CPPDB_MYSQL_CONNECTION_H
