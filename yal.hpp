#ifndef __YAL__
#define __YAL__

namespace yal {
/*
	range over iterator
*/
template <typename TIter, typename T>
struct irange {
	typedef TIter             iter_t;
	typedef irange<iter_t, T> this_t;

private:
	TIter   begin_;
	TIter   end_;

public:
	irange(TIter b, TIter e)
		: begin_ (b), end_ (e)
	{ }

	bool          ok() const  { return begin_ != end_; }
	iter_t        take()      { return begin_++;       }
	const iter_t& end() const { return end_;           }
};
/*
	Filtering range over range
*/
template <typename TNestedRange, typename TFilter, typename T>
struct rrange_filter {
	typedef TNestedRange                        nrange_t;
	typedef typename nrange_t::iter_t           iter_t;
	typedef rrange_filter<nrange_t, TFilter, T> this_t;

private:
	TFilter      filter_;
	TNestedRange range_;

public:
	rrange_filter(TNestedRange range1, TFilter& filter)
		: range_(range1), filter_(filter) { }

	bool ok() const
	{ return range_.ok(); }

	iter_t  take() {
		do {
			iter_t retVal = range_.take();
			if (filter_(*retVal)) return retVal;
		} while (ok());
		return end();
	}

	const iter_t& end() const
	{ return range_.end(); }
};
/*
	Mapping range over range
*/
template <typename TNestedRange, typename TMap, typename T>
struct rrange_map {
	typedef TNestedRange                  nrange_t;
	typedef typename nrange_t::iter_t     iter_t;
	typedef rrange_map<nrange_t, TMap, T> this_t;

private:
	TMap         foo_;
	TNestedRange range_;

public:
	rrange_map(TNestedRange range1, TMap& foo)
		: range_(range1), foo_(foo)
	{ }

	bool    ok() const
	{ return range_.ok(); }

	iter_t  take() {
		iter_t retVal = range_.take();
		if (retVal != end())
			foo_(*retVal);
		return retVal;
	}

	const iter_t& end() const
	{ return range_.end(); }
};
/*
	Atom is basic data type which encapsulates ranges, along with possible operations over them
*/
template <typename TColl, typename T, typename TRange>
struct atom {
	typedef atom<TColl, T, TRange> this_t;
	typedef TRange                 range_t;

	TRange      range_;

	atom(range_t r)
		: range_(r)
	{ }

	std::vector<T> toVector() {
		std::vector<T> v;
		while (range_.ok()) { 
			TRange::iter_t i = range_.take(); 
			if (i != range_.end()) 
				v.push_back(*i); 
		}
		return v;
	}

	template <typename TFoo>
	atom< TColl, T, rrange_map<range_t, TFoo, T> > foreach(TFoo& foo) {
		typedef rrange_map<range_t, TFoo, T> next_range_t;
		next_range_t next_range(range_, foo);
		return atom< TColl, T, next_range_t > (next_range);
	}
	
	template <typename TPred>
	atom< TColl, T, rrange_filter<range_t, TPred, T> > where(TPred& pred) {
		typedef rrange_filter<range_t, TPred, T> next_range_t;
		next_range_t next_range(range_, pred);
		return atom< TColl, T, next_range_t > (next_range);
	}
};
/*
	Basic constructor from std::vector
*/
template <typename T>
atom<std::vector<T>, T, irange< typename std::vector<T>::iterator, T > > 
fromVector(std::vector<T>& vec) {
	typedef irange< typename std::vector<T>::iterator, T > range_t;
	range_t range(vec.begin(), vec.end());

	return atom<std::vector<T>, T, range_t> (range);
}
} // namespace yal
#endif // __YAL