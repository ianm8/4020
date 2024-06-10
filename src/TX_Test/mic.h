#ifndef DSP_H
#define DSP_H

#define FIR_LENGTH 256
#define __mac_tap(_h) acc += (_h)*x[i++]

#define TEST_SIG_SAMPLES 125
static const int16_t test_1k[TEST_SIG_SAMPLES] =
{
  2000,
  1960,
  1840,
  1647,
  1387,
  1072,
  713,
  325,
  -75,
  -473,
  -852,
  -1196,
  -1492,
  -1728,
  -1894,
  -1984,
  -1994,
  -1924,
  -1776,
  -1557,
  -1275,
  -941,
  -570,
  -176,
  226,
  618,
  985,
  1313,
  1588,
  1799,
  1937,
  1997,
  1977,
  1877,
  1702,
  1458,
  1155,
  806,
  424,
  25,
  -375,
  -760,
  -1114,
  -1423,
  -1675,
  -1860,
  -1969,
  -1999,
  -1949,
  -1820,
  -1618,
  -1351,
  -1029,
  -666,
  -276,
  126,
  522,
  897,
  1236,
  1525,
  1753,
  1910,
  1990,
  1990,
  1910,
  1753,
  1525,
  1236,
  897,
  522,
  126,
  -276,
  -666,
  -1029,
  -1351,
  -1618,
  -1820,
  -1949,
  -1999,
  -1969,
  -1860,
  -1675,
  -1423,
  -1114,
  -760,
  -375,
  25,
  424,
  806,
  1155,
  1458,
  1702,
  1877,
  1977,
  1997,
  1937,
  1799,
  1588,
  1313,
  985,
  618,
  226,
  -176,
  -570,
  -941,
  -1275,
  -1557,
  -1776,
  -1924,
  -1994,
  -1984,
  -1894,
  -1728,
  -1492,
  -1196,
  -852,
  -473,
  -75,
  325,
  713,
  1072,
  1387,
  1647,
  1840,
  1960
};

static const int16_t test_500[TEST_SIG_SAMPLES] =
{
  2000,
  1990,
  1960,
  1910,
  1840,
  1753,
  1647,
  1525,
  1387,
  1236,
  1072,
  897,
  713,
  522,
  325,
  126,
  -75,
  -276,
  -473,
  -666,
  -852,
  -1029,
  -1196,
  -1351,
  -1492,
  -1618,
  -1728,
  -1820,
  -1894,
  -1949,
  -1984,
  -1999,
  -1994,
  -1969,
  -1924,
  -1860,
  -1776,
  -1675,
  -1557,
  -1423,
  -1275,
  -1114,
  -941,
  -760,
  -570,
  -375,
  -176,
  25,
  226,
  424,
  618,
  806,
  985,
  1155,
  1313,
  1458,
  1588,
  1702,
  1799,
  1877,
  1937,
  1977,
  1997,
  1997,
  1977,
  1937,
  1877,
  1799,
  1702,
  1588,
  1458,
  1313,
  1155,
  985,
  806,
  618,
  424,
  226,
  25,
  -176,
  -375,
  -570,
  -760,
  -941,
  -1114,
  -1275,
  -1423,
  -1557,
  -1675,
  -1776,
  -1860,
  -1924,
  -1969,
  -1994,
  -1999,
  -1984,
  -1949,
  -1894,
  -1820,
  -1728,
  -1618,
  -1492,
  -1351,
  -1196,
  -1029,
  -852,
  -666,
  -473,
  -276,
  -75,
  126,
  325,
  522,
  713,
  897,
  1072,
  1236,
  1387,
  1525,
  1647,
  1753,
  1840,
  1910,
  1960,
  1990
};

static const int16_t __not_in_flash_func(lpf_2400)(const int16_t sample)
{
   // 31250
   static int32_t x[FIR_LENGTH] = { 0 };
   static uint8_t sample_index = 0;
   uint8_t i = sample_index;
   x[sample_index--] = sample;
   int32_t acc = 0;
   __mac_tap(-3);
   __mac_tap(-5);
   __mac_tap(-6);
   __mac_tap(-5);
   __mac_tap(-2);
   __mac_tap(2);
   __mac_tap(7);
   __mac_tap(12);
   __mac_tap(15);
   __mac_tap(14);
   __mac_tap(8);
   __mac_tap(-1);
   __mac_tap(-14);
   __mac_tap(-26);
   __mac_tap(-34);
   __mac_tap(-35);
   __mac_tap(-27);
   __mac_tap(-10);
   __mac_tap(14);
   __mac_tap(39);
   __mac_tap(59);
   __mac_tap(68);
   __mac_tap(60);
   __mac_tap(36);
   __mac_tap(-3);
   __mac_tap(-49);
   __mac_tap(-91);
   __mac_tap(-117);
   __mac_tap(-118);
   __mac_tap(-89);
   __mac_tap(-32);
   __mac_tap(42);
   __mac_tap(118);
   __mac_tap(177);
   __mac_tap(201);
   __mac_tap(177);
   __mac_tap(105);
   __mac_tap(-6);
   __mac_tap(-133);
   __mac_tap(-247);
   __mac_tap(-318);
   __mac_tap(-320);
   __mac_tap(-242);
   __mac_tap(-90);
   __mac_tap(109);
   __mac_tap(315);
   __mac_tap(476);
   __mac_tap(546);
   __mac_tap(489);
   __mac_tap(298);
   __mac_tap(-8);
   __mac_tap(-375);
   __mac_tap(-726);
   __mac_tap(-972);
   __mac_tap(-1026);
   __mac_tap(-824);
   __mac_tap(-338);
   __mac_tap(414);
   __mac_tap(1367);
   __mac_tap(2415);
   __mac_tap(3430);
   __mac_tap(4276);
   __mac_tap(4836);
   __mac_tap(5033);
   __mac_tap(4836);
   __mac_tap(4276);
   __mac_tap(3430);
   __mac_tap(2415);
   __mac_tap(1367);
   __mac_tap(414);
   __mac_tap(-338);
   __mac_tap(-824);
   __mac_tap(-1026);
   __mac_tap(-972);
   __mac_tap(-726);
   __mac_tap(-375);
   __mac_tap(-8);
   __mac_tap(298);
   __mac_tap(489);
   __mac_tap(546);
   __mac_tap(476);
   __mac_tap(315);
   __mac_tap(109);
   __mac_tap(-90);
   __mac_tap(-242);
   __mac_tap(-320);
   __mac_tap(-318);
   __mac_tap(-247);
   __mac_tap(-133);
   __mac_tap(-6);
   __mac_tap(105);
   __mac_tap(177);
   __mac_tap(201);
   __mac_tap(177);
   __mac_tap(118);
   __mac_tap(42);
   __mac_tap(-32);
   __mac_tap(-89);
   __mac_tap(-118);
   __mac_tap(-117);
   __mac_tap(-91);
   __mac_tap(-49);
   __mac_tap(-3);
   __mac_tap(36);
   __mac_tap(60);
   __mac_tap(68);
   __mac_tap(59);
   __mac_tap(39);
   __mac_tap(14);
   __mac_tap(-10);
   __mac_tap(-27);
   __mac_tap(-35);
   __mac_tap(-34);
   __mac_tap(-26);
   __mac_tap(-14);
   __mac_tap(-1);
   __mac_tap(8);
   __mac_tap(14);
   __mac_tap(15);
   __mac_tap(12);
   __mac_tap(7);
   __mac_tap(2);
   __mac_tap(-2);
   __mac_tap(-5);
   __mac_tap(-6);
   __mac_tap(-5);
   __mac_tap(-3);
   return (int16_t)(acc >> 15);
}

static const int16_t __not_in_flash_func(lpf_fs4)(const int16_t sample)
{
   // 31250
   // Taps: 199 (effectively 100 since cutoff is Fs/4)
   // Att: 45
   static int32_t x[FIR_LENGTH] = { 0 };
   static uint8_t sample_index = 0;
   uint8_t i = sample_index;
   x[sample_index--] = sample;
   int32_t acc = 0;
   __mac_tap(-10);
   __mac_tap(0);
   __mac_tap(11);
   __mac_tap(0);
   __mac_tap(-14);
   __mac_tap(0);
   __mac_tap(15);
   __mac_tap(0);
   __mac_tap(-18);
   __mac_tap(0);
   __mac_tap(20);
   __mac_tap(0);
   __mac_tap(-24);
   __mac_tap(0);
   __mac_tap(25);
   __mac_tap(0);
   __mac_tap(-30);
   __mac_tap(0);
   __mac_tap(32);
   __mac_tap(0);
   __mac_tap(-37);
   __mac_tap(0);
   __mac_tap(40);
   __mac_tap(0);
   __mac_tap(-45);
   __mac_tap(0);
   __mac_tap(49);
   __mac_tap(0);
   __mac_tap(-54);
   __mac_tap(0);
   __mac_tap(59);
   __mac_tap(0);
   __mac_tap(-65);
   __mac_tap(0);
   __mac_tap(70);
   __mac_tap(0);
   __mac_tap(-78);
   __mac_tap(0);
   __mac_tap(83);
   __mac_tap(0);
   __mac_tap(-92);
   __mac_tap(0);
   __mac_tap(99);
   __mac_tap(0);
   __mac_tap(-108);
   __mac_tap(0);
   __mac_tap(116);
   __mac_tap(0);
   __mac_tap(-127);
   __mac_tap(0);
   __mac_tap(136);
   __mac_tap(0);
   __mac_tap(-148);
   __mac_tap(0);
   __mac_tap(160);
   __mac_tap(0);
   __mac_tap(-174);
   __mac_tap(0);
   __mac_tap(187);
   __mac_tap(0);
   __mac_tap(-204);
   __mac_tap(0);
   __mac_tap(220);
   __mac_tap(0);
   __mac_tap(-240);
   __mac_tap(0);
   __mac_tap(260);
   __mac_tap(0);
   __mac_tap(-284);
   __mac_tap(0);
   __mac_tap(309);
   __mac_tap(0);
   __mac_tap(-340);
   __mac_tap(0);
   __mac_tap(373);
   __mac_tap(0);
   __mac_tap(-413);
   __mac_tap(0);
   __mac_tap(459);
   __mac_tap(0);
   __mac_tap(-516);
   __mac_tap(0);
   __mac_tap(583);
   __mac_tap(0);
   __mac_tap(-669);
   __mac_tap(0);
   __mac_tap(778);
   __mac_tap(0);
   __mac_tap(-929);
   __mac_tap(0);
   __mac_tap(1142);
   __mac_tap(0);
   __mac_tap(-1478);
   __mac_tap(0);
   __mac_tap(2076);
   __mac_tap(0);
   __mac_tap(-3472);
   __mac_tap(0);
   __mac_tap(10428);
   __mac_tap(16384);
   __mac_tap(10428);
   __mac_tap(0);
   __mac_tap(-3472);
   __mac_tap(0);
   __mac_tap(2076);
   __mac_tap(0);
   __mac_tap(-1478);
   __mac_tap(0);
   __mac_tap(1142);
   __mac_tap(0);
   __mac_tap(-929);
   __mac_tap(0);
   __mac_tap(778);
   __mac_tap(0);
   __mac_tap(-669);
   __mac_tap(0);
   __mac_tap(583);
   __mac_tap(0);
   __mac_tap(-516);
   __mac_tap(0);
   __mac_tap(459);
   __mac_tap(0);
   __mac_tap(-413);
   __mac_tap(0);
   __mac_tap(373);
   __mac_tap(0);
   __mac_tap(-340);
   __mac_tap(0);
   __mac_tap(309);
   __mac_tap(0);
   __mac_tap(-284);
   __mac_tap(0);
   __mac_tap(260);
   __mac_tap(0);
   __mac_tap(-240);
   __mac_tap(0);
   __mac_tap(220);
   __mac_tap(0);
   __mac_tap(-204);
   __mac_tap(0);
   __mac_tap(187);
   __mac_tap(0);
   __mac_tap(-174);
   __mac_tap(0);
   __mac_tap(160);
   __mac_tap(0);
   __mac_tap(-148);
   __mac_tap(0);
   __mac_tap(136);
   __mac_tap(0);
   __mac_tap(-127);
   __mac_tap(0);
   __mac_tap(116);
   __mac_tap(0);
   __mac_tap(-108);
   __mac_tap(0);
   __mac_tap(99);
   __mac_tap(0);
   __mac_tap(-92);
   __mac_tap(0);
   __mac_tap(83);
   __mac_tap(0);
   __mac_tap(-78);
   __mac_tap(0);
   __mac_tap(70);
   __mac_tap(0);
   __mac_tap(-65);
   __mac_tap(0);
   __mac_tap(59);
   __mac_tap(0);
   __mac_tap(-54);
   __mac_tap(0);
   __mac_tap(49);
   __mac_tap(0);
   __mac_tap(-45);
   __mac_tap(0);
   __mac_tap(40);
   __mac_tap(0);
   __mac_tap(-37);
   __mac_tap(0);
   __mac_tap(32);
   __mac_tap(0);
   __mac_tap(-30);
   __mac_tap(0);
   __mac_tap(25);
   __mac_tap(0);
   __mac_tap(-24);
   __mac_tap(0);
   __mac_tap(20);
   __mac_tap(0);
   __mac_tap(-18);
   __mac_tap(0);
   __mac_tap(15);
   __mac_tap(0);
   __mac_tap(-14);
   __mac_tap(0);
   __mac_tap(11);
   __mac_tap(0);
   __mac_tap(-10);
   return (int16_t)(acc >> 15);
}

static const int16_t __not_in_flash_func(lpf_fs4_i)(const int16_t sample)
{
   // 31250
   // Taps: 15
   // Att: 60db
   static int32_t x[FIR_LENGTH] = { 0 };
   static uint8_t sample_index = 0;
   uint8_t i = sample_index;
   x[sample_index--] = sample;
   int32_t acc = 0;
   __mac_tap(-31);
   __mac_tap(0);
   __mac_tap(462);
   __mac_tap(0);
   __mac_tap(-2127);
   __mac_tap(0);
   __mac_tap(9896);
   __mac_tap(16384);
   __mac_tap(9896);
   __mac_tap(0);
   __mac_tap(-2127);
   __mac_tap(0);
   __mac_tap(462);
   __mac_tap(0);
   __mac_tap(-31);
   return (int16_t)(acc >> 15);
}

static const int16_t __not_in_flash_func(lpf_fs4_q)(const int16_t sample)
{
   // 31250
   // Taps: 15
   // Att: 60db
   static int32_t x[FIR_LENGTH] = { 0 };
   static uint8_t sample_index = 0;
   uint8_t i = sample_index;
   x[sample_index--] = sample;
   int32_t acc = 0;
   __mac_tap(-31);
   __mac_tap(0);
   __mac_tap(462);
   __mac_tap(0);
   __mac_tap(-2127);
   __mac_tap(0);
   __mac_tap(9896);
   __mac_tap(16384);
   __mac_tap(9896);
   __mac_tap(0);
   __mac_tap(-2127);
   __mac_tap(0);
   __mac_tap(462);
   __mac_tap(0);
   __mac_tap(-31);
   return (int16_t)(acc >> 15);
}

static const int16_t __not_in_flash_func(dc)(const int16_t in)
{
  static int32_t s = 0;
  static int32_t x1 = 0;
  static int16_t y1 = 0;
  s -= x1;
  x1 = (int32_t)in << 16;
  s += x1 - ((int32_t)y1 << 8);
  return (y1 = s >> 16);
}

const void __not_in_flash_func(process_mic)(const int16_t s,int16_t &out_i,int16_t &out_q)
{
  // generate a quadrature version of the input signal
  // this is done by applying a local oscillator at one quarter
  // of the sample rate and low pass filtering, rather than a phase
  // shift FIR filter

  static uint32_t lo = 0;

  // 2400Hz LPF
  const int16_t lpf2400 = lpf_2400(dc(s));

  // up convert to FS/4
  // note LO signal reduces to 0, 1 and -1 at FS/4
  int16_t v = 0;
  switch (lo&0x3)
  {
    case 0: v = +lpf2400; break;
    case 2: v = -lpf2400; break;
  }

  // remove the upper image
  // note with a LPF at FS/4 there are many zeros
  v = lpf_fs4(v); // *2

  // LO to down convert in quadrature
  // note at FS/4 the LO signal reduces to 0, 1 and -1
  int16_t i = 0;
  switch (lo&0x3)
  {
    case 0: i = +v; break;
    case 2: i = -v; break;
  }
  int16_t q = 0;
  switch (lo&0x3)
  {
    case 1: q = +v; break;
    case 3: q = -v; break;
  }

/*
  int16_t i = 0;
  int16_t q = 0;
  switch (lo&0x3)
  {
    case 0: i = +v; break;
    case 1: q = +v; break;
    case 2: i = -v; break;
    case 3: q = -v; break;
  }
*/
  // remove the higher image with another FS/4 LPF
  out_i = lpf_fs4_i(i);
  out_q = lpf_fs4_q(q);

  lo++;
}

#endif