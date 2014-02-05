#ifndef __YAL__
#define __YAL__
#include <vector>
#include <iostream>
namespace yal {

/*
	range over iterator
*/
template <typename TIter>
struct irange {
	typedef TIter          iter_t;
	typedef irange<iter_t> this_t;

private:
	TIter   begin_;
	TIter   end_;

public:
	irange(TIter b, TIter e)
		: begin_ (b), end_ (e)
	{ }

	bool          ok() const  { return begin_ != end_; }
	iter_t        take()      { return begin_++;       }
    template <typename TIt>
	bool          take2(TIt& it) { if (false == ok()) return false; it = *begin_; begin_++; return true; }
	const iter_t& end() const { return end_;           }
};
/*
	Filtering range over range
*/
template <typename TNestedRange, typename TFilter>
struct rrange_filter {
	typedef TNestedRange                      nrange_t;
	typedef typename nrange_t::iter_t         iter_t;
	typedef rrange_filter<nrange_t, TFilter>  this_t;

private:
	TFilter      filter_;
	TNestedRange range_;

public:
	rrange_filter(TNestedRange range1, TFilter& filter)
		:  filter_(filter), range_(range1)
	{ }

	bool ok() const
	{ return range_.ok(); }

	iter_t  take() {
		do {
			iter_t retVal = range_.take();
			if (filter_(*retVal)) return retVal;
		} while (ok());
		return end();
	}

	template <typename TIt>
	bool take2(TIt& it) {
		do {
			if (range_.take2(it)) {
				if (filter_(*it))
					return true;
				it.rollback();
			}
		} while (ok());
		return false;
	}

	const iter_t& end() const
	{ return range_.end(); }
};
/*
	Mapping range on range
*/
template <typename TNestedRange, typename TMap>
struct rrange_map {
	typedef TNestedRange                nrange_t;
	typedef typename nrange_t::iter_t   iter_t;
	typedef rrange_map<nrange_t, TMap>  this_t;

private:
	TMap         foo_;
	TNestedRange range_;

public:
	rrange_map(TNestedRange range1, TMap& foo)
		: foo_(foo), range_(range1) 
	{ }

	bool    ok() const
	{ return range_.ok(); }

	iter_t  take() {
		iter_t retVal = range_.take();
		if (retVal != end())
			foo_(*retVal);
		return retVal;
	}

	template <typename TIt>
	bool take2(TIt& it) {
		if (ok() && range_.take2(it)) {
			foo_(*it);
			return true;
		}
		return false;
	}

	const iter_t& end() const
	{ return range_.end(); }
};

/*
  Insert range
*/
template 
<
	typename TContainer // Concept that supports insert
	, typename TIt      // Iterator type
>
struct rrange_insert {
	typedef typename TContainer::reference reference;
private:
	TContainer& container_;
	TIt         it_;
	TIt         previous_;

public:
	rrange_insert(TContainer& container, TIt it)
		: container_(container) , it_(it), previous_(container.end())
	{ }
	
	void operator=(typename TContainer::value_type& value) {
		it_ = container_.insert(it_, value);
		previous_ = it_++;
	}

	reference operator*() {
		return *previous_; // fails if no assignment was called
	}

	void rollback() {
		it_ = container_.erase(previous_);
		previous_ = it_++;
	}
};
/*
	Atom is basic data type which encapsulates ranges, along with possible operations over them
*/
template <typename TColl, typename TRange>
struct atom {
	typedef atom<TColl, TRange> this_t;
	typedef TRange              range_t;
	typedef typename TColl::value_type value_type_t;

	TRange      range_;

	atom(range_t r)
		: range_(r)
	{ }

	std::vector<value_type_t> toVector() {
		std::vector<value_type_t> v;
		while (range_.ok()) { 
			typename TRange::iter_t i = range_.take(); 
			if (i != range_.end()) 
				v.push_back(*i); 
		}
		return v;
	}

	template<typename T>
	T to() {
		T t;
		while (range_.ok()) { 
			typename TRange::iter_t i = range_.take(); 
			if (i != range_.end()) 
				t.push_back(*i); 
		}
		return t;
	}

	template <typename TContainer>
	void fill(TContainer& container) {
		rrange_insert<TContainer, typename TContainer::iterator> insert_range(container, container.begin());
		while (range_.ok() && range_.take2(insert_range));
	}

	template <typename TFoo>
	atom< TColl, rrange_map<range_t, TFoo> > foreach(TFoo foo) {
		typedef rrange_map<range_t, TFoo> next_range_t;
		next_range_t next_range(range_, foo);
		return atom< TColl, next_range_t > (next_range);
	}
	
	template <typename TPred>
	atom< TColl, rrange_filter<range_t, TPred> > where(TPred pred) {
		typedef rrange_filter<range_t, TPred> next_range_t;
		next_range_t next_range(range_, pred);
		return atom< TColl, next_range_t > (next_range);
	}
};
/*
	Basic constructor from std::vector
*/

template <typename T>
atom<T, irange< typename T::iterator> > 
from(T& vec) {
	typedef irange< typename T::iterator > range_t;
	range_t range(vec.begin(), vec.end());
	return atom<T, range_t> (range);
}
} // namespace yal
#endif // __YAL
