// Arca ASM
//

namespace Arca {
	// Arca
namespace Asm {
	// Arca::Asm

	// TODO:  BSWAP, PREFETCH, CMOV (if needed)
	// XADD, CMPXCHG

	// helper for setccx intructions



#define ARCA_BUILD_ASM1(type, func, ins) \
	inline Reg8 func(type x) { \
		type y; \
		asm( \
			#ins " %1, %0" \
			: "=r" (y) \
			: "r" (x) \
			: "cc"); \
		return (Reg8) y; \
	}

ARCA_BUILD_ASM1(Reg32, popcnt, popcnt)  // Reg8 popcnt(Reg32)
ARCA_BUILD_ASM1(Reg64, popcnt, popcnt)  // Reg8 popcnt(Reg64)

#define ARCA_BUILD_ASM2(type, func, ins) \
	inline Reg8 func(type x) { \
		Reg8 f; \
		asm( \
			"TEST %1, %1\n\t" \
			#ins " %0\n\t" \
			: "=r" (f) \
			: "r" (x) \
			: "cc"); \
		return f; \
	}

ARCA_BUILD_ASM2(Reg32, setz, SETZ) // Reg8 setz(Reg32)
ARCA_BUILD_ASM2(Reg64, setz, SETZ) // Reg8 setz(Reg64)
ARCA_BUILD_ASM2(Reg32, setnz, SETNZ) // Reg8 setnz(Reg32)
ARCA_BUILD_ASM2(Reg64, setnz, SETNZ) // Reg8 setnz(Reg64)
				 
#define ARCA_BUILD_ASM3(type, func, ins) \
	inline type func(type x, Reg8 i) { \
		type y = x; \
		asm( \
			#ins " %1, %0\n\t" \
			: "+r" (y) \
			: "r" (i) \
			: "cc"); \
		return y; \
	}

ARCA_BUILD_ASM3(Reg32, shl, SHL) // Reg32 shl(Reg32)
ARCA_BUILD_ASM3(Reg64, shl, SHL) // Reg64 shl(Reg32)
ARCA_BUILD_ASM3(Reg32, shr, SHR) // Reg32 shr(Reg64)
ARCA_BUILD_ASM3(Reg64, shr, SHR) // Reg64 shr(Reg64)
ARCA_BUILD_ASM3(Reg32, rol, ROL) // Reg32 rol(Reg32)
ARCA_BUILD_ASM3(Reg64, rol, ROL) // Reg64 rol(Reg64)
ARCA_BUILD_ASM3(Reg32, ror, ROR) // Reg32 ror(Reg32)
ARCA_BUILD_ASM3(Reg64, ror, ROR) // Reg64 ror(Reg64)

uint32_t ror32(uint32_t x, uint8_t i) {
	return ror((Reg32) x, i);
}
uint64_t ror64(uint64_t x, uint8_t i) {
	return ror((Reg64) x, i);
}

#define ARCA_BUILD_ASM4(type, func) \
	inline Reg8 func(type x, Reg8 i) { \
		type j; j = (type) i;\
		Reg8 f; \
		asm( \
			"BT %2, %1\n\t" \
			"SETC %0\n\t" \
			: "=r" (f) \
			: "r" (x), "r" (j) \
			: "cc"); \
		return f; \
	}

ARCA_BUILD_ASM4(Reg32, bt) // Reg8 bt(Reg32, Reg8)
ARCA_BUILD_ASM4(Reg64, bt) // Reg8 bt(Reg64, Reg8)
			   
#define ARCA_BUILD_ASM5(type, func, ins) \
	inline type func(type x, Reg8 i) { \
		type j; j = (type) i;\
		type y = x; \
		asm( \
			#ins " %1, %0\n\t" \
			: "+r" (y) \
			: "r" (j) \
			: "cc"); \
		return y; \
	}

ARCA_BUILD_ASM5(Reg32, btr, BTR) // Reg32 btr(Reg32 x, Reg8 i)
ARCA_BUILD_ASM5(Reg64, btr, BTR) // Reg64 btr(Reg32 x, Reg8 i)
ARCA_BUILD_ASM5(Reg32, bts, BTS) // Reg32 bts(Reg32 x, Reg8 i)
ARCA_BUILD_ASM5(Reg64, bts, BTS) // Reg64 bts(Reg32 x, Reg8 i)

#define ARCA_BUILD_ASM6(type, func, ins) \
	inline Reg8 func(type x, type y) { \
		Reg8 f; \
		asm( \
			"cmp %2, %1\n\t" \
			#ins " %0\n\t" \
			: "=r" (f) \
			: "r" (x), "r" (y) \
			: "cc"); \
		return f; \
	}

ARCA_BUILD_ASM6(Reg32, seta, SETA) // Reg8 seta(Reg32)
ARCA_BUILD_ASM6(Reg64, seta, SETA) // Reg8 seta(Reg64)
ARCA_BUILD_ASM6(Reg32, setb, SETB) // Reg8 setb(Reg32)
ARCA_BUILD_ASM6(Reg64, setb, SETB) // Reg8 setb(Reg64)
ARCA_BUILD_ASM6(Reg32, setae, SETAE) // Reg8 seta(Reg32)
ARCA_BUILD_ASM6(Reg64, setae, SETAE) // Reg8 seta(Reg64)
ARCA_BUILD_ASM6(Reg32, setbe, SETBE) // Reg8 setb(Reg32)
ARCA_BUILD_ASM6(Reg64, setbe, SETBE) // Reg8 setb(Reg64)
ARCA_BUILD_ASM6(Reg32, seteq, SETZ) // Reg8 seteq(Reg32)
ARCA_BUILD_ASM6(Reg64, seteq, SETZ) // Reg8 seteq(Reg64)
ARCA_BUILD_ASM6(Reg32, setne, SETNZ) // Reg8 setne(Reg32)
ARCA_BUILD_ASM6(Reg64, setne, SETNZ) // Reg8 setne(Reg64)
			
#define ARCA_BUILD_ASM7(type, func, ins) \
	inline Reg8 func(type x) { \
		type v; \
		asm( \
			#ins " %1, %0\n\t" \
			: "=r" (v) \
			: "r" (x) \
			: "cc"); \
		return v; \
	}

ARCA_BUILD_ASM7(Reg32, bsf, BSF) // Reg8 bsf(Reg32)
ARCA_BUILD_ASM7(Reg64, bsf, BSF) // Reg8 bsf(Reg64)
ARCA_BUILD_ASM7(Reg32, bsr, BSR) // Reg8 bsr(Reg32)
ARCA_BUILD_ASM7(Reg64, bsr, BSR) // Reg8 bsr(Reg64)
				
#define ARCA_BUILD_ASM8(func, ins) \
	inline void func() { \
		asm volatile( \
			#ins "\n\t" \
			: : : );  \
	}

ARCA_BUILD_ASM8(cpu_mfence, MFENCE) // void mfence()
ARCA_BUILD_ASM8(cpu_sfence, SFENCE) // void sfence()
ARCA_BUILD_ASM8(cpu_lfence, LFENCE) // void lfence()
ARCA_BUILD_ASM8(cpu_pause, PAUSE) // void lfence()

#define ARCA_BUILD_ASM9(type, func) \
	inline type set_if(Bool f, type x, type y) { \
		type v = y; \
		asm( \
			"TEST %2, %2\n" \
			"CMOVNZ %1, %0\n" \
			: "+r" (v) \
			: "r" (x), "r" (f) \
			: "cc"); \
		return v; \
	}

ARCA_BUILD_ASM9(Reg8, if_t)
ARCA_BUILD_ASM9(Reg16, if_t)
ARCA_BUILD_ASM9(Reg32, if_t)
ARCA_BUILD_ASM9(Reg64, if_t)

#define ARCA_BUILD_ASM10(type) \
	void xor_sort(type & x, type & y) { \
		Bool f = setb( (type) x, (type) y); \
		Reg32 m = (type) f; m -= 1; \
		Reg32 k = x ^ y; k &= m; \
		x ^= k; y ^= k; \
	}

ARCA_BUILD_ASM10(Reg32)
ARCA_BUILD_ASM10(Reg64)

#define ARCA_BUILD_ASM11(type, func, ins) \
	template <typename INT> \
	inline type func(INT e, type x, type y) { \
		type v = x; \
		asm( \
			"CMP $0, %2\n\t" \
			#ins " %1, %0\n\t" \
			: "+r" (v) \
			: "r" (y), "r" (e) \
			: "cc"); \
		return v; \
	}

ARCA_BUILD_ASM11(Reg32, cmovz, CMOVZ) // INT cmovz(INT, Reg32, Reg32)
ARCA_BUILD_ASM11(Reg64, cmovz, CMOVZ) // INT cmovz(INT, Reg64, Reg64)
ARCA_BUILD_ASM11(Reg32, cmovnz, CMOVNZ) // INT cmovnz(INT, Reg32, Reg32)
ARCA_BUILD_ASM11(Reg64, cmovnz, CMOVNZ) // INT cmovnz(INT, Reg64, Reg64)

#define ARCA_BUILD_ASM12(type, func, ins) \
	template <typename INT> \
	inline type func(INT a, INT b, type x, type y) { \
		type v = x; \
		asm( \
			"CMP %3, %2\n\t" \
			#ins " %1, %0\n\t" \
			: "+r" (v) \
			: "r" (y), "r" (a), "r" (b) \
			: "cc"); \
		return v; \
	}

ARCA_BUILD_ASM12(Reg32, cmova, CMOVA) // INT cmova(INT, INT, Reg32, Reg32)
ARCA_BUILD_ASM12(Reg64, cmova, CMOVA) // INT cmova(INT, INT, Reg64, Reg64)
ARCA_BUILD_ASM12(Reg32, cmovb, CMOVB) // INT cmovb(INT, INT, Reg32, Reg32)
ARCA_BUILD_ASM12(Reg64, cmovb, CMOVB) // INT cmovb(INT, INT, Reg64, Reg64)
ARCA_BUILD_ASM12(Reg32, cmovae, CMOVAE) // INT cmovae(INT, INT, Reg32, Reg32)
ARCA_BUILD_ASM12(Reg64, cmovae, CMOVAE) // INT cmovae(INT, INT, Reg64, Reg64)
ARCA_BUILD_ASM12(Reg32, cmovbe, CMOVBE) // INT cmovbe(INT, INT, Reg32, Reg32)
ARCA_BUILD_ASM12(Reg64, cmovbe, CMOVBE) // INT cmovbe(INT, INT, Reg64, Reg64)




	}; // namespace Asm
}; // namespace Arca




