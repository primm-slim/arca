
namespace Arca {
	// Arca

namespace Tree {
    // Arca::Tree

/*
    using Index8 = uint8_t;
    using VoidP = void *;

    using PageId = uint8_t;

    struct PackU64 {
        void reset() { val = 0;}
        void pack(uint64_t x, uint8_t bits) {
            uint64_t m = 1ULL;

        }
        uint64_t val;
    };

    template <typename IndexBlock, typename DataBlock>
    class BlockAlloc {
    public:
        using Id = uint64_t;
        using Index = uint32_t;
        using Index8 = uint8_t;
        BlockAlloc();
        ~BlockAlloc();
        IndexBlock & req_index_block();
        DataBlock & req_data_block();
        void rel_index_block(IndexBlock & index_block);
        void rel_data_block(DataBlock & data_block);
    private:

        static void to_page_pos(U64 & id, U8 & pg, U32 & pos) {
            U64 x, y; x = y = id;
            pg = (U8) (x & 0xff);
            pos =

        }
        struct EmptyBlock {
            EmptyBlock * next;
        };

        union Block {
            EmptyBlock empty_block;
            IndexBlock index_block;
            DataBlock data_block;
        };

        struct Page {
            Block * data;
            Index8 * code;
            Index32

            Index pos, maxos;
            Block
        };

    };



    template <typename IndexBlock, typename DataBlock>
    struct Block {
        Index8 code;
        union {
            IndexBlock index_block;
            DataBlock data_block;
            Block * next;
        };

        Block() {
            code = 0;
        }

        IndexBlock *
    };


     VarBlock {
    private:

        struct Block {
            Index8 code;
            union {
                IndexBlock index_block;
                DataBlock data_block;
                Block * next;
            };

        };

    };

    template <typename>
    class PageAlloc {
    public:
        using PageId = uint8_t;
        using PagePos = uint32_t;

        void req(PageId & pg_id, PagePos & pg_pos) {

        }


    template <typename Elem>
    class PageAlloc {
    public:
        using PageId = uint8_t;
        using PagePos = uint32_t;

        Elem * ref(PageId id, PagePos pos) {
            return pages[id].data + pos;
        }

        void remap(BlockPtr p) {



        }
    private:
        Page * page_top;
        Index next_page_size;
        Index next_page_gen;


        struct Page {
            Page * next;
            Elem * data;
            Index free_elem;
            Index * next_elem;
            Index pos, maxpos;
            Index gen;

            Page(Index page_gen, Index len) {
                data = new Elem[len];
                next_elem = new Index[len];
                maxpos = pos = 0;
                free_elem = len;
                gen = page_gen;
            }

            ~Page() {
                delete[] data;
                delete[] next_elem;
            }

            Bool avail() {
                return (pos != maxpos) || (free_elem != len);
            }

            Index req() {
                if (free_elem != len) {
                    Index i = free_elem;
                    free_elem = next_elem[i];
                    return i;
                } else
                    return pos++;
            }

            void rel(Index i) {
                next_elem[i] = free_elem;
                free_elem = i;
            }
        };

        union Chunk {
            Block block;

        };

        struct Page {
            Page * prev, * next;
            Elem * data;
            Index * next_blk;
            Index pos, maxpos;
        };

        void add_page() {
            if (page_ptr == nullptr) {

            }
        }





        struct EmptyBlock {
            EmptyBlock * next;
        };

        union Block {
            IndexBlock idx;
            DataBlock data;
            FreeBlock empty;
        };



    };


    template <typename Elem>
    void copy_elem(Elem * src, Elem * dest, SizeT len) {
        Elem * p = src;
        Elem * q = dest;
        Elem * e = src + len;
        while (p != e)
            *q++ = *p++;
    }

    template <typename Elem>
    Elem * create_array(SizeT len) {
        assert(len != 0);
        return new Elem[len];
    }

    template <typename Elem>
    Elem * resize_array(Elem * odata, SizeT olen, SizeT nlen) {
        assert(((odata ))
        Elem * ndata = new Elem[nlen];
        Index c = olen;
        if (c > nlen)
            c = nlen;
        copy_elem<Elem>(odata, ndata, c);
        if ((olen > 0) && (odata != nullptr))
            delete[] odata;
        return ndata;
    }

    template <typename Elem>
    void rel_array(Elem * data) {
        delete[] data;
    }



    template <typename Index, typename Elem>
    class PageAlloc {
    public:
        PageAlloc() {
            next_page_len = 32;
            page_ct = 0;
            page_max = 0;
            page_gen = 0;
            pages = nullptr;
        }
        ~PageAlloc() {

        }
        Index req() {
            if (page_ct == 0)
                create_first_page();


        }

        Elem & ref(Index i) {

        }
        Index remap(Index i) {

        }
        void rel(Index i) {

        }

    private:

        struct Page {
            Elem * data;
            Index8 * next_blk;
            Index8 free_blk;
            Index pos, maxpos;
            Index gen;
        };

        Page * pages;
        Index page_ct;
        Index page_max;

        void create_page_array(Index len) {
            page_max = len;
            page_ct = 0;
            pages = new Page[len];
        }

        void extend_page_array() {
            Index olen = page_max;
            Index nlen = olen * 2;
            Elem * odata = data;
            Elem * ndata = new Page[nlen];
            for (Index)
            Index i = 0;
            while (i < olen) {

            }
        }





        struct Page {
            Page * prev, Page * next;
            Index8 free_blk, free_ct;
            Index8 next_blk[N];
            Elem data[N];

            Page() {
                free_blk = N;
                for (Index8 i = 0; i != N; i += 1)
                    push(i);
            }

            void push(Index i) {
                next_blk[i] = free_blk;
                free_blk = i;
                free_ct += 1;
            }

            Index pop() {
                Index i = free_blk;
                free_blk = next_blk[i];
                free_ct -= 1;
                return i;
            }

            Bool avail() { return free_ct > 0;}

            Elem * req() {
                Index i = free_blk;
                free_blk = next_blk[i];
                Elem & e = data[i];
                e.page_link = this;
                return e;
            }

            void rel(Elem * e) {
                Index i = &e - data;
                next_blk[i] = free_blk;
                free_blk = i;
            }
        };

        struct Page {
            Elem * data;
            Index * next_blk;
            Index free_blk;
            Index free_ct;
            Index pos, maxpos;
            Index gen;

            Page(Index page_gen, Index len) {
                data = new Elem[len];
                next_blk = new Index[len];
                pos = 0; maxpos = len;
                free_ct = 0;
                gen = page_gen;
                free_blk = maxpos;
            }

            ~Page() {
                delete[] data;
                delete[] next_blk;
            }

            Bool avail() {
                return (free_ct > 0) || (pos != maxpos);
            }

            Index req() {
                if (free_ct > 0) {
                    Index i = free_blk;
                    free_blk = next_blk[i];
                    free_ct -= 1;
                    return i;
                } else {
                    return pos++;
                }
            }

            void rel(Index i) {
                assert(free_blk != maxpos);
                next_blk[i] = free_blk;
                free_blk = i;
                free_ct += 1;
            }

            Elem * ref(Index i) {
                return data + i;
            }
        };

        void add_page() {
            if (page_ct == page_max)
                extend_page_array();
            pages[page_ct] = Page(next_page_gen, next_page_len);
            next_page_gen += 1;
            next_page_len += (next_page_len >> 2);
        }
        void create_first_page() {
            page_max = 8;
            pages = new Page[page_max];
            pages[0] = Page(0)

            page_ct = 1;
            page[1] =

        }
        Index page_ct;
        Index page_max;
        Page * pages;

        Index tot_busy, tot_free;
        Index page_gen;





    };


    template <typename Elem>
    class PtrChain {
    public:
        using ElemPtr = Elem *;
        void reset() { top = nullptr;}
        Bool is_empty() { return top == nullptr;}
        void push(Elem & e) {
            ElemPtr p = top;
            e.next = p;
            e.prev = nullptr;
            if (p != nullptr)
                (*p).prev = e;
            top = &e;
        }
        Elem & pop() {
            ElemPtr p = top;
            top = (*p).next;
            if (top != nullptr)
                (*top).prev = nullptr;
            return *p;
        }
        void del(Elem & e) {
            ElemPtr x = e.prev;
            ElemPtr y = e.next;
            if (x != nullptr)
                (*x).next = y;
            else
                top = y;
            if (y != nullptr)
                (*y).prev = next;
        }
    private:
        ElemPtr top;
    };

    template <typename Index, typename Block, typename N>
    class BlockAlloc {
    public:
        BlockAlloc() {
            full_pages = nullptr;
            free_pages = nullptr;
        }
        ~BlockAlloc();
        Index req() {}
        void rel(Index p) {}
        Block & ref()
        Index remap(Index p)
    private:
        struct Page {
            Page * prev, * next;
            Index8 busy_ct, free_ct;
            Index8 map[N];
            Block blk[N];
        };

        Page * full_pages, * avail_pages;




    };


    class TreeMap {
    public:
    private:

        struct IndexPage;

        struct IndexBlock {
            IndexPage * page;
            IndexBlock * parent;
            Key key[6];
            Ref ptr[7];
            Index8 count[7];
        };

        struct DataBlock {
            IndexBlock * parent;
            Key key[12];
            Val val[12];
        };




    };


    union Chunk {
        IndexBlock idx_block;
        DataBlock data_block;
        FreeBlock free_block;
        char empty[64];
    };

    struct Page {
        Page * page_prev;
        Page * page_next;
        Code code;
    };

    struct Chunk {
        Chunk * next;

    };

    struct IndexBlock {

    };

    struct IndexPage {
        IndexPage * prev;
        IndexPage * next;
        Reg32 map;
        IndexBlock block[32];
    };



    using Reg32 = uint32_t;

    template <typename Elem>
    class Seq {
    public:
        using ElemPtr = Elem *;
        Seq(ElemPtr buf, SizeT n) : p(buf), st(buf), en(buf + n) {}
        Elem & operator*() { return *p;}
        void operator++() {p++;}
        operator bool() { return p != en;}
        SizeT index() { return p - st;}
    private:
        ElemPtr p, st, en;
    };


    template <typename Elem>
    class Seq {
    public:
        Seq(Elem * buf, Index n) : data(buf), len(n);

    private:

    };

    uint32_t bsf(uint32_t )

    class Map {
    public:
        using Index = uint32_t;
        void reset() {
            for (Index i = 0; i != 8; i += 1)
                data[i] = 0;
        }
        Index req() {
            Index i = 0;
            while (i < 8) {
                if (data[i] != 0)
                    break;
            }
            Index j = Asm::bsf(data[i], )


        }
    private:
        uint32_t data[8];
    };

    struct Page {
        Page * prev;
        Page * next;

    };

    class SlotAlloc {
    public:

        #ifdef ARCA_TESTER
            friend void ::tree_test();
        #endif

        using PgPos

    private:

    };


    class Alloc {

#ifdef ARCA_TESTER
    friend void ::tree_test();
#endif

    private:


        using Index = uint32_t;



    };

*/

};  // namespace Tree
}; // namespace Arca
