// #include "erl_nif.h"

// #include "erl_nif.h"
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>

// #define ARCEX_ERTS_ENIF_ALLOC

#define ARCA_TESTER

#include "assert.h"
#include "tester.h"

#include "arcex-base.hpp"
#include "arcex-no-erts.hpp"
#include "arcex-array.hpp"
#include "slip-stats.hpp"
#include "slip-alloc.hpp"
#include "slip-forms.hpp"







// Arcex::Mutex ctrl_mutex;



void test_str() {
    START_TEST("Str Test");

    Char buf[40];
    Mem::clear<Char>(buf, 40, 'x');

    Str one = Str((CharP) "one", 3);
    Str two = Str((CharP) "two", 3);
    Str three = Str((CharP) "three", 5);

    one.copy_to(buf);

    TEST_VAL(buf[0], 'o');
    TEST_VAL(buf[1], 'n');
    TEST_VAL(buf[2], 'e');
    TEST_VAL(buf[3], 'x');

    buf[0] = 't'; buf[1] = 'w'; buf[2] = 'o';

    TEST_VAL(two.match(buf, 3), 1);
    TEST_VAL(three.match(buf, 4), 0);

    three.copy_to(buf);

    TEST_VAL(two.match(buf, 3), 0);
    TEST_VAL(three.match(buf, 5), 1);

    Str three_2 = Str("three");
    TEST_VAL(three_2.match(buf, 5), 1);

    END_TEST;
}

void test_entry() {
    START_TEST("Entry Test");

    Str hello = "hello";
    Str world = "world";

    //Char buf[20];

    Entry e; e.acquire(hello, world);

    TEST_VAL(e.match_key(hello), 1);
    Str vstr = e.fetch_val();
    TEST_VAL(vstr.match_cstr("world"), 1);
    TEST_VAL(e.calc_len(), 10);

    e.release();

    Str universe = "universe";
    e.acquire(hello, universe);
    TEST_VAL(e.calc_len(), 13 );


    END_TEST;
}



void cache_test() {
    //using namespace Arca::Alloc;
    START_TEST("Cache Test");

    using Cache = SlipCache;

    Cache my_cache;

    PRINT_STR("INIT\n");
    my_cache.init(16);
    PRINT_STR("INIT2\n");

    Str dayk[7];
    Str dayv[7];

    dayk[0] = "Mon"; dayv[0] = "Monday";
    dayk[1] = "Tue"; dayv[1] = "Tuesday";
    dayk[2] = "Wed"; dayv[2] = "Wednesday";
    dayk[3] = "Thu"; dayv[3] = "Thursday";
    dayk[4] = "Fri"; dayv[4] = "Friday";
    dayk[5] = "Sat"; dayv[5] = "Saturday";
    dayk[6] = "Sun"; dayv[6] = "Sunday";
    Index pos;

    for (Index i = 0; i != 7; i += 1) {
        Str key_str = dayk[i];
        Str val_str = dayv[i];
        WATCH("i",i);
        my_cache.set(key_str, val_str);
    }

    for (Index i = 0; i != 7; i += 1) {
        Str key_str = dayk[i];
        Str val_str;
        Bool match_flag = my_cache.get(key_str, val_str);
        TEST_VAL(match_flag, 1);
        TEST_VAL(val_str.match(dayv[i].buf, dayv[i].len), 1);
    }

    for (Index i = 0; i != 7; i += 1) {
        Str key_str = dayk[i];
        my_cache.del(key_str);
    }

    for (Index i = 0; i != 7; i += 1) {
        Str key_str = dayk[i];
        Str val_str;
        Bool match_flag = my_cache.get(key_str, val_str);
        TEST_VAL(match_flag, 0);
    }


  END_TEST;

}

int main() {
    test_str();
    test_entry();
	cache_test();

}

