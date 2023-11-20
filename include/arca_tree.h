
// Arca Tree
//

namespace Arca {
	// Arca

namespace Tree {
	// Arca::Tree



	using Index = uint16_t;
//	using Code = uint32_t;
//	using VoidP = void *;


	template <typename Key, Index N>
	struct TreeSetFn {

	// type definitions

	using VoidP = void *;
	using Code = uint32_t;
	using Index = uint32_t;

	struct DataBlock;
	struct IndexBlock;




	// top level functions

	void reset() {
		top = nullptr;
	}

	Bool has_key(Key key) {
		VoidP p; Index pos; Bool match_flag;
		setup_search_from_top(p, pos, match_flag);
		search_down(key, p, pos, match_flag);
		return match_flag;
	}

	Bool add_key(Key key) {
		VoidP p; Index pos; Bool match_flag;
		setup_search_from_top(p, pos, match_flag);
		search_down(key, p, pos, match_flag);
		if (match_flag)
			return 0;

		if (is_null(p))
			add_first_data_block(key);
		else
			add_key_to_data_block(key, p, pos);
		return 1;
	}


/*
	Bool del_key(Key key) {
		VoidP p; Index pos; Bool match_flag;
		setup_search_from_top(p, pos, match_flag);
		search_down(key, p, pos, match_flag);
		if (match_flag == 0)
			return 0;

		if (is_index_block(p))
			del_key_from_index(key, p, pos);
		else
			del_key_from_data_block(key, p, pos);
		return 1;
	}
*/

	template <typename F>
	void each_key(F fn) {
		each_key_recursive<F>(fn, top);
	}

	template <typename F>
	void each_key_recursive(F fn, VoidP p) {
		//PRINT_HEX((Reg64) p); PRINT_LN;
		if (is_null(p))
			return;
		if (is_data_block(p)) {
			DataBlock & data = to_data_block(p);
			Index n = data.count;
			for (Index i = 0; i != n; i += 1)
				fn(data.key[i]);
		}
		if (is_index_block(p)) {
			IndexBlock & idx = to_index_block(p);
			Index n = idx.count;

			for (Index i = 0; i != n; i += 1) {
				each_key_recursive(fn, idx.ptr[i]);
				fn(idx.key[i]);
			}
			each_key_recursive(fn, idx.ptr[n]);

		}
	}


	Reg64 elem_count() {
		Reg64 c = 0;
		auto f = [&c](Key k){c += 1;};
		each_key(f);
		return c;
	}

	// search functions

	void setup_search_from_top(VoidP & p, Index & pos, Bool & match_flag) {
		p = top;
		match_flag = 0;
		pos = 0;
	}

	static void search_down(Key key, VoidP & p, Index & pos, Bool & match_flag) {
		match_flag = 0;
		while (is_nn(p) && is_index_block(p)) {
			IndexBlock & idx = to_index_block(p);
			pos = idx.scan(key);
			match_flag = idx.check_match(pos, key);
			if (match_flag)
				return;
			p = idx.ptr[pos];
		}
		if (is_nn(p) && is_data_block(p)) {
			DataBlock & data = to_data_block(p);
			pos = data.scan(key);
			match_flag = data.check_match(pos, key);
			return;
		}
	}

	static Index scan_for_key(Key * key, Index n, Key k) {
		Index pos = n;
		Index step = n >> 1;
		while (step > 1) {
			if (key[pos - step - 1] > k )
				pos -= step;
			step >>= 1;
		}
		while (pos > 0) {
			if (key[pos - 1] < k)
				break;
			pos -= 1;
		}
		assert(pos <= n);
		return pos;
	}


	static void search_for_prev_key_from_index(VoidP & p, Index & pos) {
		p = to_index_block(p).ptr[pos];
		while (is_nn(p) && is_index_block(p)) {
			IndexBlock & idx = to_index_block(p);
			Index n = idx.count;
			p = idx.ptr[n + 1];
		}
		DataBlock & data = to_data_block(p);
		Index n = data.count;
		pos = n - 1;
	}


	static Index pos_in_parent(IndexBlock & parent, DataBlock & data) {
		assert(data.count > 0);
		Key k = data.key[0];
		Index pos = parent.scan(k);
		assert(parent.ptr[pos] == to_voidp(&data));
		return pos;
	}
	static Index pos_in_parent(IndexBlock & parent, IndexBlock & idx) {
		assert(idx.count > 0);
		Key k = idx.key[0];
		Index pos = parent.scan(k);
		assert(parent.ptr[pos] == to_voidp(&idx));
		return pos;
	}
	// add_key helpers

	void add_first_data_block(Key key) {
		DataBlock & data = req_data_block();
		data = DataBlock(key);
		assert(data.parent == nullptr);
		top = to_voidp(&data);
	}

	void add_key_to_data_block(Key key, VoidP & p, Index & pos) {
		DataBlock & data = to_data_block(p);

		if ((data.count == N) && add_try_full_slide(data, key))
			return;
		if (data.count == N) {
			DataBlock & ndata = req_data_block();
			Key pivot = add_to_split_data_block(data, ndata, key);
			add_index_walkup(pivot, data, ndata);
		} else
			add_elem_to_data_block(data, pos, key);
	}

/*
	static Key add_key_split_data(DataBlock & data, DataBlock & ndata, Key key) {
		ndata.reset();
		Index n = data.count;
		Index c = (n - 1) / 2;
		Index i = n - c;
		ndata = DataBlock(data.key + i, c);
		Key pivot = data.key[i - 1];
		data.remove_last(c + 1);
		DataBlock * datap = &data;
		if (key > pivot)
			datap = &ndata;
		Index pos = (*datap).scan(key);
		(*datap).add_key(pos, key);
	}
*/
	void add_index_walkup(Key nkey, DataBlock & data, DataBlock & ndata) {
		VoidP p = to_voidp(data.parent);
		VoidP q = to_voidp(&data);
		VoidP s = to_voidp(&ndata);
		Key k = nkey;

		while (is_nn(p) && is_index_block(p)) {
			WATCH("nkey",nkey);
			PRINT_STR("p = ");
			PRINT_HEX((Reg64) p);
			PRINT_LN;
			PRINT_STR("q = ");
			PRINT_HEX((Reg64) q);
			PRINT_LN;
			PRINT_STR("s = ");
			PRINT_HEX((Reg64) s);
			PRINT_LN;

			IndexBlock & idx = to_index_block(p);

			if ((idx.count == N) && (add_try_full_slide(idx, nkey, s)==1))
				return;
			if (idx.count < N) {
				//TRACE;
				add_entry_to_index(idx, k, s);
				return;
			} else {
				TRACE;
				IndexBlock & nidx = req_index_block();
				Key pivot = add_entry_split_index(idx, nidx, k, s);
				WATCH("pivot", pivot);
				q = p; p = to_voidp(idx.parent);
				k = pivot; s = to_voidp(&nidx);
				idx.print();
				nidx.print();
			}
		}
		add_key_create_new_index_block(k, q, s);
	}


	static Key add_entry_split_index(IndexBlock & idx, IndexBlock & nidx, Key k, VoidP s) {
		nidx.reset();
		Index n = idx.count;
		Index c = (n - 1) / 2;
		Index i = n - c;
		Key pivot = idx.key[i - 1];
		Index j = 0;
		while (j < c)
			nidx.key[j++] = idx.key[i++];
		i = n - c;
		j = 0;
		while (j < c + 1) {
			nidx.ptr[j++] = idx.ptr[i++];
		}
		idx.count = n - c - 1;
		nidx.count = c;
		reset_parent_links(nidx);
		add_with_pivot(idx, nidx, pivot, k, s);
		return pivot;
	}


	static void add_elem_to_data_block(DataBlock & data, Index pos, Key k) {
		Index len = data.count;
		assert(len != N);  assert(pos <= len);
		for (Index i = len; i > pos; i -= 1)
			data.key[i] = data.key[i - 1];
		data.key[pos] = k;
		data.count += 1;
	}

	static void add_elem_to_data_block(DataBlock & data, Key k) {
		Index pos = data.scan(k);
		add_elem_to_data_block(data, pos, k);
	}

	static void add_with_pivot(DataBlock & left, DataBlock & right, Key pivot, Key k) {
		DataBlock * datap = nullptr;
		if (k < pivot)
			datap = &left;
		else
			datap = &right;
		add_elem_to_data_block(*datap, k);
	}

	static void add_with_pivot(IndexBlock & left, IndexBlock & right, Key pivot, Key k, VoidP s) {
		IndexBlock * indexp = nullptr;
		if (k < pivot)
			indexp = &left;
		else
			indexp = &right;
		add_entry_to_index(*indexp, k, s);
	}

	static Key add_to_split_data_block(DataBlock & data, DataBlock & ndata, Key k) {
		ndata.reset();
		Index n = data.count;
		Index c = (n - 1) / 2;
		Index i = n - c;
		Key pivot = data.key[i - 1];
		Index j = 0;
		while (i < n)
			ndata.key[j++] = data.key[i++];
		data.count = n - c - 1;
		ndata.count = c;
		add_with_pivot(data, ndata, pivot, k);
		return pivot;
	}

	static void add_entry_to_index(IndexBlock & idx, Key k, VoidP s) {
		assert(idx.count != N);  // assert(pos <= idx.count);
		//WATCH("k", k);
		//WATCH("idx.count",idx.count);
		Index pos = idx.scan(k);
		//WATCH("pos",pos);
		for (Index i = idx.count; i > pos; i -= 1) {
			idx.key[i] = idx.key[i - 1];
			idx.ptr[i + 1] = idx.ptr[i];
		}
		idx.key[pos] = k;
		idx.ptr[pos + 1] = s;
		idx.count += 1;
		set_parent(idx, s);
	}

	void add_key_create_new_index_block(Key k, VoidP q, VoidP s) {
		IndexBlock & idx = req_index_block();
		idx.key[0] = k;
		idx.ptr[0] = q;
		idx.ptr[1] = s;
		idx.count = 1;
		set_parent(idx, q);
		set_parent(idx, s);
		top = to_voidp(&idx);
		PRINT_STR("new index block\n");
		idx.print();
	}

	Bool add_try_full_slide(DataBlock & data, Key key) {
		if (is_null(data.parent))
			return 0;
		IndexBlock & parent = *(data.parent);
		Index pos = pos_in_parent(parent, data);
		if (pos > 0) {
			DataBlock & left = to_data_block(parent.ptr[pos - 1]);
			Key & div_left = parent.key[pos - 1];
			if (check_high_slide_data_avail(data, left)==1) {
				slide_data_hi_left(data, left, div_left);
				add_with_pivot(left, data, div_left, key);
				return 1;
			}
		}
		if (pos < parent.count) {
			DataBlock & right = to_data_block(parent.ptr[pos + 1]);
			Key & div_right = parent.key[pos];
			if (check_high_slide_data_avail(data, right)==1) {
				slide_data_hi_right(data, right, div_right);
				add_with_pivot(data, right, div_right, key);
				return 1;
			}
		}
		return 0;
	}

	Bool add_try_full_slide(IndexBlock & idx, Key key, VoidP s) {
		if (is_null(idx.parent))
			return 0;
		TRACE;
		IndexBlock & parent = *(idx.parent);
		Index pos = pos_in_parent(parent, idx);
		if (pos > 0) {
			IndexBlock & left = to_index_block(parent.ptr[pos - 1]);
			Key & div_left = parent.key[pos - 1];
			if (check_high_slide_index_avail(idx, left)==1) {
				slide_index_hi_left(idx, left, div_left);
				add_with_pivot(left, idx, div_left, key, s);
				reset_parent_links(left);
				reset_parent_links(idx);
				return 1;
			}
		}
		if (pos < parent.count) {
			IndexBlock & right = to_index_block(parent.ptr[pos + 1]);
			Key & div_right = parent.key[pos];
			if (check_high_slide_index_avail(idx, right)==1) {
				slide_index_hi_right(idx, right, div_right);
				add_with_pivot(idx, right, div_right, key, s);
				reset_parent_links(right);
				reset_parent_links(idx);
				return 1;
			}
		}
		return 0;
	}

	// slide functions and helpers

	static Bool check_high_slide_data_avail(DataBlock & data, DataBlock & buddy) {
		return (data.count >= buddy.count + 2 * calc_min_slide_count());
	}

	static Bool check_high_slide_index_avail(IndexBlock & idx, IndexBlock & buddy) {
		return (idx.count >= buddy.count + 2 * calc_min_slide_count());
	}

	static void slide_data_hi_left(DataBlock & data, DataBlock & left, Key & div_left) {
		Index n1 = left.count;
		Index n2 = data.count;
		assert(n2 > n1);
		Index c = (n2 - n1) >> 1;
		Index d = c - 1;
		Index i = n1;
		left.key[i++] = div_left;
		Index j = 0;
		while (j < d)
			left.key[i++] = data.key[j++];
		div_left = data.key[j++];
		i = 0;
		while (j < n2)
			data.key[i++] = data.key[j++];
		left.count += c;
		data.count -= c;
	}

	static void slide_data_hi_right(DataBlock & data, DataBlock & right, Key & div_right) {
		Index n1 = data.count;
		Index n2 = right.count;
		assert(n1 > n2);
		Index c = (n1 - n2) >> 1;
		Index d = c - 1;
		Index i = n2 + c;
		Index j = n2;
		while (j > 0) {
			right.key[i - 1] = right.key[j - 1];
			i -= 1; j -= 1;
		}
		right.key[i - 1] = div_right;
		i -= 1;
		j = n1;
		while (i > 0) {
			right.key[i - 1] = data.key[j - 1];
			i -= 1; j -= 1;
		}
		div_right = data.key[j - 1];
		j -= 1;
		right.count += c;
		data.count -= c;
	}

	static void slide_index_hi_left(IndexBlock & idx, IndexBlock & left, Key & div_left) {
		//assert(idx.parent != nullptr);
		Index n1 = left.count;
		Index n2 = idx.count;
		assert(n2 > n1);
		Index c = (n2 - n1) >> 1;
		Index d = c - 1;
		Index i = n1;
		left.key[i++] = div_left;
		Index j = 0;
		while (i < n1 + c)
			left.key[i++] = idx.key[j++];
		div_left = idx.key[j++];
		i = 0;
		while (j < n2)
			idx.key[i++] = idx.key[j++];
		i = n1 + 1;
		j = 0;
		while (j < c)
			left.ptr[i++] = idx.ptr[j++];
		i = 0;
		while (j < n2 + 1)
			idx.ptr[i++] = idx.ptr[j++];


		left.count += c;
		idx.count -= c;
	}


	static void slide_index_hi_right(IndexBlock & idx, IndexBlock & right, Key & div_right) {
		//assert(idx.parent != nullptr);

		Index n1 = idx.count;
		Index n2 = right.count;

		Index c = (n1 - n2) >> 1;
		Index d = c - 1;
		Index i = n2 + c;
		Index j = n2;
		while ((j > 0)) {
			right.key[i - 1] = right.key[j - 1];
			i -= 1; j -= 1;
		}
		right.key[i - 1] = div_right; i -= 1;
		j = n1;
		while (i > 0) {
			right.key[i - 1] = idx.key[j - 1];
			i -= 1; j -= 1;
		}
		div_right = idx.key[j - 1];
		j = n2 + 1 + c;
		i = n2 + 1;
		while (i > 0) {
			right.ptr[j - 1] = right.ptr[i - 1];
			i -= 1; j -= 1;
		}
		i = n1 + 1;
		while (j > 0) {
			right.ptr[j - 1] = idx.ptr[i - 1];
			i -= 1; j -= 1;
		}
		right.count += c;
		idx.count -= c;
	}



	// codes for datablocks, indexblocks, etc

	static Code live_index_block_code() { return 0x21426384;}
	static Code live_data_block_code() { return 0x2c4266a4;}

	static Code deleted_index_block_code() { return 0x21926cc4;}
	static Code deleted_data_block_code() { return 0x3c4226a1;}






		// common functions

		// Code deleted_data_block_code() { return 0x5142f314;}
		// Code deleted_data_index_code() { return 0x236763d4;}

		// block structs

		struct IndexBlock {
			Code code;
			Index count;
			IndexBlock * parent;
			Key key[N];
			VoidP ptr[N+1];

			void reset(Index c) {
				code = live_index_block_code();
				parent = nullptr;
				count = c;
			}

			void reset() {
				count = 0;
				code = live_index_block_code();
				parent = nullptr;
			}

			IndexBlock() { reset(0); }
/*
			IndexBlock(Key k, VoidP x, VoidP y) {
				assert(x != nullptr); assert(y != nullptr);
				reset(1);
				key[0] = k;
				ptr[0] = x;
				ptr[1] = y;
			}
*/
			IndexBlock(Key * key_buf, VoidP * ptr_buf, Index ct) {
				assert(ct <= N);
				reset(ct);
				for (Index i = 0; i < ct; i += 1) {
					key[i] = key_buf[i];
					ptr[i] = ptr_buf[i];
				}
				ptr[ct] = ptr_buf[ct];
			}

			Index scan(Key k) {
				return scan_for_key(key, count, k);
/*
				Index pos = count;
				Index step = count >> 1;
				while (step > 1) {
					if (key[pos - step - 1] > k )
						pos -= step;
					step >>= 1;
				}
				while (pos > 0) {
					if (key[pos - 1] < k)
						break;
					pos -= 1;
				}
				assert(pos <= count);
				return pos;
*/

			}

			Bool check_match(Index pos, Key k) {
				return (key[pos] == k);
			}

			void append(Key k, VoidP p) {
				key[count] = k;
				ptr[count + 1] = p;
				count += 1;
			}
/*
			void insert(Index pos, Key k, VoidP p) {
				assert(count != N);  assert(pos <= count);
				for (Index i = count; i > pos; i -= 1) {
					key[i] = key[i - 1];
					ptr[i + 1] = ptr[i];
				}
				key[pos] = k;
				ptr[pos + 1] = p;
				count += 1;
			}

			void remove(Index pos) {
				assert(count != 0); assert(pos < count);
				Index i = pos;
				while (i < count - 1) {
					key[i] = key[i + 1];
					ptr[i + 1] = ptr[i + 2];
					i += 1;
				}
				count -= 1;
			}


			void append(Key * key_buf, VoidP * ptr_buf, Index c, Key div_key) {
				assert(count + c <= N);  assert(c >= 2);
				Index d = c - 1;
				Index i = count;
				key[i] = div_key;
				ptr[i + 1] = ptr_buf[0];
				i += 1;
				Index j = 0;
				while (j < d) {
					key[i] = key_buf[j];
					ptr[i + 1] = ptr_buf[j + 1];
					i += 1; j += 1;
				}
				count += c;
			}

			void prepend(Key * key_buf, VoidP * ptr_buf, Index c, Key div_key) {
				assert(count + c <= N); assert(c >= 2);
				Index n = count;
				Index i = c + n;
				ptr[i] = ptr[i - c];
				while (i > c) {
					key[i - 1] = key[i - c - 1];
					ptr[i - 1] = ptr[i - c - 1];
					i -= 1;
				}
				key[i - 1] = div_key;
				ptr[i - 1] = ptr_buf[c - 1];
				i -= 1;
				while (i > 0) {
					key[i - 1] = key_buf[i - 1];
					ptr[i - 1] = ptr_buf[i - 1];
					i -= 1;
				}
				count += c;
			}


			void remove_first(Index c) {
				assert(c < count);
				Index n = count;
				Index i = 0;
				while (i < n) {
					key[i] = key[i + c];
					ptr[i] = ptr[i + c];
					i += 1;
				}
				ptr[i] = ptr[i + c];
				count -= c;
			}

			void remove_last(Index c) {
				assert(c < count);
				count -= c;
			}

*/
			Index ptr_scan(VoidP p) {
				Index n = count;
				for (Index i = 0; i != n; i += 1) {
					if (ptr[i] == p)
						return i;
				}
				return n;
			}




#ifdef ARCA_TESTER

			void print() {
				PRINT_STR("Indexblock address: ");
				PRINT_HEX((Reg64) this);PRINT_LN;
				PRINT_STR("parent: ");
				PRINT_HEX((Reg64) parent); PRINT_LN;

				PRINT_STR("count: ");
				PRINT_VAL(count); PRINT_LN;

				PRINT_STR("keys: "); PRINT_LN;

				for (Index i = 0; i != count; i += 1) {
					PRINT_VAL(i);
					PRINT_STR(":");
					PRINT_VAL(key[i]);
					PRINT_STR(" ");
				}
				PRINT_LN;

				PRINT_STR("ptrs: ");
				PRINT_LN;

				for (Index i = 0; i != count + 1; i += 1) {
					PRINT_VAL(i);
					PRINT_STR(":");
					PRINT_HEX((Reg64) ptr[i]);
					PRINT_STR(" ");
				}
				PRINT_LN;

			}

#endif

		};

		struct DataBlock {
			Code code;
			Index count;
			IndexBlock * parent;
			Key key[N];

			void reset(Index c) {
				code = live_data_block_code();
				parent = nullptr;
				count = c;
			}

			void reset() {
				code = live_data_block_code();
				parent = nullptr;
				count = 0;
			}

			DataBlock() { reset(0); }

			DataBlock(Key k) {
				reset(1);
				key[0] = k;
			}

			DataBlock(Key * key_buf, Index ct) {
				assert(ct <= N);
				reset(ct);
				for (Index i = 0; i < ct; i += 1) {
					key[i] = key_buf[i];
				}
			}

			Index scan(Key k) {
				return scan_for_key(key, count, k);
			}

			Bool check_match(Index pos, Key k) {
				return (key[pos] == k);
			}

			void append(Key k) {
				key[count++] = k;
			}

/*
			void insert(Index pos, Key k) {
				assert(count != N);  assert(pos <= count);
				for (Index i = count; i > pos; i -= 1)
					key[i] = key[i - 1];
				key[pos] = k;
				count += 1;
			}

			void remove(Index pos) {
				assert(count != 0); assert(pos < count);
				Index i = pos;
				while (i < count - 1) {
					key[i] = key[i + 1];
					i += 1;
				}
				count -= 1;
			}


			void append(Key * key_buf, Index c, Key div_key) {
				assert(count + c <= N);  assert(c >= 2);
				Index d = c - 1;
				Index i = count;
				key[i] = div_key;
				i += 1;
				Index j = 0;
				while (j < d) {
					key[i] = key_buf[j];
					i += 1; j += 1;
				}
				count += c;
			}

			void prepend(Key * key_buf, Index c, Key div_key) {
				assert(count + c <= N); assert(c >= 2);
				Index n = count;
				Index i = c + n;
				while (i > c) {
					key[i - 1] = key[i - c - 1];
					i -= 1;
				}
				key[i - 1] = div_key;
				i -= 1;
				while (i > 0) {
					key[i - 1] = key_buf[i - 1];
					i -= 1;
				}
				count += c;
			}

			void remove_first(Index c) {
				assert(c < count);
				Index n = count;
				Index i = 0;
				while (i < n) {
					key[i] = key[i + c];
					i += 1;
				}
				count -= c;
			}

			void remove_last(Index c) {
				assert(c < count);
				count -= c;
			}
*/
#ifdef ARCA_TESTER

			void print() {
				PRINT_STR("Datablock address: ");
				PRINT_HEX((Reg64) this);PRINT_LN;
				PRINT_STR("parent: ");
				PRINT_HEX((Reg64) parent); PRINT_LN;
				PRINT_STR("count: ");
				PRINT_VAL(count); PRINT_LN;
				PRINT_STR("keys: "); PRINT_LN;
				for (Index i = 0; i != count; i += 1) {
					PRINT_VAL(i);
					PRINT_STR(":");
					PRINT_VAL(key[i]);
					PRINT_STR(" ");
				}
				PRINT_LN;
			}
#endif


		};

	// type checking / casting and other utility functions

	static Bool is_null(VoidP p) { return p == nullptr;}
	static Bool is_nn(VoidP p) { return p != nullptr;}

	static Bool is_null(IndexBlock * p) { return p == nullptr;}
	static Bool is_nn(IndexBlock * p) { return p != nullptr;}

	static DataBlock & to_data_block(VoidP p) {
		assert(p != nullptr);
		return *(static_cast<DataBlock *>(p));
	}

	static IndexBlock & to_index_block(VoidP p) {
		assert(p != nullptr);
		return *(static_cast<IndexBlock *>(p));
	}

	static VoidP to_voidp(DataBlock * p) { return static_cast<VoidP>(p);}
	static VoidP to_voidp(IndexBlock * p) { return static_cast<VoidP>(p);}

	static Bool is_data_block(VoidP p) {
		return ((p != nullptr) && (to_data_block(p).code == live_data_block_code()));
	}

	static Bool is_index_block(VoidP p) {
		return ((p != nullptr) && (to_index_block(p).code == live_index_block_code()));
	}

	static DataBlock & req_data_block() {
		return *(new DataBlock());
	}

	static IndexBlock & req_index_block() {
		return *(new IndexBlock());
	}

	static void rel_data_block(DataBlock * p) { delete p;}
	static void rel_index_block(IndexBlock * p) { delete p;}

	// structural functions

/*
	VoidP restruct_full_block(DataBlock & data, VoidP & top) {
		assert(data.count > N / 2); assert(data.parent != nullptr);
		if (is_null(data.parent)) {
			// WATCH("NOT IMPLEMENTED", 0);
		}
		IndexBlock & parent = *(data.parent);
		Index pos = get_pos_in_parent(data);
		Index min_slide_ct = calc_min_slide_count();
		if (pos > 0) {
			DataBlock & left = to_data_block(parent.ptr[pos - 1]);
			if (left.count + 2 * min_slide_ct <= data.count)
				return restruct_data_hi_slide_left(left, data, pos);
		}
		if (pos < parent.count) {
			DataBlock & right = to_data_block(parent.ptr[pos - 1]);
			if (right.count + 2 * min_slide_ct <= data.count)
				return restruct_data_hi_slide_right(data, right, pos);
		}
		return restruct_split(data, parent, top);
	}

	static VoidP restruct_data_hi_slide_left(DataBlock & left, DataBlock & data, Index pos) {
		IndexBlock & parent = *(data.parent);
 		Key & div_left = parent.key[pos - 1];
		Index c = (data.count - left.count) / 2;
		assert (c >= 2); assert (c < data.count);
		left.append(data.key, c, div_left);
		div_left = data.key[c - 1];

		data.remove_first(c);
		//left.count += c; data.count -= c;
		return to_voidp(&parent);
	}

	static VoidP restruct_data_hi_slide_right(DataBlock & data, DataBlock & right, Index pos) {
		IndexBlock & parent = *(data.parent);
 		Key & div_right = parent.key[pos];
		Index c = (data.count - right.count) / 2;
		assert (c >= 2); assert (c < data.count);
		Index i = data.count - c + 1;
		right.prepend(data.key + i, c, div_right);
		div_right = data.key[i - 1];
		data.remove_last(c);
		return to_voidp(&parent);
	}


	static VoidP restruct_split(DataBlock & data, IndexBlock & parent, VoidP & top) {
		Index n = data.count;
		Index c = (n - 1) / 2;
		Index i = data.count - c;
		DataBlock & ndata = req_data_block();

		ndata = DataBlock(data + i, c);
		Key pivot = data.key[i - 1];
		data.remove_last(c + 1);

		VoidP p = to_voidp(&parent);
		VoidP q = to_voidp(&data);
		VoidP s = to_voidp(&ndata);
		Key k = pivot;
		return restruct_add_index_walkup(p, q, s, k, top);
	}

	static VoidP restruct_add_index_walkup(VoidP & p, VoidP & q, VoidP & s, Key & pivot, VoidP & top) {
		while (is_index_block(p)) {
			IndexBlock & parent = to_index_block(p);
			if (parent.count == N) {
				if (restruct_is_index_slide_avail()==1)
					return
				if (restruct_try_index_slide()==1);
			}
			if ((parent.count == N) && (restruct_try_hi_slide())
			if (parent_count < N)
				return restruct_add_index_entry();
			if (parent.count < N) {
				Index pos = parent.scan(k);
				parent.insert(k, s);
				set_parent(parent, s);
				return to_voidp(&parent);
			}
			if



		}


	}
*/
	// helpers for structurals

	/*
	static Index get_pos_in_parent(DataBlock & data) {
		if (data.count > 0) {
			Key k = data.key[0];
			IndexBlock & parent = *(data.parent);
			return parent.scan(k);
		} else {
			IndexBlock & parent = *(data.parent);
			VoidP p = to_voidp(&data);
			return parent.ptr_scan(p);
		}
	}

	static Index get_pos_in_parent(IndexBlock & idx) {
		if (idx.count > 0) {
			Key k = data.key[0];
			IndexBlock & parent = *(idx.parent);
			return parent.scan(k);
		} else {
			IndexBlock & parent = *(idx.parent);
			VoidP p = to_voidp(&idx);
			return parent.ptr_scan(p);
		}
	}


	Index neighbor_data_with_lowest_count(IndexBlock & parent, Index pos) {
		if (pos == 0)
			return pos + 1;
		if (pos == parent.count)
			return pos - 1;
		DataBlock & left = to_data_block(parent.ptr[pos - 1]);
		DataBlock & right = to_data_block(parent.ptr[pos + 1]);
		if (left.count < right.count)
			return pos - 1;
		else
			return pos + 1;
	}
*/
/*
	static void check_for_neighbors(IndexBlock & parent, Index pos, Bool & has_left, Bool & has_right) {
		has_left = (pos > 0) ? 1 : 0;
		has_right = (pos < parent.count) ? 0 : 1;
	}
*/

	static void set_parent(IndexBlock & parent, VoidP p) {
		assert(p != nullptr);
		if (is_data_block(p))
			to_data_block(p).parent = &parent;
		else
			to_index_block(p).parent = &parent;
	}

	static void reset_parent_links(IndexBlock & parent) {
		Index n = parent.count;
		for (Index i = 0; i != n; i += 1)
			set_parent(parent, parent.ptr[i]);
		set_parent(parent, parent.ptr[n]);
	}

//	DataBlock & get_data_block_from_parent(IndexBlock & parent, Index pos) {
//		DataBlock & data = to_data_block(parent.ptr[pos])
//	}

	static Index calc_min_slide_count() {
		if (N < 12)
			return 2;
		else
			return (N / 4);
	}


	// search functions and helpers

	// del key helpers

	/*
	void del_key_from_index(VoidP & p, Index & pos) {
		IndexBlock & idx = to_index_block(p);
		Key & index_key = idx.key[pos];
		search_prev_key_from_index(p, pos);
		DataBlock & data = to_data_block(p);
		index_key = data.pop_last();
		if (data.count < N / 2 - 1)
			del_key_restruct_short_block(p);
	}

	void del_key_from_data_block(VoidP & p, Index & pos) {
		DataBlock & data = to_data_block(p);
		data.remove(pos);
		if (data.count < N / 2 - 1)
			del_key_restruct_short_block(data);
	}


	void del_key_restruct_short_block(DataBlock & data) {
		if (data.parent == nullptr) {
			if (data.count == 0) {
				rel_data_block(data);
				top = nullptr;
			}
			return;
		}

		try_short_block_slide(data);
		if (data.count >= N / 2 - 1)
			return;
		Index pos = get_pos_in_parent(data);
		Index npos = del_key_hi_ct_neighbor(data.parent, pos) {

		}
		if ((pos > 0) &&
		{
			if (data)
		}


	{
			if (data.count > 0)
				return;
		}
	}
*/





#ifdef ARCA_TESTER
			void print() {
				PRINT_STR("Printing Tree:\n");
				uint64_t c = 0;
				auto f = [&c](Key k){ PRINT_VAL(k); PRINT_STR(":"); c += 1;};
				each_key(f);
				PRINT_LN;
				PRINT_STR("count = ");
				PRINT_VAL(c);
				PRINT_LN;
			}

			void dump() {
				if (top != nullptr)
					dump_recursive(top);
			}

			void dump_recursive(VoidP p) {
				if (is_index_block(p)) {
					IndexBlock & idx = to_index_block(p);
					idx.print();
					for (Index i = 0; i <= idx.count; i += 1) {
						dump_recursive(idx.ptr[i]);
					}
				}
				if (is_data_block(p)) {
					DataBlock & data = to_data_block(p);
					data.print();
				}
			}

			static uint32_t ror32(uint32_t x, uint8_t y) {
				return ((x >> y) | (x << (32 - y)));
			}

			Key hash_contents() {
				uint32_t h = 0;
				auto f = [&h](Key k){
					h += (uint32_t) k;
					h ^= ror32(h, 3);
					h += ror32(h, 8);
					h ^= ror32(h, 11);
				};
				each(f);
				return h;
			}


#endif


	TreeSetFn(VoidP & p) : top(p) {};

	VoidP & top;


	};
};




}; // namespace Arca




