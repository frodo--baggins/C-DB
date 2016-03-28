#ifndef CPPDB_MYSQL_RESULT_SET_H
#define CPPDB_MYSQL_RESULT_SET_H

#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <utility>
#include <cstdio>
#include <memory>
#include "../exception.h"
#include "../result_set.h"

namespace cppdb {
	
	namespace _MySQLPrivateNS {
		struct primary_key_info {
			std::string table_name;
			std::string key;
			int key_col;
			primary_key_info(const std::string& tn, const std::string& k, int kc) : table_name(tn), key(k), key_col(kc) {}
		};
		
		template<class T> std::string to_string(const T& value) {
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}

		/*template<> std::string to_string(const std::string& value) {
			return value;
		}
	
		template<const char*> std::string to_string(const char* value) {
			return std::string(value);
		}*/
		
		primary_key_info get_primary_key_info(sql::Connection* conn, sql::ResultSet* res);
	}
	
	class mysql_result_set: public result_set, private result_set::iterator { // Private inheritance used to call result_set::iterator::get_raw_iter()
	public:
		typedef result_set::iterator iterator;
		typedef result_set::row::db_proxy db_proxy;
		
		class mysql_row: public result_set::row {
			friend class mysql_result_set; // To call protected member functions
			// friend class _mysql_iterator;  // So _mysql_iterator can call has_non_null_res() and set_res()
			size_t rownum;
			sql::ResultSet* res;
			sql::ResultSetMetaData* resmeta;
			sql::Connection* conn;
			
		protected:
			mysql_row(size_t rn = 0, sql::ResultSet* r = NULL, sql::Connection* c = NULL)
			: rownum(rn), res(r), resmeta(res->getMetaData()), conn(c) {}
			bool has_non_null_res() {
				return res != NULL;			
			}
			
			void set_res(sql::ResultSet* r) { res = r; }
			
			static void adjust_res(sql::ResultSet* r, int x) {
				int curr_row = r->getRow();
				// std::cout << x << ' ' << curr_row << ' ' << x - curr_row << '\n';
				// if(curr_row == x + 1) r->previous();
				// else if(curr_row == x - 1) r->next();
				if(curr_row != x) r->relative(x - curr_row);
				r->relative(1); // MySQL Connector C++ does not operate in array-index style
			}
			
			class MysqlProxyImpl: public result_set::row::ProxyImpl {
				std::string str;
				sql::Connection* conn;
				sql::ResultSet* res;
				std::string rowname;
			public:
				MysqlProxyImpl(const std::string& s, sql::Connection* c, sql::ResultSet* r, const std::string& rn) 
				: str(s), conn(c), res(r), rowname(rn) {}
				virtual ~MysqlProxyImpl() {}
				
				virtual void assign(const std::string& s) {
					_MySQLPrivateNS::primary_key_info keyinfo = _MySQLPrivateNS::get_primary_key_info(conn, res);
					std::cout << keyinfo.key << '\n';
					std::string qry = "UPDATE "; // Query will be: "UPDATE tablename SET rowname='s' WHERE primary_key='primary_key_value'"
					qry += keyinfo.table_name;
					qry += "SET ";
					qry += rowname;
					qry += "='";
					qry += s;
					qry += "' WHERE ";
					qry += keyinfo.key;
					qry += "='";
					//std::cout << keyinfo.key << std::endl;
					qry += res->getString(keyinfo.key);
					qry += '\'';
					std::cout << qry << '\n';
					std::unique_ptr<sql::Statement> stmt(conn->createStatement());
					stmt->execute(qry);
				}
				
				virtual std::string to_string() { return str; }
			};
			
		public:
			virtual size_t columns() const {
				return resmeta->getColumnCount();
			}
			mysql_row(const mysql_row&) = default; // These are safe because we don't have to worry about deleting the sql::ResultSet;
			mysql_row& operator=(const mysql_row& rhs) = default;
			virtual ~mysql_row() {}
			virtual size_t row_number() const { return rownum; }
			
			virtual std::string operator[](const std::string& key) const { adjust_res(res, rownum); return res->getString(key); }
			
			virtual std::string at(const std::string& key) const;
			
			virtual db_proxy operator[](const std::string& key) {
				adjust_res(res, rownum);
				std::string val = res->getString(key);
				return db_proxy(new MysqlProxyImpl(val, conn, res, key));
			}
			
			virtual db_proxy at(const std::string& key);
			
			virtual result_set::row* clone() const { return new mysql_row(*this); }
		}; // end class mysql_row; continue class mysql_result_set
		
		class _mysql_iterator : public result_set::polymorphic_iterator {
			friend class mysql_result_set;
			sql::ResultSet* res;
			mysql_row* rw;
			//_mysql_iterator(sql::ResultSet* r, mysql_row* a, size_t i = 0) : res(r), arr(a), idx(i) {}
			_mysql_iterator(sql::ResultSet* r, mysql_row* mrw) : res(r), rw(mrw) {}
		public:
			virtual ~_mysql_iterator() {}
			virtual row& get_reference() {
				// adjust_res(res, idx);
				return *rw;
			}
			virtual row* get_pointer() {
				// adjust_res(res, idx);
				return rw;
			}
			virtual void next() { ++rw; }
			virtual void prev() { --rw; }
			virtual bool equal_to(std::shared_ptr<result_set::polymorphic_iterator> ptr) {
				std::shared_ptr<_mysql_iterator> tmp = std::dynamic_pointer_cast<_mysql_iterator>(ptr);
				if(tmp.use_count() == 0) throw db_exception("Implementer error: Invalid equal_to() comparison");
				return rw == tmp->rw;
			}
		}; // end class mysql_iterator; continue class mysql_result_set
		
	private:
		sql::Connection* conn;
		sql::ResultSet* res;
		mysql_row* rows;
		mutable char* tblnam;
		char* primkey_name;
		size_t sz;
		
		std::shared_ptr<_mysql_iterator> get_mysql_iterator(iterator it) {
			std::shared_ptr<_mysql_iterator> ptr = std::dynamic_pointer_cast<_mysql_iterator>(it.get_raw_iter());
			if(ptr.use_count() == 0) throw db_exception("Implemeter error: Invalid iterator");
			return ptr;
		}
		
		void get_table_name() const {
			if(tblnam != NULL) return;
			try {
				sql::ResultSetMetaData* meta_data = res->getMetaData();
				std::string s = meta_data->getTableName(1);
				tblnam = new char[s.length() + 1];
				for(size_t i = 0; i < s.length(); ++i) {
					tblnam[i] = s[i];				
				}
			} catch(sql::SQLException& ex) {
				std::cout << "Exception caught (get_table_name): " << ex.what() << '\n';
			}
		}
		
		void get_primary_key_name() {
			if(primkey_name != NULL) return;
			try {
				// Build the query
				std::string qry = "SHOW KEYS FROM ";
				get_table_name();
				qry += tblnam;
				qry += " WHERE Key_name = 'PRIMARY'";
			
				std::unique_ptr<sql::ResultSet> table_res(basic_query(qry));
				table_res->next();
				std::string s = table_res->getString("Column_name");
				primkey_name = new char[s.length() + 1];
				for(size_t i = 0; i < s.length(); ++i) {
					primkey_name[i] = s[i];				
				}
			} catch(sql::SQLException& ex) {
				std::cout << "Exception caught (get_primary_key_name): " << ex.what() << '\n';
			}
		}
		
		sql::ResultSet* basic_query(const std::string& qry) {
			std::unique_ptr<sql::Statement> stmt(conn->createStatement());
			if(stmt->execute(qry)) {
				return stmt->getResultSet();
			} else {
				return NULL;			
			}
		}
		
	protected:
		friend class mysql_statement; // To call constructor
		explicit mysql_result_set(sql::Connection* c, sql::ResultSet* r)
		: conn(c), res(r), tblnam(NULL), primkey_name(NULL) {
			if(!res->isBeforeFirst()) { // Check whether result set is empty
				rows = NULL;
				sz = 0;
			} else {
				size_t i = 0;
				sz = res->getRow();
				res->first();
				rows = new mysql_row[sz];
				while(res->next()) {
					
				}
				res->first();
			}
		}
	public:
		
		typedef size_t size_type;
		typedef result_set::row* pointer;
		typedef result_set::row& reference;
		
		virtual ~mysql_result_set() {
			delete[] tblnam;
			delete[] primkey_name;
			delete[] rows;
			delete res;
		}
		
		virtual size_t size() const { return sz; }
		virtual bool empty() const { return sz == 0; }
		
		virtual const char* name() const {
			get_table_name();
			return tblnam;		
		}
		
		virtual iterator begin() {
			std::shared_ptr<result_set::polymorphic_iterator> ptr(new _mysql_iterator(res, &rows[0]));
			return iterator(ptr);
		}
		
		virtual iterator end() {
			std::shared_ptr<result_set::polymorphic_iterator> ptr(new _mysql_iterator(res, &rows[sz]));
			return iterator(ptr);
		}
		
		virtual iterator erase(iterator pos);
		
		virtual void clear() {
			std::string qry = "DELETE FROM ";
			get_table_name();
			qry += tblnam;
			qry += " WHERE 1=1";
			rows = NULL;
			sz = 0;
			basic_query(qry);
		}
	};
}

#endif // CPPDB_MYSQL_RESULT_SET_H
