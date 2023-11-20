// Header for Elixir / Erlang specific functions and classes


namespace Mem {
    /*
    static VoidP req(SizeT len) { return enif_alloc(len);}
    static void rel(VoidP p) { enif_free(p);}
*/

    template <typename Elem>
    static Elem * req_elem() {
        SizeT bytes = sizeof(Elem);
        return (Elem *) malloc(bytes);
    }

    template <typename Elem>
    static void rel_elem(Elem * p) {
        free((VoidP) p);
    }

    template <typename Elem>
    static Elem * req_array(SizeT n) {
        SizeT bytes = n * sizeof(Elem);
        return (Elem *) malloc(bytes);
    }

    template <typename Elem>
    static void rel_array(Elem * p) {
        free((VoidP) p);
    }

    template <typename Elem>
    static Elem * resize_array(Elem * odata, SizeT nlen, SizeT olen, Elem e) {
        Elem * ndata = nullptr;
        if (nlen > 0)
            ndata = req_array<Elem>(nlen);
        SizeT c = (olen < nlen) ? olen : nlen;
        SizeT i = 0;
        while (i != c) {
            ndata[i] = odata[i];
            i += 1;
        }
        while (c != nlen) {
            ndata[i] = e;
            i += 1;
        }
        return ndata;
    }

};



