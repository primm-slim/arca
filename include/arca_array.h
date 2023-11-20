// arca_array.h
//
namespace Arca {
	// Arca
	
namespace Mem {
	// Arca::Mem
	//
	//
	
	template <typename Elem>
	class Seq {
	public:
		using ElemPtr = Elem *;


		Seq(ElemPtr buf, SizeT n) : p(buf), st(buf), en(buf + n) {}
		Elem & operator*() { return *p;}
		ElemPtr ptr() { return p;}
		void operator++() {p++;}
		operator bool() { return p != en;}
		void rewind() { p = st;}


	private:
		ElemPtr p, st, en;
	};

	template <typename Elem>
	class SeqRev {
	public:
		using ElemPtr = Elem *;


		SeqRev(ElemPtr buf, SizeT n) : p(buf + n), st(buf + n), en(buf) {}
		Elem & operator*() { return *(p-1);}
		ElemPtr ptr() { return p - 1;}
		void operator++() {p--;}
		operator bool() { return p != en;}
		void rewind() { p = st;}


	private:
		ElemPtr p, st, en;
	};



	template <typename Elem>
	void copy(Elem * dest, Elem * src, size_t len) {
		auto seq = Seq<Elem>(src, len);
		Elem * p = dest;
		SEQ (seq) {
			*p++ = *seq;
		}
	};

	template <typename Elem>
	void rcopy(Elem * dest, Elem * src, size_t len) {
		auto seq = SeqRev<Elem>(src, len);
		Elem * p = dest + len;
		SEQ (seq) {
			*(p -1) = *seq;
		}
	};

	template <typename Elem>
	void clear(Elem * dest, size_t len, Elem e) {
		auto seq = Seq<Elem>(dest, len);
		SEQ (seq) {
			*seq = e;
		}
	};

	template <typename Elem>
	void cmp(Elem * i, Elem * j, SizeT len, Bool & match_flag, Bool & cmp_flag ) {
		Elem p = j;
		auto seq = Seq<Elem>(i, len);
		SEQ (seq) {
			Elem a = *seq;
			Elem b = *p++;
			if (a != b) {
				cmp_flag = (a < b) ? 0 : 1;
				match_flag = 0;
				return;
			}
		}
		match_flag = 1;
	}

	template <typename Elem>
	Elem * seek(Elem * buf, SizeT len, Elem e ) {
		auto seq = Seq<Elem>(buf, len);
		SEQ (seq) {
			if (*seq == e)
				return seq.ptr();
		}
		return buf + len;
	}

	template <typename Elem>
	void shl(Elem * data, SizeT len, SizeT d) {
		auto seq = Seq<Elem>(data, len);
		Elem * p = data - d;
		SEQ(seq) {
			*p++ = *seq;
		}
	}

	template <typename Elem>
	void shr(Elem * data, SizeT len, SizeT d) {
		auto seq = SeqRev<Elem>(data, len);
		Elem * p = data + len + d;
		SEQ(seq) {
			*(p -1) = *seq;
			--p;
		}
	}



}; // namespace Mem

namespace Array {

	template <typename Index, typename Elem>
	class ArrayIdxSeq {
	public:
		using ElemPtr = Elem *;
		ArrayIdxSeq(ElemPtr buf, Index n) : data(buf), len(n) {pos = 0;} 
		void rewind() {pos = 0;}
                Elem & operator*() { return data[pos];}
                void operator++() {pos += 1;}
                operator bool() { return pos != len;}
		Index get_idx() { return pos;}

	private:
		ElemPtr data;
		Index len, pos;
	};

	template <typename Index, typename Elem>
	class ArrayPtrSeq {
	public:
		using ElemPtr = Elem *;
		ArrayPtrSeq(ElemPtr buf, Index n) : st(buf), en(buf + n) {p = st;} 
		void rewind() {p = st;}
                Elem & operator*() { return *p;}
                void operator++() {p += 1;}
                operator bool() { return p != en;}
		ElemPtr get_elem_ptr() { return p;}

	private:
		ElemPtr st, en, p;
	};

	template <typename Index, typename Elem>
	class ArrayFn {
	public:
		using ElemPtr = Elem *;
		ArrayFn(ElemPtr buf, Index n) : data(buf), len(n) {} 

		template <typename F>
		void each(F f) {
			auto seq = get_ptr_seq();
			SEQ(seq) { f(*seq);}
		}

		template <typename F>
		void each_index(F f) {
			auto seq = get_idx_seq();
			SEQ(seq) { f(seq.get_idx(), *seq);}
		}

		void fill(Elem e) {
			auto seq = get_ptr_seq();
			SEQ(seq) { *seq = e;}
		}

		void fill(Elem e, Index p, Index n) {
			auto seq = ArrayPtrSeq<Index, Elem>(data + p, n);
			SEQ(seq) { *seq = e;}
		}


		// copy_to / copy_from (pos, len)
		// rindex?
		// shuffle
		// sum
		// uniq

		template <typename F>
		Bool check_any(F f) {
			auto seq = get_ptr_seq();
			SEQ(seq) {
				if (f(*seq)==1) return 1;
			}
			return 0;
		}

		template <typename F>
		Bool check_all(F f) {
			auto seq = get_ptr_seq();
			SEQ(seq) {
				if (f(*seq)==0) return 0;
			}
			return 1;
		}

		Elem find_max_elem() {
			auto seq = get_ptr_seq();
			Elem e = *seq; ++seq;
			SEQ(seq) {
				if (*seq > e) e = *seq;
			}
			return e;
		}

		Elem find_min_elem() {
			auto seq = get_ptr_seq();
			Elem e = *seq; ++seq;
			SEQ(seq) {
				if (*seq < e) e = *seq;
			}
			return e;
		}

		Pair<Elem, Elem> find_min_max_elem() {
			Elem x, y;
			auto seq = get_ptr_seq();
			x = y = *seq; ++seq;
			SEQ(seq) {
				if (*seq < x) x = *seq;
				if (*seq > y) y = *seq;
			}
			return Pair<Elem, Elem>(x, y);
		}

		void reverse() {
			Index i = 0;
			Index j = len - 1;
			while (i < j) {
				swap_elem(i, j);
				i += 1; j -= 1;
			}
		}

		void clear(Elem e) {
			auto seq = get_ptr_seq();
			SEQ(seq) { *seq = e;}
		}

		template <typename F>
		void map_to(ElemPtr dest, F f) {
			auto seq = get_ptr_seq();
			auto seq2 = ArrayPtrSeq<Index, Elem>(dest, len);
			SEQ(seq) {
				*seq2 = f(*seq);
				++seq2;
			}
		}

		template <typename F>
		void map(F f) {
			auto seq = get_ptr_seq();
			SEQ(seq) 
				*seq = f(*seq);
			
		}


		Index find(Elem e) {
			auto seq = get_idx_seq();
			SEQ(seq) {
				if (*seq == e)
					return seq.get_idx();
			}
			return len;
		}

		Index match_buf(ElemPtr buf) {
			auto seq = get_idx_seq();
			auto seq2 = ArrayPtrSeq<Index, Elem>(buf, len);
			SEQ(seq) {
				if (*seq2 != *seq)
					return seq.get_idx();
				++seq2;
			}
			return len;
		}

		void copy_to(ElemPtr dest) {
			auto seq = get_ptr_seq();
			auto seq2 = ArrayPtrSeq<Index, Elem>(dest, len);
			SEQ(seq) {
				*seq2 = *seq;
				++seq2;
			}
		}
		void copy_from(ElemPtr src) {
			auto seq = get_ptr_seq();
			auto seq2 = ArrayPtrSeq<Index, Elem>(src, len);
			SEQ(seq) {
				*seq = *seq2;
				++seq2;
			}
		}
		void swap_elem(Index x, Index y) {
			Elem a, b;
			a = data[x]; b = data[y];
			data[x] = b; data[y] = a;
		}
		void ins_sort() {
			if (len <= 1) return;
			Index i = 1;
			while (i < len) {
				Index j = i;
				while ((j > 0) && (data[j-1] > data[j])) {
					swap_elem(j-1, j);
					j -= 1;
				}
				i += 1;
			}
		}

		Index quick_sort_partition(Index low, Index high) {
			Index pivot = data[high];
			Index i = low;

			for (Index j = low; j <= high - 1; j++) {
				if (data[j] < pivot) {
					swap_elem(i, j);
					i += 1;
				}
			}
			swap_elem(i, high);
			return i;
		}


		void quick_sort_step(Index low, Index high) {
			if (low < high) {
				Index pi = quick_sort_partition(low, high);
				if (pi > 0)
					quick_sort_step(low, pi - 1);
				quick_sort_step(pi + 1, high);
			}
		}

		void quick_sort() {
			quick_sort_step(0, len - 1);
		}

		Index check_sort() {
			if (len <= 1) return 0;
			Index i = 1;
			while (i < len) {
				if (data[i] < data[i-1]) return i;
				i += 1;
			}
			return i;
		}

		Index bsearch(Elem val) {
			Index l, h, m;
			Elem x;

			l = 0; h = len - 1;

			while (l < h) {
				m = l + (h - l) / 2;
				if (m ==l)
					break;
				x = data[m];
				if (x == val)
					break;
				if (x < val)
					l = m;
				if (x > val)
					h = m;
			}
			return m;
		}



		void shl(Index pos, Index n, Index d) {
			Elem * p = data + pos;
			Elem * e = p + n;
			Elem * q = p - d;
			while (p != e) 
				*q++ = *p++;
		}

		void shr(Index pos, Index n, Index d) {
			Elem * p = data + pos + n;
			Elem * e = data + pos;
			Elem * q = p + d;
			while (p != e) {
				*(q-1) = *(p-1);
				--p; --q;
			}
		}


	private:
		ElemPtr data;
		Index len;

		ArrayPtrSeq<Index, Elem> get_ptr_seq() { return ArrayPtrSeq<Index, Elem>(data, len);}
		ArrayIdxSeq<Index, Elem> get_idx_seq() { return ArrayIdxSeq<Index, Elem>(data, len);}
	};

	template <typename Index, typename Elem>
	class ArrayListFn {
	public:
		using ElemPtr = Elem *;
		ArrayListFn(ElemPtr buf, Index n, Index & c) :
			data(buf), len(n), ct(c) {}

		void clear() {
			ct = 0;
		}

		Index count() { return ct;}

		void pushf(Elem e) {
			shr(0, ct, 1);
			data[0] = e;
			ct += 1;
		}

		Elem popf() {
			Elem e = data[0];
			shl(1, ct - 1, 1);
			ct -= 1;
			return e;
		}

		void pushb(Elem e) {
			data[ct++] = e;
		}

		void append(Elem e) { pushb(e);}

		Elem popb() {
			ct -= 1;
			Elem e = data[ct];
			return e;
		}

		void del_at(Index pos) {
			ct -= 1;
			shl(pos, ct - pos , 1);
		}

		void insert(Elem e, Index pos) {
			shr(pos, ct - pos, 1);
			data[pos] = e;
			ct += 1;
		}

		Index find(Elem e, Index pos) {
			Index i = pos;
			while ((i < ct) && (data[i] != e))
				i += 1;
			return i;
		}

		template <typename F>
		void keep_if(F f) {
			Index i, j;
			i = 0; j = 0;
			while (i != ct) {
				Elem e = data[i++];
				if (f(e)) 
					data[j++] = e;
			}
			ct = j;
		}

		Elem & operator[] (Index i) { return data[i];}

		template <typename F>
		void each(F f) {
			if (ct == 0) return;
			array_fn().each(f);
		}


		template <typename F>
		Bool check_any(F f) {
			if (ct == 0) return 0;
			return array_fn().check_any(f);
		}
		template <typename F>
		Bool check_all(F f) {
			if (ct == 0) return 0;
			return array_fn().check_all(f);
		}

		Elem find_max_elem() {
			if (ct == 0) return 0;
			return array_fn().find_max_elem();
		}

		Elem find_min_elem() {
			if (ct == 0) return 0;
			return array_fn().find_min_elem();
		}

		Pair<Elem, Elem> find_min_max_elem() {
			if (ct == 0) return 0;
			return array_fn().find_min_max_elem();
		}

		void reverse() {
			if (ct <= 1) return;
			return array_fn().reverse();
		}

		void map_to(ElemPtr buf) {
			if (ct == 0) return;
			array_fn().map_to(buf);
		}

		template <typename F>
		void map(F f) {
			if (ct == 0) return;
			array_fn().map(f);
		}

		Index match_buf(ElemPtr buf) {
			if (ct == 0) return;
			return array_fn().match_buf(buf);
		}

		void copy_to(ElemPtr buf) {
			if (ct == 0) return;
			return array_fn().copy_to(buf);
		}

		void copy_from(ElemPtr buf, Index n) {
			if (n == 0) return;
			if (n > len) n = len;
			auto array = ArrayFn<Index, Elem>(data, n);
			array.copy_from(buf);
		}
		
		void swap_elem(Index i, Index j) {
			if ((i < ct) && (j < ct)) {
				Elem a, b;
				a = data[i]; b = data[j];
				data[j] = a; data[i] = b;
			}
		}
		void ins_sort() {
			if (ct <= 1) return;
			array_fn().ins_sort();
		}
		void quick_sort() {
			if (ct <= 1) return;
			array_fn().quick_sort();
		}
		Bool check_sort() {
			if (ct <= 1) return 1;
			return array_fn().check_sort();
		}

		Index bsearch(Elem e) {
			if (ct == 0) return 0;
			array_fn().bsearch(e);
		}

		void shl(Index pos, Index n, Index d) {
			Elem * p = data + pos;
			Elem * e = p + n;
			Elem * q = p - d;
			while (p != e) 
				*q++ = *p++;
		}

		void shr(Index pos, Index n, Index d) {
			Elem * p = data + pos + n;
			Elem * e = data + pos;
			Elem * q = p + d;
			while (p != e) {
				*(q-1) = *(p-1);
				--p; --q;
			}
		}

	private:

		ElemPtr data;
		Index len;
		Index & ct;

		ArrayFn<Index, Elem> array_fn() {
			return ArrayFn<Index, Elem>(data, ct);
		}
	};

	template <typename Index, typename Elem>
	class SortedArrayFn {
	public:
		using ElemPtr = Elem *;
		SortedArrayFn(ElemPtr buf, Index n, Index & c) :
			data(buf), len(n), ct(c) {}


		void add_key(Elem e) {
			Index i = ct;
			Index step = ct >> 1;
			while (step >= 2) {
				if (data[i - step - 1]>e)
					i -= step;
				step >>= 1;
			}
			while (i > 0) {
				if (data[i-1] < e )
					break;
				data[i] = data[i-1];
				i -= 1;
			}
			data[i] = e;
			ct += 1;
		}

		void append(Elem e) {
			data[ct++] = e;
		}

		Bool has_key(Elem e) {
			Index i = 0;
			Index step = ct >> 1;
			while (step >= 2) {
				if (data[i+step] < e)
					i += step;
				step >>= 1;
			}
			while (i < ct) {
				if (data[i] == e)
					return 1;
				if (data[i] > e)
					break;
				i += 1;
			}
			return 0;
		}

		Bool remove(Elem e) {
			Index i = ct;
			Index step = ct >> 1;
			while (step >= 2) {
				if (data[i - step - 1]>e)
					i -= step;
				step >>= 1;
			}
			while (i != ct) {
				if (data[i]<e)
					i += 1;
			}
			if ((i == ct) || (data[i] != e))
				return 0;
			while (i < ct - 1) {
				data[i] = data[i+1];
				i += 1;
			}
			ct -= 1;
			return 1;
		}

		template <typename F>
		void each(F f) {
			for (Index i = 0; i != ct; i += 1)
				f(data[i]);
		}

		template <typename F>
		void keep_if(F f) {
			Index i = 0;
			Index j = 0;
			while (i != ct) {
				Elem e = data[i++];
				if (f(e)==1)
					data[j++];
			}
			ct = j;
		}

		Index avail() { return len - ct;}
		Index count() { return ct;}

		void split_to(Index c, ElemPtr buf) {
			Index i = 0;
			Index j = c;
			while (j != ct) {
				buf[i++] = data[j++];
			}
			ct = c;
		}

		Elem split_pivot(Index c, ElemPtr buf) {
			Elem e = data[c];
			Index i = c + 1;
			Index j = 0;
			while (i < ct) {
				buf[i++] = data[j++];
			}
			ct = c;
			return e;
		}

		Elem pop_last() {
			ct -= 1;
			Elem e = data[ct];
			return e;
		}

		void merge_from(ElemPtr buf, Index c) {
			Index i = ct;
			Index j = 0;
			while (j < c) {
				data[i++] = buf[j++];
			}
			ct += c;
		}

		void import_from(ElemPtr buf, Index c) {
			Index j = 0;
			while (j != c) {
				Elem e = buf[j++];
				Index i = ct;
				while (i > 0) {
					if (data[i - 1] < e)
						break;
					data[i] = data[i - 1];
					i -= 1;
				}
				data[i] = e;
				ct += 1;
			}
		}

		Elem & operator[](Index i) { return data[i];}

		template <typename F>
		void range_query(Elem st, Elem en, F f) {
			Index i = 0;
			Index step = ct; step >>= 1;
			while (step >= 2) {
				if (data[i + step] < st)
					i += step;
				step >>= 1;
			}
			while (i < ct) {
				if (data[i] >= st)
					break;
				i += 1;
			}
			while (i < ct) {
				Elem e = data[i];
				if (e > en)
					break;
				f(e);
				i += 1;
			}
		}

		void dump_to(ElemPtr buf) {
			Mem::copy(buf, data, ct);
		}

	private:

		Elem * data;
		Index len;
		Index ct;
	};

	template <typename Index, typename Key, typename Val>
	class SortedArrayMapFn {
	public:
		using KeyPtr = Key *;
		using ValPtr = Val *;

		SortedArrayMapFn(KeyPtr key_buf, ValPtr val_buf, Index n, Index & c) :
			key(key_buf), val(val_buf), len(n), ct(c) {}


		void add_elem(Key k, Val v) {
			Index i = ct;
			Index step = ct >> 1;
			while (step >= 2) {
				if (key[i - step - 1]>k)
					i -= step;
				step >>= 1;
			}
			while (i > 0) {
				if (key[i-1] < k )
					break;
				key[i] = key[i-1];
				val[i] = val[i-1];
				i -= 1;
			}
			key[i] = k;
			val[i] = v;
			ct += 1;
		}

		void append(Key k, Val v) {
			key[ct] = k;
			val[ct] = v;
			ct += 1;
		}

		Index find_key(Key k) {
			Index i = 0;
			Index step = ct >> 1;
			while (step >= 2) {
				if (key[i+step] < k)
					i += step;
				step >>= 1;
			}
			while (i < ct) {
				if (key[i] == k)
					return i;
				if (key[i] > k)
					break;
				i += 1;
			}
			return i;
		}

		Bool remove(Key k) {
			Index i = ct;
			Index step = ct >> 1;
			while (step >= 2) {
				if (key[i - step - 1]>k)
					i -= step;
				step >>= 1;
			}
			while (i != ct) {
				if (key[i]<k)
					i += 1;
			}
			if ((i == ct) || (key[i] != k))
				return 0;
			while (i < ct - 1) {
				key[i] = key[i+1];
				val[i] = val[i+1];
				i += 1;
			}
			ct -= 1;
			return 1;
		}

		template <typename F>
		void each(F f) {
			for (Index i = 0; i != ct; i += 1)
				f(key[i], val[i]);
		}

		template <typename F>
		void keep_if(F f) {
			Index i = 0;
			Index j = 0;
			while (i != ct) {
				Key k = key[i];
				Val v = val[i];
				if (f(k, v)==1) {
					key[j] = k;
					val[j] = v;
					++i;
				}
			}
			++j;
			ct = j;
		}

		Index avail() { return len - ct;}
		Index count() { return ct;}

		void split_to(Index c, KeyPtr key_buf, ValPtr val_buf) {
			Index i = 0;
			Index j = c;
			while (j != ct) {
				key_buf[i] = key[j];
				val_buf[i] = val[j];
				++i; ++j;
			}
			ct = c;
		}

		Pair<Key, Val> split_pivot(Index c, KeyPtr key_buf, ValPtr val_buf) {
			auto pair = Pair<Key, Val>(key[c], val[c]);
			Index i = c + 1;
			Index j = 0;
			while (i < ct) {
				key_buf[i] = key[j];
				val_buf[i] = val[j];
				++i; ++j;
			}
			ct = c;
			return pair;
		}

		Pair<Key, Val> pop_last() {
			ct -= 1;
			auto pair = Pair<Key, Val>(key[ct], val[ct]);
			return pair;
		}

		void merge_from(KeyPtr key_buf, ValPtr val_buf, Index c) {
			Index i = ct;
			Index j = 0;
			while (j < c) {
				key[i] = key_buf[j];
				val[i] = val_buf[j];
				++i; ++j;
			}
			ct += c;
		}

		void import_from(KeyPtr key_buf, ValPtr val_buf, Index c) {
			Index j = 0;
			while (j != c) {
				Key k = key_buf[j];
				Val v = val_buf[j];
				j += 1;
				Index i = ct;
				while (i > 0) {
					if (key[i - 1] < k)
						break;
					key[i] = key[i - 1];
					val[i] = val[i - 1];
					i -= 1;
				}
				key[i] = k;
				val[i] = v;
				ct += 1;
			}
		}

		Key & get_key(Index i) { return key[i];}
		Val & get_val(Index i) { return val[i];}

		template <typename F>
		void range_query(Key st, Key en, F f) {
			Index i = 0;
			Index step = ct; step >>= 1;
			while (step >= 2) {
				if (key[i + step] < st)
					i += step;
				step >>= 1;
			}
			while (i < ct) {
				if (key[i] >= st)
					break;
				i += 1;
			}
			while (i < ct) {
				Key k = key[i];
				Val v = val[i];
				if (k > en)
					break;
				f(k, v);
				i += 1;
			}
		}

		void dump_to(KeyPtr key_buf, ValPtr val_buf) {
			Mem::copy(key_buf, key, ct);
			Mem::copy(val_buf, val, ct);

		}

	private:

		Key * key;
		Val * val;
		Index len;
		Index ct;
	};



	template <typename Index, typename Base>
	class ArrayFreeListFn {
	public:

		ArrayFreeListFn(Index & free_top, Index * buf, Index n):
			top(free_top), data(buf), len(n)  {}

		Bool avail() { return top != null_ref();}

		void setup() {
			top = null_ref();
			Index i = 0;
			while (i != len - 1) {
				data[i] = i + 1; ++i;
			}
			data[i] = null_ref();
			top = 0;
		}

		void expand_to(Index * ndata, Index nlen) {
			Index i = 0;
			while (i != len) {
				ndata[i] = data[i]; ++i;
			}
			while (i != nlen - 1) {
				data[i] = i + 1; ++i;
			}
			data[i] = top;
			top = len;
		}

		void push(Index i) {
			data[i] = top;
			top = i;
		}

		Index pop() {
			Index p = top;
			top = data[p];
			return p;
		}

	private:

		// from Base

		Index null_ref() { return Base::null_ref;}

		Bool is_null(Index p) { return p == null_ref();}


		Index & top;
		Index * data;
		Index len;
	};

	/*
	// MaxHeap, F is a comparison function
	
	template <typename Index, typename Elem, typename Base>
	class ArrayHeap : public Base {
	public:
		using ElemPtr = Elem *;
		ArrayHeap(ElemPtr & buf, Index & c, Index n ) : data(buf), ct(c), len(n) {}
		void clear() { reset();}
		void reset() { ct = 0;}

		Bool is_empty() { return ct == 0;}
		Bool is_full() { return ct == len;}

		void push(Elem e) {
			Index pos = ct;
			data[ct++] = e;
			sift_up(pos);
		}

		Elem pop() {
			Elem e = data[0];
			ct -= 1;
			if (ct != 0) {
				data[0] = data[ct];
				sift_down(0);
			}
			return e;
		}

		Elem peek() { return data[0];}



	private:
		Bool cmp(Elem x, Elem y) {
			return Base::compare(x, y);
		}
		void swap_elem(Index i, Index j) {
			Elem t1 = data[i]; Elem t2 = data[j];
			data[j] = t1; data[i] = t2;
		}
		Index compare_down(Index k) {
			Index a, b;
			Index h = k;
			a = (k << 1) + 1;
			b = (k << 1) + 2;
			if ((a < ct) && cmp(data[a], data[h])) h = a;
			if ((b < ct) && cmp(data[b], data[h])) h = b;
			return h;
		}

		Index compare_up(Index k) {
			Index u = (k - 1) >> 1;

			if (cmp(data[u], data[k]))
				return k;
			else
				return u;
		}

		void sift_down(Index pos) {
			Index k = pos;
			LOOP {
				Index h = compare_down(k);
				if (h == k) break;
				swap_elem(k, h);
				k = h;
			}
		}

		void sift_up(Index pos) {
			Index k = pos;
			while (k > 0) {
				Index h = compare_up(k);
				if (h == k) break;
				swap_elem(k, h);
				k = h;
			}
		}


		ElemPtr data;
		Index & ct;
		Index len;
		// Bool (*cmp_func)(Elem, Elem); 
	};

	template <typename Index, typename Elem>
	class ArrayStack {
	public:
		using ElemPtr = Elem *;
		ArrayStack(ElemPtr buf, Index n, Index & c) :
			data(buf), len(n), ct(c) {}

		void clear() { reset();}
		void reset() {
			ct = 0;
		}

		Bool is_empty() { return ct == 0;}
		Bool is_full() { return ct == len;}

		void push(Elem e) {
			if (is_full()) return;
			data[ct++] = e;
		}
		Elem pop() {
			if (is_empty()) return;
			ct -= 1;
			return data[ct];
		}
		// need copy_to, copy_from
		
		Index count() { return ct;}
		Index avail() { return len - ct;}
	private:
		ElemPtr data;
		Index len;
		Index & ct;
	};

	// Todo: Ring Buffer

	template <typename Index, typename Elem, typename Base>
	class ArraySet : public Base {
	public:
		using ElemPtr = Elem *;
		ArraySet(ElemPtr buf, Index n, Index & c) :
			data(buf), len(n), ct(c) {}

		void clear() { reset();}
		void reset() {
			auto seq = ArrayPtrSeq<Index, Elem>(data, len);
			SEQ(seq) { *seq = empty_value();}
			ct = 0;
		}

		Bool search(Elem v) {
			auto seq = FwdSeq(data, len, hash_elem(v) % len);
			SEQ(seq) {
				Elem e = *seq;
				if (is_empty(e))
					return 0;
				if (e == v)
					break;
			}
			return 1;
		}

		Bool insert(Elem v) {
			if (is_empty(v) )
				return 0;
			if (avail()==0)
				return 0;
			auto seq = FwdSeq(data, len, hash_elem(v) % len);
			Elem e;
			SEQ(seq) {
				e = *seq;
				if (is_empty(e))
					break;
				if (e == v)
					return 0;
			}
			*seq = v;
			ct += 1;
			return 1;
		}

		Bool remove(Elem v) {
			auto seq = FwdSeq(data, len, hash_elem(v) % len);
			SEQ(seq) {
				Elem e = * seq;
				if (is_empty(e))
					return 0;
				if (e == v)
					break;
			}
			delete_from_pos(seq.pos);
			return 1;
		}

		Index extract_to(ElemPtr buf) {
			Index t = 0;
			auto seq = ArraySeq(data, len);
			SEQ(seq) {
				Elem e = * seq;
				if (is_elem(e)) {
					buf[t++] = e;
				}
			}
			return t;
		}

		Index load_from(ElemPtr buf, Index n) {
			auto seq = ArraySeq(buf, n);
			Index t = 0;
			SEQ(seq) {
				Elem e = *seq;
				t += insert(e);
			}
			return t;
		}

		Index rebuild_to(ElemPtr buf, Index n) {
			auto buf_seq0 = ArraySeq(buf, n);
			SEQ(buf_seq0) { *buf_seq0 = empty_value();}

			Index c = 0;
			auto seq = ArraySeq(data, len);
			SEQ(seq) {
				Elem e = *seq;
				if (is_elem(e) && (c < (len - 1))) {
					auto seq2 = Seq(buf, n, hash_elem(e) % n);
					SEQ(seq2) {
						Elem t = *seq2;
						if (is_empty(t))
							break;
					}
					*seq2 = e;
					c += 1;
				}
			}
			return c;
		}

		template <typename F>
		void each(F f) {
			auto seq = ArraySeq(data, len);
			SEQ(seq) {
				Elem e = * seq;
				if (is_elem(e))
					f(e);
			}
		}

		template <typename F>
		void keep_if(F f) {
			auto seq = ArraySeq(data, len);
			SEQ(seq) {
				Elem e = *seq;
				if ((is_elem(e)) && (f(e)==0)) 
					delete_from(seq.pos);
			}
		}


		Index count() { return ct;}

		Index avail() { return len - ct - 1;}

		Index stats() {
			Index c = 0;
			for (Index i = 0; i != len; i += 1) {
				Elem e = data[i];
				if (is_elem(e)) {
					Index h = hash_elem(e) % len;
					if (h != i)
						c += 1;
				}
			}
			return c;
		}


	private:
		ElemPtr data;
		Index len;
		Index & ct;

		Index hash_elem(Elem e) { return Base::hash_elem(e);}

		Elem empty_value() { return Base::empty_value();}

		Bool is_empty(Elem e) { return e == Base::empty_value();}
		Bool is_elem(Elem e) { return e != Base::empty_value();}

		struct FwdSeq {
			ElemPtr data;
			Index pos, len;

			FwdSeq(ElemPtr buf, Index c, Index p) : data(buf), pos(p), len(c) {}

			Elem & operator*() { return data[pos];}
			operator Bool() { return 1;}

			void operator++() { 
				next_pos(pos, len);
			}
		};

		static void next_pos(Index & pos, Index len) {
			pos += 1; if (pos == len) pos = 0;
		}

		void delete_from_pos(Index pos) {
			Index i = pos;
			data[pos] = empty_value();
			ct -= 1;
			next_pos(i, len);
			LOOP {
				Elem e = data[i];
				if (is_empty(e))
					break;
				if (is_elem(e)) {
					data[i] = empty_value();
					ct -= 1;
					insert(e);
				}
				next_pos(i, len);
			}
		}



	};

	*/



}; // namespace ArrayLib
}; // namespace Arca


