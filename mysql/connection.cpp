#include "connection.h"
#include "statement.h"
#include "../lock.h"
#include <my_global.h>
#include <mysql.h>

template<> cppdb::DBConnection* cppdb::connect<cppdb::mysql>(const std::string& url, const std::string& user, const std::string& pass) {
	return new MySQLConnection(url, user, pass);	
}

cppdb::MySQLConnection::MySQLConnection(const std::string& url, const std::string& u,
	const std::string& p): conn(mysql_init(NULL)), dbname("") {
	
	if(mysql_real_connect(conn, url.c_str(), u.c_str(), p.c_str(), NULL, 0, NULL, 0) == NULL) {
		std::string err = "Could not connect to database: ";
		err += mysql_error(conn);
		mysql_close(conn);
		throw ConnectException(err);
	}
}

void cppdb::MySQLConnection::select_database(const char* dbnme) {
	if(mysql_select_db(conn, dbnme) != 0) {
		std::string err = "Could not select database: ";
		err += mysql_error(conn);
		throw ConnectException(err);
	}
	dbname = dbnme;
}

cppdb::Statement* cppdb::MySQLConnection::make_statement() {
	return new MySQLStatement(conn);
}

/*cppdb::DBLock cppdb::MySQLConnection::lock_tables(const std::string... tables, LockType ltype) {
	
}*/
