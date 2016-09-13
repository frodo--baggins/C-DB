// Inside class result_set

/*
		// class const_iterator;
		
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
		};*/
