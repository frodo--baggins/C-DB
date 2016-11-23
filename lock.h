#ifndef CPPDB_LOCK_H
#define CPPDB_LOCK_H

#include <my_global.h>
#include <mysql.h>
#include <string>
#include <vector>
#include "connection.h"

namespace cppdb {
	enum class LockType { read, write };

	class DBLock {
		DBConnection* dbconn;
		std::vector<std::string>* tables; // This variable is a pointer to avoid the cost of
						  // copy construction (see DBConnection::lock_tables())
	public:
		DBLock(DBConnection* dbc, std::vector<std::string>* t): dbconn(dbc), tables(t) {}
		~DBLock() {
			dbconn->unlock_tables(*tables);
			delete tables;
		}
		
	};
}


#endif // CPPDB_LOCK_H
