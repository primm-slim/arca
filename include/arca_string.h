
// arca_string.h
//
namespace Arca {
	// Arca
	
namespace Str {
	// Arca::Str
	//
	//
	
	class CharSeq {
	public:
		using CharPtr = Char *;


		CharSeq(CharPtr buf, SizeT n) : p(buf), st(buf), en(buf + n) {}
		Char & operator*() { return *p;}
		CharPtr ptr() { return p;}
		void operator++() {p++;}
		operator bool() { return p != en;}
		void rewind() { p = st;}
		SizeT idx() { return p - st;}

	private:
		CharPtr p, st, en;
	};

	class ZCharSeq {
	public:
		using CharPtr = Char *;


		ZCharSeq(CharPtr buf, SizeT n) : p(buf), st(buf), en(buf + n) {}
		Char & operator*() { return *p;}
		CharPtr ptr() { return p;}
		void operator++() {p++;}
		operator bool() { return ((p != en) && (*p != 0));}
		void rewind() { p = st;}
		bool overflow() { return p != en;}
		SizeT idx() { return p - st - 1;}

	private:
		CharPtr p, st, en;
	};

	SizeT zlen(Char * str, SizeT buflen) {
		SizeT i = 0;
		auto seq = ZCharSeq(str, buflen);
		SEQ(seq) {
			i += 1;
		}
		return i;
	}

	void zcopy(Char * dest, Char * str, SizeT buflen) {
		Char * p = dest;
		auto seq = ZCharSeq(str, buflen);
		SEQ(seq) {
			*p++ = *seq;
		}
		if (seq.idx() != buflen)
			*p = '\0';
	}

	void zmatch(char * i, char * j, SizeT buflen, Bool & cmp_flag, Bool & match_flag) {
		match_flag = 0;
		auto seq = ZCharSeq(i, buflen);
		Char * p = j;
		SEQ(seq) {
			if (*seq != *p)
				break;
			++p;
		}
		if (seq.overflow()) {
			match_flag = 0;
			cmp_flag = 0;
			return;
		}
		Char a = *seq; Char b = *p;
		if ((a == 0) && (b == 0)) {
			match_flag = 1;
			return;
		} 
		match_flag = 0;
		if ((a == 0) && (b != 0)) {
			cmp_flag = 1;
		}
		if ((a != 0) && (b == 0)) {
			cmp_flag = 0;
		}
		if ((a != 0) && (b != 0)) {
			cmp_flag = (a < b) ? 0 : 1;
		}
		return;
	}


	void copy(char * dest, char * src, size_t len) {
		auto seq = CharSeq(src, len);
		char * p = dest;
		SEQ (seq) {
			*p++ = *seq;
		}
	};

	void clear(char * dest, size_t len) {
		auto seq = CharSeq(dest, len);
		SEQ (seq) {
			*seq = 0;
		}
	};

	SizeT seek(char * buf, SizeT len, char e) {
		auto seq = CharSeq(buf, len);
		SEQ (seq) {
			if (*seq == e)
				return seq.idx();
		}
		return len;
	}

	void shl(char * buf, SizeT len, SizeT d) {
		char * p = buf;
		char * e = buf + len;
		char * q = buf - d;
		while (p != e) {
			*q++ = *p++;
		}
	}

	void shr(char * buf, SizeT len, SizeT d) {
		char * p = buf + len;
		char * e = buf;
		char * q = buf + len + d;
		while (p != e) {
			*(q-1) = *(p-1);
			--p; --q;
		}
	}

	void reverse(char * str, SizeT len) {
		if (len <= 1)
			return;
		SizeT i = 0;
		SizeT j = len - 1;
		while (i < j) {
			char a = str[i];
			char b = str[j];
			str[j] = a;
			str[i] = b;
			i += 1; j += 1;
		}
	}

	template <typename F>
	void map(char * src, SizeT len, F f) {
		auto seq = CharSeq(src, len);
		SEQ (seq) {
			char ch = *seq;
			*seq = f(ch);
		}
	};

	template <typename F>
	void map_to(char * src, char * dest, SizeT len, F f) {
		char * p = dest;
		auto seq = CharSeq(src, len);
		SEQ (seq) {
			*p++ = f(*seq);
		}
	};

	namespace Arca::Asm = Asm;

	uint32_t hash32(char * str, SizeT len) {
		uint8_t code1[16] = {19, 41, 52, 67, 73, 74, 80, 89, 115, 175, 195, 214, 230, 233, 250, 253};
		uint8_t code2[16] = {4, 18, 44, 58, 59, 65, 118, 121, 133, 139, 151, 164, 200, 213, 245, 247};

		uint32_t x, y, h;
		h = 0;
		auto seq = CharSeq(str, len);
		SEQ(seq) {
			x = code1[*seq & 15];
			y = code2[*seq >> 4]; 

			h ^= x; h += Asm::ror32(h, 11); h ^= Asm::ror32(h, 30);
			h += 0xaa863171; h ^= Asm::ror32(h, 6); h += Asm::ror32(h, 6);
			h ^= y; h += Asm::ror32(h, 29); h ^= Asm::ror32(h, 23);
			h += 0x1d0ca70e; h ^= Asm::ror32(h, 13); h += Asm::ror32(h, 17);
		}
		h ^= 0x79bbaa27; h += Asm::ror32(h, 25); h ^= Asm::ror32(h, 18);
		h += 0x5d5d47b4; h ^= Asm::ror32(h, 24); h += Asm::ror32(h, 10);
		h ^= 0x2a319554; h += Asm::ror32(h, 18); h ^= Asm::ror32(h, 29);
		h += 0xdcaa8c50; h ^= Asm::ror32(h, 13); h += Asm::ror32(h, 14);
		return h;
	}

	uint64_t hash64(char * str, SizeT len) {
		uint8_t code1[16] = {16, 27, 35, 38, 49, 132, 147, 148, 165, 170, 177, 216, 222, 237, 252, 254};
		uint8_t code2[16] = {10, 14, 22, 54, 62, 83, 85, 92, 108, 122, 146, 153, 166, 198, 206, 249};


		uint64_t x, y, h;
		h = 0;
		auto seq = CharSeq(str, len);
		SEQ(seq) {
			x = code1[*seq & 15];
			y = code2[*seq >> 4]; 

			h ^= x; h += Asm::ror64(h, 5); h ^= Asm::ror64(h, 19);
			h += 0x90981efe71928e0c; h ^= Asm::ror64(h, 59); h += Asm::ror64(h, 28);
			h ^= y; h += Asm::ror64(h, 26); h ^= Asm::ror64(h, 12);
			h += 0x65c67b9dfe19b36b; h ^= Asm::ror64(h, 44); h += Asm::ror64(h, 61);
		}
		h ^= 0xe8fcd485c3d76a77; h += Asm::ror64(h, 56); h ^= Asm::ror64(h, 26);
		h += 0xe40f6620c3d879d4; h ^= Asm::ror64(h, 53); h += Asm::ror64(h, 42);
		h ^= 0x454e2ae0fc91dcee; h += Asm::ror64(h, 8); h ^= Asm::ror64(h, 31);
		h += 0xcc3642b0f4103511; h ^= Asm::ror64(h, 19); h += Asm::ror64(h, 51);
		return h;
	}


}; // namespace Str

namespace StringFn {
	// Arca::String

	namespace Arca::Str = Str;

	template <typename Index>
	class Roll {
	public:
		Roll(char * buf, Index imaxlen, Index & imaxpos)
			: data(buf), maxlen(imaxlen), maxpos(imaxpos) {}

		void clear() {
			Str::clear(data, maxlen,0);
			maxpos = 0;
		}


		Bool data_avail(Index pos) { return maxpos - pos;}

		Bool space_avail() { return maxlen - maxpos;}

		Index str_len(Index pos) {
			return Str::zlen(data + pos, maxlen - pos);
		}

		Index copy_in(char * buf, Index len) {
			Char * q = data + maxpos;
			Char * p = buf;
			Char * e = data + maxlen;
			Index n = 0;
			while ((n < len) && (p != e)) {
				if (*p == 0)
					break;
				*q++ = *p++; n += 1;
			}
			if ((p == e) || (n == len))
				return 0;
			maxpos += n;
			data[maxpos] = '\0';
			maxpos += 1;
			return n;
		}

		Index copy_out(Index pos, char * buf, Index len) {
			Char * q = buf;
			Char * p = data + pos;
			Char * e = buf + len;
			Char * z = data + maxlen;
			Index n = 0;
			while ((n < len) && (p != e) && (q != z)) {
				if (*p == 0) {
					*q = '\0';
					return n;
				}
				*q++ = *p++; n += 1;
			}
			return 0;
		}

		void dump_to(char * buf) {
			Str::copy(buf, data, maxpos);
		}

		Index max_pos() { return maxpos;}
		Index max_len() { return maxlen;}


	private:

		char * data;
		Index maxlen;
		Index & maxpos;

	};





}; // namespace String
}; // namespace Arca



/*

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



	template <typename Index, typename Elem>
	class ArrayList {
	public:
		using ElemPtr = Elem *;
		ArrayList(ElemPtr buf, Index n, Index & c) :
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
			return array_fn().check_any(f);
		}

		Elem find_max_elem() {
			if (ct == 0) return 0;
			return array_fn().find_max_elem();
		}

		Elem find_min_elem() {
			if (ct == 0) return 0;
			return array_fn().find_max_elem();
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





