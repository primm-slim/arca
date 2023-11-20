
// arca_bitmap.h
//
namespace Arca {
	// Arca
	
namespace Bitmap {
	// Arca::Bitmap
	
	namespace Asm = Arca::Asm;
	namespace Mem = Arca::Mem;
	
	class Bitmap64 {
	public:
		Bitmap64(Reg64 * buf, SizeT n)
		: map(buf), len(n) {}
		void clear() {
			Mem::clear<Reg64>(map, len, 0);
		}
		void set(Reg64 i) {
			Reg64 & v = get_elem(i);
			Reg8 y = calc_bit(i);
			v = Asm::bts(v, y);
		}

		void reset(Reg64 i) {
			Reg64 & v = get_elem(i);
			Reg8 y = calc_bit(i);
			v = Asm::btr(v, y);
		}

		void bitset(Reg64 i, Reg8 f) {
			Reg64 & v = get_elem(i);
			Reg8 y = calc_bit(i);
			v = bitset(v, y, f);
		}


	private:
		Reg64 bitset(Reg64 v, Reg8 y, Reg8 f) {
			Reg64 a = Asm::btr(v, y);
			Reg64 b = Asm::bts(v, y);
			return Asm::cmovnz(f, a, b);
		}

		Reg64 & get_elem(Reg64 i) {
			Reg64 x = i; x >>= 6;
			return map[x];
		}

		Reg8 calc_bit(Reg64 i) {
			Reg64 k = i & 63;
			return (Reg8) k;
		}

		Reg64 *  map;
		Reg64 len;
	};


}; // namespace Bitmap

}; // namespace Arca


