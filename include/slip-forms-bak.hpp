
// use a 32-bit index for everything

using Index = uint32_t;



// Simple string structure



struct Str {

    CharP buf;
    SizeT len;

    Str() {}
    Str(CharP p, SizeT n) : buf(p), len(n) {}


    Bool match(CharP p, SizeT n) {
        if (n != len)
            return 0;
        return Mem::match(buf, p, len);
    }
    void copy_to(CharP dest) {
        Mem::copy(dest, buf, len);
    }

    // used for testing, requires <cstring>

#ifdef ARCA_TESTER

    Str(const char * p) {
        buf = (CharP) p;
        len = strlen(p);
    }

    Bool match_cstr(const char * inp) {
        CharP p = (CharP) inp;
        for (Index i = 0; i != len; i += 1) {
            if (*p++ != buf[i])
                return 0;
        }
        if (*p != 0)
            return 0;
        else
            return 1;
    }

    void print_str() {
        int d = (int) len;
        printf("%d:", d);
        printf("%.*s", (int) len, buf);
    }
#endif

};


// Struct that holds a Key and Value
// with associated malloc / free operations
// there is no "empty" state, that has to be
// tracked somewhere else

struct Entry {
    CharP buf;
    Index key_len;
    Index val_len;

    using Index = uint32_t;

    void acquire(Str key_str, Str val_str) {
        key_len = (SizeT) key_str.len;
        val_len = (SizeT) val_str.len;

        SizeT len = calc_len();
        buf = mem_alloc.req(len);
        key_str.copy_to(buf);
        val_str.copy_to(buf +(SizeT) key_len);
    }

    void release() {
        if (buf != nullptr)
            mem_alloc.rel(buf, calc_len() );
    }

    Bool match_key(Str key_str) {
        return key_str.match(buf, (SizeT) key_len);
    }

    Str fetch_val() {
        return Str(buf + (SizeT) key_len, (SizeT) val_len);
    }

    Bool match_val(Str val_str) {
        return val_str.match(buf + (SizeT) key_len, (SizeT) val_len);
    }

    SizeT calc_len() {
        return ((SizeT) key_len + (SizeT) val_len);
    }
};

// For each key, a 32-bit hash code is calculated which is used to help locate
// the item and find it in searches.  The code is both a non-unique identifier
// (as there may be collissions) and a block selector.  The 32-bit code is mapped
// into two 24-bit indexes (k0 and k1), and each, after mapping,  selects one block
// to search, which means each entry is in one of two blocks.  Code > 0, as 0 means
// an empty entry.


using Code = uint32_t;

Code hash_key(Str key_str) {
    Code code = hash_char_array(key_str.buf, key_str.len);
    if (code != 0)
        return code;
    else
        return code ^ 1;
}

void gen_hash_index(Code code, Index nblk, Index & k0, Index & k1) {
    Index m = nblk - 1;
    k0 = code; k1 = (~code) >> 8;
    k0 &= m; k1 &= m;
    if (k0 == k1)
        k1 ^= 1;
}


// BlkFn classes define functions on a block of entries and keycodes
// Blocks do not exist as sepearate entities, they are created by
// referencing slices of existing arrays as needed.  They are independent
// of each other and represent different strategies for storing blocks
// of entries.  Each key maps to two different blocks and is always stored
// in the block with fewest keys.




// Simplest block manager, each block is one entry, count is either 0 or 1
// Used to jumpstart development and benchmark other block managers.

class SimpleBlockFn {
public:
        void init(Index n) {
            nrows = n;
            entry = Mem::req_array<Entry>(n);
            key_code = Mem::req_array<Code>(n);
        }

        void destroy() {
            Mem::rel_array<Entry>(entry);
            Mem::rel_array<Code>(key_code);
        }

        Index get_block_ct() {
            return nrows;
        }

        Index get_row_ct() {
            return nrows;
        }

        // void resize(Index n, Bool keep_flag) {}

        Bool search_pos(Index k, Code code, Str key_str, Index & pos) {
            if (key_code[k] == code) {
                pos = k;
                Entry & e = entry[pos];
                return e.match_key(key_str);
            } else
                return 0;
        }

        Bool find_free_pos(Index k, Code code, Index & pos) {
            UNUSED(code);
            pos = k;
            return (key_code[pos] == 0);
        }

        Index get_count(Index k) {
            Index pos = k;
            if (key_code[pos] == 0)
                return 0;
            else
                return 1;
        }

        void del_pos(Index k, Index pos) {
            UNUSED(k);
            Entry & e = entry[pos];
            e.release();
            key_code[pos] = 0;
        }

        Code & get_key_code(Index k, Index pos) {
            UNUSED(k);
            return key_code[pos];
        }

        Entry & get_entry(Index k, Index pos) {
            UNUSED(k);
            return entry[pos];
        }

private:
        Entry * entry;
        Code * key_code;
        Index nrows;
};


// Row-based block manager, each block is a fixed # of rows with a counter,
// entries are added in code order

class RowBlockFn {
public:

        void init(Index n) {
            nrows = n;
            entry = Mem::req_array<Entry>(n);
            key_code = Mem::req_array<Code>(n);
            Mem::clear<Code>(key_code, n, 0);
            zpos = 0;
        }

        void destroy() {
            Mem::rel_array<Entry>(entry);
            Mem::rel_array<Code>(key_code);
        }

        Index get_block_ct() {
            return nrows / 4;
        }

        Index get_row_ct() {
            return nrows;
        }

        // void resize(Index n, Bool keep_flag) {}

        Bool search_pos(Index k, Code code, Str key_str, Index & pos) {
            Index st, en;
            calc_block_info(k, st, en);

            for (pos = st; pos != en; pos += 1) {
                if (key_code[pos] == code) {
                    Entry & e = entry[pos];
                    if (e.match_key(key_str)==1)
                        return 1;
                }
            }
            return 0;
        }

        Bool find_free_pos(Index k, Code code, Index & pos) {
            UNUSED(code);

            Index st, en;
            calc_block_info(k, st, en);

            PRINT_STR("TRACE3\n");
            if (key_code[en - 1] != 0) {
                pos = st + zpos;
                zpos += 1; zpos &= 7;
                return 1;
            }
            PRINT_STR("TRACE4\n");

            pos = en - 1;
            while (pos > st) {
                WATCH("pos",pos);
                if (key_code[pos - 1] < code) {
                    key_code[pos] = key_code[pos - 1];
                    entry[pos] = entry[pos - 1];
                    pos -= 1;
                } else
                    break;
            }
            return 0;
        }

        Index get_count(Index k) {
            Index st, en;
            calc_block_info(k, st, en);

            Index ct = 0;
            for (Index i = st; i != en; i += 1) {
                if (key_code[i] != 0)
                    ct += 1;
            }
            return ct;
        }

        void del_pos(Index k, Index pos) {
            Index st, en;
            calc_block_info(k, st, en);
            while (pos != en - 1) {
                key_code[pos] = key_code[pos + 1];
                entry[pos] = entry[pos + 1];
                pos += 1;
            }
            key_code[pos] = 0;
        }

        Code & get_key_code(Index k, Index pos) {
            UNUSED(k);
            return key_code[pos];
        }

        Entry & get_entry(Index k, Index pos) {
            UNUSED(k);
            return entry[pos];
        }

private:
        void calc_block_info(Index k, Index & st, Index & en) {
            st = k * 8;
            en = st + 8;
        }

        Entry * entry;
        Code * key_code;
        Index nrows;
        Index zpos;
};



// Slipcache proper

template <typename BlockFn>
class SlipCache {
public:
    void init(Index n) {
        blk.init(n);
        stats.reset();
    }

    void destroy() {
        blk.destroy();
    }

    // checks two blocks (k0, k1)
    // if both are full, will do a swap
    // and then try to store the item
    // that was swapped out, or discard
    // it if no space is available

    void set(Str key_str, Str val_str) {
        Index k0, k1;  Code code;
        code = hash_key(key_str);
        setup_hash_data(code, k0, k1);

        if (try_update_existing(code, k0, k1, key_str, val_str))
            return;

        stats.set_miss_ct += 1;
        Entry e;
        e.acquire(key_str, val_str);

        Bool retry_flag = 0;
        LOOP {
            Index k, pos;  Bool busy_flag;
            PRINT_STR("TRACE1\n");
            busy_flag = find_free_space(k0, k1, code, k, pos);
            PRINT_STR("TRACE2\n");
            WATCH("busy_flag", busy_flag);
            stats.check_free_ct += 1;

            if (busy_flag==0) {
                store_entry(k, pos, code, e);
                stats.find_free_ct += 1;

                return;
            }

            swap_entry(k, pos, code, e);
            if (retry_flag == 0) {
                stats.reloc_ct += 1;
                setup_hash_data(code, k0, k1);
                retry_flag = 1;
            } else {
                stats.slip_ct += 1;
                e.release();
                return;
            }
        }
    }

    Bool del(Str key_str) {
        Index k0, k1;  Code code;
        setup_hash_data(key_str, code, k0, k1);

        Index k, pos;
        if (match_existing_key(code, key_str, k0, k1, k, pos)) {
            del_entry(k, pos);
            return 1;
        }
        return 0;
    }

     Bool get(Str key_str, Str & val_str) {
        Index k0, k1;  Code code;
        setup_hash_data(key_str, code, k0, k1);

        Index k, pos;
        if (match_existing_key(code, key_str, k0, k1, k, pos)) {
            val_str = fetch_val(k, pos);
            stats.get_hit_ct += 1;
            return 1;
        }
        stats.get_miss_ct += 1;
        return 0;
    }

    CacheStats get_stats() { return stats;}

private:

    inline void setup_hash_data(Str key_str, Code & code, Index & k0, Index & k1) {
        code = hash_key(key_str);
        setup_hash_data(code, k0, k1);
    }

    inline void setup_hash_data(Code code, Index & k0, Index & k1) {
        Index nblk = blk.get_block_ct();
        gen_hash_index(code, nblk, k0, k1);
    }

    Bool match_existing_key(Code code, Str key_str, Index k0, Index k1, Index & k, Index & pos) {

        k = k0;
        if (blk.search_pos(k, code, key_str, pos))
            return 1;

        k = k1;
        if (blk.search_pos(k, code, key_str, pos))
            return 1;

        return 0;
    }

    Bool try_update_existing(Code code, Index k0, Index k1, Str key_str, Str val_str) {
        Index k, pos;
        if (match_existing_key(code, key_str, k0, k1, k, pos)) {
            Entry & e = blk.get_entry(k, pos);
            if (e.match_val(val_str))
                return 1;  // same key, same val, nothing to do
            e.release();
            e.acquire(key_str, val_str);
            stats.set_hit_ct += 1;
            return 1;
        } else
            return 0;
    }

    Bool find_free_space(Index k0, Index k1, Code code, Index & k, Index & pos) {
        Index ct0 = blk.get_count(k0);
        Index ct1 = blk.get_count(k1);

        k = (ct0 <= ct1) ? k0 : k1;
        return blk.find_free_pos(k, code, pos);
    }

    inline void store_entry(Index k, Index pos, Code code, Entry e) {
        Entry & entry = blk.get_entry(k, pos);
        Code & key_code = blk.get_key_code(k, pos);
        key_code = code;
        entry = e;
    }

    inline void swap_entry(Index k, Index pos, Code & code, Entry & e) {
        Entry & entry = blk.get_entry(k, pos);
        Code & key_code = blk.get_key_code(k, pos);
        swap_var<Entry>(entry, e);
        swap_var<Code>(key_code, code);
    }

    inline void del_entry(Index k, Index pos) {
        blk.del_pos(k, pos);
    }

    inline Str fetch_val(Index k, Index pos) {
       Entry & e = blk.get_entry(k, pos);
       return e.fetch_val();
    }

    BlockFn blk;
    CacheStats stats;

};

/*

void prefetch(VoidP p) {
    asm inline(
        "prefetcht0 %0\n\t"
        :
        : "m" (p)
        : "cc");
}

*/
