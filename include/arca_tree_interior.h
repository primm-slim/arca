
// Arca Tree
//

namespace Arca {
	// Arca

namespace Tree {
	// Arca::Tree

	using Index = uint16_t;
	using Code = uint32_t;

	Code index_block_code() { return 0x21426384;}
	Code data_block_code() { return 0x32537496;}

	template <typename Key, uint16_t N>
	Index key_scan_fwd(Key k, Key * key, Index ct) {
		Index i = 0;
		Index step = count >> 1;
		while (step > 1) {
			if (key[i + step] < k)
				i += step;
			step >>= 1;
		}
		while (i != ct) {
			if (key[i] >= k)
				break;
			i += 1;
		}
		return i;
	}

	template <typename Key, uint16_t N>
	Index key_scan_back(Key k, Key * key, Index ct) {
		Index i = ct;
		Index step = ct / 2;
		while (step > 1) {

			if (key[i - step - 1] > k)
				i -= step;
			step >>= 1;
		}
		while (i > 0) {

			if (key[i - 1] <= k)
				break;
			i -= 1;
		}
		return i;
	}

	template <typename Elem>
	void shl(Elem * buf, Index len, Index ct) {
		Elem * p = buf;
		Elem * e = buf + len;
		Elem * q = buf - ct;
		while (p != e)
			*q++ = *p++;
	}

	template <typename Elem>
	void shr(Elem * buf, Index len, Index ct) {
		Elem * p = buf + len;
		Elem * e = buf;
		Elem * q = buf + len + ct;
		while (p != e) {
			*(q-1) = *(p-1);
			--p; --q;
		}

	}

	template <typename Elem>
	void copy_to(Elem * src, Elem * dest, Index len) {
		Elem * p = dest;
		Elem * e = p + len;
		Elem * q = src;
		while (p != e)
			*p++ = *q++;
	}



	/*
	template <typename Key, uint8_t N>
	struct KeyFn {
		KeyFn(Key * buf, Index & ct) :
			key(buf), count(ct) {}

		Key * key;
		Index & ct;

		Index scan_fwd(Key k) {
			Index i = 0;
			Index step = count >> 1;
			while (step >= 2) {
				if (key[i + step] < k)
					i += step;
				step >>= 1;
			}
			while (i < count) {
				if (key[i] >= k)
					break;
				i += 1;
			}
			return i;
		}

		Index scan_rev(Key k) {
			Index i = count;
			Index step = count >> 1;
			while (step >= 2) {
				if (key[i - step - 1] > k)
					i -= step;
				step >>= 1;
			}
			while (i > 0) {
				if (key[i] < k)
					break
				i -= 1;
			}
			return i;
		}

		Bool check_for_match(Index i, Key k) {
			if ((i == count) || (key[i] != k))
				return 0;
			else
				return 1;
		}

		Key first_key() { return key[0];}
		Key last_key() { return key[count - 1];}

		Index avail() { return N - count;}
		Bool empty() { return count == 0;}
		Bool full() { return count == N;}
	};


	template <typename Key>
	Bool block_has_key(Key * key, Index count, Key k) {
		Index i = scan_fwd(key, count, k);
		if ((i == count) || (key[i] != k))
			return 0;
		else
			return 1;
	}

	template <typename Key, typename F>
	void block_insert(Key * key, Index count, Key k, F f) {
		Index i = count;
		while (i > 0) {
			if (key[i - 1] < k)
				break;
			f(i); ++i;
		}
		return i;
	}

	template <typename Key, typename F>
	void block_remove(Key * key, Index count, Key k, F f) {
		Index i = scan_fwd(key, count, k);
		while (i < count - 1) {
			f(i); i += 1;
		}
	}

	template <typename Elem>
	void block_split_to(Elem * dest, Elem * src, Index count, Index ct) {
		Index i = count - ct;
		Index j = 0;
		while (i != count) {
			dest[j++] = src[i++];
		}
	}
	template <typename Key, typename F>
	void block_split_to(Key * key2, Index ct) {
		Index i = count - ct;
		Index j = 0;
		while (i != count) {
			key2[j++] = key[i++]
		}
		count -= ct;
	}


*/


	template <typename Key, typename Ref, uint8_t N>
	struct KeyBlock {
		Code code;
		Index count;
		Ref parent;

		Key key[N];

		KeyBlock() { code = data_block_code(); count = 0; }

		Index avail() { return N - count;}

		Index scan_fwd(Key k) { return key_scan_fwd<Key, N>(k, key, count);}
		//Index scan_back(Key k) { return key_scan_back<Key, N>(k, key, count);}

		Bool has_key(Key k) {
			Index i = scan_fwd(k);
			//WATCH("i",i);
			if ((i != count) && (key[i] == k))
				return 1;
			else
				return 0;
		}

		void append(Key k) {
			key[count++] = k;
		}

		void insert(Key k) {
			Index i = scan_fwd(k);
			if (i != count)
				shr<Key>(key + i, count - i, 1);
			key[i] = k;
			count += 1;
		}

		Key pop_last() {
			return key[count--];
		}

		void remove(Key k) {
			Index i = scan_fwd(k);
			if (i != count - 1)
				shl<Key>(key + i + 1, count - i - 1, 1);
			count -= 1;
		}

		Key split_to(Key * buf, Index ct) {
			Index i = count - ct - 1;
			Key k = key[i]; i += 1;
			copy_to<Key>(key + i, buf, ct);
			count -= (ct + 1);
			return k;
		}

		void merge_from_right(Key * buf, Index ct, Key k) {
			Index i = count;
			key[i++] = k;
			copy_to<Key>(key + i, buf, ct);
			count += 1; count += ct;
		}

		Key partial_merge_fix(Index ct) {
			Key k = key[ct];
			shl<Key>(key + ct + 1, count - ct - 1, ct + 1);
			count -= (ct + 1);
			return k;
		}

	};



	template <typename Key, typename Ref, uint8_t N>
	struct KeyRefBlock {
		Code code;
		Index count;
		Ref parent;

		Key key[N];
		Ref ref[N+1];

		KeyRefBlock() { code = index_block_code(); count = 0;}

		Index scan_fwd(Key k) { return key_scan_fwd<Key, N>(k, key, count);}
		Index scan_back(Key k) { return key_scan_back<Key, N>(k, key, count);}

		Index avail() { return N - count;}

		Ref scan_for_ref(Index k, Bool & match_flag) {
			Index i = scan_back(k);
			if ((i != count) && (key[i] == k))
				match_flag = 1;
			else
				match_flag = 0;
			return ref[i];
		}


		void create_first_entry(Key k, Ref p1, Ref p2) {
			ref[0] = p1;
			ref[1] = p2;
			key[0] = k;
			count = 1;
		}

		void add_entry(Key k, Ref p) {
			Index i = scan_fwd(k);
			if (i != count) {
				shr<Key>(key + i, count - i, 1);
				shr<Ref>(ref + i + 1, count - i, 1);
			}
			key[i] = k;
			ref[i + 1] = p;
			count += 1;
		}

		Key remove_entry(Ref p) {
			Index i = 0;
			for (i = 0; i != count; i += 1) {
				if (ref[i+1] == p)
					break;
			}
			while (i != count - 1) {
				key[i] = key[i + 1];
				ref[i + 1] = ref[i + 2];
			}
			count -= 1;
		}


		Bool check_for_match(Index i, Key k) {
			if (i == 0)
				return 0;
			if (key[i - 1] != k)
				return 0;
			else
				return 1;
		}

		Ref first_ref() { return ref[0];}
		Ref last_ref() { return ref[count];}

		Key first_key() { return key[0];}
		Key last_key() { return key[count - 1];}


		/*
		Bool has_key(Key k) {
			Index i = scan_back(k);

			for (Index i = 0; i != count; i += 1) {
				if (key[i] == k)
					return 1;
			}
			return 0;
		}
		*/


		Index scan_for_ref(Ref e) {
			Index i = 0;
			while (i < count + 1) {
				if (ref[i] == e)
					break;
				i += 1;
			}
			return i;
		}

		void insert_before(Index pos, Key k, Ref p) {
			Index i = count;
			while (i > pos) {
				key[i] = key[i - 1];
				ref[i + 1] = ref[i];
				i -= 1;
			}
			key[i] = k;
			ref[i+1] = p;
			count += 1;
		}

		void append(Key k, Ref p) {
			key[count] = k;
			ref[count+1] = p;
			count += 1;
		}

		void remove_at(Index pos) {
			Index i = pos;
			while (i < count - 1) {
				key[i] = key[i + 1];
				ref[i + 1] = ref[i + 2];
				i += 1;
			}
			count -= 1;
		}

		Key split_to(Key * key_buf, Ref * ref_buf, Index ct) {
			Index i = count - ct - 1;
			Key k = key[i++];
			Index j = 0;
			while (i < count) {
				key_buf[j] = key[i];
				ref_buf[j] = ref[i];
				++i; ++j;
			}
			ref_buf[j] = ref[i];
			count -= ct; count -= 1;
			return k;
		}

		void merge_from_right(Key * key_buf, Ref * ref_buf, Index ct, Key k) {
			Index i = count;
			key[i++] = k;
			Index j = 0;
			while (j < ct) {
				key[i] = key_buf[j];
				ref[i] = ref_buf[j];
				++i; ++j;
			}
			ref[i] = ref_buf[j];
			count += ct; count += 1;
		}

		Key partial_merge_fix(Index ct) {
			Index i = ct;
			Index j = 0;
			Key k = key[i++];
			while (i < count) {
				key[j] = key[i];
				ref[j] = ref[i];
				++i; ++j;
			}
			ref[j] = ref[i];
			count -= ct; count -= 1;
			return k;
		}

	};

}; // namespace Tree

// namespace Arca

	template <typename Key, uint8_t N>
	class TreeSet {
	public:
		using DataBlock = Tree::KeyBlock<Key, VoidP, N>;
		using IndexBlock = Tree::KeyRefBlock<Key, VoidP, N>;

		using Index = uint8_t;

		TreeSet() {top = nullptr;}

		Bool is_empty() { return is_null(top);}

		Bool has_key(Key k) {
			Bool match_flag;
			search_down(k, match_flag);
			return match_flag;
		}

		Bool add_key(Key k) {
			Void p = top;
			Index pos; Bool match_flag;
			search_down(k, p, pos, match_flag);
			if (match_flag == 1)
				return 0;
			if (is_null(p)) {
				p = create_first_datablock;
				pos = 0;
			}
			DataBlock & data = to_data_block(p);
			if (data.avail()==0) {
				DataBlock & ndata = req-data_block();
				Key pivot = add_split(data, ndata);
				attach_up(data.parent, pivot, to_voidp(&ndata));
			} else
				data.insert_key(pos, k);
			return 1;
		}

		Bool add_key(Key nkey) {
			Key k = nkey;
			Void p = top;
			while (is_nn(p)) {
				if (is_index_block(p)) {
					IndexBlock & idx = to_index_block(p);
					Index pos = idx.scan_fwd(k);
					if (idx.check_match(pos, k))
						return 0;
					p = idx.ref[pos];
				}
			}
			if (is_null(p)) {
				DataBlock & data = req_data_block();
				p = to_voidp(&data);
				top = p;
				data.parent = nullptr;
			}
			DataBlock & data = to_data_block(p);
			if (data.avail() > 0) {
				data.add_key(k);
				return 1;
			}
			DataBlock & ndata = req_data_block();
			Key pivot = splt_right(data, ndata);
			if (k < pivot)
				data.add_key(k);
			else
				ndata.add_key(k);
			q = p; p = data.parent;
			k = pivot; s = to_voidp(&ndata);
			while (is_nn(p)) {
				IndexBlock & idx = to_index_block(p);
				if (idx.avail() > 0) {
					idx.add_entry(k, s);
					set_parent_link(s, p);
					return 1;
				}
				IndexBlock & nidx = req_index_block();
				Key pivot = split_right(idx, nidx);
				if (k < pivot)
					idx.add_entry(k, s);
				else
					nidx.add_entry(k, s);
				update_parent_pointers(nidx);
				q = p; p = idx.parent;
				k = pivot; s = to_voidp(nidx);
			}
			IndexBlock & idx = req_index_block();
			idx.create_first_entry(k, p, s);
			set_parent_link(p, to_voidp(&idx));
			set_parent_link(s, to_voidp(&idx));
			idx.parent = nullptr;
			top = to_voidp(&idx);
			return 1;
		}


/*

			VoidP s = nullptr;
			VoidP q = nullptr;
			while (is_nn(p)) {
				if (is_data_block(p)) {
					DataBlock & data = to_data_block(p);
					if (data.avail()>0) {
						data.add_key(k);
						return 1;
					}
;
					k = pivot;
					s = nullptr;
					q = p;
					p = parent;
				}


			}

		}

		Bool add_key(Key k) {
		//	PRINT_STR("add key:");
		//	PRINT_VAL(k);
		//	PRINT_LN;
			VoidP p;
			Bool match_flag;
			p = search_down(k, match_flag);
			if (match_flag == 1) // no duplicates allowed
				return 0;
			add_recursive(nullptr, p, k, nullptr);
			return 1;
		}

*/

		Bool remove_key(Key k) {
			VoidP p; Index pos, Bool match_flag;
			search_down(k, p, pos, match_flag);
			if (match_flag == 0)
				return 0;
			if (is_index_block(p))
				delete_from_index_block(p, k, pos);
			else
				delete_from_data_block(p, k, pos);
			return 1;
		}


		template <typename F>
		void each_recursive(F f, VoidP p) {
			if (is_nn(p) && is_data_block(p)) {
				DataBlock & data = to_data_block(p);
				for (Index i = 0; i != data.count; i += 1)
					f(data.key[i]);
				return;
			}
			if (is_nn(p) && is_index_block(p)) {
				IndexBlock & idx = to_index_block(p);
				each_recursive(f, idx.ref[0]);
				for (Index i = 0; i != idx.count; i += 1) {
					f(idx.key[i]);
					each_recursive(f, idx.ref[i + 1]);
				}
			}
		}

		void clear() {
			VoidP p = top;
			if (is_nn(p))
				clear_recursive(p);
			top = nullptr;
		}

		uint64_t count_all() {
			uint64_t c = 0;
			auto f = [&c](Reg64 x){ c++;};
			each(f);
			return c;
		}


		template <typename F>
		void each(F f) {
			VoidP p = top;
			if (is_nn(p))
				each_recursive(f, p);
		}

		void dump_tree() {
			VoidP p = top;
			if (is_nn(p))
				dump_tree_recursive(nullptr, p);
		}

		void dump_tree_recursive(VoidP q, VoidP p) {
			if (is_nn(p) && is_data_block(p)) {
				DataBlock & data = to_data_block(p);
				PRINT_STR("===data block\n");
				PRINT_STR("address:");
				PRINT_HEX((Reg64) p);
				PRINT_LN;
				PRINT_STR("parent:");
				PRINT_HEX((Reg64) data.parent);
				PRINT_LN;
				if ((Reg64) data.parent != (Reg64) q)
					PRINT_STR("*** potential bad parent link *** \n");
				PRINT_STR("count:");
				PRINT_VAL(data.count);
				PRINT_LN;
				PRINT_STR("data >> ");
				for (Index i = 0; i != data.count; i += 1) {
					PRINT_VAL(data.key[i]);
					PRINT_STR(" ");
				}
				PRINT_LN;
				return;
			}
			if (is_nn(p) && is_index_block(p)) {
				IndexBlock & idx = to_index_block(p);
				PRINT_STR("===index block\n");
				PRINT_STR("address:");
				PRINT_HEX((Reg64) p);
				PRINT_LN;
				PRINT_STR("parent:");
				PRINT_HEX((Reg64) idx.parent);
				PRINT_LN;
				if ((Reg64) idx.parent != (Reg64) q)
					PRINT_STR("*** potential bad parent link *** \n");
				PRINT_STR("count:");
				PRINT_VAL(idx.count);
				PRINT_LN;
				PRINT_STR("data >> ");
				for (Index i = 0; i != idx.count; i += 1) {
					PRINT_VAL(idx.key[i]);
					PRINT_STR(" ");
				}
				PRINT_LN;
				PRINT_STR("ref >> ");
				for (Index i = 0; i != idx.count + 1; i += 1) {
					PRINT_HEX((Reg64) idx.ref[i]);
					PRINT_STR(" ");
				}
				PRINT_LN;
				for (Index i = 0; i != idx.count + 1; i += 1) {
					dump_tree_recursive(p, idx.ref[i]);
				}
			}

		}

	private:
		VoidP top;

		Bool is_null(VoidP p) { return p == nullptr;}
		Bool is_nn(VoidP p) { return p != nullptr;}

		VoidP to_voidp(DataBlock * datap) { return (VoidP) datap;}
		VoidP to_voidp(IndexBlock * idxp) { return (VoidP) idxp;}

		DataBlock & to_data_block(VoidP p) { return *((DataBlock *) p);}
		IndexBlock & to_index_block(VoidP p) { return *((IndexBlock *) p);}

		DataBlock & req_data_block() { return *(new DataBlock());}
		IndexBlock & req_idx_block() { return *(new IndexBlock());}

		Index get_level(VoidP p) {
			if (is_data_block(p))
				return to_data_block(p).level;
			if (is_index_block(p))
				return to_index_block(p).level;
			return 0xff;
		}


		Bool is_data_block(VoidP p) {
			if ((is_nn(p)) && ((DataBlock *) p)->code == Tree::data_block_code())
				return 1;
			else
				return 0;
		}

		Bool is_index_block(VoidP p) {
			if ((is_nn(p)) && ((IndexBlock *) p)->code == Tree::index_block_code())
				return 1;
			else
				return 0;
		}



		void clear_recursive(VoidP p) {
			if (is_nn(p) && is_data_block(p)) {
				delete (DataBlock *) p;
				return;
			}
			if (is_nn(p) && is_index_block(p)) {
				IndexBlock & idx = to_index_block(p);
				for (Index i = 0; i != idx.count + 1; i += 1) {
					VoidP q = idx.ref[i];
					clear_recursive(idx.ref[i]);
					delete (IndexBlock *) q;
				}
			}
		}

		void set_parent_link(VoidP p, IndexBlock & idx) {
			VoidP q = to_voidp(&idx);
			if (is_data_block(p)) {
				to_data_block(p).parent = q;
			} else if (is_index_block(p)) {
				to_index_block(p).parent = q;
			}
		}

		void redo_parent_links(IndexBlock & idx) {
			for (Index i = 0; i != idx.count + 1; i += 1) {
				set_parent_link(idx.ref[i], idx);

			}
		}


		VoidP search_down(Key k, Bool & match_flag ) {
			//PRINT_STR("search_down:");
			//PRINT_VAL(k);
			//PRINT_LN;
			VoidP p = top;
			match_flag = 0;
			while (is_nn(p) && (is_index_block(p))) {
				IndexBlock & idx = to_index_block(p);
				Index pos = idx.scan_back(k);
				match_flag = idx.check_for_match(pos, k);

				if (match_flag==1) {
					//PRINT_STR("--- Index Match Found ---\n");
					return p;
				}
				else
					p = idx.ref[pos];
			}
			if (is_nn(p) && (is_data_block(p))) {
				DataBlock & data = to_data_block(p);
				match_flag = data.has_key(k);
				if (match_flag==1) {
					//PRINT_STR("--- Data Match Found ---\n");
					return p;
				}
			}
			return p;
		}

		void search_down(Key k, VoidP & p, Index & pos, Bool & match_flag) {
			p = top; pos = 0; match_flag = 0;
			if (is_null(p)))
				return;
			while (is_index_block(p)) {
				IndexBlock & idx = to_index_block(p);
				pos = idx.scan_fwd(k);
				match_flag = idx.check_for_match(pos, k);
				if (match_flag == 1)
					return;
				p = idx.ref[pos];
			}
			DataBlock & data = to_data_block(p);
			pos = data.scan_fwd(k);
			match_flag = data.check_for_match(pos, k);
		}

		VoidP create_first_datablock() {
			DataBlock & data = req_data_block();
			data.parent = nullptr;
			top = to_voidp(&data);
		}

		Key add_split(DataBlock & data, DataBlock & ndata, Key k) {
			Index ct = N / 2;
			Index i = ct;
			Key pivot = data.key[i++];
			while (i < N)
				ndata.append(data.key[i++]);
			data.count = ct;
			if (k < pivot)
				data.add_key(k);
			else
				ndata.add_key(k);
			return pivot;
		}

		Key add_split(IndexBlock & idx, IndexBlock & nidx, Key k, VoidP s) {
			Index ct = N;
			Index i = ct;
			Key pivot = idx.key[i++];
			nidx.ref[0] = idx.ref[i];
			while (i < N) {
				nidx.append(key[i], ref[i + 1]);
				++i;
			}
			update_parents(nidx);
			idx.count = ct;
			if (k < pivot) {
				idx.add_entry(k, s);
				set_parent(s, to_voidp(&idx));
			} else {
				nidx.add_entry(k, s);
				set_parent(s, to_voidp(&idx));
			}
			return pivot;
		}

		void attach_up(VoidP node, VoidP child, Key nkey ) {
			VoidP p = node;
			VoidP s = child;
			Key k = nkey;
			while (is_nn(p)) {
				IndexBlock & idx = to_indexblock(p);
				if (idx.avail()==0) {
					IndexBlock & nidx = req_idx_block();
					k = add_split(idx, nidx, k, s);
					s = to_voidp(&idx);
					p = idx.parent;
				} else {
					idx.add_entry(k, s);
					set_parent(s, to_voidp(&idx));
					return;
				}
			}
			IndexBlock & idx = req_idx_block();
			idx.parent = nullptr;
			idx.ref[0] = p;
			idx.append(k, s);
			top = to_voidp(&idx);
		}



		void add_recursive(VoidP q, VoidP p, Key k, VoidP s) {
			//PRINT_STR("add recursive:");
			//PRINT_HEX((Reg64) q); PRINT_STR(":");
			//PRINT_HEX((Reg64) p); PRINT_STR(":");
			//PRINT_VAL((Reg64) k); PRINT_STR(":");
			//PRINT_HEX((Reg64) s); PRINT_LN;


			if (is_null(p) && is_nn(q)) {  // at the top. need new index block
				//PRINT_STR("add new index block\n");
				IndexBlock & idx = req_idx_block();
				idx.level = get_level(q) + 1;
				idx.create_first_entry(k, q, s);
				top = to_voidp(&idx);
				set_parent_link(q, idx);
				set_parent_link(s, idx);
				return;
			}
			if (is_null(p) && is_null(q)) { // at the top, need new data block
				DataBlock & data = req_data_block();
				data.append(k);
				data.parent = nullptr;
				top = to_voidp(&data);
				return;
			}
			if (is_nn(p) && is_data_block(p)) {  // in a data block, add or split up
				DataBlock & data = to_data_block(p);
				//PRINT_STR("add recursive data parent:");
				//PRINT_HEX((Reg64) data.parent);
				//PRINT_LN;
				if (data.avail() != 0) {
					data.insert(k);
					return;
				}
				DataBlock & ndata = req_data_block();
				Index ct = N >> 1;
				Key nkey = data.split_to(ndata.key, ct);
				ndata.count = ct;
				if (k < nkey)
					data.insert(k);
				else
					ndata.insert(k);
				add_recursive(to_voidp(&data), data.parent, nkey, to_voidp(&ndata));
				return;
			}
			if (is_nn(p) && is_index_block(p)) {
				IndexBlock & idx = to_index_block(p);
				//WATCH("idx.avail()", idx.avail());
				if (idx.avail() != 0) {
					idx.add_entry(k, s);
					set_parent_link(s, idx);
					return;
				}
				IndexBlock & nidx = req_idx_block();
				Index ct = N >> 1;
				Key nkey = idx.split_to(nidx.key, nidx.ref, ct);
				nidx.count = ct;
				redo_parent_links(nidx);
				if (k < nkey) {
					idx.add_entry(k, s);
					set_parent_link(s, idx);
				} else {
					nidx.add_entry(k, s);
					set_parent_link(s, nidx);
				}
				add_recursive(p, idx.parent, nkey, to_voidp(&nidx));
				return;
			}

		}

		void remove_from_index(IndexBlock & idx, Key k) {
			Index t = get_key_pos(k);
			VoidP p = idx.ref[t];
			while (is_index_block(p)) {
				IndexBlock & oidx = to_index_block(p);
				p = oidx.ref[oidx.count];
			}
			DataBlock & data = to_data_block(p);
			Key nkey = data.pop_last();
			idx.key[t] = nkey;
			check_for_merge(&data);
		}

		void remove_from_datablock(DataBlock & data, Key k) {
			data.remove(k);
			check_for_merge(data);
		}

/*
		void del_recursive(VoidP q, Voidp p, Key k) {
			Index lo = N >> 2;
			if (is_nn(p) && is_data_block(p)) {
				DataBlock & data = to_data_block(p);
				data.remove(k);
				if (data.count <= lo) {

				}
			}

		}
*/
	};

/*
 *
	template <typename Index, typename Elem>
	void shl(Elem * buf, Index pos, Index len, Index ct) {
		Elem * p = buf + pos;
		Elem * e = buf + pos + len;
		Elem * q = buf + pos - ct;
		while (p != e)
			*q++ = *p++;
	}

	template <typename Index, typename Elem>
	void shr(Elem * buf, Index pos, Index len, Index ct) {
		Elem * p = buf + pos + len;;
		Elem * e = buf + pos;
		Elem * q = buf + pos + len + ct;
		while (p != e) {
			*(q - 1) = *(p - 1);
			--q; --p;
		}
	}

	template <typename Index, typename Elem>
	void copy_to(Elem * dest, Elem * src, Index ct) {
		Elem * p = src;
		Elem * e = src + ct;
		Elem * q = dest;
		while (p != e)
			*q++ = *p++;
	}







	template <typename Key, uint16_t N>
	struct IndexBlock {
		using Index = uint16_t;

		Index code;
		Index level;
		Index count;
		IndexBlock * parent;
		Key key[N];
		VoidP ptr[N+1];

		IndexBlock() {
			code = index_block_code();
			count = 0;
			level = 0;
			parent = nullptr;
		}

		void add_first_ptr(VoidP p) {ptr[0] = nullptr; }



		void add_entry(Key k, VoidP p) {
			Index i = count;
			while (i > 0) {
				if (key[i - 1] < k)
					break;
				key[i - 1] = key[i];
				ptr[i] = ptr[i + 1];
				i -= 1;
			}
			key[i] = k;
			ptr[i + 1] - p;
			count += 1;
		}

		Key remove_entry_return_key(VoidP p) {
			Index i = 0;
			Key e;
			while (i < count) {
				if (ptr[i + 1]==p)
					break;
				i += 1;
			}
			e = key[i];
			while (i < count - 1) {
				key[i] = key[i + 1];
				ptr[i + 1] = ptr[i + 2];
				i += 1;
			}
			count -= 1;
			return e;
		}

		template <typename F>
		Index scan_for_key(Key k) {
			Index i = count;
			while (i > 0) {


			}
		}
	};


	template <typename Key, typename Entry, uint16_t N>
	struct IdxEntryBlock {
		using Index = uint16_t;

		Index code;
		Index level;
		Index count;
		IndexBlock * parent;
		Key key[N];
		VoidP ptr[N+1];
	};


	template <typename Key, uint16_t N>
	struct KeyBlock {
		using Index = uint16_t;

		Index code;
		Index level;
		Index count;
		IndexBlock * parent;
		Key key[N];
	};


	template <typename Key, typename Entry, uint16_t N>
	struct EntryBlock {
		using Index = uint16_t;

		Index code;
		Index level;
		Index count;
		IndexBlock * parent;
		Key key[N];
		Entry entry[N];
	};



		void replace_key

		VoidP scan_for_key(Key k, Bool & match_flag) {
			Index i = count;
			while (i > 0) {
				Key e = key[i - 1];
				if (e == k) {
					match_flag = 1;

				}
				if (key[i - 1] <= k)

			}
		}

	};


	template <typename Key, uint16_t N>
	struct KeyBlock {
		using Index = uint16_t;

		Index code;
		Index level;
		Index count;
		IndexBlock parent;
		Key[N];
		VoidP Ptr[N+1];

	};

	template <typename Key, uint16_t N>
	struct KeyValueBlock {
		using Index = uint16_t;

		Index code;
		Index level;
		Index count;
		IndexBlock parent;
		Key[N];
		Value[N];
		VoidP Ptr[N+1];

	};

};

namespace Tree {
// Arca::Tree

	// Arca does not have conventional binary trees, it only uses a higher order B+ tree.  This
	// is far more efficient
	

	
	template <typename Key, uint16_t N>
	class TreeSet {
	public:
		using Index = uint16_t;

		TreeSet() {
			top = nullptr;
		}

		Bool has_key(Key k) {
			VoidP p, q;
			Bool match_flag;
			scan_for_key(k, q, p, match_flag);
			return match_flag;
		}

		Bool add_key(Key k) {
			Key key = k;
			VoidP p, q;
			Bool match_flag;
			scan_for_key(k, q, p, match_flag);

			if (match_flag == 1)
				return 0;

			recursive_add_key(q, p, key, nullptr);
			return 1;
		}

		Bool remove_key(Key k) {
			Key key = k;
			VoidP p, q;
			Bool match_flag;
			scan_for_key(k, q, p, match_flag);

			if (match_flag == 1)
				return 0;

			recursive_del_key(q, p, key, nullptr);
			return 1;

		}

		template <typename F>
		void range_scan(Key st, Key en) {}

#ifndef ARCA_TREE_TESTER_H

		void print_data_block(DataPtr data_ptr) {
			PRINT_STR("--- datablock\n");
			DataBlock & data = *data_ptr;
			PRINT_STR("address: ");
			PRINT_HEX((U64) data_ptr);
			PRINT_LN();
			PRINT_STR("parent: ");
			PRINT_HEX((U64) data.parent);
			PRINT_LN();
			PRINT_STR("count:");
			PRINT_VAL(data.count);
			PRINT_LN();
			PRINT_STR("contents:\n");
			for (Index i = 0; i != count; i += 1) {
				x = (U64) key[i];
				PRINT_VAL(x);
				PRINT_STR(" ");
			}
			PRINT_LN;
		}

		void print_index_block(IndexPtr idx_ptr) {
			PRINT_STR("--- indexblock\n");
			DataBlock & idx = *idx_ptr;
			PRINT_STR("address: ");
			PRINT_HEX((U64) idx_ptr);
			PRINT_LN();
			PRINT_STR("parent: ");
			PRINT_HEX((U64) data.parent);
			PRINT_LN();
			PRINT_STR("level:");
			PRINT_VAL(idx.level);
			PRINT_LN();
			PRINT_STR("count:");
			PRINT_VAL(idx.count);
			PRINT_LN();
			PRINT_STR("contents:\n");
			PRINT_VAL((U64) ptr[0]);
			PRINT_STR(" ");
			for (Index i = 0; i != count; i += 1) {
				x = (U64) key[i];
				PRINT_VAL((U64) key[i]);
				PRINT_STR(" ");
				PRINT_VAL((U64) ptr[i+1]);
				PRINT_STR(" ");
			}
			PRINT_LN;
			for (Index i = 0; i != count + 1; i += 1) {
				VoidP p = ptr[i];
				UnknownBlock * blk = (UnknownBlock *) p;
				if (blk->is_data_block()==1) {
					DataBlock * d = (DataBlock *) d;
					print_data_block(d);
				} else {
					IndexBlock * idx = (IndexBlock *) i;
					print_index_block(i);
				}
			}
		}

		void dump() {
			PRINT_STR("Tree status:\n");
			if (top == nullptr) {
				PRINT_STR("top == nullptr\n");
				return;
			}
			VoidP p = top;

			UnknownBlock * blk = top;

			if (blk->is_data_block()) {
				print_data_block((DataBlock *) p);
			}
			if (blk->is_index_block()) {
				print_index_block((IndexBlock *) p);
			}

			PRINT_STR("=== END TREE ===");
			PRINT_LN;
		}




	private:

#endif


		static Index low_block_limit() {
			return N / 4;
		}

		VoidP top;

		struct IndexBlock;
		using IndexPtr = IndexBlock *;
		struct DataBlock;
		using DataPtr = DataBlock *;

		static VoidP to_voidp(IndexPtr p) { return (VoidP) p;}
		static VoidP to_voidp(DataPtr p) { return (VoidP) p;}

		static IndexBlock to_index_block(VoidP p) { return (IndexBlock *) p;}
		static DataBlock to_data_block(VoidP p) { return (DataBlock *) p;}

		// magic numbers

		Index data_block_code() { return 0xda30;}
		Index index_block_code() { return 0xb02a;}

		struct UnknownBlock {
			Index code;
			Index level;
			Index count;
			IndexPtr parent;

			Bool is_data_block() { return code == data_block-code();}
			Bool is_index_block() { return code == index_block_code();}
		};

		struct IndexBlock {
			Index code;
			Index level;
			Index count;
			IndexPtr parent;
			Key key[N];
			VoidP ptr[N+1];

			IndexBlock() {
				code = index_block_code();
				level = 0xff;
				count = 0;
				parent = nullptr;
			}

			Index avail() { return N - count;}

			void add_left_ptr(VoidP p) {
				ptr[0] = p;
			}

			void append(Key k, VoidP p) {
				key[count] = k;
				ptr[count + 1] = p;
				count += 1;
			}

			void add_entry(Key k, VoidP p) {
				Index i = count;
				while (i > 0) {
					if (key[i - 1] < k)
						break;
					key[i] = key[i - 1];
					ptr[i + 1] = ptr[i];
					i -= 1;
				}
				key[i] = k;
				ptr[i+1] = p;
				count += 1;
			}

			void remove_entry(Key k, VoidP p) {
				Index i = 0;
				while (i != count) {
					if (key[i] >= k)
						break;
					i += 1;
				}
				while (i != count - 1) {
					key[i] = key[i + 1];
					ptr[i+1] = ptr[i + 2];
					i += 1;
				}
				count -= 1;
			}

			Key swap_entry(VoidP p, Key nkey) {
				for (Index i = 0; i != count; i += 1) {
					if (ptr[i+1==p) {
						Key k = key[i];
						key[i] = nkey;
						return k;
					}
				}
				return nkey;
			}

			Index scan(Key k, Bool & match_flag) {
				match_flag = 0;
				Index i = 0;
				while (i != count) {
					if (key[i] >= k)
						break;
					i += 1;
				}
				if (key[i]==k)
					match_flag = 1;
				return i;
			}

#ifdef ARCA_TREE_TESTER_H

			void dump() {
				U64 x = count;
				printf("count = %" PRIu64  "\n", x);
				for (Index i = 0; i != count; i += 1) {
					x = (U64) key[i];
					PRINT_VAL(x);
					PRINT_STR(" ");
				}
				PRINT_LN;
				for (Index i = 0; i != count + 1; i += 1) {
					x = (U64) ptr[i];
					PRINT_HEX(x);
					PRINT_STR(" ");
				}
				PRINT_LN;
			}

#endif
		};

		struct DataBlock {
			using Index = U16;
			Index code;
			Index level;
			Index count;
			IndexPtr parent;
			Key key[N];

			DataBlock() {
				parent = nullptr;
				level = 0;
				count = 0;
				code = data_block_code();
			}

			Index avail() { return N - count;}
			Bool empty() { return count == 0;}

			void append(Key k) {
				key[count++] = k;
			}

			Key pop_last() {
				count -= 1;
				return key[count];
			}

			void add_key(Key k) {
				Index i = count;
				while (i > 0) {
					if (key[i - 1] < k)
						break;
					key[i] = key[i - 1];
					i += 1;
				}
				key[i] = k;
				count += 1;
			}

			void insert_at(Index pos, Key k) {
				Index i = count;
				while (i > pos) {
					key[i] = key[i - 1];
					i -= 1;
				}
				key[pos] = k;
				count += 1;
			}
			void remove_at(Index pos) {
				Index i = pos;
				while (i != count - 1) {
					key[i] = key[i + 1];
					i += 1;
				}
				count -= 1;
			}

			Key first() { return key[0];}
			Key last() { return key[count - 1];}

			Index scan_fwd(Key k) {
				Index i = 0;
				while (i != count) {
					if (key[i] >= k)
						break;
					i += 1;
				}
				return i;
			}

			Index scan_rev(Key k) {
				Index i = count;
				while (i > 0) {
					if (key[i - 1] <= k)
						break;
					i -= 1;
				}
				return i;
			}

#ifdef ARCA_TREE_TESTER_H

			void dump() {
				U64 x = count;
				printf("count = %" PRIu64  "\n", x);
				for (Index i = 0; i != count; i += 1) {
					x = (U64) key[i];
					PRINT_VAL(x);
					PRINT_STR(" ");
				}
				PRINT_LN;
			}

#endif
		};


		void scan_for_key(Key k, VoidP & q, VoidP & p, Bool & match_flag) {
			q = nullptr;
			p = nullptr;
			match_flag = 0;
			p = top;
			while (p != nullptr) {
				UnknownBlock * blk = (UnknownBlock *) p;
				if (blk->is_data_block()==1) {
					DataPtr data = (DataPtr) p;
					match_flag = data->has_key(k);
					return;
				}
				if (blk->is_index_block()==1) {
					IndexPtr idx = (IndexPtr) p;
					Index pos = (*idx).scan_fwd(k, match_flag);
					if (match_flag == 1)
						return;
					q = p; p = (*idx).ptr[pos];
				}
//				unknown block type
				break;
			}
			return;
		}

		void recursive_add_key(VoidP q, VoidP p, Key key, VoidP ptr) {
			UnknownBlock * blk_p = (UnknownBlock *) p;
			UnknownBlock * blk_q = (UnknownBlock *) q;

			if (blk_p == nullptr) {
				if (blk_q == nullptr) {
					// first data block
					DataPtr data = new DataBlock();
					top = to_voidp(data);
					recursive_add_key(nullptr, to_voidp(data), key, nullptr);
				} else {
					Index last_level = blk_q->level;
					IndexPtr idx = new IndexBlock();
					idx->level = last_level + 1;
					idx->add_left_ptr(q);
					top = to_voidp(idx);
					recursive_add_key(q, top, key, ptr);
				}
				return;
			}
			if (blk_p->is_data_block()) {
				DataPtr data = (DataPtr) p;
				if ((*data).avail()>0) {
					(*data).add_key(key);
				} else {
					DataPtr data2 = new DataBlock();
					Key pivot = split_right(*data, *data2);
					if (key < pivot)
						(*data).add_key(key);
					else
						(*data2).add_key(key);
					q = p;
					p = to_voidp(data->parent);
					recursive_add_key(q, p, pivot, data2);
					data2->parent = to_index_block(p);
				}
				return;
			}
			if (blk_p->is_index_block()) {
				IdxPtr idx = (IndexPtr) p;
				if ((*idx).avail()>0) {
					(*idx).add_entry(key, ptr);
				} else {
					IndexPtr idx2 = new IndexBlock();
					Key pivot = split_right(*idx, *idx2);
					if (key < pivot)
						(*idx).add_entry(key);
					else
						(*idx2).add_entry(key);
					q = p;
					p = to_voidp(idx->parent);
					recursive_add_key(q, p, pivot, idx2);
					idx2->parent = to_index_block(p);
				}
				return;
			}
		}



		static void add_split(Key & k, DataBlock & b1, DataBlock & b2) {
			split_to_right(b1, b2, N / 2);
			Key pivot = b1.pop_last();
			if (k < pivot)
				b1.add_key(k);
			else
				b2.add_key(k);
			k = pivot;
		}

		static void split_to_right(DataBlock & b1, DataBlock & b2, Index ct) {
			Index i = 0;
			Index j = N - ct;
			while (j != N) {
				b2.key[i++] = b1.key[j++];
			}
			b1.count -= ct; b2.count += ct;
		}

		static void even_split(DataBlock & b1, DataBlock & b2) {
			Index ct = N / 2;
			Index i = ct;
			Index j = 0;
			while (i != N) {
				b2.key[j++] = b1.key[i++];
			}
			b1.count -= ct; b2.count += ct;
		}

		static void merge_from_right(DataBlock & b1, DataBlock & b2, Index ct) {
			Index i = b1.count;
			Index j = 0;
			while (j != ct) {
				b1.key[i++] = b2.key[j++];
			}
			i = 0;
			while (j < b2.count) {
				b2.key[i++] = b2.key[j++];
			}
			b1.count += ct; b2.count -= ct;
		}

		void scan_for_key(Key k, Index & cur_lvl, IndexPtr & idx_ptr, DataPtr & data_ptr, Bool & match_flag) {
			Index idx_pos, data_pos;
			cur_lvl = level_ct;
			idx_ptr = nullptr; data_ptr = nullptr;
			idx_pos = N; data_pos = N;
			match_flag = 0;
			VoidP p = top;
			if (p == nullptr)
				return;
			while (cur_lvl != 0) {
				idx_ptr = (IndexPtr) p;
				idx_pos = (*idx_ptr).scan_fwd(k);
				//WATCH("idx_pos", idx_pos);
				if ((*idx_ptr).key[idx_pos]==k) {
					match_flag = 1;
					return;
				}
				p = (*idx_ptr).ptr[idx_pos];
				cur_lvl -= 1;
			}
			data_ptr = (DataPtr) p;
			data_pos = (*data_ptr).scan_fwd(k);
			match_flag = (*data_ptr).key[data_pos] == k ? 1: 0;
		}

		void add_key_recursive(VoidP p, VoidP q, Key key, VoidP ptr ) {
			IndexPtr idx = (IndexPtr) p;
			if (idx->avail()>0) {
				idx->insert(key, ptr);
			} else {
				IndexPtr idx2 = new IndexBlock();
				Key k2 = split_index_block(idx, idx2);
				if (key < k2) {
					idx->insert(key, ptr);
				else
					idx2->insert(key, ptr);
				add_key_recursive()

				add_split_index_block(key, )
				IndexPtr idx2 = new IndexBlock;

				(*)
			}



		}

		void scan_for_key(Key k, Index & cur_lvl, Bool & match_flag, VoidP & p, Index & pos) {
			p = top;
			pos = count;
			match_flag = 0;
			cur_level = level_count;
			if (top == nullptr)
				return nullptr;
			while (cur_level != 0) {
				IndexBlock * idx = (IndexBlock *) p;
				pos = search_index_block(*idx, k, match_flag)
				if (match_flag==1)
					return;
				p = idx->ptr[pos];
				cur_lvl -= 1;
			}
			DataBlock * data = (DataBlock *) p;
			pos = search_data_block(*data, k, match_flag);
		}





		void add_key(DataBlock & data, Key k) {
			Index i = data.count;
			while ((i > 0) && (data.key[i - 1] > k)) {
					data.key[i] = data.key[i - 1];
					i -= 1;
			}
			data,key[i] = k;
			data.count += 1;
		}

		Index has_key(DataBlock & data, Key k) {
			for (Index i = 0; i != data.count; i += 1) {
				if (data.key[i] == k)
					return i;
			}
			return data.count;
		}

		void remove_key_at(DataBlock & data, Index pos) {
			Index i = pos;
			while (i != count - 1) {
				data.key[i] = data.key[i + 1];
				i += 1;
			}
			data.count -= 1;
		}

		void append(DataBlock & data, Key k) {
			Index i = data.count;
			data.key[i] = k;
			data.count += 1;
		}

		void setup_first_datablock(DataBlock & data, Key k) {
			append(data, k);
			data.parent = nullptr;
			data.level = 0;
		}

		void even_split(DataBlock & d1, DataBlock & d2) {
			Index c = N; c >>= 1;
			Index i = c;
			Index j = 0;
			while (i != d1->count) {
				d2.key[j++] = d1.key[i++];
			}
			d2.count = d1.count - c;
			d1.count = c;
		}

		void take_from_right(DataBlock & d1, DataBlock & d2, Index ct) {
			Index i = d1->count;
			Index j = 0;
			while (i < ct) {
				d1.key[i++] = d2.key[j++];
			}
			Index i = 0;
			while (j < d2.count) {
				d2.key[i++] = d2.key[j++];
			}
			d1.count += ct; d2.count -= ct;
		}

		void take_from_right(DataBlock & d1, DataBlock & d2) {
			Index i = d1->count;
			Index j = 0;
			while (i < d2->count) {
				d1.key[i++] = d2.key[j++];
			}
			d1.count += d2.count;
			d2.count = 0;
		}

		void setup_first_index_block(IndexBlock & idx, DataBlock & d1, DataBlock & d2) {
			idx->parent = nullptr;
			idx->count = 2;
			idx->level = 1;
			idx->key[0] = d1->key[0];
			idx->key[1] = d2->key[0];
			idx->blk_ct[0] = d1->count;
			idx->blk_ct[1] = d2->count;
			idx->ptr[0] = to_voidp(d1);
			idx->ptr[1] - to_voidp(d2);
		}

		void add_index_entry(IndexBlock * idx, Key k, VoidP p) {


		}





		struct PtrBlock {
			using Index = U16;

			Index count;
			Index level;

			PtrBlock * parent;
			Key key[N];
			VoidP ptr[N];

			void clear() { count = 0;}

			Index avail() { return N - count;}

			Key first_key() {
				return key[0];
			}

			Key last_key() {
				return key[count - 1];
			}

			Index search(Key k) {
				Index i = count;
				while (i > 0) {
					Key e = key[i - 1];
					if (e <= k)
						return i - 1;
					i -= 1;
				}
				return count;
			}

			Index find_ptr(VoidP t) {
				for (Index i = 0; i != count; i += 1) {
					if (ptr[i] == t)
						return i;
				}
				return count;
			}

			void insert(Key k, VoidP p) {
				Index i = count;
				while (i > 0) {
					if (key[i-1]>k) {
						key[i] = key[i - 1];
						ptr[i] = ptr[i - 1];
 					} else
						break;
					i -= 1;
				}
				key[i] = k;
				ptr[i] = p;
				count += 1;
			}

			void remove(Key k) {
				Index i = 0;
				while ((i != count) && (key[i] < k))
					i += 1;
				while (i < count - 1) {
					key[i] = key[i + 1];
					ptr[i] = ptr[i + 1];
					i += 1;
				}
				count -= 1;
			}

			void append(Key k, VoidP p) {
				key[count] = k;
				ptr[count] = p;
				count += 1;
			}

			// deletes ct to the left

			void shl(Index ct) {
				Index n = count - ct;
				for (Index i = 0; i != n; i += 1) {
					key[i] = key[i + ct];
					ptr[i] = ptr[i + ct];
				}
				count -= ct;
			}

			// inserts ct to the left

			void shr(Index ct) {
				Index n = count;
				Index i = n;
				while (i > 0) {
					key[i - 1 + ct] = key[i - 1];
					ptr[i - 1 + ct] = ptr[i - 1];
					i -= 1;
				}
				count += ct;
			}
		};

		struct KeyBlock {
			using Index = U16;

			Index count;
			PtrBlock * parent;

			Key key[N];

			void clear() { count = 0;}

			Index avail() { return N - count;}

			Bool has_key(Key k) {
				for (Index i = 0; i != count; i += 1) {
					if (key[i] == k)
						return i;
				}
				return count;
			}

			Key first_key() {
				return key[0];
			}

			Key last_key() {
				return key[count - 1];
			}

			void insert(Key k) {
				Index i = count;
				while (i > 0) {
					if (key[i-1]>k)
						key[i] = key[i - 1];
					else
						break;
					i -= 1;
				}
				key[i] = k;
				count += 1;
			}

			void remove(Key k) {
				Index i = 0;
				while ((i != count) && (key[i] < k))
					i += 1;
				while (i < count - 1) {
					key[i] = key[i + 1];
					i += 1;
				}
				count -= 1;
			}

			void append(Key k) {
				key[count++] = k;
			}

			// erases to the left

			void shl(Index ct) {
				Index n = count - ct;
				for (Index i = 0; i != n; i += 1) {
					key[i] = key[i + ct];
				}
				count -= ct;
			}

			// creates space to the left

			void shr(Index ct) {
				Index n = count;
				Index i = n;
				while (i > 0) {
					key[i + ct - 1] = key[i - 1];
					i -= 1;
				}
				count += ct;
			}
		};


		// simple operations

		void keyblock_match(KeyBlock )

		// Keyblock splits / merges

		void even_split(KeyBlock * b1, KeyBlock * b2) {
			using Index = U16;
			Index c = N / 2;
			Index i = 0;
			Index j = c;
			while (j != N) {
				b2->key[i++] = b1->key[j++];
			}
			b1->count = c;
			b2->count = i - c;
		}

		void even_split(PtrBlock * b1, PtrBlock * b2) {
			using Index = U16;
			Index c = N / 2;
			Index i = 0;
			Index j = c;
			while (j != N) {
				b2->ptr[i] = b1->ptr[j];
				b2->key[i++] = b1->key[j++];
			}
			b1->count = c;
			b2->count = i - c;
		}

		// can consume entire block

		void fill_from_next(KeyBlock * b1, KeyBlock * b2, Index ct) {
			Index i = b1->count;
			Index j = 0;
			while (j < ct) {
				b1->key[i++] = b2->key[j++];
			}
			Index i = 0;
			while (j < b2->count) {
				b2->key[i++] = b2->key[j++];
			}
			b1->count += ct;
			b2->count -= ct;
		}

		void fill_from_next(PtrBlock * b1, PtrBlock * b2, Index ct) {
			Index i = b1->count;
			Index j = 0;
			while (j < ct) {
				b1->ptr[i] = b2->ptr[j];
				b1->key[i++] = b2->key[j++];
			}
			Index i = 0;
			while (j < b2->count) {
				b2->ptr[i] = b2->ptr[j];
				b2->key[i++] = b2->key[j++];
			}
			b1->count += ct;
			b2->count -= ct;
		}

		// adjustments

		KeyBlock * create_first_key_block(Key k) {
			KeyBlock * kb = req_key_block();
			kb->count = 1;
			kb->key[0] = k;
			kb->parent = nullptr;
			return kb;
		}

		PtrBlock * create_first_ptr_block(KeyBlock * b1) {
			PtrBock * pb = req_ptr_block();
			KeyBlock * b2 = req_key_block();
			b2->count = 0;
			even_split(b1, b2);
			pb->key[0] = b1->key[0];
			pb->key[1] = b2->key[1];
			pb->ptr[0] = to_voidp(b1);
			pb->ptr[1] = to_voidp(b2);
			pb->parent = nullptr;
			pb->count = 2;
			pb->level = 0;
			return pb;
		}




		// voidp

		VoidP to_voidp(PtrBlock * blk) { return (VoidP) blk;}
		VoidP to_voidp(KeyBlock * blk) { return (VoidP) blk;}

		KeyBlock * to_key_block(VoidP p) { return (KeyBlock *) p;}
		PtrBlock * to_ptr_block(VoidP p) { return (PtrBlock *) p;}






		// free list for key blocks

		KeyBlock * req_key_block() {
			if (free_key-ct == 0)
				return new KeyBlock;
			else {
				free_key_ct -= 1;
				KeyBlock * blk = free_key;
				free_key = to_key_block(blk->parent);
				return blk;
			}
		}

		void rel_key_block(KeyBlock * blk) {
			if (free_key_ct == 8)
				delete(blk);
			else {
				blk->parent = to_voidp(free_key);
				free_key = blk;
				free_key_ct += 1;
			}
		}

		// free slot for ptr blocks

		PtrBlock * req_ptr_block() {
			if (free_ptr == nullptr)
				return new PtrBlock;
			else {
				PtrBlock * blk = free_ptr;
				free_ptr = nullptr;
				return blk;
			}
		}

		void rel_ptr_block(PtrBlock * blk) {
			if (free_ptr != nullptr)
				delete blk;
			else
				free_ptr = blk;
		}

		// release all

		void release_free_lists() {
			if (free_ptr != nullptr)
				delete free_ptr;
			KeyBlock * p = free_key;
			while (p != nullptr) {
				KeyBlock * n = to_key_block(p->parent);
				delete(p);
				p = n;
			}
		}

		void rel(KeyBlock * p) {
			if (free_key_ct != 8)
				delete(p);
			else
				push_free(p);
		}

		void rel(PtrBlock * p) {
			if (free_ptr_ct != 3)
				delete(p);
			else
				push_free(p);
		}



		KeyBlock

		struct FreeCell {
			PtrBlock pblk[2];
			KeyBlock kblk[4];
		};

		PtrBlock * free_ptr;
		KeyBlock * free_key;
		U8 free_ptr_ct, free_key_ct;



		voud push_ptr_block(PtrBlock * blk) {
			blk->parent = to_voidp(free_ptr);
			free_ptr = blk;
			free_ptr_ct += 1;
		}
		voud push_key_block(KeyBlock * blk) {
			blk->parent = to_voidp(free_key);
			free_key = blk;
			free_key_ct += 1;
		}

		PtrBlock pop_ptr_block() {
			PtrBlock blk = free_ptr;
			free_ptr =
		}


		PtrBlock req_ptr_block() {
			if (free_ptr_ct > 0) {
				PtrBlock b = free_ptr;
				free_ptr = to_ptr_block(b->parent);
				free_ptr_ct -= 1;
				return b;
			}
			free_ptr_ct += 1;
			return new PtrBlock;
		}


		void push_ptr_block(PtrBlock p) {
			p->parent = to_voidp(ptr_free);
			ptr_free = p;
			ptr_free_ct += 1;
		}

		PtrBlock pop_ptr_block() {
			PtrBlock p =
		}


		VoidP top;
		U8 set_flag;

	};

		*/



}; // namespace Arca




