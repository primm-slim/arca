
// Arca Tree
//

namespace Arca {
	// Arca
	
namespace Tree {
// Arca::Tree

	// Arca does not have conventional binary trees, it only uses a higher order B+ tree.  This
	// is far more efficient
	

	
	template <typename Key, size_t N>
	class TreeSet {
	public:
		//friend class TreeTester<Key, N>;


		TreeSet() {
			clear_free_list();
			top = nullptr;
			tree_flag = 0;
		}

		void add_key(Key k) {
			//WATCH("k",k);
			if (tree_flag == 0) {
				top = to_voidp(create_first_key_block(k));
				tree_flag = 1;
				return;
			}

			if (tree_flag == 1) {
				KeyBlock * blk = to_key_block(top);
				if (blk->avail()>0) {
					blk->insert(k);
					return;
				} else {
					top = to_voidp(create_first_ptr_block(blk));
					tree_flag = 2;
					// fall through
				}
			}

			if (tree_flag == 2) {
				PtrBlock * pb = to_ptr_block(top);
				add_key_from_top(pb, k);
			}
		}

		Bool has_key(Key k) {
			if (tree_flag == 0)
				return 0;

			if (tree_flag == 1) {
				KeyBlock * kb = to_key_block(top);
				return kb->has_key(k);
			}

			if (tree_flag == 2) {
				PtrBlock * pb = to_ptr_block(top);
				return has_key_from_top(pb, key);
			}
		}

		Bool remove_key(Key k) {
			if (tree_flag == 0)
				return 0;

			if (tree_flag == 1) {
				KeyBlock * kb = to_key_block(top);
				if (blk->has_key(k)==0)
					return 0;
				blk->remove_key(k);
				if (blk->count==0) {
					rel(kb);
					top = nullptr;
					tree_flag = 0;
				}
				return 1;
			}

			if (tree_flag == 2) {
				PtrBlock * pb = to_ptr_block(top);
				return remove_key_from_top(pb, key);
			}
		}




	private:

		VoidP top;
		KeyBlock * key_free;
		PtrBlock * ptr_free;

		U8 tree_flag;
		U8 ptr_free_ct;
		U8 key_free_ct;


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



}; // namespace Tree
}; // namespace Arca




