
// Arca Tree
//

namespace Arca {
	// Arca

namespace Tree {
	// Arca::Tree


    class TreeFn {
    public:
        using Index8 = uint8_t;
        using Index32 = uint32_t;

        template <typename Key>
        static Index8 search(Key k, Index8 count, Key * keys) {
            Index8 i = 0;
            while ((i < count) && (k >= keys[i]))
                i += 1;
            return i;
        }

        template <typename Key>
        static void ins_key(Key k, Index8 pos, Index8 & count, Key * keys) {
            Index8 i = count;
            while (i != pos) {
                keys[i] = keys[i - 1];
                i--;
            }
            keys[i] = k;
            count += 1;
        }

        template <typename Key>
        static void ins_key_ptr(Key k, VoidP p, Index8 pos, Index8 & count, Key * keys, VoidP * ptrs) {
            Index8 i = count;
            while (i != pos) {
                keys[i] = keys[i - 1];
                ptrs[i + 1] = ptrs[i];
                i--;
            }
            keys[i] = k;
            ptrs[i+1] = p;
            count += 1;
        }

        template <typename Key>
        static void del_key(Index8 pos, Index8 & count, Key * keys) {
            Index8 i = pos;
            while (i != count) {
                keys[i - 1] = keys[i];
                i++;
            }
            count -= 1;
        }

        template <typename Key>
        static void del_key_ptr(Index8 pos, Index8 & count, Key * keys, VoidP * ptrs) {
            Index8 i = pos;
            while (i != count) {
                keys[i - 1] = keys[i];
                ptrs[i] = ptrs[i + 1];
                i++;
            }
            count -= 1;
        }

        template <typename Key>
        static void split_key(Index8 & ct1, Index8 & ct2, Key * key1, Key * key2, Key & pivot) {
            Index8 i, j, n;
            n = ct1;
            i =  n >> 1;
            ct1 = i;
            ct2 = n - i - 1;
            pivot = key1[i];
            i += 1;
            j = 0;
            while (i < n) {
                key2[j++] = key1[i++];
            }
        }

        template <typename Key>
        static void split_key_ptr(Index8 & ct1, Index8 & ct2, Key * key1, Key * key2, VoidP * ptr1, VoidP * ptr2, Key & pivot) {
            Index8 i, j, n;
            n = ct1;
            i = n >> 1;
            ct1 = i;
            ct2 = n - i - 1;
            pivot = key1[i];
            i += 1;
            j = 0;
            while (i < n) {
                key2[j] = key1[i];
                ptr2[j] = ptr1[i];
                j += 1; i += 1;
            }
            ptr2[j] = ptr1[i];
        }

        template <typename Key>
        static void merge_key(Index8 & ct1, Index8 & ct2, Key * key1, Key * key2, Key pivot) {
            Index8 i, j, n;
            n = ct1 + ct2 + 1;
            i = ct1;
            key1[i] = pivot;
            i += 1;
            j = 0;
            while (i < n) {
                key1[i++] = key2[j++];
            }
            ct1 = n;
            ct2 = 0;
        }

        template <typename Key>
        static void merge_key_ptr(Index8 & ct1, Index8 & ct2, Key * key1, Key * key2, VoidP * ptr1, VoidP * ptr2, Key pivot) {
            Index8 i, j, n;
            n = ct1 + ct2 + 1;
            i = ct1;
            key1[i] = pivot;
            i += 1;
            j = 0;
            while (i < n) {
                key1[i] = key2[j];
                ptr1[i] = ptr2[j];
                i += 1; j += 1;
            }
            ptr1[i] = ptr2[j];
            ct1 = n;
            ct2 = 0;
        }
    };

    template <typename Key, uint8_t N>
    class TreeSet {
    public:
        TreeSet() {
            top = nullptr;
        }

        void add_key(Key k) {
            VoidP p;  Index8 pos;
            p = top;
            while ((p != nullptr) && (is_indexblk(p))) {
                IndexBlock & idx = to_indexblk(p);
                pos =  idx.search(k);
                p = idx.ptr[pos];
            }
            add_split_up(p, k);
        }

        template <typename Fn>
        void each(Fn fn) {
            if (top == nullptr)
                return;
            if (is_datablk(top))
                each_datablk(top, fn);
            else
                each_indexblk(top, fn);
        }

        void dump() {
            if (top != nullptr)
                dump_recursive(top);
        }


    private:
        using Code = uint8_t;
        using Index8 = uint8_t;
        using Index32 = uint32_t;

        VoidP top;
        //Index elem_ct;
        //Index blk_ct;

        struct DataBlock {
            Code code;
            Index8 count;
            VoidP parent;
            Key keys[N];

            void init() {
                code = 1;
                count = 0;
            }

            Bool empty() { return count == 0;}
            Bool avail() { return count != N;}

            Index8 search(Key k) {
                return TreeFn::search<Key>(k, count, keys);
            }
            void add_key(Key k) {
                Index8 pos = search(k);
                TreeFn::ins_key(k, pos, count, keys);
            }

			void print() {
				PRINT_STR("Datablock address: ");
				PRINT_HEX((Reg64) this);PRINT_LN;
				PRINT_STR("parent: ");
				PRINT_HEX((Reg64) parent); PRINT_LN;
				PRINT_STR("count: ");
				PRINT_VAL(count); PRINT_LN;
				PRINT_STR("keys: "); PRINT_LN;
				for (Index8 i = 0; i != count; i += 1) {
					PRINT_VAL(i);
					PRINT_STR(":");
					PRINT_VAL(keys[i]);
					PRINT_STR(" ");
				}
				PRINT_LN;
			}
        };

        struct IndexBlock {
            Code code;
            Index8 count;
            VoidP parent;
            Key keys[N];
            VoidP ptr[N + 1];

            void init() {
                code = 2;
                count = 0;
            }
            Bool empty() { return count == 0;}
            Bool avail() { return count != N;}

            Index8 search(Key k) {
                return TreeFn::search<Key>(k, count, keys);
            }

            void add_key_ptr(Key k, VoidP p) {
                Index8 pos = search(k);
                TreeFn::ins_key_ptr(k, p, pos, count, keys, ptr);
            }

			void print() {
				PRINT_STR("Indexblock address: ");
				PRINT_HEX((Reg64) this);PRINT_LN;
				PRINT_STR("parent: ");
				PRINT_HEX((Reg64) parent); PRINT_LN;

				PRINT_STR("count: ");
				PRINT_VAL(count); PRINT_LN;

				PRINT_STR("keys: "); PRINT_LN;

				for (Index8 i = 0; i != count; i += 1) {
					PRINT_VAL(i);
					PRINT_STR(":");
					PRINT_VAL(keys[i]);
					PRINT_STR(" ");
				}
				PRINT_LN;

				PRINT_STR("ptrs: ");
				PRINT_LN;

				for (Index8 i = 0; i != count + 1; i += 1) {
					PRINT_VAL(i);
					PRINT_STR(":");
					PRINT_HEX((Reg64) ptr[i]);
					PRINT_STR(" ");
				}
				PRINT_LN;

			}

        };

        void add_split_up(VoidP ptr, Key key) {
            WATCH("key", key);
            if (ptr == nullptr) {
                TRACE;
                DataBlock & blk = req_datablk();
                blk.parent = nullptr;
                blk.count = 1;
                blk.keys[0] = key;
                top = to_voidp(&blk);
                return;
            }

            DataBlock & blk0 = to_datablk(ptr);
            if (blk0.avail()) {
                blk0.add_key(key);
                return;
            }
            TRACE;

            Key pivot;
            DataBlock & blk1 = req_datablk();
            VoidP q = to_voidp(&blk1);
            VoidP s = to_voidp(&blk0);

            TreeFn::split_key(blk0.count, blk1.count, blk0.keys, blk1.keys, pivot);

            if (key < pivot)
                blk0.add_key(key);
            else
                blk1.add_key(key);

            VoidP p = blk0.parent;
            Key k = pivot;

            while (is_nn(p)) {
                TRACE;
                IndexBlock & idx0 = to_indexblk(p);
                s = to_voidp(&idx0);
                idx0.print();
                if (idx0.avail()) {
                    TRACE;
                    idx0.add_key_ptr(k, q);
                    set_parent(q, p);
                    return;
                }
                TRACE;
                IndexBlock & idx1 = req_indexblk();
                TreeFn::split_key_ptr(idx0.count, idx1.count, idx0.keys, idx1.keys, idx0.ptr, idx1.ptr, pivot);
                if (k < pivot) {
                    idx0.add_key_ptr(k, q);
                    set_parent(q, to_voidp(&idx0));
                } else {
                    idx1.add_key_ptr(k, q);
                    set_parent(q, to_voidp(&idx1));
                }
                k = pivot;
                q = to_voidp(&idx1);
                p = idx0.parent;
            }

            TRACE;
            WATCH("k", k);
            IndexBlock & idx = req_indexblk();
            top = to_voidp(&idx);

            idx.parent = nullptr;
            idx.count  = 1;
            idx.keys[0] = k;
            idx.ptr[0] = s;
            idx.ptr[1] = q;
            set_parent(s, top);
            set_parent(q, top);
            //idx.print();
        }

        void set_parent(VoidP q, VoidP p) {
            if (is_datablk(q)) {
                DataBlock & blk = to_datablk(q);
                blk.parent = p;
            } else {
                IndexBlock & idx = to_indexblk(q);
                idx.parent = p;
            }
        }


        void create_first_data_block(Key k) {
            DataBlock & blk = req_datablk();
            blk.count = 1;
            blk.keys[0] = k;
            blk.parent = nullptr;
            top = to_voidp(&blk);
        }

        void create_first_index_block(Key k) {
            DataBlock & blk0 = to_datablk(top);
            DataBlock & blk1 = req_datablk();
            IndexBlock & idx = req_indexblk();
            Key pivot;
            TreeFn::split_key(blk0.count, blk1.count, blk0.keys, blk1.keys, pivot);
            idx.count = 1;
            idx.keys[0] = pivot;
            idx.ptr[0] = to_voidp(&blk0);
            idx.ptr[1] = to_voidp(&blk1);
            top = to_voidp(&idx);
            if (k < pivot)
                add_to_data_block(idx.ptr[0], k);
            else
                add_to_data_block(idx.ptr[1], k);
        }

        void add_to_data_block(VoidP p, Key k) {
        }

        template <typename Fn>
        void each_indexblk(VoidP p, Fn fn) {
            IndexBlock & idx = to_indexblk(p);
            Index8 i = 0;
            Index8 n = idx.count;
            while (i != n) {
                each_datablk(idx.ptr[i], fn);
                fn(idx.keys[i++]);
            }
            each_datablk(idx.ptr[i], fn);
        }

        template <typename Fn>
        void each_datablk(VoidP p, Fn fn) {
            DataBlock & blk = to_datablk(p);
            Index8 i = 0;
            Index8 n = blk.count;
            while (i != n)
                fn(blk.keys[i++]);
        }

        Bool is_datablk(VoidP p) {
            DataBlock * blk = (DataBlock *) p;
            return blk->code == 1;
        }

        Bool is_indexblk(VoidP p) {
            IndexBlock * blk = (IndexBlock *) p;
            return blk->code == 2;
        }

        DataBlock & to_datablk(VoidP p) {
            DataBlock * blk = (DataBlock *) p;
            return * blk;
        }

        IndexBlock & to_indexblk(VoidP p) {
            IndexBlock * idx = (IndexBlock *) p;
            return * idx;
        }

        DataBlock * to_datablkp(VoidP p) {

            return (DataBlock * ) p;
        }
        IndexBlock * to_indexblkp(VoidP p) {
            return (IndexBlock *) p;
        }

        VoidP to_voidp(DataBlock * blk) { return (VoidP) blk;}
        VoidP to_voidp(IndexBlock * idx) { return (VoidP) idx;}

        DataBlock & req_datablk() {
            VoidP p = malloc(sizeof(DataBlock));
            DataBlock * blk = to_datablkp(p);
            blk->init();
            return *blk;
        }
        IndexBlock & req_indexblk() {
            VoidP p = malloc(sizeof(IndexBlock));
            IndexBlock * idx = to_indexblkp(p);
            idx->init();
            return *idx;
        }

        Bool is_nn(VoidP p) { return p != nullptr;}
        Bool is_null(VoidP p) { return p == nullptr;}



        void dump_recursive(VoidP p) {
            if (is_indexblk(p)) {
                IndexBlock & idx = to_indexblk(p);
                idx.print();
                for (Index8 i = 0; i <= idx.count; i += 1) {
                    dump_recursive(idx.ptr[i]);
                }
            }
            if (is_datablk(p)) {
                DataBlock & data = to_datablk(p);
                data.print();
            }
        }

    };





}; // namespace Tree
}; // namespace Arca




