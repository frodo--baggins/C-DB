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
	
	class db_exception: public std::exception {
		std::string x;
	public:
		db_exception(const std::string& str = "") : std::exception(), x(str) {}
		virtual ~db_exception() CPPDB_NOEXCEPT {}
		virtual const char* what() const CPPDB_NOEXCEPT { return x.c_str(); }
	};
	
	class connect_error: public db_exception {
	public:
		connect_error(const std::string& str = "") : db_exception(str) {}
		virtual ~connect_error() CPPDB_NOEXCEPT {}
		virtual const char* what() const CPPDB_NOEXCEPT { return db_exception::what(); }
	};
	
	class query_failure: public db_exception {
	public:
		query_failure(const std::string& str = "") : db_exception(str) {}
		virtual ~query_failure() CPPDB_NOEXCEPT {}
		virtual const char* what() const CPPDB_NOEXCEPT { return db_exception::what(); }
 	};
 	
 	class bad_column: public db_exception {
 	public:
 		bad_column(const std::string& str = "") : db_exception(str) {}
 		virtual ~bad_column() CPPDB_NOEXCEPT {}
 		virtual const char* what() const CPPDB_NOEXCEPT { return db_exception::what(); }
 	};
 	
 	class bad_row: public db_exception {
	public:
		bad_row(const std::string& str = ""): db_exception(str) {}
		virtual ~bad_row() CPPDB_NOEXCEPT {}
		virtual const char* what() const CPPDB_NOEXCEPT { return db_exception::what(); } 	
 	};
}

#endif // _CPPDB_EXCEPTION_H