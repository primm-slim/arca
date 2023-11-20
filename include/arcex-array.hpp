// Arcex Array

// forward memory sequence

namespace Mem {

    template <typename Elem>
    class Seq {
    public:
        using ElemPtr = Elem *;


        Seq(ElemPtr buf, SizeT n) : p(buf), st(buf), en(buf + n) {}
        Elem & operator*() { return *p;}
        ElemPtr ptr() { return p;}
        void operator++() {p++;}
        operator bool() { return p != en;}
        void rewind() { p = st;}


    private:
        ElemPtr p, st, en;
    };


    // memcpy
	// left to right

	template <typename Elem>
	void copy(Elem * dest, Elem * src, size_t len) {
		for (SizeT i = 0; i != len; i += 1)
			dest[i] = src[i];
	};

	// right to left

	template <typename Elem>
	void rcopy(Elem * dest, Elem * src, size_t len) {
		SizeT i = len;
		while (i != 0) {
			i -= 1;
			dest[i] = src[i];
		}
	}

    // memset

    template <typename Elem>
	void clear(Elem * dest, size_t len, Elem e) {
		for (SizeT i = 0; i != len; i += 1)
			dest[i] = e;
	};

    // memcmp, sort of

	template <typename Elem>
	Bool match(Elem * a, Elem * b, SizeT len) {
		for (SizeT i = 0; i != len; i += 1) {
			if (a[i] != b[i])
				return 0;
		}
		return 1;
	}


}; // namespace Mem




template <typename Key, typename Elem, typename MapFn>
class ArrayMap {
public:
    using Index = uint32_t;
    using Code = uint32_t;

    void init() {
        build_starting_table();
    }

    Code hash_key(Key k) {
        Code h = MapFn::hash_key(k);
        if ((h == 0) | (h == 1))
            h = 2;
        return h;
    }

    // to modify, use key_exists to check for presence
    // of key, then use add or update

    Bool key_exists(Code h, Key k) {
        Index pos;
        if (scan_key(h, k, pos))
            return 1;
        else
            return 0;
    }

    Bool update(Code h, Key k, Elem e) {
        Index pos;

        if (scan_key(h, k, pos)) {
            elem_data[pos] = e;
            return 1;
        } else
            return 0;
    }

    void add(Code h, Key k, Elem e) {
        if (need_rewrite())
            rewrite();

        Index pos;
        scan_free(h, pos);
        Code code = key_code[pos];
        store(pos, h, k, e);

        if (code == 0)
            free_ct -= 1;

        if (code == 1)
            del_ct -= 1;

        busy_ct += 1;
    }

    Bool get(Code h, Key k, Elem & e) {
        Index pos;

        if (scan_key(h, k, pos)) {
            e = elem_data[pos];
            return 1;
        } else
            return 0;
    }

    Bool del(Code h, Key k) {
        Index pos;
        if (scan_key(h, k, pos)) {
            key_code[pos] = 1;
            del_ct += 1;
            busy_ct -= 1;
            return 1;
        } else
            return 0;
    }

private:

    Index busy_ct, free_ct, del_ct, max_elem;
    Code * key_code;
    Key * key_data;
    Elem * elem_data;

    void build_starting_table(Index n) {
        max_elem = n;
        busy_ct = 0;
        del_ct = 0;
        free_ct = max_elem;
        key_code = Mem::req_array<Code>(max_elem);
        key_data = Mem::req_array<Key>(max_elem);
        elem_data = Mem::req_array<Elem>(max_elem);
        Mem::clear<Code>(key_code, max_elem, 0);
    }

    void mask(Index & k) {
        Index m = max_elem - 1;
        k &= m;
    }

    Bool scan_key(Code h, Key k, Index & pos) {
        Bool reloc_flag; Index reloc_pos;
        reloc_flag = 0;
        pos = h; mask(pos);
        LOOP {
            Code code = key_code[pos];
            if ((code == h) && (MapFn::match_key(key_data[pos], k)))
                break;
            if (code == 0)
                return 0;
            if ((code == 1) && (reloc_flag == 0)) {
                reloc_flag = 1;
                reloc_pos = pos;
            }
            pos += 1; mask(pos);
        }
        if (reloc_flag) {
            relocate(pos, reloc_pos);
            pos = reloc_pos;
        }
        return 1;
    }

    void relocate(Index i, Index j) {
        store(j, key_code[i], key_data[i], elem_data[i]);
        key_code[i] = 1;
    }

    Bool scan_free(Code h, Index & pos) {
        pos = h; mask(pos);

        LOOP {
            Code code = key_code[pos];
            if ((code == 0) || (code == 1)) {
                return 1;
            }
            pos += 1; mask(pos);
        };
        return 0;
    }

    void store(Index pos, Code h, Key k, Elem e) {
        key_code[pos] = h;
        key_data[pos] = k;
        elem_data[pos] = e;
    }

    Bool need_rewrite() {
        return (free_ct * 5 <= max_elem);
    }

    void rewrite() {
        Index olen = max_elem;
        Index nlen = (busy_ct * 4 < max_elem) ? max_elem : max_elem * 2;

        Code * code_temp = key_code;
        Key * key_temp = key_data;
        Elem * elem_temp = elem_data;

        key_code = Mem::req_array<Code>(nlen);
        key_data = Mem::req_array<Key>(nlen);
        elem_data = Mem::req_array<Elem>(nlen);

        Mem::clear<Code>(key_code, nlen, 0);

        for (Index i = 0; i != olen; i += 1) {
            Code code = code_temp[i];
            if ((code == 0) || (code == 1))
                continue;
            Index pos;
            scan_free(code, pos);
            store(code, key_temp[i], elem_temp[i]);
        }

        Mem::rel_array<Code>(code_temp);
        Mem::rel_array<Key>(key_temp);
        Mem::rel_array<Elem>(elem_temp);

        max_elem = nlen;
        del_ct = 0;
        free_ct += (nlen - olen);
    }

};



// string hash function


// bytes are translated to random 32 bit values
// which are then hashed with a crc32 function.
// If the crc32 function is not available, an
// alt hash function can be selected via a
// compile-time define, the alt uses a slower
// ror32 sub/xor function.

uint32_t arca_hash_constants[256] = {
        3478793222,3123602648,93177941,446142668,1306308566,2011704855,1557866963,3558637898,2359062447,2069526023,
        3107361783,3267247282,2124602734,1880277191,1921185144,1371175791,1767835358,2050831406,1494781344,1472641495,
        3053639087,439905041,2120759395,3262261342,2646939137,1933769772,2988161091,3694646574,506253532,3170525888,
        363851723,3438332847,235811344,1194952119,549625171,3823215876,4061183187,2072033826,1712080591,2436509006,
        404578977,3803337274,3539594343,991781563,1358963451,869611638,1189862965,3127092622,2359490224,3190819575,
        3834520045,3720815606,3523979137,3574362309,3278259883,2093940268,2189227604,970252471,1435893437,3808632282,
        1456393911,3331272963,1503132636,2777782168,604347600,3034257335,1120375136,38115736,1131601420,678110825,
        380825053,1715912011,685064815,1152323703,2164061902,3011417468,738993669,2663268238,2869617300,3016831248,
        1350736295,3760449682,410437067,1968791967,1978563042,1558384323,2638124918,131555793,3466861280,3299313146,
        1087601218,3456123939,2578497841,626492473,3611637625,620932606,1619643820,3467456543,10958795,4171047039,
        2257922517,643739595,2475390598,969785874,2017473743,4031419608,950857262,1945672680,1201673640,1172438409,
        2907154428,2190077810,16700430,697360841,429452125,158006791,3556418396,2495387224,822422989,369579667,
        3778700087,317834390,3553518956,609354111,2723052155,3368303800,2517988508,3973395032,4190209970,2685353370,
        699305010,2595161181,2025842561,1977589641,325479358,2359231353,284058701,2241065008,925335509,3170346108,
        310159940,910476080,1300325370,4187532806,3428503836,3400784677,1277916873,3395030506,4272178228,438849913,
        1627278087,10417995,3854010653,2329996633,2456712414,1932503532,4112185101,2579145608,3163091538,3137933671,
        3429611556,1456499308,3103163481,1881872625,3503792528,3173173664,3586716893,3328134271,107043442,1487827366,
        1162406381,3833755285,195947891,3987755138,1125903487,79899843,3498239320,3463374897,939321700,1417386160,
        633581084,831949652,3001121772,2015156747,1086998525,3823535324,234854674,291990329,736195824,133027168,
        4137775068,596771171,1945798137,1318270635,868797561,1759590585,3876288327,3506206714,405250361,1473359940,
        400542713,1776054553,551989488,847182632,2238635481,3537270309,2964668745,2125591647,1104260935,1908861563,
        3151145010,963533038,10105519,4090359737,133442401,2397744939,1002470743,2175934213,2684654348,1511898428,
        1259206760,3111789522,1262196986,275016109,1665079802,3163431389,598383557,179979927,3393901532,100745281,
        3731098334,161440064,1695178645,1369986864,3742008798,1827064320,3283207070,2764397277,3298552287,1234136085,
        261367661,1164377475,1646174979,85722693,1396475966,2626486424,3765890097,2647905162,2402842315,716373389,
        2748747603,306676070,2358462295,3145578506,1565963609,2147465395
};


/*
uint64_t hash_string_ror64(uint64_t h, uint8_t a) {
	uint64_t x, y;
	uint8_t b;
	x = h >> a;
	b = (64 - a);
	y = h << b;
	return x | y;
}
*/

// standard hash function, uses intel crc32 instruction, may not work on older systems

#ifndef SLIP_CACHE_ALT_HASH_FN

void crc32(uint32_t & h, uint32_t t) {
    asm inline(
        "crc32 %1, %0\n\t"
        : "+r" (h)
        : "r" (t)
        : "cc");
}

uint32_t hash_char_array(Char * data, SizeT len) {
	uint32_t h = 0x6cb3b86d;
    uint8_t z = 0;
	CharP p, e;
	p = data; e = p + len;

	while (p != e) {
		uint8_t t = (uint8_t) *p++;
        t ^= z; z += 1;
        crc32(h, (uint32_t) arca_hash_constants[t] );
	}
	return h;
}

#endif

// alt hash function, uses intel crc32 instruction, may not work on older systems
// uses a slower custom xor / sub hash function.

#ifdef SLIP_CACHE_ALT_HASH_FN

uint32_t ror32(uint32_t h, uint8_t i) {
    uint32_t x = h << i;
    uint32_t y = h >> (32 - i);
    return x | y;
}

uint32_t hash_char_array(Char * data, SizeT len) {
	uint32_t h = 0x6cb3b86d;
    uint8_t z = 0;
	CharP p, e;
	p = data; e = p + len;

	while (p != e) {
		uint8_t t = (uint8_t) *p++;
        t ^= z; z += 1;
        h ^= (uint32_t) arca_hash_constants[t];
        h -= ror32(h, 23); h ^= ror32(h, 6); h -= ror32(h, 17);
        h ^= ror32(h,7); h -= ror32(h, 11); h ^= ror32(h, 12);
        h -= ror32(h, 27); h ^= ror32(h, 10); h -= ror32(h, 24);
	}
	return h;
}

#endif







