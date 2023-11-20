// Header for Elixir / Erlang specific functions and classes

// Elixir / Erlang Types

using ExTerm = ERL_NIF_TERM;
using ExTermP = ERL_NIF_TERM *;
using ExEnv = ErlNifEnv;
using ExEnvP = ErlNifEnv *;
using ExBinary = ErlNifBinary;
using ExBinaryP = ErlNifBinary *;
using NifFunc = ErlNifFunc;
using ExMutex = ErlNifMutex;


namespace Arcex {

// Simple Mutex wrapper
// declare a global variable
// e.g. ArcexMutex ctrl_mutex;
// and create it in the nif load function


class Mutex {
public:
    void init(const char * name) {
        mutex = enif_mutex_create( (char *) name);
    }
    void destroy() {
        enif_mutex_destroy(mutex);
    }

    void lock() {
        enif_mutex_lock(mutex);
    }
    void unlock() {
        enif_mutex_unlock(mutex);
    }
private:
    ErlNifMutex * mutex;
};

SizeT term_to_binary(ExEnvP env, ExTerm term, ExBinary & bin) {
    if (enif_term_to_binary(env, term, &bin))
        return bin.size;
    else
        return 0;
}

ExTerm make_atom(ExEnvP env, CharP p, SizeT len) {
    return enif_make_atom_len(env, p, len);
}

ExTerm make_atom(ExEnvP env, const char * p) {
    return enif_make_atom(env, p);
}

ExTerm make_int(ExEnvP envp, int x) {
    return enif_make_int(envp, x );
}

ExTerm make_int64(ExEnvP envp, int64_t x) {
    return enif_make_int(envp, (ErlNifSInt64) x );
}

ExTerm make_error_tuple(ExEnvP env, const char * s) {
    ExTerm err_atom = enif_make_atom(env, "error" );
    ExTerm err_msg = enif_make_atom(env, s);
    return enif_make_tuple2(env, err_atom, err_msg);
}

}; // namespace Arcex'



#ifdef ARCEX_ERTS_ENIF_ALLOC

namespace Mem {
    /*
    static VoidP req(SizeT len) { return enif_alloc(len);}
    static void rel(VoidP p) { enif_free(p);}
*/

    template <typename Elem>
    static Elem * req_elem() {
        SizeT bytes = sizeof(Elem);
        return (Elem *) enif_alloc(bytes);
    }

    template <typename Elem>
    static void rel_elem(Elem * p) {
        enif_free((VoidP) p);
    }

    template <typename Elem>
    static Elem * req_array(SizeT n) {
        SizeT bytes = n * sizeof(Elem);
        return (Elem *) enif_alloc(bytes);
    }

    template <typename Elem>
    static void rel_array(Elem * p) {
        enif_free((VoidP) p);
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

#endif


