// Helpers, mostly taken from my arca / arcex library

using Reg8 = std::uint8_t;
using Reg16 = std::uint16_t;
using Reg32 = std::uint32_t;
using Reg64 = std::uint64_t;

using Int8 = std::int8_t;
using Int16 = std::int16_t;
using Int32 = std::int32_t;
using Int64 = std::int64_t;

using Bool = std::uint8_t;

using SizeT = std::size_t;

using Char = char;

using Float = float;
using Double = double;

using VoidP = void *;
using CharP = char *;

template <typename Elem>
void swap_var(Elem & a, Elem & b) {
    Elem temp = a; a = b; b = temp;
}

#define UNUSED(...) (void)(__VA_ARGS__)

#define LOOP while (1)
#define SEQ(seq) for (; seq != 0; ++seq)

#define BRK_IF(x) if (x) {break;}
#define NEXT_IF(x) if (x) {continue;}
#define RET_IF(x) if (x) {return;}
#define RET_WHEN(x, y) if (y) {return x;}

#define ROLL(x, y) if (x == y) { x = 0;}

