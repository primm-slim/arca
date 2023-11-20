//#include <cstdlib>
//#include <cstdio>
//#include <cstdint>

#define ARCA_TESTER

#include "assert.h"
#include "tester.h"
#include "arca.h"


using namespace Arca;



/*


  using Key = uint64_t;
  using Set = Tree::TreeSetFn<Key, 10>;
  using IndexBlock = Set::IndexBlock;
  using DataBlock = Set::DataBlock;



  using Index = Set::Index;

  VoidP top = nullptr;

  auto set = Set(top);

  //set.reset();

  Bool flag;

  //flag = set.has_key(20);

  //TEST_VAL(flag, 0);

  //set.add_key(20);
  //flag = set.has_key(20);

  //TEST_VAL(flag, 1);


  DataBlock data0 = DataBlock();
  DataBlock data1 = DataBlock();
  DataBlock data2 = DataBlock();


  IndexBlock idx0 = IndexBlock();

  VoidP p0 = Set::to_voidp(&data0);
  VoidP p1 = Set::to_voidp(&idx0);

  TEST_VAL(Set::is_index_block(p0), 0);
  TEST_VAL(Set::is_index_block(p1), 1);
  TEST_VAL(Set::is_data_block(p0), 1);
  TEST_VAL(Set::is_data_block(p1), 0);




  TEST_VAL(Set::calc_min_slide_count(), 2);

  TRACE;

  idx0.ptr[0] = Set::to_voidp(&data0);
  idx0.ptr[1] = Set::to_voidp(&data1);
  idx0.ptr[2] = Set::to_voidp(&data2);
  idx0.count = 2;


  Set::reset_parent_links(idx0);

  TEST_VAL(data0.parent == &idx0, 1);
  TEST_VAL(data1.parent == &idx0, 1);
  TEST_VAL(data2.parent == &idx0, 1);

  data0.reset();
  data1.reset();

  for (Index i = 0; i != 5; i += 1)
    data0.append(i* 10 + 10);

  for (Index i = 0; i != 10; i += 1)
    data1.append(i* 10 + 110);


  //data0.print();
  //data1.print();

  Key k = 100;

  TEST_VAL(data0.count, 5);
  TEST_VAL(data0.key[1], 20);
  TEST_VAL(data0.key[2], 30);
  TEST_VAL(data0.key[3], 40);
  TEST_VAL(data0.key[4], 50);

  TEST_VAL(data1.count, 10);
  TEST_VAL(data1.key[0], 110);
  TEST_VAL(data1.key[1], 120);
  TEST_VAL(data1.key[2], 130);
  TEST_VAL(data1.key[9], 200);

  TEST_VAL(Set::check_high_slide_data_avail(data1, data0), 1);
  TEST_VAL(Set::check_high_slide_data_avail(data0, data1), 0);

  Set::slide_data_hi_left(data1, data0, k);

  TEST_VAL(data0.count, 7);
  TEST_VAL(data0.key[1], 20);
  TEST_VAL(data0.key[2], 30);
  TEST_VAL(data0.key[3], 40);
  TEST_VAL(data0.key[4], 50);
  TEST_VAL(data0.key[6], 110);

  TEST_VAL(data1.count, 8);
  TEST_VAL(data1.key[0], 130);
  TEST_VAL(data1.key[2], 150);
  TEST_VAL(data1.key[4], 170);

  //data0.print();
  //data1.print();

  TEST_VAL(k, 120);

  data0.reset();
  data1.reset();

  for (Index i = 0; i != 10; i += 1)
    data0.append(i* 10 + 10);

  for (Index i = 0; i != 4; i += 1)
    data1.append(i* 10 + 110);


  //data0.print();
  //data1.print();
  TRACE;


  k = 105;

  TEST_VAL(data0.count, 10);
  TEST_VAL(data0.key[1], 20);
  TEST_VAL(data0.key[2], 30);
  TEST_VAL(data0.key[3], 40);
  TEST_VAL(data0.key[4], 50);
  TEST_VAL(data0.key[9], 100);

  TEST_VAL(data1.count, 4);
  TEST_VAL(data1.key[0], 110);
  TEST_VAL(data1.key[1], 120);
  TEST_VAL(data1.key[3], 140);


  TEST_VAL(Set::check_high_slide_data_avail(data0, data1), 1);

  Set::slide_data_hi_right(data0, data1, k);

  TEST_VAL(k, 80);

  TEST_VAL(data0.count, 7);
  TEST_VAL(data0.key[1], 20);
  TEST_VAL(data0.key[2], 30);
  TEST_VAL(data0.key[3], 40);
  TEST_VAL(data0.key[4], 50);
  TEST_VAL(data0.key[6], 70);

  TEST_VAL(data1.count, 7);
  TEST_VAL(data1.key[0], 90);
  TEST_VAL(data1.key[1], 100);
  TEST_VAL(data1.key[2], 105);
  TEST_VAL(data1.key[3], 110);
  TEST_VAL(data1.key[6], 140);

  //data0.print();
  //data1.print();

  IndexBlock idx1;

  idx0.reset();
  idx1.reset();

  idx0.ptr[0] = (VoidP) (Reg64) 0xA000;
  idx1.ptr[0] = (VoidP) (Reg64) 0xB000;


  for (Index i = 0; i != 5; i += 1) {
    idx0.append(1100 + (i * 100), (VoidP) (Reg64) (0xA100 + 0x100 * i));
  }
  for (Index i = 0; i != 10; i += 1) {
    idx1.append(2100 + (i * 100), (VoidP) (Reg64) (0xB100 + 0x100 * i));
  }

  //idx0.print();
  //idx1.print();

  k = 2000;

  TEST_VAL(idx0.count, 5);
  TEST_VAL(idx0.key[0], 1100);
  TEST_VAL(idx0.key[1], 1200);
  TEST_VAL(idx0.key[2], 1300);
  TEST_VAL(idx0.key[3], 1400);
  TEST_VAL(idx0.key[4], 1500);
  TEST_VAL((Reg64) idx0.ptr[0], (Reg64) (0xA000));
  TEST_VAL((Reg64) idx0.ptr[1], (Reg64) (0xA100));
  TEST_VAL((Reg64) idx0.ptr[2], (Reg64) (0xA200));
  TEST_VAL((Reg64) idx0.ptr[3], (Reg64) (0xA300));
  TEST_VAL((Reg64) idx0.ptr[4], (Reg64) (0xA400));
  TEST_VAL((Reg64) idx0.ptr[5], (Reg64) (0xA500));

  TEST_VAL(idx1.count, 10);
  TEST_VAL(idx1.key[0], 2100);
  TEST_VAL(idx1.key[1], 2200);
  TEST_VAL(idx1.key[2], 2300);
  TEST_VAL(idx1.key[3], 2400);
  TEST_VAL(idx1.key[4], 2500);
  TEST_VAL(idx1.key[5], 2600);
  TEST_VAL(idx1.key[6], 2700);
  TEST_VAL(idx1.key[7], 2800);
  TEST_VAL(idx1.key[8], 2900);
  TEST_VAL(idx1.key[9], 3000);

  TEST_VAL((Reg64) idx1.ptr[0], (Reg64) (0xB000));
  TEST_VAL((Reg64) idx1.ptr[1], (Reg64) (0xB100));
  TEST_VAL((Reg64) idx1.ptr[2], (Reg64) (0xB200));
  TEST_VAL((Reg64) idx1.ptr[3], (Reg64) (0xB300));
  TEST_VAL((Reg64) idx1.ptr[4], (Reg64) (0xB400));
  TEST_VAL((Reg64) idx1.ptr[5], (Reg64) (0xB500));
  TEST_VAL((Reg64) idx1.ptr[6], (Reg64) (0xB600));
  TEST_VAL((Reg64) idx1.ptr[7], (Reg64) (0xB700));
  TEST_VAL((Reg64) idx1.ptr[8], (Reg64) (0xB800));
  TEST_VAL((Reg64) idx1.ptr[9], (Reg64) (0xB900));
  TEST_VAL((Reg64) idx1.ptr[10], (Reg64) (0xBA00));

  TEST_VAL(Set::check_high_slide_index_avail(idx1, idx0), 1);

  TRACE;

  Set::slide_index_hi_left(idx1, idx0, k);
  //idx0.print();
  //idx1.print();

  TRACE;


  TEST_VAL(k, 2200);


  TEST_VAL(idx0.count, 7);
  TEST_VAL(idx0.key[0], 1100);
  TEST_VAL(idx0.key[1], 1200);
  TEST_VAL(idx0.key[2], 1300);
  TEST_VAL(idx0.key[3], 1400);
  TEST_VAL(idx0.key[4], 1500);
  TEST_VAL(idx0.key[5], 2000);
  TEST_VAL(idx0.key[6], 2100);
  TEST_VAL((Reg64) idx0.ptr[0], (Reg64) (0xA000));
  TEST_VAL((Reg64) idx0.ptr[1], (Reg64) (0xA100));
  TEST_VAL((Reg64) idx0.ptr[2], (Reg64) (0xA200));
  TEST_VAL((Reg64) idx0.ptr[3], (Reg64) (0xA300));
  TEST_VAL((Reg64) idx0.ptr[4], (Reg64) (0xA400));
  TEST_VAL((Reg64) idx0.ptr[5], (Reg64) (0xA500));
  TEST_VAL((Reg64) idx0.ptr[6], (Reg64) (0xB000));
  TEST_VAL((Reg64) idx0.ptr[7], (Reg64) (0xB100));

  TEST_VAL(idx1.count, 8);
  TEST_VAL(idx1.key[0], 2300);
  TEST_VAL(idx1.key[1], 2400);
  TEST_VAL(idx1.key[2], 2500);
  TEST_VAL(idx1.key[3], 2600);
  TEST_VAL(idx1.key[4], 2700);
  TEST_VAL(idx1.key[5], 2800);
  TEST_VAL(idx1.key[6], 2900);
  TEST_VAL(idx1.key[7], 3000);

  TEST_VAL((Reg64) idx1.ptr[0], (Reg64) (0xB200));
  TEST_VAL((Reg64) idx1.ptr[1], (Reg64) (0xB300));
  TEST_VAL((Reg64) idx1.ptr[2], (Reg64) (0xB400));
  TEST_VAL((Reg64) idx1.ptr[3], (Reg64) (0xB500));
  TEST_VAL((Reg64) idx1.ptr[4], (Reg64) (0xB600));
  TEST_VAL((Reg64) idx1.ptr[5], (Reg64) (0xB700));
  TEST_VAL((Reg64) idx1.ptr[6], (Reg64) (0xB800));
  TEST_VAL((Reg64) idx1.ptr[7], (Reg64) (0xB900));
  TEST_VAL((Reg64) idx1.ptr[8], (Reg64) (0xBA00));

  idx0.reset();
  idx1.reset();

  idx0.ptr[0] = (VoidP) (Reg64) 0xA000;
  idx1.ptr[0] = (VoidP) (Reg64) 0xB000;


  for (Index i = 0; i != 10; i += 1) {
    idx0.append(1100 + (i * 100), (VoidP) (Reg64) (0xA100 + 0x100 * i));
  }
  for (Index i = 0; i != 5; i += 1) {
    idx1.append(2100 + (i * 100), (VoidP) (Reg64) (0xB100 + 0x100 * i));
  }

  //idx0.print();
  //idx1.print();

  TRACE;


  k = 2050;

  TEST_VAL(Set::check_high_slide_index_avail(idx0, idx1), 1);

  Set::slide_index_hi_right(idx0, idx1, k);
  //idx0.print();
  //idx1.print();

  TEST_VAL(k, 1900);

  TEST_VAL(idx0.count, 8);
  TEST_VAL(idx0.key[0], 1100);
  TEST_VAL(idx0.key[1], 1200);
  TEST_VAL(idx0.key[2], 1300);
  TEST_VAL(idx0.key[3], 1400);
  TEST_VAL(idx0.key[4], 1500);
  TEST_VAL(idx0.key[5], 1600);
  TEST_VAL(idx0.key[6], 1700);
  TEST_VAL(idx0.key[7], 1800);

  TEST_VAL((Reg64) idx0.ptr[0], (Reg64) (0xA000));
  TEST_VAL((Reg64) idx0.ptr[1], (Reg64) (0xA100));
  TEST_VAL((Reg64) idx0.ptr[2], (Reg64) (0xA200));
  TEST_VAL((Reg64) idx0.ptr[3], (Reg64) (0xA300));
  TEST_VAL((Reg64) idx0.ptr[4], (Reg64) (0xA400));
  TEST_VAL((Reg64) idx0.ptr[5], (Reg64) (0xA500));
  TEST_VAL((Reg64) idx0.ptr[6], (Reg64) (0xA600));
  TEST_VAL((Reg64) idx0.ptr[7], (Reg64) (0xA700));
  TEST_VAL((Reg64) idx0.ptr[8], (Reg64) (0xA800));

  TEST_VAL(idx1.count, 7);
  TEST_VAL(idx1.key[0], 2000);
  TEST_VAL(idx1.key[1], 2050);
  TEST_VAL(idx1.key[2], 2100);
  TEST_VAL(idx1.key[3], 2200);
  TEST_VAL(idx1.key[4], 2300);
  TEST_VAL(idx1.key[5], 2400);
  TEST_VAL(idx1.key[6], 2500);

  TEST_VAL((Reg64) idx1.ptr[0], (Reg64) (0xa900));
  TEST_VAL((Reg64) idx1.ptr[1], (Reg64) (0xaa00));
  TEST_VAL((Reg64) idx1.ptr[2], (Reg64) (0xb000));
  TEST_VAL((Reg64) idx1.ptr[3], (Reg64) (0xB100));
  TEST_VAL((Reg64) idx1.ptr[4], (Reg64) (0xB200));
  TEST_VAL((Reg64) idx1.ptr[5], (Reg64) (0xB300));
  TEST_VAL((Reg64) idx1.ptr[6], (Reg64) (0xB400));
  TEST_VAL((Reg64) idx1.ptr[7], (Reg64) (0xB500));

  idx0.reset();
  idx1.reset();

  idx0.ptr[0] = Set::to_voidp(&data0);



  for (Index i = 0; i != 10; i += 1) {
    idx0.append(1100 + (i * 100), Set::to_voidp(&data0));
  }

  //idx0.print();



  k = Set::add_entry_split_index(idx0, idx1, 1120, Set::to_voidp(&data1));

  TEST_VAL(k, 1600);

  TEST_VAL(idx0.count, 6);
  TEST_VAL(idx0.key[0], 1100);
  TEST_VAL(idx0.key[1], 1120);
  TEST_VAL(idx0.key[2], 1200);

  TEST_VAL((Reg64) idx0.ptr[0], (Reg64) (&data0));
  TEST_VAL((Reg64) idx0.ptr[1], (Reg64) (&data0));
  TEST_VAL((Reg64) idx0.ptr[2], (Reg64) (&data1));

  TEST_VAL(idx1.count, 4);
  TEST_VAL(idx1.key[0], 1700);
  TEST_VAL(idx1.key[1], 1800);
  TEST_VAL(idx1.key[2], 1900);

  //idx0.print();
  //idx1.print();

  TRACE;


  data0.reset();
  data1.reset();

  for (Index i = 0; i != 10; i += 1) {
    data0.append(i * 13 + 100);
  }

  //data0.print();

  k = Set::add_to_split_data_block(data0, data1, 150);

  TEST_VAL(k, 165);

  //data0.print();
  //data1.print();


  TEST_VAL(data0.count, 6);

  TEST_VAL(data0.key[0],100);
  TEST_VAL(data0.key[1],113);
  TEST_VAL(data0.key[2],126);
  TEST_VAL(data0.key[3],139);
  TEST_VAL(data0.key[4],150);
  TEST_VAL(data0.key[5],152);

  TEST_VAL(data1.count, 4);
  TEST_VAL(data1.key[0],178);
  TEST_VAL(data1.key[1],191);
  TEST_VAL(data1.key[2],204);
  TEST_VAL(data1.key[3],217);



  Key tkey[2000];

  for (Index i = 0; i != 500; i += 1) {
    tkey[i] = 10000 + i * 10;
    tkey[500 + i] = 10000 + i * 10 + 5;
    tkey[1000 + i] = 10000 + i * 10 + 3;
    tkey[1500 + i] = 10000 + i * 10 + 7;
  }

  set.reset();

  for (Index i = 0; i != 1061; i += 1) {
    Index j = 0;
    set.add_key(tkey[i]);
    flag = 1;
    j = 0;
    while (j <= i) {
      k = tkey[j];
      if (set.has_key(k)==0)
        flag = 0;
      j += 1;
    }
    if (flag == 0) {
      WATCH("missing k",k);
      WATCH("i",i);


    }
  }


  set.print();


  flag = 1;
  for (Index i = 0; i != 500; i += 1) {
    if (set.has_key(i * 4)==0) {
      flag = 0;
      WATCH("not found", i * 4);
    }
    if (set.has_key(i * 4 + 1)==0) {
      flag = 0;
      WATCH("not found", i * 4 + 1);
    }
    if (set.has_key(i * 4 + 2)==0) {
      flag = 0;
      WATCH("not found", i * 4 + 2);
    }
    if (set.has_key(i * 4 + 3)==0) {
      flag = 0;
      WATCH("not found", i * 4 + 3);
    }
  }

  set.print();

  TEST_VAL(flag, 1);

  set.dump();

  //set.print();

//  TEST_VAL(Set::is_index_block(top), 1);

  IndexBlock idx0 = IndexBlock();

  TEST_VAL(idx0.count, 0);
  TEST_VAL(idx0.parent == nullptr, 1);

  idx0 = IndexBlock(150, (VoidP) (Reg64) (0xA000), (VoidP) (Reg64) (0xA100));

  Index pos;

  pos = idx0.scan(150);
  TEST_VAL(pos, 0);

  TEST_VAL(idx0.check_match(pos, 150), 1);

  idx0.append(200, (VoidP) (Reg64) (0xA200));
  idx0.append(250, (VoidP) (Reg64) (0xA300));
  idx0.append(300, (VoidP) (Reg64) (0xA400));
  idx0.append(350, (VoidP) (Reg64) (0xA500));
  idx0.append(400, (VoidP) (Reg64) (0xA600));
  idx0.append(450, (VoidP) (Reg64) (0xA700));

  TEST_VAL(idx0.count, 7);

  //idx0.print();


  pos = idx0.scan(275);
  idx0.insert(pos, 275, (VoidP) (Reg64) (0xA800));
  //idx0.print();

  TEST_VAL(idx0.count, 8);
  TEST_VAL(idx0.key[2], 250);
  TEST_VAL((Reg64) idx0.ptr[3], (Reg64) (0xA300));
  TEST_VAL(idx0.key[3], 275);
  TEST_VAL((Reg64) idx0.ptr[4], (Reg64) (0xA800));
  TEST_VAL(idx0.key[4], 300);
  TEST_VAL((Reg64) idx0.ptr[5], (Reg64) (0xA400));

  pos = idx0.scan(300);
  TEST_VAL(pos, 4);
  idx0.remove(pos);
  //idx0.print();

  TEST_VAL(idx0.count, 7);
  TEST_VAL(idx0.key[2], 250);
  TEST_VAL((Reg64) idx0.ptr[3], (Reg64) (0xA300));
  TEST_VAL(idx0.key[4], 350);
  TEST_VAL((Reg64) idx0.ptr[5], (Reg64) (0xA500));
  TEST_VAL(idx0.key[6], 450);
  TEST_VAL((Reg64) idx0.ptr[7], (Reg64) (0xA700));


  IndexBlock idx1 = idx0;

  idx0.remove_last(2);

  //idx0.print();

  TEST_VAL(idx0.count, 5);
  TEST_VAL(idx0.key[2], 250);
  TEST_VAL((Reg64) idx0.ptr[3], (Reg64) (0xA300));
  TEST_VAL(idx0.key[4], 350);
  TEST_VAL((Reg64) idx0.ptr[5], (Reg64) (0xA500));

  idx0 = idx1;

  Key key_buf[20];
  VoidP ptr_buf[20];

  key_buf[0] = 560;
  key_buf[1] = 620;
  ptr_buf[0] = (VoidP) (Reg64) 0xB100;
  ptr_buf[1] = (VoidP) (Reg64) 0xB200;
  ptr_buf[2] = (VoidP) (Reg64) 0xB300;


  idx0.append(key_buf, ptr_buf, 3, 490);

  TEST_VAL(idx0.count, 10);
  TEST_VAL(idx0.key[7], 490);
  TEST_VAL((Reg64) idx0.ptr[8], (Reg64) (0xb100));
  TEST_VAL(idx0.key[9], 620);
  TEST_VAL((Reg64) idx0.ptr[10], (Reg64) (0xb300));


  idx0 = idx1;

  key_buf[0] = 50;
  key_buf[1] = 100;
  ptr_buf[0] = (VoidP) (Reg64) 0xB100;
  ptr_buf[1] = (VoidP) (Reg64) 0xB200;
  ptr_buf[2] = (VoidP) (Reg64) 0xB300;


  //idx0.print();

  idx0.prepend(key_buf, ptr_buf, 3, 130);

  //idx0.print();

  TEST_VAL(idx0.count, 10);
  TEST_VAL(idx0.key[0], 50);
  TEST_VAL((Reg64) idx0.ptr[0], (Reg64) (0xb100));
  TEST_VAL(idx0.key[1], 100);
  TEST_VAL((Reg64) idx0.ptr[1], (Reg64) (0xb200));
  TEST_VAL(idx0.key[2], 130);
  TEST_VAL((Reg64) idx0.ptr[2], (Reg64) (0xb300));
  TEST_VAL(idx0.key[3], 150);
  TEST_VAL((Reg64) idx0.ptr[3], (Reg64) (0xA000));

  DataBlock data0;

  data0 = DataBlock(20);

  TEST_VAL(data0.count, 1);

  data0.append(36);
  data0.append(42);
  data0.append(49);
  data0.append(54);
  data0.append(61);
  data0.append(67);

  TEST_VAL(data0.count, 7);
  TEST_VAL(data0.key[0], 20);
  TEST_VAL(data0.key[1], 36);
  TEST_VAL(data0.key[2], 42);
  TEST_VAL(data0.key[5], 61);
  TEST_VAL(data0.key[6], 67);

  //data0.print();

  pos = data0.scan(49);
  TEST_VAL(data0.check_match(pos, 49), 1);
  TEST_VAL(pos, 3);

  pos = data0.scan(57);
  TEST_VAL(data0.check_match(pos, 57), 0);
  TEST_VAL(pos, 5);

  data0.insert(pos, 57);

  TEST_VAL(data0.count, 8);
  TEST_VAL(data0.key[0], 20);
  TEST_VAL(data0.key[1], 36);
  TEST_VAL(data0.key[2], 42);
  TEST_VAL(data0.key[5], 57);
  TEST_VAL(data0.key[6], 61);
  TEST_VAL(data0.key[7], 67);

  pos = data0.scan(20);
  data0.remove(pos);

  TEST_VAL(data0.count, 7);
  TEST_VAL(data0.key[0], 36);
  TEST_VAL(data0.key[1], 42);
  TEST_VAL(data0.key[4], 57);
  TEST_VAL(data0.key[5], 61);
  TEST_VAL(data0.key[6], 67);

  key_buf[0] = 82;
  key_buf[1] = 99;

  DataBlock data1 = data0;

  data0.append(key_buf, 3, 71);

  TEST_VAL(data0.count, 10);
  TEST_VAL(data0.key[0], 36);
  TEST_VAL(data0.key[1], 42);
  TEST_VAL(data0.key[4], 57);
  TEST_VAL(data0.key[5], 61);
  TEST_VAL(data0.key[6], 67);
  TEST_VAL(data0.key[7], 71);
  TEST_VAL(data0.key[8], 82);
  TEST_VAL(data0.key[9], 99);

  //data0.print();

  data0 = data1;

  key_buf[0] = 21;
  key_buf[1] = 27;

  data0.prepend(key_buf, 3, 33);

  TEST_VAL(data0.count, 10);
  TEST_VAL(data0.key[0], 21);
  TEST_VAL(data0.key[1], 27);
  TEST_VAL(data0.key[2], 33);
  TEST_VAL(data0.key[3], 36);
  TEST_VAL(data0.key[4], 42);
  TEST_VAL(data0.key[5], 49);
  TEST_VAL(data0.key[6], 54);
  TEST_VAL(data0.key[7], 57);
  TEST_VAL(data0.key[8], 61);
  TEST_VAL(data0.key[9], 67);

  //data0.print();

  data0.remove_first(5);

  TEST_VAL(data0.count, 5);
  TEST_VAL(data0.key[0], 49);
  TEST_VAL(data0.key[1], 54);
  TEST_VAL(data0.key[2], 57);
  TEST_VAL(data0.key[3], 61);
  TEST_VAL(data0.key[4], 67);

  data0.remove_last(2);

  TEST_VAL(data0.count, 3);
  TEST_VAL(data0.key[0], 49);
  TEST_VAL(data0.key[1], 54);
  TEST_VAL(data0.key[2], 57);

  data0 = data1;


  Key data_set[40] = { 12, 17, 23, 29, 34, 44, 50, 55, 61, 66,
    69, 73, 78, 85, 89, 94, 96, 101, 114, 121,
    127, 133, 138 ,144, 149, 156, 161, 174, 179, 188,
    189, 194, 199, 201, 207, 214, 217, 225, 236, 239};

  IndexBlock * idxp0, *idxp1;
  DataBlock * datap0, * datap1, * datap2, * datap3;

  datap0 = &Set::req_data_block();
  datap1 = &Set::req_data_block();
  datap2 = &Set::req_data_block();
  datap3 = &Set::req_data_block();

  (*datap0).reset(0);
  (*datap1).reset(0);
  (*datap2).reset(0);
  (*datap3).reset(0);

  Index k0, k1, k2;

  pos = 0;

  for (Index i = 0; i !=  5; i += 1) {
    (*datap0).append(data_set[pos++]);
  }

  k0 = data_set[pos++];

  for (Index i = 0; i !=  10; i += 1) {
    (*datap1).append(data_set[pos++]);
  }

  k1 = data_set[pos++];

  for (Index i = 0; i !=  4; i += 1) {
    (*datap2).append(data_set[pos++]);
  }

  k2 = data_set[pos++];

  for (Index i = 0; i !=  7; i += 1) {
    (*datap3).append(data_set[pos++]);
  }

  idxp0 = &Set::req_index_block();
  (*idxp0) = IndexBlock(k0, (VoidP) datap0, (VoidP) datap1);
  (*idxp0).append(k1, (VoidP) datap2);
  (*idxp0).append(k2, (VoidP) datap3);

  Set::reset_parent_links(*idxp0);


  VoidP top = Set::to_voidp(idxp0);

  Set set = Set(top);
  set.print();

  uint32_t hash_code = set.hash_contents();

  (*idxp0).print();
  (*datap0).print();
  (*datap1).print();

  TEST_VAL((*datap0).count, 5);
  TEST_VAL((*datap1).count, 10);

  TEST_VAL((*datap0).key[4], 34);
  TEST_VAL((*datap1).key[0], 50);
  TEST_VAL((*datap1).key[1], 55);
  TEST_VAL((*datap1).key[7], 85);
  TEST_VAL((*datap1).key[8], 89);
  TEST_VAL((*datap1).key[9], 94);

  TEST_VAL((*idxp0).key[0], 44);
  TEST_VAL((*idxp0).key[1], 96);


  Set::restruct_data_hi_slide_left(*datap0, *datap1, 1);

  TEST_VAL((*datap0).count, 7);
  TEST_VAL((*datap1).count, 8);

  TEST_VAL(hash_code, set.hash_contents());


  set.print();

  (*datap0).reset(0);
  (*datap1).reset(0);
  (*datap2).reset(0);
  (*datap3).reset(0);

  pos = 0;

  for (Index i = 0; i !=  5; i += 1) {
    (*datap0).append(data_set[pos++]);
  }

  k0 = data_set[pos++];

  for (Index i = 0; i !=  10; i += 1) {
    (*datap1).append(data_set[pos++]);
  }

  k1 = data_set[pos++];

  for (Index i = 0; i !=  4; i += 1) {
    (*datap2).append(data_set[pos++]);
  }

  k2 = data_set[pos++];

  for (Index i = 0; i !=  7; i += 1) {
    (*datap3).append(data_set[pos++]);
  }

  (*idxp0) = IndexBlock(k0, (VoidP) datap0, (VoidP) datap1);
  (*idxp0).append(k1, (VoidP) datap2);
  (*idxp0).append(k2, (VoidP) datap3);

  Set::reset_parent_links(*idxp0);

  TEST_VAL((*datap0).count, 5);
  TEST_VAL((*datap1).count, 10);

  TEST_VAL((*datap0).key[4], 34);
  TEST_VAL((*datap1).key[0], 50);
  TEST_VAL((*datap1).key[1], 55);
  TEST_VAL((*datap1).key[7], 85);
  TEST_VAL((*datap1).key[8], 89);
  TEST_VAL((*datap1).key[9], 94);

  TEST_VAL((*idxp0).key[0], 44);
  TEST_VAL((*idxp0).key[1], 96);

//  set.print();
  (*idxp0).print();
  (*datap0).print();

  (*datap1).print();
  (*datap2).print();
  (*datap3).print();

  set.print();

  Set::restruct_data_hi_slide_right(*datap1, *datap2, 1);

  (*idxp0).print();
  (*datap0).print();

  (*datap1).print();
  (*datap2).print();
  (*datap3).print();

  set.print();

  TEST_VAL((*datap1).count, 7);
  TEST_VAL((*datap2).count, 7);

  TEST_VAL(hash_code, set.hash_contents());

  END_TEST;
};
*/



void array_map_test() {
  using namespace Arca::Array;

  using Index = uint32_t;
  using Elem = uint64_t;

  START_TEST("ArrayMap Test");

  Elem data[200];
  //Elem data2[200];

  struct HeapBase {
	  Bool compare(Elem x, Elem y) {
		  return (x > y) ? 1: 0;
	  }
  };

  Index count;

  Elem * p = data;
  auto heap = ArrayHeap<Index, Elem, HeapBase>(p, count, 20);
  heap.clear();

  TEST_VAL(count, 0);
  TEST_VAL(heap.is_empty(), 1);

  heap.push(64);
  heap.push(31);
  heap.push(7);
  heap.push(25);
  heap.push(88);

  TEST_VAL(count, 5);
  TEST_VAL(heap.peek(), 88);

  Elem e = heap.pop();
  TEST_VAL(count, 4);
  TEST_VAL(e, 88);
  e = heap.pop();
  TEST_VAL(count, 3);
  TEST_VAL(e, 64);
  e = heap.pop();
  TEST_VAL(count, 2);
  TEST_VAL(e, 31);
  e = heap.pop();
  TEST_VAL(count, 1);
  TEST_VAL(e, 25);
  e = heap.pop();
  TEST_VAL(count, 0);
  TEST_VAL(e, 7);

  struct SetBase1 {
	  static Index hash_elem(Elem e) {
		  return e;
	  }
	  static Elem empty_value() {
		  return 999;
	  }
  };

  Index set_ct = 0;
  auto set1 = ArraySet<Index, Elem, SetBase1>(data, 200, set_ct);
  set1.clear();

  TEST_VAL(set_ct,0);

  for (Index i = 0; i != 50; i += 1) { set1.insert(i);}
  TEST_VAL(set_ct,50);

  // k = set1.stats();

  // WATCH("stats - k", k);

  Elem sum = 0;
  for (Index i = 0; i != 50; i += 1) { sum += set1.search(i);}
  TEST_VAL(sum,50);

  sum = set1.search(99);
  TEST_VAL(sum,0);

  sum = set1.remove(99);
  TEST_VAL(sum,0);

  sum = set1.remove(0);
  TEST_VAL(sum,1);

  TEST_VAL(set_ct,49);
  sum = 0;
  for (Index i = 0; i != 50; i += 1) { sum += set1.search(i);}
  TEST_VAL(sum,49);
  sum = 0;
  for (Index i = 0; i != 50; i += 1) { sum += set1.remove(i);}
  TEST_VAL(sum,49);
  TEST_VAL(set_ct,0);

  END_TEST;
};


void arca_array_test() {
  START_TEST("arca_array Test");


  using namespace Arca::Array;

  using Index = uint32_t;
  using Elem = uint64_t;


  Elem data[200];
  auto array_fn = ArrayFn<Index, Elem>(data, 200);

  array_fn.clear(0);

  Elem sum = 0;
  for (Index i = 0; i < 200; i += 1) {
        sum += data[i];
  }

  TEST_VAL(sum, 0);

  for (Index i = 0; i < 200; i += 10) {
	  data[i] = 10;
  }

  sum = 0;
  auto f1 = [&sum](Elem e){sum += e;};
  array_fn.each(f1);

  TEST_VAL(sum, 200);

  array_fn.fill(1);
  array_fn.each(f1);

  TEST_VAL(sum, 400);

  sum = 0;
  array_fn.fill(2,100,100);
  array_fn.each(f1);
  TEST_VAL(sum, 300);
  auto f2 = [](Elem e) {return (Bool) (e == 7);}; 

  Index k = array_fn.find_max_elem();
  TEST_VAL(k, 2);

  Bool b1 = array_fn.check_any(f2);
  TEST_VAL(b1, 0);
  data[3] = 7;
  b1 = array_fn.check_any(f2);
  TEST_VAL(b1, 1);
  b1 = array_fn.check_all(f2);
  TEST_VAL(b1, 0);
  array_fn.fill(7);
  b1 = array_fn.check_all(f2);

  k = array_fn.find_max_elem();
  TEST_VAL(k, 7);
  data[199] = 401;
  k = array_fn.find_max_elem();
  data[3] = 403;
  k = array_fn.find_max_elem();
  TEST_VAL(k, 403);
  k = array_fn.find_min_elem();
  TEST_VAL(k, 7);

  data[0] = 9;

  Pair<Elem, Elem> p0;
  p0 = array_fn.find_min_max_elem();

  TEST_VAL(p0.a, 7);
  TEST_VAL(p0.b, 403);

  data[0] = 3;
  data[1] = 1000;
  data[99] = 601;
  data[199] = 440;
  p0 = array_fn.find_min_max_elem();
  TEST_VAL(p0.a, 3);
  TEST_VAL(p0.b, 1000);
  array_fn.reverse();

  TEST_VAL(data[199],3);
  TEST_VAL(data[198],1000);
  TEST_VAL(data[0],440);
  TEST_VAL(data[100],601);

  auto f3 = [](Elem e){return (Elem) e * 2;};

  array_fn.map(f3);

  data[1] = 66;
  data[101] = 333;
  TEST_VAL(data[100],1202);

  array_fn.shl(1, 100, 1);

  TEST_VAL(data[101],333);
  TEST_VAL(data[99],1202);
  TEST_VAL(data[0],66);

  array_fn.shr(0, 100, 1);
  TEST_VAL(data[1],66);
  TEST_VAL(data[100],1202);


  array_fn.clear(10);

  sum = 0;
  for (Index i = 0; i < 200; i += 1) {
        sum += data[i];
  }
  TEST_VAL(sum, 2000);

  Elem data2[200];
  Elem data3[200];

  array_fn.copy_to(data2);

  sum = 0;
  for (Index i = 0; i < 200; i += 1) {
        sum += data2[i];
  }
  TEST_VAL(sum, 2000);

  sum = 0;
  for (Index i = 0; i < 200; i += 1) {
        sum += data2[i];
  }
  array_fn.copy_to(data3);
  TEST_VAL(sum, 2000);

  for (Index i = 0; i < 200; i += 1) {
        data[i] = i * 10;
  }

  Index pos = array_fn.find(14);

  TEST_VAL(pos, 200);

  pos = array_fn.find(140);

  TEST_VAL(pos,14 );

  array_fn.copy_to(data2);

  pos = array_fn.match_buf(data2);

  TEST_VAL(pos,200 );

  data[100] = 0;

  pos = array_fn.match_buf(data2);

  TEST_VAL(pos,100 );

  for (Index i = 0; i < 20; i += 1)
          data[i] = 20 - i;

  // here
  
  auto array_fn2 = ArrayFn<Index, Elem>(data, 20);

  array_fn2.ins_sort();
  TEST_VAL(data[0],1);
  TEST_VAL(data[19],20);

  for (Index i = 0; i < 20; i += 1)
          data[i] = 20 - i;

  array_fn2.quick_sort();

  TEST_VAL(data[0],1);
  TEST_VAL(data[1],2);
  TEST_VAL(data[7],8);
  TEST_VAL(data[10],11);
  TEST_VAL(data[19],20);

  pos = array_fn2.check_sort();

  TEST_VAL(pos, 20);

  data[10] = 0;
  pos = array_fn2.check_sort();

  TEST_VAL(pos, 10);

  for (Index i = 0; i < 20; i += 1)
          data[i] = i * 4;

  pos = array_fn2.check_sort();

  TEST_VAL(pos, 20);

  pos = array_fn2.bsearch(16);

  TEST_VAL(pos, 4);
  pos = array_fn2.bsearch(14);
  TEST_VAL(pos, 3);

  Index counter = 0;
  auto list = ArrayListFn<Index, Elem>(data, 200, counter);

  list.clear();

  TEST_VAL(counter,0);

  list.pushf(23);
  list.pushf(16);
  list.append(44);

  TEST_VAL(counter, 3);

  TEST_VAL(data[0], 16);
  TEST_VAL(data[1], 23);
  TEST_VAL(data[2], 44);

  k = list.popb();

  TEST_VAL(counter, 2);
  TEST_VAL(k, 44);

  list.insert(18,1); 

  TEST_VAL(data[0], 16);
  TEST_VAL(data[1], 18);
  TEST_VAL(data[2], 23);
  END_TEST;
}

void tree_test() {
  START_TEST("Tree Test");

  using TreeFn = Arca::Tree::TreeFn;

  uint32_t key1[100] = {20,30,40,50,0,0,0,0};

  uint8_t count = 4;
  uint8_t pos = TreeFn::search<uint32_t>(19, count, key1);
  TEST_VAL(pos, 0);
  pos = TreeFn::search<uint32_t>(20, count, key1);
  TEST_VAL(pos, 1);
  pos = TreeFn::search<uint32_t>(30, count, key1);
  TEST_VAL(pos, 2);
  pos = TreeFn::search<uint32_t>(40, count, key1);
  TEST_VAL(pos, 3);
  pos = TreeFn::search<uint32_t>(50, count, key1);
  TEST_VAL(pos, 4);
  TreeFn::ins_key<uint32_t>(55, pos, count, key1);
  TEST_VAL(count, 5);
  TEST_VAL(key1[3], 50);
  TEST_VAL(key1[4], 55);
  pos = TreeFn::search<uint32_t>(25, 4, key1);
  TreeFn::ins_key<uint32_t>(25, pos, count, key1);
  TEST_VAL(count, 6);
  TEST_VAL(key1[0], 20);
  TEST_VAL(key1[1], 25);
  TEST_VAL(key1[2], 30);
  TEST_VAL(key1[3], 40);
  TEST_VAL(key1[4], 50);
  TEST_VAL(key1[5], 55);
  pos = TreeFn::search<uint32_t>(40, count, key1);
  TEST_VAL(pos, 4);
  pos = TreeFn::search<uint32_t>(56, count, key1);
  TEST_VAL(pos, 6);
  pos = TreeFn::search<uint32_t>(30, count, key1);
  TEST_VAL(pos, 3);
  TreeFn::del_key<uint32_t>(pos, count, key1);
  TEST_VAL(count, 5);
  TEST_VAL(key1[0], 20);
  TEST_VAL(key1[1], 25);
  TEST_VAL(key1[2], 40);
  TEST_VAL(key1[3], 50);
  TEST_VAL(key1[4], 55);
  pos = TreeFn::search<uint32_t>(20, count, key1);
  TreeFn::del_key<uint32_t>(pos, count, key1);
  TEST_VAL(count, 4);
  TEST_VAL(key1[0], 25);
  TEST_VAL(key1[1], 40);
  TEST_VAL(key1[2], 50);
  TEST_VAL(key1[3], 55);
  pos = TreeFn::search<uint32_t>(55, count, key1);
  TreeFn::del_key<uint32_t>(pos, count, key1);
  TEST_VAL(count, 3);
  TEST_VAL(key1[0], 25);
  TEST_VAL(key1[1], 40);
  TEST_VAL(key1[2], 50);

  uint8_t ct1 = 8;
  uint8_t ct2 = 0;
  uint32_t p = 0;

  uint32_t key2[8] = {20,30,40,50,60,70,80,90};
  uint32_t key3[8] = {0,0,0,0,0,0,0,0};

  TreeFn::split_key<uint32_t>(ct1, ct2, key2, key3, p);
  TEST_VAL(key3[0], 70);
  TEST_VAL(key3[1], 80);
  TEST_VAL(key3[2], 90);
  TEST_VAL(ct1, 4);
  TEST_VAL(ct2, 3);
  TEST_VAL(p, 60);

  key2[4] = 0;
  key2[5] = 0;
  key2[6] = 0;
  key2[7] = 0;

  TreeFn::merge_key<uint32_t>(ct1, ct2, key2, key3, p);
  TEST_VAL(ct1, 8);
  TEST_VAL(ct2, 0);
  TEST_VAL(key2[0], 20);
  TEST_VAL(key2[1], 30);
  TEST_VAL(key2[2], 40);
  TEST_VAL(key2[3], 50);
  TEST_VAL(key2[4], 60);
  TEST_VAL(key2[5], 70);
  TEST_VAL(key2[6], 80);
  TEST_VAL(key2[7], 90);

  count = 2;
  key1[0] = 20;
  key1[1] = 30;
  VoidP ptr1[9];
  VoidP ptr2[9];

  ptr1[0] = (VoidP) 100;
  ptr1[1] = (VoidP) 200;
  ptr1[2] = (VoidP) 300;
  pos = TreeFn::search<uint32_t>(25, count, key1);
  TEST_VAL(pos, 1);
  TreeFn::ins_key_ptr<uint32_t>(25, (VoidP) 350, pos, count, key1, ptr1);

  TEST_VAL(count, 3);
  TEST_VAL(key1[0], 20);
  TEST_VAL(key1[1], 25);
  TEST_VAL(key1[2], 30);

  TEST_VAL((uint64_t) ptr1[0], 100);
  TEST_VAL((uint64_t) ptr1[1], 200);
  TEST_VAL((uint64_t) ptr1[2], 350);
  TEST_VAL((uint64_t) ptr1[3], 300);

  pos = TreeFn::search<uint32_t>(25, count, key1);
  TEST_VAL(pos, 2);
  TreeFn::del_key_ptr<uint32_t>(pos, count, key1, ptr1);

  TEST_VAL(count, 2);
  TEST_VAL(key1[0], 20);
  TEST_VAL(key1[1], 30);

  TEST_VAL((uint64_t) ptr1[0], 100);
  TEST_VAL((uint64_t) ptr1[1], 200);
  TEST_VAL((uint64_t) ptr1[2], 300);

  pos = TreeFn::search<uint32_t>(30, count, key1);
  TreeFn::del_key_ptr<uint32_t>(pos, count, key1, ptr1);

  TEST_VAL(pos, 2);

  TEST_VAL(count, 1);
  TEST_VAL(key1[0], 20);

  TEST_VAL((uint64_t) ptr1[0], 100);
  TEST_VAL((uint64_t) ptr1[1], 200);

  ct1 = 8;
  key1[0] = 20;
  key1[1] = 25;
  key1[2] = 30;
  key1[3] = 35;
  key1[4] = 40;
  key1[5] = 45;
  key1[6] = 50;
  key1[7] = 55;

  ptr1[0] = (VoidP) 100;
  ptr1[1] = (VoidP) 200;
  ptr1[2] = (VoidP) 300;
  ptr1[3] = (VoidP) 400;
  ptr1[4] = (VoidP) 500;
  ptr1[5] = (VoidP) 600;
  ptr1[6] = (VoidP) 700;
  ptr1[7] = (VoidP) 800;
  ptr1[8] = (VoidP) 900;

  TreeFn::split_key_ptr<uint32_t>(ct1, ct2, key1, key2, ptr1, ptr2, p);

  TEST_VAL(ct1, 4);
  TEST_VAL(ct2, 3);

  TEST_VAL(p, 40);

  TEST_VAL(key2[0], 45);
  TEST_VAL(key2[1], 50);
  TEST_VAL(key2[2], 55);

  TEST_VAL((uint64_t) ptr2[0], 600);
  TEST_VAL((uint64_t) ptr2[1], 700);
  TEST_VAL((uint64_t) ptr2[2], 800);
  TEST_VAL((uint64_t) ptr2[3], 900);

  key1[4] = 0;
  key1[5] = 0;
  key1[6] = 0;
  key1[7] = 0;
  ptr1[5] = (VoidP) 0;
  ptr1[6] = (VoidP) 0;
  ptr1[7] = (VoidP) 0;
  ptr1[8] = (VoidP) 0;


  TreeFn::merge_key_ptr<uint32_t>(ct1, ct2, key1, key2, ptr1, ptr2, p);

  TEST_VAL(ct1, 8);
  TEST_VAL(key1[0], 20);
  TEST_VAL(key1[1], 25);
  TEST_VAL(key1[2], 30);
  TEST_VAL(key1[3], 35);
  TEST_VAL(key1[4], 40);
  TEST_VAL(key1[5], 45);
  TEST_VAL(key1[6], 50);
  TEST_VAL(key1[7], 55);

  TEST_VAL((uint64_t) ptr1[0], 100);
  TEST_VAL((uint64_t) ptr1[1], 200);
  TEST_VAL((uint64_t) ptr1[2], 300);
  TEST_VAL((uint64_t) ptr1[3], 400);
  TEST_VAL((uint64_t) ptr1[4], 500);
  TEST_VAL((uint64_t) ptr1[5], 600);
  TEST_VAL((uint64_t) ptr1[6], 700);
  TEST_VAL((uint64_t) ptr1[7], 800);
  TEST_VAL((uint64_t) ptr1[8], 900);

  using Set = Arca::Tree::TreeSet<uint32_t, 8>;

  Set set = Set();
  set.add_key(20);
  set.add_key(31);
  set.add_key(14);
  set.add_key(64);
  set.add_key(21);
  set.add_key(15);
  set.add_key(19);
  set.add_key(6);

  uint8_t i = 0;

  auto f = [&i, &key1](uint32_t x) { key1[i++] = x;};
  set.each(f);

  TEST_VAL(key1[0], 6);
  TEST_VAL(key1[1], 14);
  TEST_VAL(key1[2], 15);
  TEST_VAL(key1[3], 19);
  TEST_VAL(key1[4], 20);
  TEST_VAL(key1[5], 21);
  TEST_VAL(key1[6], 31);
  TEST_VAL(key1[7], 64);

  set.add_key(10);
  i = 0;
  //auto f = [&i, &key1](uint32_t x) { key1[i++] = x;};
  set.each(f);

  TEST_VAL(key1[0], 6);
  TEST_VAL(key1[1], 10);
  TEST_VAL(key1[2], 14);
  TEST_VAL(key1[3], 15);
  TEST_VAL(key1[4], 19);
  TEST_VAL(key1[5], 20);
  TEST_VAL(key1[6], 21);
  TEST_VAL(key1[7], 31);
  TEST_VAL(key1[8], 64);

  uint8_t n = 49;

  Set set1 = Set();
  for (uint8_t i = 0; i != n; i += 1) {
      set1.add_key(i * 10 + 10);
  }

  i = 0;
  set1.each(f);
  set1.dump();
  TEST_VAL(i, n);
  for (uint8_t i = 0; i != n; i += 1) {
      TEST_VAL(key1[i], i * 10 + 10);
  }

  END_TEST;
}



void asm_test() {
  START_TEST("ASM Test");

	using namespace Arca::Asm;

// Reg64 x, y;
  TEST_VAL(popcnt((Reg64) 19), 3);
  TEST_VAL(popcnt((Reg32) 24), 2);
  TEST_VAL(setz((Reg32) 45), 0);
  TEST_VAL(setnz((Reg64) 0), 0);
  TEST_VAL(shl((Reg32) 3, 1), 6);
  TEST_VAL(ror((Reg64) (1024 * 1024), 4), (Reg64) (1024 * 64));
  TEST_VAL(ror((Reg64) 0xf000000000000000, 4), 0x0f00000000000000);
  TEST_VAL(rol((Reg64) 0xf000000000000000, 4), 0x000000000000000f);
  TEST_VAL(bt((Reg32) 3, 1), 1);
  TEST_VAL(btr((Reg32) 3, 0), 2);
  TEST_VAL(btr((Reg64) 19, 4), 3);
  TEST_VAL(bts((Reg64) 0xf000000000000000, 32), 0xf000000100000000);
  TEST_VAL(bts((Reg64) 64, 4), 80);
  TEST_VAL(seta((Reg32) 20, (Reg32) 10), 1);
  TEST_VAL(seteq((Reg64) 2011, (Reg64) 2011), 1);

//  TEST_VAL(via_xor_mask( (Reg8) 0, (Reg32) 45, (Reg32) 33), (Reg32) 45);
//  TEST_VAL(via_xor_mask( (Reg8) 1, (Reg32) 45, (Reg32) 33), (Reg32) 33);

  TEST_VAL(bsf( (Reg32) 64), 6);
  TEST_VAL(bsf( (Reg32) 65), 0);
  TEST_VAL(bsr( (Reg64) 65), 6);

  TEST_VAL(cmovz<Reg32>( (Reg32) 0, (Reg32) 49, (Reg32) 12), (Reg32) 12);
  TEST_VAL(cmovz<Reg32>( (Reg32) 100, (Reg64) 49, (Reg64) 12), (Reg64) 49);

  TEST_VAL(cmovnz<Reg64>( (Reg64) 0, (Reg32) 49, (Reg32) 12), (Reg32) 49);
  TEST_VAL(cmovnz<Reg64>( (Reg64) 100, (Reg64) 49, (Reg64) 12), (Reg64) 12);

  TEST_VAL(cmova<Reg32>( (Reg32) 0, (Reg32) 100, (Reg32) 49, (Reg32) 12), (Reg32) 49);
  TEST_VAL(cmova<Reg32>( (Reg32) 100, (Reg32) 0, (Reg32) 49, (Reg32) 12), (Reg32) 12);

  Reg64 x, y; x = 14; y = 21;
  xor_sort(x, y);

  TEST_VAL(x, 14);
  TEST_VAL(y, 21);

  x = 34; y = 21;
  xor_sort(x, y);
  TEST_VAL(x, 21);
  TEST_VAL(y, 34);


  cpu_pause();
  cpu_sfence();


  END_TEST;
}

int main() {
	asm_test();
	arca_array_test();
	array_map_test();
	//alloc_test();
	//tree_block_test();
	tree_test();


}
