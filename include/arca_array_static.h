
// arca_array_dyn.h
//
namespace Arca {
	// Arca
	

	template <typename Index, typename Elem, SizeT N>
	class StaticArray {
	public:
		using ElemPtr = Elem *;

		Index maxlen() { return N;}

		template <typename F>
		void each(F f) { array_fn().each(f);}
		template <typename F>
		void each_index(F f) { array_fn().each_index(f);}
		void fill(Elem e) { array_fn().fill(e);}
		void fill(Elem e, Index p, Index n) { array_fn().fill(e, p, n);}

		template <typename F>
		Bool check_any(F f) { return array_fn().check_any(f);}
		template <typename F>
		Bool check_all(F f) { return array_fn().check_all(f);}

		Elem find_max_elem() { return array_fn().find_max_elem();}
		Elem find_min_elem() { return array_fn().find_min_elem();}
		Pair<Elem, Elem> find_min_max_elem() { return array_fn().find_min_max_elem();}
		void reverse() { return array_fn().reverse();}
		void clear() { return array_fn().clear();}

		template <typename F>
		void map(F f) { array_fn().map(f);}

		template <typename F>
		void map_to(ElemPtr p, F f) { array_fn().map_to(p, f);}

		Index find(Elem e) { return array_fn().find(e);}
		Index match_buf(ElemPtr p) { return array_fn().match_buf(p);}
		void copy_to(ElemPtr p) { return array_fn().copy_to(p);}
		void copy_from(ElemPtr p) { return array_fn().copy_from(p);}
		void swap_elem(Index i, Index j) { return array_fn().swap_elem(i, j);}
		void ins_sort() { array_fn().ins_sort();}
		void quick_sort() { array_fn().quick_sort();}
		Index check_sort() { return array_fn().check_sort();}
		Index bsearch(Elem val) { return array_fn().bsearch(val);}
		void shl(Index pos, Index n, Index d) { array_fn().shl(pos, n, d);}
		void shr(Index pos, Index n, Index d) { array_fn().shr(pos, n, d);}

		Elem & operator[] (Index i) { return data[i];}


	private:
		Array::ArrayFn<Index, Elem> array_fn() { return Array::ArrayFn<Index, Elem>(data, N);}
		Elem data[N];
	};

	template <typename Index, typename Elem, SizeT N>
	class StaticArrayList {
	public:
		using ElemPtr = Elem *;

		void clear() { array_fn().clear();}
		void pushf(Elem e) { array_fn().pushf(e);}
		void pushb(Elem e) { array_fn().pushb(e);}
		void append(Elem e) { array_fn().append(e);}
		Elem popf() { return array_fn().popf();}
		Elem popb() { return array_fn().popb();}
		void del_at(Index pos) { array_fn().del_at(pos);}
		void insert(Elem e, Index pos) { array_fn().insert(e, pos);}
		Index find(Elem e, Index pos) { array_fn().find(e, pos);}

		template <typename F>
		void keep_if(F f) { array_fn().keep_if(f);}

		template <typename F>
		Bool check_any(F f) { return array_fn().check_any(f);}
		template <typename F>
		Bool check_all(F f) { return array_fn().check_all(f);}

		Elem find_max_elem() { return array_fn().find_max_elem();}
		Elem find_min_elem() { return array_fn().find_min_elem();}
		Pair<Elem, Elem> find_min_max_elem() { return array_fn().find_min_max_elem();}
		void reverse() { return array_fn().reverse();}

		template <typename F>
		void map(F f) { array_fn().map(f);}

		template <typename F>
		void map_to(ElemPtr p, F f) { array_fn().map_to(p, f);}

		Index find(Elem e) { return array_fn().find(e);}
		Index match_buf(ElemPtr p) { return array_fn().match_buf(p);}
		void copy_to(ElemPtr p) { return array_fn().copy_to(p);}
		void copy_from(ElemPtr p) { return array_fn().copy_from(p);}
		void swap_elem(Index i, Index j) { return array_fn().swap_elem(i, j);}
		void ins_sort() { array_fn().ins_sort();}
		void quick_sort() { array_fn().quick_sort();}
		Index check_sort() { return array_fn().check_sort();}
		Index bsearch(Elem val) { return array_fn().bsearch(val);}
		void shl(Index pos, Index n, Index d) { array_fn().shl(pos, n, d);}
		void shr(Index pos, Index n, Index d) { array_fn().shr(pos, n, d);}

		Index elem_ct() { return count;}

		Index maxlen() { return N;}
	private:
		Array::ArrayListFn<Index, Elem> array_fn() { return Array::ArrayListFn<Index, Elem>(data, N, count);}

		Elem data[N];
		Index count;
	};

	template <typename Index, typename Elem, SizeT N>
	class StaticArrayStack {
	public:
		Index maxlen() { return N;}
	private:
		Elem data[N];
		Index count;
	};

	template <typename Index, typename Elem, typename Base, SizeT N>
	class StaticArrayHeap {
	public:
		using ElemPtr = Elem *;
		void clear() { heap().clear();}
		void reset() { heap().reset();}

		Bool is_empty() { return heap().is_empty();}
		Bool is_full() { return heap().is_full();}
		void push(Elem e) { heap().push(e);}
		Elem pop() { return heap().pop();}
		Elem peek() { return heap().peek();}
		Index elem_ct() { return count;}
		void load_from(ElemPtr buf, Index n) { heap().load_from(buf, n);}
		void store_to(ElemPtr buf) { heap().store_to(buf);}



		Index maxlen() { return N;}
	private:
		Array::ArrayHeap<Index, Elem, Base> heap() { return Array::ArrayHeap<Index, Elem, Base>(data, count, N);}
		Elem data[N];
		Index count;
	};

	template <typename Index, typename Elem, SizeT N>
	class StaticArraySet {
	public:
		Index maxlen() { return N;}
	private:
		Elem data[N];
		Index count;
	};


}; // namespace Arca


