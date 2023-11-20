
namespace ArrayFn {

	template <typename Index, typename Elem, size_t N>
	class StaticArrayBase {
	public:
		Elem & operator(Index i) {
			return data[i];
		}

		Index length() { return N;}

	private:
		Elem data[N];
	};

	template <typename Index, typename Elem, typename Alloc>
	class DynamicArrayBase {
	public:
		DynamicArrayBase() {
			data = nullptr;
		}
		Elem & operator(Index i) {
			return data[i];
		}

		Index length() { return buflen;}

		void resize(Index nlen) {
			resize(nlen, 0);
		}

		void resize(Index nlen, Elem e) {
			Index olen = buflen;
			Elem * odata = data;
			Elem * ndata = nullptr;
			if (nlen > 0)
				ndata = Alloc::req(sizeof(Elem) * nlen);
			Index i = 0;
			Index c = MAX(olen, nlen);
			while (i < c)
				ndata[i++] = odata[i];
			c = nlen;
			while (i < c)
				ndata[i++] = e;
			if (odata != nullptr)
				Alloc::rel(odata);
		}

	private:
		Elem * data;
		Index buflen;

		
	};

	template <typename Base>
	class ArrayFn {
	};



};

