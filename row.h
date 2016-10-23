#ifndef _CPPDB_ROW_H
#define _CPPDB_ROW_H

#include <string>
#include <memory>
#include <iterator>
#include <initializer_list>
#include "exception.h"

namespace cppdb {
	
	class Row {
	protected:
	
		class ProxyImpl {
			std::string str;
		public:
			ProxyImpl(const std::string& s = ""): str(s) {}
			virtual ~ProxyImpl() {}
			virtual void assign(const std::string& s) {
				str = s;			
			}
			virtual std::string to_string() {
				return str;			
			}
		};
		
	private:
		size_t sz;
		std::string* arr;
	protected:
		
		Row(size_t s): sz(s), arr(new std::string[s]) {}
		
		std::string* fetch_array() { return arr; }

		void assign_to_array(size_t s, const std::string& val) {
			if(s >= sz) throw DBException("Programmer error");
			arr[s] = val;
		}

		std::string fetch_from_array(size_t s) const { // Acts as a non-const operator[], but not
							 // returning a db_proxy
			return arr[s];
		}
		
	public:
		
		Row(): sz(0), arr(nullptr) {}
		
		Row(std::initializer_list<std::string> il): sz(il.size()), arr(new std::string[sz]) {
			size_t i = 0;
			std::initializer_list<std::string>::iterator it = il.begin();
			for( ; i < sz; ++i, ++it) {
				arr[i] = *it;
			}
		}
		Row(const Row& rhs): sz(rhs.size()), arr(new std::string[sz]) {
			for(size_t i = 0; i < sz; ++i) {
				arr[i] = rhs[i];
			}
		}
		
		Row(Row&& rhs): sz(rhs.sz), arr(rhs.arr) {
			rhs.arr = nullptr;
		}
		
		virtual Row& operator=(const Row& rhs) {
			if(this == &rhs) return *this;
			delete[] arr;
			sz = rhs.sz;
			arr = new std::string[sz];
			for(size_t i = 0; i < sz; ++i) {
				arr[i] = rhs.arr[i];			
			}
		}
		
		virtual ~Row() {
			delete[] arr;		
		}
		
		
		class db_proxy {
			ProxyImpl* impl;
		public:
			db_proxy(ProxyImpl* pi) : impl(pi) {}
			~db_proxy() { delete impl; }
			const std::string& operator=(const std::string& s) {
				impl->assign(s); return s;
			}
			operator std::string() const { return impl->to_string(); }
			bool operator==(const std::string& s) {
				return impl->to_string() == s;				
			}
			bool operator!=(const std::string& s) {
				return impl->to_string() != s;				
			}
			bool operator==(const db_proxy& dbp) {
				return impl->to_string() == dbp.impl->to_string();				
			}
			bool operator!=(const db_proxy& dbp) {
				return impl->to_string() != dbp.impl->to_string();				
			}
			
			template<class charT, class Traits>
			friend std::basic_ostream<charT, Traits>& operator<<(std::basic_ostream<charT, Traits>& out, const db_proxy& dbp) {
				out << dbp.impl->to_string();
				return out;				
			}
		};
		
		typedef size_t size_type;
		
		virtual std::string operator[](size_t idx) const {
			return arr[idx];		
		}
		virtual std::string operator[](const std::string& key) const {
			throw BadRowException("Unable to index into row using a column name (Perhaps you are using a row that was not retrieved from a database?)");		
		}
		virtual std::string at(size_t idx) const {
			if(idx >= sz) {
				throw BadColumnException("Out of range error");			
			}
			return arr[idx];
		}
		virtual std::string at(const std::string& key) const {
			throw BadRowException("Unable to index into row using a column name (Perhaps you are using a row that was not retrieved from a database?)");		
		}
		
		virtual db_proxy operator[](size_t idx) {
			return db_proxy(new ProxyImpl(arr[idx]));		
		}
		virtual db_proxy operator[](const std::string& key) {
			throw BadRowException("Unable to index into row using a column name (Perhaps you are using a row that was not retrieved from a database?)");		
		}
		virtual db_proxy at(size_t idx) {
			if(idx >= sz) {
				throw BadColumnException("Out of range error");			
			}
			return db_proxy(new ProxyImpl(arr[idx]));
		}
		virtual db_proxy at(const std::string& key) {
			throw BadRowException("Unable to index into row using a column name (Perhaps you are using a row that was not retrieved from a database?)");
		}
		
		virtual size_t size() const {
			return sz;
		}

		virtual size_t row_number() const {
			throw BadRowException("This row does not have a corresponding row number (Perhaps you are using a row that was not retrieved from a database?)");
		}
		
		virtual Row* clone() const {
			return new Row(*this);	
		}

		virtual bool operator==(const Row& rhs) const {
			if(sz != rhs.size()) {
				return false;
			}
			for(size_t i = 0; i < sz; ++i) {
				if(arr[i] != rhs[i]) return false;
			}
			return true;
		}
		virtual bool operator!=(const Row& rhs) {
			return !(*this == rhs);
		}
	};

}

#endif // _CPPDB_ROW_H
