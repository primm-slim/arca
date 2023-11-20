
namespace ArrayFn {

	template <typename Index, typename Elem>
	void clear(Elem * st, Index ct, Elem val) {
		Elem * p = st;
		Elem * e = p + ct;
		while (p != e) {*p++ = val;}
	}

	template <typename Index, typename Elem>
	void copy(Elem * dest, Elem * src, Index ct) {
		Elem * p = src;
		Elem * e = src + ct;
		Elem * q = dest;
		while (p != e) { *q++ = *p++;}
	}

	template <typename Index, typename Elem>
	void rcopy(Elem * dest, Elem * src, Index ct) {
		Elem * p = src + ct;
		Elem * e = src;
		Elem * q = dest + ct;
		while (p != e) {
			*--q = *--p;
		}
	}

	template <typename Index, typename Elem>
	Index find(Elem * st, Index ct, Elem val) {
		Index i = 0;
		while (i != ct) {
			if (st[i] == val) break;
			i += 1;
		}
		return i;
	}

	template <typename Index, typename Elem>
	Index match(Elem * a, Elem * b, Index ct) {
		Index i = 0;
		while ((i != ct) && (a[i] == b[i])) {
			i += 1;
		}
		return i;
	}


	template <typename Index, typename Elem>
	void inline swap_elem(Elem * buf, Index x, Index y) {
		Elem a, b;
		a = buf[x]; b = buf[y];
		buf[x] = b; buf[y] = a;
	}

	template <typename Index, typename Elem>
	void ins_sort(Elem * buf, Index ct) {
		if (ct <= 1) return;
		Index i = 1;
		while (i < ct) {
			Index j = i;
			while ((j > 0) && (buf[j-1] > buf[j])) {
				swap_elem<Index, Elem>(buf, j-1, j);
				j -= 1;
			}
			i += 1;
		}
	}

	template <typename Index, typename Elem>
	Index check_heap(Elem * buf, Index n) {
		for (Index i = 0; i < n; i ++) {
			Index pos = i;
			Index left = i * 2 +1;
			Index right = i * 2 + 2;
			if ((left < n) && (buf[pos] < buf[left]))
				return 1;
			if ((right < n) && (buf[pos] < buf[right]))
				return 1;
		}
		return 0;
	}


	template <typename Index, typename Elem>
	void heapify(Elem * buf, Index pos, Index n) {
		Index high = pos;
		Index left = 2 * pos + 1;
		Index right = 2 * pos + 2;


		if ((left < n) && (buf[left] > buf[pos])) 
			high = left;
		if ((right < n) && (buf[right] > buf[high]))
			high = right;
		if (high != pos) {
			swap_elem<Index, Elem>(buf, pos, high);
			heapify<Index, Elem>(buf, high, n);
		}
	}

	template <typename Index, typename Elem>
	void heap_sort(Elem * buf, Index n) {
		for (Index i = n / 2 ; i > 0; i--)
			heapify<Index, Elem>(buf, i - 1, n);

		for (Index i = n - 1; i > 0; i--) {
			swap_elem<Index, Elem>(buf, 0, i);
			heapify<Index, Elem>(buf, 0, i);
		}
	}

	template <typename Index, typename Elem>
	Index quick_sort_partition(Elem * buf, Index low, Index high) {
		Index pivot = buf[high];
		Index i = low;

		for (Index j = low; j <= high - 1; j++) {
			if (buf[j] < pivot) {
				swap_elem<Index, Elem>(buf, i, j);
				i += 1;
			}
		}
		swap_elem<Index, Elem>(buf, i, high);
		return i;
	}


	template <typename Index, typename Elem>
	void quick_sort_step(Elem * buf, Index low, Index high) {
		if (low < high) {
			Index pi = quick_sort_partition(buf, low, high);
			if (pi > 0)
				quick_sort_step<Index, Elem>(buf, low, pi - 1);
			quick_sort_step<Index, Elem>(buf, pi + 1, high);
		}
	}

	template <typename Index, typename Elem>
	void quick_sort(Elem * buf, Index n) {
		quick_sort_step<Index, Elem>(buf, 0, n - 1);
	}

	template <typename Index, typename Elem>
	Index check_sort(Elem * buf, Index n) {
		if (n <= 1) return 0;
		Index i = 1;
		while (i < n) {
			if (buf[i] < buf[i-1]) return i;
			i += 1;
		}
		return i;
	}

	template <typename Index, typename Elem>
	Index bsearch(Elem * buf, Index n, Elem val) {
		Index l, h, m;
		Elem x;

		l = 0; h = n - 1;

		while (l < h) {
			m = l + (h - l) / 2;
			if (m ==l)
				break;
			x = buf[m];
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
	class FwdSeq {
	public:
		FwdSeq(Elem * p, Index n) {
			data = p; len = n; pos = 0;
		}
		Elem & operator*() {
			return data[pos];
		}
		void operator++() {
			pos += 1;
		}
		operator bool() {
			return pos != len;
		}
	private:
		Index pos, len;
		Elem * data;
	};

	template <typename Index, typename Elem>
	class RevSeq {
	public:
		RevSeq(Elem * p, Index n) {
			data = p; len = n; pos = n;
		}
		Elem & operator*() {
			return data[pos - 1];
		}
		void operator++() {
			pos -= 1;
		}
		operator bool() {
			return pos != 0;
		}
	private:
		Index pos, len;
		Elem * data;
	};





};


