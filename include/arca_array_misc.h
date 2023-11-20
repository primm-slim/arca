
namespace Arca {
namespace ArrayLib {

	template <typename Index, typename Elem, typename Base>
	class ArraySet {
	public:
		using ElemPtr = Elem *;
		ArraySet(ElemPtr buf, Index & b_ct, Index & f_ct, Index & d_ct, Index n) :
			data(buf), len(n), busy_ct(b_ct), free_ct(f_ct), del_ct(d_ct) {}

		void clear() {
			for (Index i = 0; i != len; i += 1)
				data[i] = empty_value();
			free_ct = len;
			busy_ct = del_ct = 0;
		}
		Index busy_count() { return busy_ct;}
		Index free_count() { return free_ct;}
		Index del_count() { return del_ct;}

		Index extract_to(ElemPtr buf) {
			Index c = 0;
			Index i = 0;
			while (i != len) {
				Elem e = data[i];
				if ((e != deleted_value()) && (e != empty_value())) 
					buf[c++] = e;
				i += 1;
			}
			return c;
		}

		void load_from(ElemPtr buf, Index ct) {
			clear();
			for (Index j = 0; j != ct; j += 1) {
				Elem e = buf[j];
				Index i; start_pos(e, i);
				while (data[i] != empty_value()) next_pos(i);
				data[i] = e;
			}
			busy_ct = ct;
			del_ct = 0;
			free_ct = len - ct;
		}

		Index insert(Elem v) {
			Index i; start_pos(v, i);
			Index j; j = i;
			Elem e;
			LOOP {
				e = data[i];
				if (e == v) 
					return 0;
				if ((e == empty_value()) || (e == deleted_value()))
					break;
				next_pos(i);
				if (i == j) return 0;
			};
			if (e == deleted_value()) {
				del_ct -= 1;
			} else {
				cleanup_from_empty(i);
				free_ct -= 1;
			}
			data[i] = v;
			busy_ct += 1;
			return 1;
		}

		Index remove(Elem v) {
			Index i; start_pos(v, i);
			Index j; j = i;
			Elem e;
			LOOP {
				e = data[i];
				if (e == empty_value()) {
					cleanup_from_empty(i);
					return 0;
				}
				if (e == v)
					break;
				next_pos(i);
				if (i == j) return 0;
			};
			data[i] = deleted_value();
			del_ct += 1;
			busy_ct -= 1;
			return 1;
		}

		Bool search(Elem v) {
			Index i; start_pos(v, i);
			Index j; j = i;
			Bool del_flag = 0;
			Index del_pos;
			Elem e;
			LOOP {
				e = data[i];
				if ((e == empty_value()) || (e == v))
					break;
				if ((e == deleted_value()) && !del_flag) {
					del_flag = 1; del_pos = i;
				}
				next_pos(i);
				if (i == j) return 0;
			}

			if (e == v) {
				if (del_flag) {
					data[del_pos] = v;
					data[i] = deleted_value();
					i = del_pos;
				}
				return 1;
			} else {
				cleanup_from_empty(i);
				return 0;
			}
		}

		Bool take(Elem v) {
			Index i; start_pos(v, i);
			Index j; j = i;
			Elem e;
			LOOP {
				e = data[i];
				if ((e == empty_value()) || (e == v))
					break;
				next_pos(i);
				if (i == j) return 0;
			}

			if (e == v) {
				data[i] = deleted_value();
				del_ct += 1;
				busy_ct -= 1;
				return 1;
			} else {
				cleanup_from_empty(i);
				return 0;
			}
		}


	private:
		Elem deleted_value() { return Base::deleted_value();}
		Elem empty_value() { return Base::empty_value();}

		void start_pos(Elem e, Index & i) {
			Index h = Base::hash_elem(e);
			h %= len; i = h;
		}
		void next_pos(Index & i) {
			i += 1;
			if (i == len) i = 0;
		}
		void prev_pos(Index & i) {
			if (i == 0) i = len;
			i -= 1;
		}
		void cleanup_from_empty(Index st) {
			Index i = st;
			prev_pos(i);
			while ((data[i] == deleted_value()) && (del_ct > 0)) {
				data[i] = empty_value();
				free_ct += 1;
				del_ct -= 1;
				prev_pos(i);
			}
		}

		ElemPtr data;
		Index len;
		Index & busy_ct, & free_ct, & del_ct;
	};









}; // namespace ArrayLib
}; // namespace Arca


