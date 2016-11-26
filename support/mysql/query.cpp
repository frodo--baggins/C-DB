#include "query.h"

void cppdb::Support::query_mysql_update(MYSQL* conn, std::initializer_list<std::string> il) {
	typedef std::initializer_list<std::string> IL;
	std::string str;
	for(IL::iterator it = il.begin(); it != il.end(); ++it) {
		str += *it;
	}

	if(mysql_query(conn, str.c_str()) != 0) {
		throw cppdb::QueryFailureException("Could not update");
	}
}
