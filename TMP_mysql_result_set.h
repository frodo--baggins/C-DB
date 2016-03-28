 	/*class mysql_result_set: public result_set {
	protected:
		friend class mysql_statement;
		friend class statement;
		explicit mysql_result_set(sql::ResultSet* r) : res(r) {
			if(res->last()) {
				sz = res->getRow();
				res->beforeFirst();			
			} else {
				sz = 0;			
			}
		}
		boost::shared_ptr<sql::ResultSet> get_res() { return res; } // res is declared below: boost::shared_ptr<sql::ResultSet> res;
	public:
		
		virtual ~mysql_result_set() {}
		
		class mysql_row;

		class _mysql_iterator;
		
		friend class mysql_row;
		friend class _mysql_iterator;
		
		
		typedef mysql_row value_type;
		typedef size_t size_type;
		typedef void allocator_type;
		typedef row* pointer;
		typedef const row* const_pointer;
		typedef row& reference;
		typedef const row& const_reference;
		typedef result_set::row row;
		typedef result_set::iterator iterator;

		virtual iterator begin();
		virtual iterator end();
		
		virtual iterator erase(iterator position) {
			if(boost::shared_ptr<_mysql_iterator> pmi = boost::dynamic_pointer_cast<_mysql_iterator>(position.iter)) {
				pmi->rw->rres->deleteRow();
			} else {
				throw db_exception("Wrong iterator type presented");
			}
			++position;
			return position;
		}
		
		virtual size_t size() const { return sz; }
		virtual bool empty()  const { return sz == 0; }
		
	private:
		sql::ResultSet* res;
		size_t sz;
		static iterator end_iter;
		
		mysql_result_set();
	};
	
	class mysql_result_set::mysql_row: public result_set::row {
		friend class mysql_result_set;
		sql::ResultSet* rres;
		size_t cr;
	protected:
		sql::ResultSet* get_res() { return rres; }
	public:
		mysql_row(mysql_result_set& mrs) : result_set::row(), rres(mrs.get_res()), cr(rres->getRow()) {}
		
		~mysql_row() { delete rres; }
		
		virtual std::string operator[](int ind) {
			return rres->getString(ind);
		}
		
		virtual std::string operator[](const std::string& key) {
			return rres->getString(key);
		}
		
		virtual size_t index() { return cr; }
	};
	
	class mysql_result_set::_mysql_iterator: public result_set::polymorphic_iterator, private mysql_result_set::mysql_row { 
		// Private inheritance used to call mysql_row::get_res()
		friend class mysql_result_set;
		boost::shared_ptr<mysql_row> rw;
	public:
		_mysql_iterator(boost::shared_ptr<mysql_row> r) : rw(r) {}
		
		virtual result_set::row& get_reference() { return *rw; }
		virtual result_set::row* get_pointer() { return rw.get(); }
		
		virtual void next() {
			if(!rw->get_res()->next()) rw = NULL;
		}
	
		virtual void prev() {
			if(!rw->get_res()->next()) rw = NULL;
		}
		
		virtual bool equal_to(boost::shared_ptr<polymorphic_iterator> pi) {
			// Return whether current polymorphic iterator is equal to PI.
			// Passing this function any polymorphic iterator whose dynamic
			// type is not equal to _mysql_iterator causes UNDEFINED BEHAVIOR.
			
			boost::shared_ptr<_mysql_iterator> mi = boost::static_pointer_cast<_mysql_iterator>(pi); // Assume pi points to _mysql_iterator
			if(rw == NULL && mi->rw == NULL) return true;
			return rw == mi->rw && (rw->get_res())->getRow() == (mi->rw->get_res())->getRow();
		}
		
	};*/