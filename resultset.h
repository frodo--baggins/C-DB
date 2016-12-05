#ifndef _CPPDB_RESULTSET_H
#define _CPPDB_RESULTSET_H

#include <string>
#include <iterator>
#include "support/polymorphic_iterator.h"
#include "support/iterator_base.h"
#include "support/error.h"
#include "exception.h"
#include "row.h"

namespace cppdb {
	
	class ResultSet {
		// The class that contains the results of a query
		// This is a CONST CLASS - the declarations "const ResultSet x" and "ResultSet x"
		// are essentially identical
	public:

		class iterator;
		typedef iterator const_iterator;
		
		typedef Row value_type;
		typedef size_t size_type;
		typedef Row* pointer;
		typedef const Row* const_pointer;
		typedef Row& reference;
		typedef const Row& const_reference;		
		
		virtual ResultSet* clone() const = 0;
		virtual ~ResultSet() {}
		
		virtual size_type size() const = 0;
		virtual bool empty()  const = 0;
		
		virtual iterator begin() const = 0;
		virtual iterator end()	 const = 0;

		virtual const Row& operator[](size_type idx) const = 0;
		virtual const Row& at(size_type idx) const = 0;
		virtual const Row& front() const = 0;
		virtual const Row& back() const = 0;
		
	};



	class ResultSet::iterator {
		Support::polymorphic_iterator* pi;
	public:
		iterator(): pi(nullptr) {}
		iterator(Support::polymorphic_iterator* p): pi(p) {}
		iterator(const iterator& rhs): pi(rhs.pi->clone()) {}
		~iterator() { delete pi; }
		
		Row& operator*() { return pi->get_reference(); }
		Row* operator->() { return pi->get_pointer(); }
		
		iterator& operator++() {
			pi->next();
			return *this;
		}
		
		const iterator operator++(int) {
			iterator it = *this;
			++(*this);
			return it;
		}

		iterator& operator--() {
			pi->prev();
			return *this;
		}

		const iterator operator--(int) {
			iterator it = *this;
			--(*this);
			return it;
		}

		friend bool operator==(const iterator& lhs, const iterator& rhs) {
			return lhs.pi->equal_to(rhs.pi);
		}

		friend bool operator!=(const iterator& lhs, const iterator& rhs) {
			return !lhs.pi->equal_to(rhs.pi);
		}
	};
}

#endif // _CPPDB_RESULTSET_H
