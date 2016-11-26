#include "mysql.h"
#include <iostream>

int main() {
	cppdb::DBConnection* dbconn = cppdb::connect<cppdb::mysql>("127.0.0.1", "evan", "WyllaisWeird24");
	dbconn->select_database("Dblib");

	std::cout << dbconn->database_name() << '\n';

	cppdb::Statement* stmt = dbconn->make_statement();
	cppdb::ResultSet* res = stmt->query("SELECT * FROM Users");
	
	delete res;
	delete stmt;
	delete dbconn;
	return 0;
}
