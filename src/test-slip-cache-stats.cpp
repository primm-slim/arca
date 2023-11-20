#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>

// #define ARCEX_ERTS_ENIF_ALLOC

#define ARCA_TESTER
// #define SLIP_CACHE_ALT_HASH_FN
#include "tester.h"

#include "arcex-base.hpp"
#include "arcex-no-erts.hpp"
#include "arcex-array.hpp"
#include "slip-stats.hpp"
#include "slip-alloc.hpp"
#include "slip-forms.hpp"
#include "assert.h"


void cache_dict_tester() {

    using Cache = SlipCache;

    Cache my_cache;
    my_cache.init(100000);
    char buf[256];

    Str word_str[100000];
    Char word_data[1024 * 1024 * 256];

    uint64_t word_pos, word_ct;

    FILE *fp = fopen("/usr/share/dict/words", "r");
    if(fp == NULL) {
        perror("Unable to open file!");
        exit(1);
    }

    word_pos = 0; word_ct = 0;

    while(fgets(buf, sizeof(buf), fp) != NULL) {
        Index len = strlen(buf);
        if ((len > 1) && (len < 64)) {
            Mem::copy<Char>(word_data + word_pos, buf, (SizeT) len - 1);
            word_str[word_ct] = Str(word_data + word_pos, (SizeT) len - 1);

//            PRINT_STR("WORD_STR:");
//            word_str[word_ct].print_str();
//            PRINT_STR("\n");
            word_pos += len;
            word_ct += 1;  if (word_ct == 100000) break;
        }
    }

    fclose(fp);

// WATCH("write count", word_ct);
// WATCH("pos", word_pos);

    for (Index i = 0; i != word_ct; i += 1) {
            //WATCH("i", i);
        my_cache.set(word_str[i], word_str[i]);
    }

    Bool first_flag = 0;
    for (Index j = 0; j != 1000; j += 1) {
        Index count  = 0;
        Str val_str;
        for (Index i = 0; i != word_ct; i += 1) {
//            WATCH("i", i);
            val_str = Str("Nope");
            if (my_cache.get(word_str[i], val_str)) {
                if (val_str.match(word_str[i].buf, word_str[i].len))
                    // WATCH("Len check", val_str.len == word_str[i].len);
/*
                    PRINT_STR("WORD_STR:");
                    word_str[i].print_str();
                    PRINT_STR(" ");
                    PRINT_STR("VAL_STR:");
                    val_str.print_str();
                    PRINT_STR("\n");
*/
                    count += 1;
            }
        }
        if (first_flag == 0) {
            WATCH("read count", count);
            first_flag = 1;
        }
    }

  //   PRINT_STR("Loaded 100x\n");
 
    /*

    CacheStats stats = my_cache.get_stats();
    WATCH("get-hits", stats.get_hit_ct);
    WATCH("get-miss", stats.get_miss_ct);
    WATCH("set-hits", stats.set_hit_ct);
    WATCH("set-miss", stats.set_miss_ct);
    WATCH("del-hits", stats.del_hit_ct);
    WATCH("del-miss", stats.del_miss_ct);
    WATCH("check_free", stats.check_free_ct);
    WATCH("find_free", stats.find_free_ct);
    WATCH("reloc", stats.reloc_ct);
    WATCH("slip", stats.slip_ct);

    */



}


int main() {
    cache_dict_tester();
}
