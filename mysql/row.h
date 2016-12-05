#ifndef _CPPDB_MYSQL_ROW_H
#define _CPPDB_MYSQL_ROW_H

#include <my_global.h>
#include <mysql.h>
#include <string>
#include <map>
#include "../row.h"
//#include "../support/mysql/query.h"
//#include "../support/mysql/assignment_info.h"

namespace cppdb {
	
	class MySQLRow: public Row {
		typedef std::map<std::string, size_t> MapType;

		MYSQL* conn;
		MYSQL_RES* res;
		size_t rownum;
		mutable MapType* ktrn; // var name short for "key to row number", used in 
			       	       // operator[](const std::string&)

		void fill_map() const;
		void range_check(size_t idx) const;


	public:
		MySQLRow(MYSQL* c, MYSQL_RES* r, MYSQL_ROW rw, size_t rn): Row(mysql_num_fields(r)), 
			conn(c), res(r), rownum(rn), ktrn(nullptr) {
			
			for(size_t i = 0; i < Row::size(); ++i) {
				Row::assign_to_array(i, rw[i]);
			}
		}

		MySQLRow(const MySQLRow& rhs): Row(rhs.size()), conn(rhs.conn), res(rhs.res),
			rownum(rhs.rownum), ktrn(nullptr) {
			
			for(size_t i = 0; i < rhs.size(); ++i) {
				Row::assign_to_array(i, rhs[i]);
			}
		}

		MySQLRow(const Row& rhs, MYSQL* c, MYSQL_RES* r, size_t rn): Row(rhs), conn(c),
			rownum(rn), ktrn(nullptr) {} 

		virtual MySQLRow& operator=(const Row& rhs) {
			if(this == &rhs) return *this;

			Row::operator=(rhs);
		}

		virtual ~MySQLRow() { delete ktrn; }

		virtual size_t row_number() const { return rownum; }
		virtual size_t size() const { return Row::size(); }

		virtual std::string operator[](size_t idx) const {
			return Row::fetch_from_array(idx);
		}

		virtual std::string operator[](const std::string& key) const {
			fill_map();
			return Row::fetch_from_array((*ktrn)[key]);
		}

		virtual std::string at(size_t idx) const {
			range_check(idx);
			return Row::fetch_from_array(idx);
		}

		virtual std::string at(const std::string& key) const {
			fill_map();
			size_t index = (*ktrn)[key];
			range_check(index);
			return Row::fetch_from_array(index);
		}

		virtual Row* clone() const {
			return new MySQLRow(*this);
		}
	};
}

#endif // _CPPDB_MYSQL_ROW_H
