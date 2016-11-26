#ifndef _CPPDB_CONNECTION_H
#define _CPPDB_CONNECTION_H

#include <string>
#include <vector>

namespace cppdb {
	class Statement;
	class PreparedStatement;
	class DBLock;
	enum class LockType;
	
	class DBConnection {
		friend class DBLock;
	protected:
		// virtual void unlock_tables(const std::vector<std::string>& v) = 0;
	public:
		virtual ~DBConnection() {}
		virtual void select_database(const char* nme) = 0;
		virtual void select_database(const std::string& nme) = 0;
		virtual const char* database_name() const = 0;
		virtual Statement* make_statement() = 0;
		// virtual DBLock lock_tables(LockType ltype, const std::string& first, const std::string... s) = 0;
		// virtual prepared_statement* make_prepared_statement() = 0;
	};
	
	enum db_token { mysql, oracle, mssql };
	
	template<db_token DB>
	DBConnection* connect(const std::string& url, const std::string& user, const std::string& password);
}

#endif // _CPPDB_CONNECTION_H
