#ifndef _CPPDB_EXCEPTION_H
#define _CPPDB_EXCEPTION_H

#include <cassert>
#include <stdexcept>
#include <exception>
#include <string>

#if __cplusplus > 199711L
#define CPPDB_NOEXCEPT noexcept
#else
#define CPPDB_NOEXCEPT throw()
#endif

namespace cppdb {
	namespace Support {
		template<class CPPDBException>
		void error(const std::string& message1, const std::string& message2) {
			std::string err = message1;
			err += message2;
			throw CPPDBException(err);	
		}
	}


	class DBException: public std::exception {
		std::string x;
	public:
		DBException(const std::string& str = "") : std::exception(), x(str) {}
		virtual ~DBException() CPPDB_NOEXCEPT {}
		virtual const char* what() const CPPDB_NOEXCEPT { return x.c_str(); }
	};
	
	class ConnectException: public DBException {
	public:
		ConnectException(const std::string& str = "") : DBException(str) {}
		virtual ~ConnectException() CPPDB_NOEXCEPT {}
		virtual const char* what() const CPPDB_NOEXCEPT { return DBException::what(); }
	};
	
	class QueryFailureException: public DBException {
	public:
		QueryFailureException(const std::string& str = "") : DBException(str) {}
		virtual ~QueryFailureException() CPPDB_NOEXCEPT {}
		virtual const char* what() const CPPDB_NOEXCEPT { return DBException::what(); }
 	};
 	
 	class BadColumnException: public DBException {
 	public:
 		BadColumnException(const std::string& str = "") : DBException(str) {}
 		virtual ~BadColumnException() CPPDB_NOEXCEPT {}
 		virtual const char* what() const CPPDB_NOEXCEPT { return DBException::what(); }
 	};
 	
 	class BadRowException: public DBException {
	public:
		BadRowException(const std::string& str = ""): DBException(str) {}
		virtual ~BadRowException() CPPDB_NOEXCEPT {}
		virtual const char* what() const CPPDB_NOEXCEPT { return DBException::what(); } 	
 	};


	class AssignmentException: public QueryFailureException {
	// This exception's base class is open for discussion
	// Assignments are performed by queries, but these queries are handled automatically:
	// there is no need for the programmer to explicitly write the query
	// So the question is this: Would it be useful or confusing to derive assignment_failure
	// from query_failure?
	public:
		AssignmentException(const std::string& str = "") : QueryFailureException(str) {}
		virtual ~AssignmentException() CPPDB_NOEXCEPT {}
		virtual const char* what() const CPPDB_NOEXCEPT { return QueryFailureException::what(); }
	};
}

#endif // _CPPDB_EXCEPTION_H
