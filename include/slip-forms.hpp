
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




// Slipcache proper

#define SLIPCACHE_TRAY_MAX 8

class SlipCache {
public:
    using Code = uint32_t;

    void init(Index n) {
        nrows = n;
        entry = Mem::req_array<Entry>(n);
        key_code = Mem::req_array<Code>(n);
        Mem::clear<Code>(key_code, n, 0);
        set_scan_ct();
        zpos = 0;
    }

    void destroy() {
        Mem::rel_array<Entry>(entry);
        Mem::rel_array<Code>(key_code);
    }

    void set(Str key_str, Str val_str) {
        Index pos;
        Index h = hash_key(key_str);


        if (scan_for_match(h, pos, key_str)) {

            update_entry(pos, key_str, val_str);
            stats.set_hit_ct += 1;
            return;
        }

        Entry e;
        e.acquire(key_str, val_str);

        Index n = 0;
        while (n != 4) {
            store_swap_entry(h, e);
            if (h == 0)
                break;
            n += 1;
        }

        if (h != 0)
            e.release();

        stats.set_miss_ct += 1;
    }

    Bool get(Str key_str, Str & val_str) {
        Index pos;
        Index h = hash_key(key_str);

        if (scan_for_match(h, pos, key_str)) {
            val_str = fetch_entry_val(pos);
            stats.get_hit_ct += 1;
            return 1;
        }

        stats.get_miss_ct += 1;
        return 0;
    }

    Bool del(Str key_str) {
        Index pos;
        Index h = hash_key(key_str);

        if (scan_for_match(h, pos, key_str)) {
            remove_entry(pos);
            stats.del_hit_ct += 1;
            return 1;
        }

        stats.del_miss_ct += 1;
        return 0;
    }

    CacheStats get_stats() { return stats;}

private:

    Entry * entry;
    Code * key_code;
    CacheStats stats;
    Index nrows;
    Index zpos;
    Index scan_ct;

    void set_scan_ct() {
        scan_ct = 12;
    }

    inline Code hash_key(Str key_str) {
        Code code = hash_char_array(key_str.buf, key_str.len);
        if (code != 0)
            return code;
        else
            return code ^ 1;
    }

    inline void calc_index_pos(Code code, Index * kpos) {
        Index h = code;
        kpos[0] = h % nrows;
        h += 1;
        kpos[1] = h % nrows;
        h += 2;
        kpos[2] = h % nrows;
        h = ~((code << 24) | (code >> 8));
        kpos[3] = h % nrows;
        h += 1;
        kpos[4] = h % nrows;
        h += 2;
        kpos[5] = h % nrows;
    }


    Bool match_entry(Index k, Code code, Str key_str) {
        if (key_code[k] != code)
            return 0;
        Entry & e = entry[k];
        return e.match_key(key_str);
    }

    Bool scan_for_match(Code code, Index & pos, Str key_str) {
        Index kpos[6];
        calc_index_pos(code, kpos);
        for (Index i = 0; i != 6; i += 1) {
            pos = kpos[i];
            if (match_entry(pos, code, key_str))
                return 1;
        }
        return 0;
    }

    Bool scan_for_free(Code code, Index & pos) {
        Index kpos[6];
        calc_index_pos(code, kpos);
        for (Index i = 0; i != 6; i += 1) {
            pos = kpos[i];
            if (key_code[pos] == 0)
                return 1;
        }
        return 0;
    }

    inline void pick_space(Code code, Index & pos) {
        Index kpos[6];
        calc_index_pos(code, kpos);
        pos = kpos[zpos];
        zpos += 1;
        if (zpos == 6)
            zpos = 0;
    }

    inline void store_swap_entry(Code & code, Entry & e) {
        Index pos;
        if (scan_for_free(code, pos)==0)
            pick_space(code, pos);
        swap_entry(entry[pos], e);
        swap_code(key_code[pos], code);
    }

    inline void add_entry(Code h, Index pos, Str key_str, Str val_str) {
        key_code[pos] = h;
        Entry & e = entry[pos];
        e.acquire(key_str, val_str);
    }

    inline void update_entry(Index pos, Str key_str, Str val_str) {
        Entry & e = entry[pos];
         if (e.match_val(val_str)==0) {
            e.release();
            e.acquire(key_str, val_str);
        }
    }

    Str fetch_entry_val(Index pos) {
        Entry & e = entry[pos];
        return e.fetch_val();
    }


    void remove_entry(Index pos) {
        key_code[pos] = 0;
        Entry & e = entry[pos];
        e.release();
    }

    inline void swap_entry(Entry & a, Entry & b) {
        Entry temp = a;
        a = b;
        b = temp;
    }

    inline void swap_code(Code & a, Code & b) {
        Code temp = a;
        a = b;
        b = temp;
    }

};

