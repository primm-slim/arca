// Arca Tuple
//

namespace Arca {
	// namespace Arca

        template <typename A, typename B>
        struct Pair {
		Pair() { a = 0; b = 0;}
		Pair(A x, B y) : a(x), b(y) {}
                A a; B b;
		void extract_to(A & x, B & y) { x = a; b = y;}
        };

        template <typename A, typename B, typename C>
        struct Triple {
		Triple() { a = 0; b = 0; c = 0;}
		Triple(A x, B y, C z) : a(x), b(y), c(z) {}
                A a; B b; C c;
		void extract_to(A & x, B & y, C & z) { x = a; y = b; z = c;}
        };


};

