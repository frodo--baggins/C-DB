#ifndef _CPPDB_RESULT_SET_H
#define _CPPDB_RESULT_SET_H

#include <string>
#include <iterator>
#include "support/polymorphic_iterator.h"
#include "support/iterator_base.h"
#include "support/error.h"
#include "exception.h"
#include "row.h"

namespace cppdb {
	
	class result_set {
	public:

		class iterator;
		class const_iterator;
		
		typedef row value_type;
		typedef size_t size_type;
		typedef row* pointer;
		typedef const row* const_pointer;
		typedef row& reference;
		typedef const row& const_reference;		
		
		// virtual result_set* clone() const = 0;
		virtual ~result_set() {}
		
		virtual size_type size() const = 0;
		virtual bool empty()  const = 0;
		
		virtual const char* name() const = 0;
		
		virtual iterator begin() = 0;
		virtual iterator end()	 = 0;
		
		virtual void insert(const value_type& val) = 0;
		virtual void insert(const_iterator hint, const value_type& val) = 0;
		virtual iterator erase(iterator pos) = 0;
		virtual void clear() = 0;
	};

	class result_set::iterator: public Support::iterator_base {
	public:
		iterator(): Support::iterator_base(nullptr) {}
		iterator(Support::polymorphic_iterator* p): Support::iterator_base(p) {}
		iterator(const iterator& rhs): Support::iterator_base(rhs.pi->clone()) {}
		
		row& operator*() { return pi->get_reference(); }
		row* operator->() { return pi->get_pointer(); }
		
	};
	

}

#endif // _CPPDB_RESULT_SET_H
