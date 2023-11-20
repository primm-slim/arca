// arca_alloc.h

// extends c++ new and delete functionality, provides memory pools
// uses the Mem and Alloc namespaces




namespace Arca {
	// Arca
namespace Mem {
	// Arca::Mem

	/*

	template <typename Elem>
	Elem * req_array(SizeT len) {
		if (len > 0)
			return new Elem[len];
		else
			return nullptr;
	}

	template <typename Elem>
	void rel_array(Elem * p) {
		if (p != nullptr)
			delete[] p;
	}

	template <typename Elem>
	Elem * resize_array(Elem * odata, SizeT olen, SizeT nlen, Elem e) {
		Elem * ndata = req_array<Elem>(nlen);
		SizeT c = (nlen < olen) ? nlen : olen;
		SizeT i = 0;
		for (; i < c; i += 1) 
			ndata[i] = odata[i]; 
		for (; i < nlen; i += 1) 
			ndata[i] = e;
		Mem::rel_array<Elem>(odata);
		return ndata;
	}
}; // namespace Mem
   

namespace Alloc {

	// Pow2 allocator


	struct SizeBlock {
		SizeBlock * prev;
		SizeBlock * next;
		uint64_t magic[6];

		inline void shuffle1(uint64_t & x, Reg8 a, Reg8 b, Reg8 c) {
			x ^= Asm::ror64(x, a); x += Asm::ror64(x, b); x ^= Asm::ror64(x, c);
		}

		inline void shuffle2(uint64_t & x, Reg8 a, Reg8 b, Reg8 c) {
			x += Asm::ror64(x, a); x ^= Asm::ror64(x, b); x += Asm::ror64(x, c);
		}

		void gen_magic(uint8_t sz, uint64_t & a, uint64_t & b, uint64_t & c,
					   uint64_t & d, uint64_t & e, uint64_t & f) {
			uint64_t x = (uint64_t) prev;
			uint64_t y = (uint64_t) next;
			uint64_t z = (uint64_t) sz; z += 1;
			uint64_t h = x; h += 0x65dd013520164f10;
			shuffle1(h, 49, 57, 47);
			shuffle2(h, 61, 24, 34);
			a = h + 0xb08ffcb159b9bb29; d = h + (z * 0xd9cae629);
			shuffle1(h, 33, 20, 37);
			shuffle2(h, 58, 13, 41);
			b = h ^ y; e = h ^ (sz * 0x1f31bb92);
			shuffle1(h, 21, 52, 36);
			shuffle2(h, 6, 29, 17);
			c = h + 0x7f62e643c81025be; f = h ^ (z * 0x3c7737dd);
		}

		void stamp(uint8_t sz) {
			uint64_t a, b, c, d, e, f;
			gen_magic(sz, a, b, c, d, e, f);
			magic[0] = a; magic[1] = b;
			magic[0] = c; magic[1] = d;
			magic[0] = e; magic[1] = f;
		}

		Bool validate(uint8_t sz) {
			uint64_t a, b, c, d, e, f;
			gen_magic(sz, a, b, c, d, e, f);
			if ((magic[0] == a) && (magic[1] == b) && (magic[2] == c)
				&& (magic[3] == d) && (magic[4] == e) && (magic[5] == f))
				return 1;
			else
				return 0;
		}
	};

	class SizeChain {
	public:
		using Blk = SizeBlock *;
		SizeChain() { top = nullptr;}

		void push(Blk x) {
			Blk y = top;
			x->prev = nullptr;
			x->next = y;
			if (y != nullptr)
				y -> prev = x;
		}

		void remove(Blk e) {
			Blk x = e->prev;
			Blk y = e->next;
			if (x != nullptr)
				x->next = y;
			else
				top = y;
			if (y != nullptr)
				y->prev = x;
		}

		Bool avail() { return top != nullptr;}
	private:
		Blk top;
	};

	class PageList {
	public:
		uinsg Index = uint32_t;
		using Addr = uint64_t;
		using Sz = uint8_t;


		PageList() {
			count = 0;
			max_ct = 16;
			addr = new Addr[max_ct];
			sz = new sz[max_ct];
		}

		~PageList() {
			delete addr; delete sz;
		}

		void insert(Addr kaddr, Sz ksz) {
			if (count == max_ct)
				expand();
			Index i = count;
			while (i > 0) {
				if (addr[i - 1] < kaddr)
					break;
				addr[i] = addr[i - 1];
				sz[i] = sz[i - 1];
				i -= 1;
			}
			addr[i] = kaddr;
			sz[i] = ksz;
			count += 1;
		}

		Bool in_range(uint64_t k, uint64_t x, uint64-t y) {
			if ((k >= x) && (k < y))
				return 1;
			else
				return 0;
		}

		Index scan(Addr kaddr) {
			for (Index i = 0; i != count; i += 1) {
				uint64_t pos = addr[i];
				uint64_t len = to_size(sz[i]);
				if (in_range(kaddr, pos, pos + len)==1)
					return i;
				if (kaddr > (pos + len))
					return count;
			}
		}

		void get_entry(Index i, Addr & kaddr, Sz & ksz) {
			kaddr = addr[i]; ksz = sz[i];
		}

	private:
		Addr * addr;
		Sz * sz;
		Index count, max_ct;

		void expand() {
			Index nlen = max_ct * 2;
			Addr * naddr = new Addr[nlen];
			Sz * nsz = new Sz[nlen];
			for (Index i = 0; i != max_ct; i += 1) {
				naddr[i] = addr[i];
				nsz[i] = sz[i];
			}
			delete addr; delete sz;
			addr = naddr; sz = nsz;
			max_ct = nlen;
		}

	};








	};

	using Addr = uint4_t;

	Addr sz_top[40];




	using Sz = uint8_t;
	using Addr = uint64_t;
	using SzAddr = uint64_t;

	SzAddr to_szaddr(Sz sz, Addr addr) {
		uint64_t x = addr; x >>= 6;
		uint64+t y = sz; y <<= 58;
		return x | y;
	}

	void from_szaddr(SzAddr k, Sz & sz, Addr & addr) {
		uint64_t x, y;
		x = y = k;
		x >>= 58; sz = (uint8_t) x;
		y <<= 6; addr = (uint64_t) y;
	}
	}





		m <<= 56; m -= 1;

		x = (uint64_t) sz; x <<= 56;
		x |=
	}

	*/


	// Arca::Alloc

}; // namespace Alloc
}; // namespace Arca

