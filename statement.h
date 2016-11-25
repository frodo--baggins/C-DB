#ifndef _CPPDB_STATEMENT_H
#define _CPPDB_STATEMENT_H

#include <vector>
#include <memory>
#include <string>
#include <utility>
#include "resultset.h"

namespace cppdb {
	class ResultSet;

	class Statement {
	public:
		virtual ~Statement() {}
		virtual ResultSet* query(const std::string&) = 0;
		//virtual std::pair<ResultSet*, size_t> multi_query(const std::string&) = 0;
	};
}

#endif // _CPPDB_STATEMENT_H
