#ifndef _CPPDB_ROW_H
#define _CPPDB_ROW_H

#include <string>
#include <memory>
#include <iterator>
#include <initializer_list>
#include "exception.h"

namespace cppdb {
	
	class row {
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
		
	public:
		
		row(): sz(0), arr(nullptr) {}
		
		row(std::initializer_list<std::string> il): sz(il.size()), arr(new std::string[sz]) {
			size_t i = 0;
			std::initializer_list<std::string>::iterator it = il.begin();
			for( ; i < sz; ++i, ++it) {
				arr[i] = *it;
			}
		}
		row(const row& rhs): sz(rhs.sz), arr(new std::string[sz]) {
			for(size_t i = 0; i < sz; ++i) {
				arr[i] = rhs.arr[i];
			}
		}
		
		row(row&& rhs): sz(rhs.sz), arr(rhs.arr) {
			rhs.arr = nullptr;
		}
		
		virtual row& operator=(const row& rhs) {
			if(this == &rhs) return *this;
			delete[] arr;
			sz = rhs.sz;
			arr = new std::string[sz];
			for(size_t i = 0; i < sz; ++i) {
				arr[i] = rhs.arr[i];			
			}
		}
		
		virtual ~row() {
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
		
		virtual std::string operator[](size_type idx) const {
			return arr[idx];		
		}
		virtual std::string operator[](const std::string& key) const {
			throw bad_row("Row does not have a corresponding database entry");		
		}
		virtual std::string at(size_type idx) const {
			if(idx >= sz) {
				throw bad_column("Out of range error");			
			}
			return arr[idx];
		}
		virtual std::string at(const std::string& key) const {
			throw bad_row("Row does not have a corresponding database entry");		
		}
		
		virtual db_proxy operator[](size_type idx) {
			return db_proxy(new ProxyImpl(arr[idx]));		
		}
		virtual db_proxy operator[](const std::string& key) {
			throw bad_row("Row does not have a corresponding database entry");		
		}
		virtual db_proxy at(size_type idx) {
			if(idx >= sz) {
				throw bad_column("Out of range error");			
			}
			return db_proxy(new ProxyImpl(arr[idx]));
		}
		virtual db_proxy at(const std::string& key) {
			throw bad_row("Row does not have a corresponding database entry");
		}
		
		virtual size_t size() const {
			return sz;
		}
		
		virtual row* clone() const {
			return new row(*this);	
		}

		virtual bool operator==(const row& rhs) const {
			if(sz != rhs.sz) {
				return false;
			}
			for(size_t i = 0; i < sz; ++i) {
				if(arr[i] != rhs[i]) return false;
			}
			return true;
		}
		virtual bool operator!=(const row& rhs) {
			return !(*this == rhs);
		}
	};

}

#endif // _CPPDB_ROW_H
