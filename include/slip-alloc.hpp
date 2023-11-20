


// Header for custom memory allocator
// roundes up requests to standard sizes
// e.g. 32, 48, 64, 96, etc
// also has a simple cache for to minimize
// allocations of smaller chunks



namespace Alloc {

    using Sz = uint8_t;
    using Index = uint32_t;

/* unused

    static SizeT sz_to_nlen(Sz sz) {
        SizeT x = 32;
        SizeT y = 16;
        SizeT z = 8;

        Sz i = 0;
        LOOP {
            if (i <= sz + 2)
                break;
            x <<= 1; y <<= 1; z <<= 1;
            i += 3;
        }
        if (i == sz)
            return x;
        if (i == sz + 1)
            return x + y;
        if (i == sz + 2)
            return x + y + z;

        // uncreachable
        return 0;
    }
*/

    static void len_to_nlen_sz(SizeT len, SizeT & nlen, Sz & sz) {

        SizeT x = 32;
        SizeT y = 16;
        SizeT z = 8;

        sz = 0xff;
        Sz i = 0;

        LOOP {
            if (len <= x + y + z)
                break;
            x <<= 1; y <<= 1; z <<= 1;
            i += 3;
        }

        if (len <= x) {
            sz = i; nlen = x;
            return;
        }

        if (len <= x + y) {
            sz = i + 1; nlen = x + y;
            return;
        }

        if (len <= x + y + z) {
            sz = i + 2; nlen = x + y + z;
            return;
        }
        // unreachable
        return;
    }


    class MemAlloc {
    public:

        void init() {
            Mem::clear<Sz>(slot_sz, 256, 0xff);
            zpos = 0;
        }

        CharP req(SizeT len) {
            Sz sz; SizeT nlen;
            len_to_nlen_sz(len, nlen, sz);
            CharP p = nullptr;
            if (nlen <= 65536)
                p = find_free_chunk(sz);
            if (p == nullptr)
                p = Mem::req_array<Char>(nlen);
            return p;
        }

        void rel(CharP p, SizeT len) {
            Sz sz; SizeT nlen;
            len_to_nlen_sz(len, nlen, sz);

            if (nlen  <= 65536)
                store_free_chunk(sz, p);
            else
                Mem::rel_array<Char>(p);
        }

    private:

        void calc_pos(Sz sz, Index & st, Index & en) {
            Index i = (Index) (sz & 15);
            st = i * 16;
            en = st + 16;
        }

        Bool search_val(Sz sz, Index & pos, Sz val) {
            Index st, en;
            calc_pos(sz, st, en);
            for (pos = st; pos != en; pos += 1) {
                if (slot_sz[pos] == val)
                    return 1;
            }
            return 0;
        }

        void make_space_avail(Sz sz, Index & pos) {
            Index st, en;
            calc_pos(sz, st, en);
            pos = st + zpos;
            zpos += 1; zpos &= 15;
            Mem::rel_array<Char>(slot_buf[pos]);
            slot_sz[pos] = 0xff;
        }


        CharP find_free_chunk(Sz sz) {
            Index pos;
            if (search_val(sz, pos, sz)) {
                slot_sz[pos] = 0xff;
                return slot_buf[pos];
            }
            return nullptr;
        }

        void store_free_chunk(Sz sz, CharP p) {
            Index pos;
            if (search_val(sz, pos, 0xff)==0)
                make_space_avail(sz, pos);

            slot_sz[pos] = sz;
            slot_buf[pos] = p;
            return;

        }

        CharP slot_buf[256];
        Sz slot_sz[256];
        Index zpos;

    };

}; // namespace

Alloc::MemAlloc mem_alloc;


