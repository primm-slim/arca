
namespace Arca {
namespace Array {


	// Base needs comparison function
	//
	
	template <typename Index, typename Elem, typename Base>
	class ArrayHeap : public Base {
	public:
		using ElemPtr = Elem *;
		ArrayHeap(ElemPtr buf, Index & c, Index n ) : data(buf), ct(c), len(n) {}
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

		void load_from(ElemPtr buf, Index n) {
			for (Index i = 0; i < n; i += 1) 
				push(buf[i]);
		}

		void dump_to(ElemPtr buf) {
			for (Index i = 0; i < ct; i += 1) 
				buf[i] = pop();
		}


	private:
		// required in base class
		
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

		// required in base class

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

	template <typename Index, typename Elem, typename Base>
	class ArrayChainSet : public Base {
	public:
		using IndexPtr = Index *;
		using ElemPtr = Elem *;

		ArrayChainSet(IndexPtr top_buf, IndexPtr next_buf, ElemPtr elem_buf,
					  Index n, Index & ftop, Index & count) :
			top(top_buf), next(next_buf), data(elem_buf), len(n), free_top(ftop), ct(count) {}

		void reset() {
			ct = 0;
			for (Index i = 0; i < len - 1; i += 1) 
				next[i] = i + 1;
			next[len - 1] = null_ref();
			free_top = 0;
		}

		Bool search(Elem e) {
			Index t = hash_elem(e) % len;
			Index p = top[t];
			while (is_nn(p)) {
				if (data[p]==e) return 1;
				p = next[p];
			}
			return 0;
		}

		Bool insert(Elem e) {
			if (ct == len) return 0;

			Index t = hash_elem(e) % len;
			Index p = top[t];
			while (is_nn(p)) {
				if (data[p]==e) return 0;
				p = next[p];
			}
			Index s = pop_free();
			next[p] = s;
			next[s] = null_ref();
			data[s] = e;
			ct += 1;
			return 1;
		}
		Bool remove(Elem e) {
			if (ct == 0) return 0;

			Index t = hash_elem(e) % len;
			Index q = null_ref();
			Index p = top[t];
			while (is_nn(p)) {
				if (data[p]==e)
					break;
				q = p;
				p = next[p];
			}
			if (is_null(p))
				return 0;
			if (is_nn(q)) 
				next[q] = next[p];
			else 
				next[q] = top[t];
			push_free(p);
			ct -= 1;
			return 1;
		}



	private:
		void push_free(Index i) {
			next[i] = free_top;
			free_top = i;
		}
		Index pop_free() {
			Index p = free_top;
			free_top = next[p];
			return p;
		}



		static Index hash_elem(Elem e) { return Base::hash_elem(e);}
		static Index null_ref() { return Base::null_ref();}

		static Bool is_null(Index i) { return i == Base::null_ref();}
		static Bool is_nn(Index i) { return i != Base::null_ref();}

		IndexPtr * top;
		IndexPtr * next;
		ElemPtr * data;
		Index len;
		Index & free_top;
		Index & ct;
	};




}; // namespace Array
}; // namespace Arca


