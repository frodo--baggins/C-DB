#ifndef _CPPDB_RESULT_SET_H
#define _CPPDB_RESULT_SET_H

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
	};
	
	class result_set {
	public:
		
		class polymorphic_iterator {
		public:
			virtual ~polymorphic_iterator() {}
			virtual polymorphic_iterator* clone() const = 0;
			virtual row& get_reference() = 0;
			virtual row* get_pointer() = 0;
			virtual void next() = 0;
			virtual void prev() = 0;
			virtual bool equal_to(const polymorphic_iterator*) const = 0;
		};
		
		class const_iterator;
		
		class iterator {
			friend class mysql_result_set;
			friend class const_iterator;
			polymorphic_iterator* iter;
		protected:
			polymorphic_iterator* get_raw_iter() {
				return iter;			
			}
			
			const polymorphic_iterator* get_raw_iter() const {
				return iter;			
			}
			
			iterator(polymorphic_iterator* pi) : iter(pi) {}
		public:
			typedef row value_type;
			typedef row* pointer;
			typedef row& reference;
			typedef std::ptrdiff_t difference_type;
			typedef std::bidirectional_iterator_tag iterator_category;
			
			iterator(): iter(nullptr) {}
			iterator(const iterator& it): iter(it.iter == nullptr ? nullptr : it.iter->clone()) {}
			iterator& operator=(const iterator& it) {
				delete iter;
				iter = (it.iter == nullptr) ? nullptr : it.iter->clone();
			}
			~iterator() { delete iter; }
			
			row& operator*() const { return iter->get_reference(); }
			row* operator->() const { return iter->get_pointer(); }
			
			iterator& operator++() { iter->next(); return *this; }
			
			iterator operator++(int) {
				iterator tmp = *this;
				++(*this);
				return tmp;
			}
			
			iterator& operator--() { iter->prev(); return *this; }
			
			const iterator operator--(int) {
				iterator tmp = *this;
				++(*this);
				return tmp;			
			}
			
			friend bool operator==(const iterator& lhs, const iterator& rhs) {
				return lhs.iter->equal_to(rhs.iter);			
			}
			
			friend bool operator!=(const iterator& lhs, const iterator& rhs) {
				return !(lhs.iter->equal_to(rhs.iter));			
			}
		}; // end class iterator; continue class result_set
		
		class const_iterator {
			polymorphic_iterator* iter;
			friend class mysql_result_set; // Temporary solution; TODO remove
		protected:
			polymorphic_iterator* get_raw_iter() {
				return iter;
			}
			const_iterator(polymorphic_iterator* pi) : iter(pi) {}
		public:
			typedef row value_type;
			typedef row* pointer;
			typedef row& reference;
			typedef std::ptrdiff_t difference_type;
			typedef std::bidirectional_iterator_tag iterator_category;
			
			const_iterator(): iter(nullptr) {}
			const_iterator(const const_iterator& it): iter(it.iter == nullptr ? nullptr : it.iter->clone()) {}
			const_iterator(const iterator& it) {
				const polymorphic_iterator* pi = it.get_raw_iter();
				iter = (pi == nullptr) ? nullptr : pi->clone();
			}
			const_iterator& operator=(const const_iterator& it) {
				delete iter;
				iter = (it.iter == nullptr) ? nullptr : it.iter->clone();
			}
			~const_iterator() { delete iter; }
			
			const row& operator*() const { return iter->get_reference(); }
			const row* operator->() const { return iter->get_pointer(); }
			
			const_iterator& operator++() { iter->next(); return *this; }
			
			const const_iterator operator++(int) {
				const_iterator tmp = *this;
				++(*this);
				return tmp;
			}
			
			const_iterator& operator--() { iter->prev(); return *this; }
			
			const const_iterator operator--(int) {
				const_iterator tmp = *this;
				++(*this);
				return tmp;			
			}
			
			friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
				return lhs.iter->equal_to(rhs.iter);			
			}
			
			friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
				return !(lhs.iter->equal_to(rhs.iter));			
			}
			
			friend bool operator==(const const_iterator& lhs, const iterator& rhs) {
				return lhs.iter->equal_to(rhs.iter);			
			}
			
			friend bool operator!=(const const_iterator& lhs, const iterator& rhs) {
				return !(lhs.iter->equal_to(rhs.iter));			
			}
			
			friend bool operator==(const iterator& lhs, const const_iterator& rhs) {
				return lhs.iter->equal_to(rhs.iter);
			}
			
			friend bool operator!=(const iterator& lhs, const const_iterator& rhs) {
				return !(lhs.iter->equal_to(rhs.iter));
			}
		};
		
		typedef row value_type;
		typedef size_t size_type;
		typedef row* pointer;
		typedef const row* const_pointer;
		typedef row& reference;
		typedef const row& const_reference;		
		
		// virtual result_set* clone() const = 0;
		virtual ~result_set() {}
		
		virtual size_t size() const = 0;
		virtual bool empty()  const = 0;
		
		virtual const char* name() const = 0;
		
		virtual iterator begin() = 0;
		virtual iterator end()	 = 0;
		
		virtual void insert(const value_type& val) = 0;
		virtual void insert(const_iterator hint, const value_type& val) = 0;
		virtual iterator erase(iterator pos) = 0;
		virtual void clear() = 0;
	};
	

}

#endif // _CPPDB_RESULT_SET_H