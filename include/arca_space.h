
namespace Arca {
	// Arca

namespace Space {
    // Arca::Space


    template <typename Reg>
    class RegPack {
    public:
        using Reg8 = uint8_t;
        void reset() { val = 0;}
        void pack(Reg x, Reg8 bits) {
            assert(bits != 0);  assert(bits < 64);
            val <<= bits;
            val |= (to_mask(bits) & x);
        }
        Reg unpack(Reg8 bits) {
            assert(bits != 0); assert(bits < 64);
            Reg v = (val & to_mask(bits));
            val >>= bits;
            return v;
        }

        #ifdef ARCA_TESTER
            friend void ::space_test();
        #endif

    private:
        static Reg to_mask(Reg8 bits) {
            Reg m = 1;
            m <<= bits;
            return (m - 1);
        }

        Reg val;
    };



};  // namespace Space
}; // namespace Arca
