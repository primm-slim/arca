

// Arca Btree
//

namespace Arca {
	// Arca
	
namespace Btree {
// Arca::Btree

	// this btree is highly efficient and uses dynamic arrays
	



	template <typename KeyT>
	struct SimpleNode {
		using Key = KeyT;
		Key key;
		U32 code;
		SimpleNode * parent, left, right;
	};

	
	template <typename NodeT>
	struct SimpleNodeAdapter {
		using Node = NodeT;
		using Key = Node::Key;
		using Ref = Node *;

		Ref & left_ref(Ref e) {return e->left;}
		Ref & right_ref(Ref e) {return e->right;}
		Ref & top_ref(Ref e) {return e->top;}
		U32 & code_ref(Ref e) { return -> e->code;}
		Key & key_ref(Ref e) { return e->key;}
		static void null_ref() { return nullptr;}

		/*
		static void set_null(Ref & e) { e = nullptr;}
		static Bool is_null(Ref & e) { return e == nullptr;}
		static Bool is_nn(Ref & e) { return e != nullptr;}
		*/
	};

		
	template <typename Adapter>
	class SearchTree {
	public:
		using Key = Adapter::Key;
		using Node = Adapter::Node;
		using Ref = Adapter::Ref;

		void add(Ref e) {
			if (is_null(top)) {
				add_first_node(e);
				return;
			}

			Ref q = null_ref();
			Ref p = top;
		}



	private:

		Iso run_search(Ref e, bool stop_match) {
			Iso iso;
			iso.q = null_ref();
			iso.p = top;
			iso.depth = 0;
			LOOP {
				if ((key(e) == key(p)) && stop_match) {
					iso.match_flag = 1;
					return;
				}

			};



		Ref & top;
		U32 next_code;

		void gen_next_code() {
			next_code ^= next_code << 13;
			next_code ^= next_code >> 17;
			next_code ^= next_code << 5;
		}




	};



}; // namespace Tree
}; // namespace Arca




