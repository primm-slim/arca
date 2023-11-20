// Stats

struct SysStats {

};

struct CacheStats {
    uint64_t set_hit_ct;
    uint64_t set_miss_ct;
    uint64_t get_hit_ct;
    uint64_t get_miss_ct;
    uint64_t del_hit_ct;
    uint64_t del_miss_ct;
    uint64_t reloc_ct;
    uint64_t slip_ct;
    uint64_t check_free_ct;
    uint64_t find_free_ct;

    void reset() {
        set_hit_ct = 0;
        set_miss_ct = 0;
        get_hit_ct = 0;
        get_miss_ct = 0;
        del_hit_ct = 0;
        del_miss_ct = 0;
        reloc_ct = 0;
        slip_ct = 0;
        check_free_ct = 0;
        find_free_ct = 0;

    }
};

struct AllocStats {

};


