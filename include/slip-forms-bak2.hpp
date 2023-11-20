
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

inline Code hash_key(Str key_str) {
    Code code = hash_char_array(key_str.buf, key_str.len);
    if (code != 0)
        return code;
    else
        return code ^ 1;
}

inline void gen_hash_index(Code code, Index nblk, Index & k0, Index & k1) {
    Index m = nblk - 1;
    k0 = code; k1 = (~code) >> 8;
    k0 &= m; k1 &= m;
    if (k0 == k1)
        k1 ^= 1;
}

// Slipcache proper

class SlipCache {
public:
    using Ref = uint32_t;

    void init(Index n) {
        nrows = n;
        entry = Mem::req_array<Entry>(n);
        next_ref = Mem::req_array<Ref>(n);
        htop = Mem::req_array<Ref>(n);
        setup_chains();
        stats.reset();
        zpos = 0;
    }

    void destroy() {
        Mem::rel_array<Entry>(entry);
        Mem::rel_array<Ref>(next_ref);
        Mem::rel_array<Ref>(htop);
    }

    void set(Str key_str, Str val_str) {
        Index h = hash_key(key_str) % nrows;

        Index p, q;
        if (find_matching_entry(h, key_str, p, q)) {
            update_existing(p, key_str, val_str);
            stats.set_hit_ct += 1;
        } else {
            if (is_null(free_top))
                clear_space();
            add_entry(h, key_str, val_str);
            stats.set_miss_ct += 1;
        }
    }

    Bool get(Str key_str, Str & val_str) {
        Index h = hash_key(key_str) % nrows;

        Index p, q;
        if (find_matching_entry(h, key_str, p, q)) {
            val_str = fetch_val(p);
            stats.get_hit_ct += 1;
            return 1;
        } else {
            stats.get_miss_ct += 1;
            return 0;
        }
    }

    Bool del(Str key_str) {
        Index h = hash_key(key_str) % nrows;

        Index p, q;
        if (find_matching_entry(h, key_str, p, q)) {
            unlink_entry(h, p, q);
            stats.del_hit_ct += 1;
            return 1;
        } else {
            stats.del_miss_ct += 1;
            return 0;
        }
    }

    CacheStats get_stats() { return stats;}

private:

    Ref * htop;
    Ref * next_ref;
    Entry * entry;
    CacheStats stats;
    Index nrows;
    Index zpos;
    Ref free_top;



    Ref null_ref() {
        return 0xffffffff;
    }

    void setup_chains() {
        Mem::clear<Ref>(htop, nrows, null_ref());

        for (Index i = 0; i != nrows - 1; i += 1)
            next_ref[i] = i + 1;


        next_ref[nrows - 1] = null_ref();
        free_top = 0;


    }

    Bool find_matching_entry(Index h, Str key_str, Index & p, Index & q) {
        p = htop[h]; q = null_ref();
        while (is_nn(p)) {
            Entry & e = entry[p];
            if (e.match_key(key_str))
                return 1;
            q = p; p = next_ref[p];
        }
        return 0;
    }

    void update_existing(Index p, Str key_str, Str val_str) {
        Entry & e = entry[p];
        if (e.match_val(val_str)==0) {
            e.release();
            e.acquire(key_str, val_str);
        }
    }

    Str fetch_val(Index p) {
        Entry & e = entry[p];
        return e.fetch_val();
    }

    void unlink_entry(Index h, Index p, Index q) {
        Ref s = next_ref[p];
        if (q != null_ref()) {
            next_ref[q] = s;
        } else {
            htop[h] = s;
            push_free(p);
        }
    }

    void add_entry(Index h, Str key_str, Str val_str) {
        Ref p = pop_free();
        Entry & e = entry[p];
        e.acquire(key_str, val_str);
        push(htop[h], p);
    }

    Index clear_space_count() {
        if (nrows <= 64)
            return 1;
        if (nrows <= 256)
            return 3;
        if (nrows <= 1024)
            return 5;
        else
            return 7;
    }

    void clear_space() {
        Index ct = clear_space_count();
        LOOP {
            if (ct == 0)
                break;
            if (is_nn(htop[zpos])) {
                Ref p = pop(htop[zpos]);
                Entry & e = entry[p];
                e.release();
                push_free(p);
                ct -= 1;
            } else {
                zpos += 1;
                if (zpos == nrows)
                    zpos = 0;
            }
        }
    }

    Bool is_null(Ref p) {
        return p == null_ref();
    }

    Bool is_nn(Ref p) {
        return p != null_ref();
    }

    void push(Ref & top, Ref p) {
        next_ref[p] = top;
        top = p;
    }

    void push_free(Ref p) {
        push(free_top, p);
    }

    Ref pop(Ref & top) {
        Ref p = top;
        top = next_ref[p];
        return p;
    }

    Ref pop_free() {
        return pop(free_top);
    }

};

