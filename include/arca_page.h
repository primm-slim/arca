
namespace Arca {
	// Arca
	
namespace Page {
	// Arca::PageFn

	template <typename Index, typename Elem, SizeT N>
	struct SimpleArrayPage {
		SimplePage * next;
		Index count;
		Elem data[N];

		Index max_len() {return N;}

	};

	template <typename Page>
	class PageArray {
	public:
		PageArray(
	};


	template <typename Elem>
	struct SimpleFwdNode {
		Elem val;
		SimpleFwdNode * next;
	};

	template <typename Elem>
	struct SimpleFwdNodeAdapter {
		using Node = SimpleFwdNode<Elem>;
		using Ref = Node *;

		Node & get_node(Ref p) { return *p;}
		Ref & next_ptr(Ref p) { return p->next;}
		static Ref null_ref() { return nullptr;}

	};

	template <typename Adapter>
	class FwdChain {
	public:
		using Ref = typename Adapter::Ref;
		using Node = typename Adapter::Node;

		FwdChain(Ref itop) : top(itop) {}

		void reset() { set_null(top);}
		void pushf(Ref p) {
			set_next(p, top);
			top = p;
		}

		Bool empty() { return is_null(top);}

		Bool avail() { return is_nn(top);}

		Ref pop() {
			Ref p = top;
			top = get_next(p);
		}

		template <typename F>
		void each_node() {
			Ref p = top;
			while (is_nn(p)) {
				f(p);
				p = get_next(p);
			}
		}

	private:
		Ref & top;

		// uses adapter
		
		void set_null(Ref & p) { p = Adapter::null_ref();}
		Bool is_null(Ref p) { return p == Adapter::null_ref();}
		Bool is_nn(Ref p) { return p != Adapter::null_ref();}
		Ref & next_ptr(Ref p) { return Adapter::next_ptr(p);}
		void set_next(Ref p, Ref x) { next_ptr(p) = x;}
		Ref get_next(Ref p) { return Adapter::next_ptr(p);}
		Node & from_ref(Ref p) { return Adapter::get_node(p);}

	};


}; // namespaceChainFn 
}; // namespace Arca


