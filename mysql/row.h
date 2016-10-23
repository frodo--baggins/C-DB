#ifndef _CPPDB_MYSQL_ROW_H
#define _CPPDB_MYSQL_ROW_H

#include <my_global.h>
#include <mysql.h>
#include <string>
#include <map>
#include "../row.h"
#include "../support/mysql/query.h"
#include "../support/mysql/assignment_info.h"

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
	protected:
		class MySQLProxyImpl: public Row::ProxyImpl {
			typedef Row::ProxyImpl ProxyImpl;
			MYSQL* conn;
			MYSQL_RES* res;
			std::string* rw; // the row
			int colnum;
		public:
			MySQLProxyImpl(const std::string& str, MYSQL* c, MYSQL_RES* rs, std::string* r,
				int cn): ProxyImpl(str), conn(c), res(rs), rw(r), colnum(cn) {}
			
			virtual ~MySQLProxyImpl() {}
			
			virtual void assign(const std::string& s) {
				using namespace Support;
				ProxyImpl::assign(s);
				AssignmentInfo ai = get_assignment_info(res, colnum);
				std::string qry = "UPDATE "; // Query will be: "UPDATE tablename SET
							     // rowname='s' WHERE primary_key=
							     // 'primary_key_value'"
				qry += ai.table_name;
				qry += "SET ";
				qry += ai.assign_col_name;
				qry += "='";
				qry += s;
				qry += "' WHERE ";
				qry += ai.key_name;
				qry += "='";
				qry += rw[ai.key_col];
				qry += '\'';

				if(mysql_query(conn, qry.c_str()) != 0) {
					throw QueryFailureException("Could not assign new value");
				}
			}

			virtual std::string to_string() {
				return ProxyImpl::to_string();
			}
		};


	public:
		MySQLRow(MYSQL* c, MYSQL_RES* r, MYSQL_ROW rw, size_t rn): Row(mysql_num_fields(res)), 
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

		virtual MySQLRow& operator=(const Row& rhs);

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

		virtual db_proxy operator[](size_t idx) {
			return db_proxy(new MySQLProxyImpl(Row::fetch_from_array(idx), conn, res, 
				Row::fetch_array(), idx));
		}

		virtual db_proxy operator[](const std::string& key) {
			fill_map();
			size_t index = (*ktrn)[key];
			return db_proxy(new MySQLProxyImpl(Row::fetch_from_array(index), conn, res, 
				Row::fetch_array(), index));
		}

		virtual db_proxy at(size_t idx) {
			range_check(idx);
			return db_proxy(new MySQLProxyImpl(Row::fetch_from_array(idx), conn, res, 
				Row::fetch_array(), idx));
		}

		virtual db_proxy at(const std::string& key) {
			fill_map();
			size_t index = (*ktrn)[key];
			range_check(index);
			return db_proxy(new MySQLProxyImpl(Row::fetch_from_array(index), conn, res, 
				Row::fetch_array(), index));
		}

		virtual Row* clone() const {
			return new MySQLRow(*this);
		}
	};
}

#endif // _CPPDB_MYSQL_ROW_H
