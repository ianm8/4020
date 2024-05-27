// 4020 - IQ TX only
// https://www.arc.id.au/FilterDesign.html

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;


const static int32_t SAMPLERATE = 31250; // 32us
const static int32_t BUFFER_SIZE = 4096;

// watch out, only use this on vars
#define _USE_MATH_DEFINES
#define NOMINMAX
#define _max(a,b) ((a)>(b)?(a):(b))
#define _min(a,b) ((a)<(b)?(a):(b))

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAKEQ 65536
#define Q 16 // Q0.16

#define FIR_LENGTH 256
#define __mac_tap(_h) acc += (_h)*x[i++]

static const int16_t lpf_2400(const int16_t sample)
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

static const int16_t lpf_fs4_1(const int16_t sample)
{
   // 31250
   // Taps: 199
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

static const int16_t hpf_fs4(const int16_t sample)
{
   // 31250
   static int32_t x[FIR_LENGTH] = { 0 };
   static uint8_t sample_index = 0;
   uint8_t i = sample_index;
   x[sample_index--] = sample;
   int32_t acc = 0;
   __mac_tap(9);
   __mac_tap(0);
   __mac_tap(-12);
   __mac_tap(0);
   __mac_tap(13);
   __mac_tap(0);
   __mac_tap(-16);
   __mac_tap(0);
   __mac_tap(17);
   __mac_tap(0);
   __mac_tap(-21);
   __mac_tap(0);
   __mac_tap(23);
   __mac_tap(0);
   __mac_tap(-26);
   __mac_tap(0);
   __mac_tap(29);
   __mac_tap(0);
   __mac_tap(-33);
   __mac_tap(0);
   __mac_tap(36);
   __mac_tap(0);
   __mac_tap(-41);
   __mac_tap(0);
   __mac_tap(44);
   __mac_tap(0);
   __mac_tap(-50);
   __mac_tap(0);
   __mac_tap(53);
   __mac_tap(0);
   __mac_tap(-60);
   __mac_tap(0);
   __mac_tap(64);
   __mac_tap(0);
   __mac_tap(-71);
   __mac_tap(0);
   __mac_tap(77);
   __mac_tap(0);
   __mac_tap(-84);
   __mac_tap(0);
   __mac_tap(91);
   __mac_tap(0);
   __mac_tap(-100);
   __mac_tap(0);
   __mac_tap(107);
   __mac_tap(0);
   __mac_tap(-117);
   __mac_tap(0);
   __mac_tap(126);
   __mac_tap(0);
   __mac_tap(-137);
   __mac_tap(0);
   __mac_tap(147);
   __mac_tap(0);
   __mac_tap(-161);
   __mac_tap(0);
   __mac_tap(173);
   __mac_tap(0);
   __mac_tap(-188);
   __mac_tap(0);
   __mac_tap(203);
   __mac_tap(0);
   __mac_tap(-221);
   __mac_tap(0);
   __mac_tap(239);
   __mac_tap(0);
   __mac_tap(-261);
   __mac_tap(0);
   __mac_tap(283);
   __mac_tap(0);
   __mac_tap(-310);
   __mac_tap(0);
   __mac_tap(339);
   __mac_tap(0);
   __mac_tap(-374);
   __mac_tap(0);
   __mac_tap(412);
   __mac_tap(0);
   __mac_tap(-460);
   __mac_tap(0);
   __mac_tap(515);
   __mac_tap(0);
   __mac_tap(-584);
   __mac_tap(0);
   __mac_tap(668);
   __mac_tap(0);
   __mac_tap(-779);
   __mac_tap(0);
   __mac_tap(928);
   __mac_tap(0);
   __mac_tap(-1143);
   __mac_tap(0);
   __mac_tap(1477);
   __mac_tap(0);
   __mac_tap(-2077);
   __mac_tap(0);
   __mac_tap(3471);
   __mac_tap(0);
   __mac_tap(-10429);
   __mac_tap(16384);
   __mac_tap(-10429);
   __mac_tap(0);
   __mac_tap(3471);
   __mac_tap(0);
   __mac_tap(-2077);
   __mac_tap(0);
   __mac_tap(1477);
   __mac_tap(0);
   __mac_tap(-1143);
   __mac_tap(0);
   __mac_tap(928);
   __mac_tap(0);
   __mac_tap(-779);
   __mac_tap(0);
   __mac_tap(668);
   __mac_tap(0);
   __mac_tap(-584);
   __mac_tap(0);
   __mac_tap(515);
   __mac_tap(0);
   __mac_tap(-460);
   __mac_tap(0);
   __mac_tap(412);
   __mac_tap(0);
   __mac_tap(-374);
   __mac_tap(0);
   __mac_tap(339);
   __mac_tap(0);
   __mac_tap(-310);
   __mac_tap(0);
   __mac_tap(283);
   __mac_tap(0);
   __mac_tap(-261);
   __mac_tap(0);
   __mac_tap(239);
   __mac_tap(0);
   __mac_tap(-221);
   __mac_tap(0);
   __mac_tap(203);
   __mac_tap(0);
   __mac_tap(-188);
   __mac_tap(0);
   __mac_tap(173);
   __mac_tap(0);
   __mac_tap(-161);
   __mac_tap(0);
   __mac_tap(147);
   __mac_tap(0);
   __mac_tap(-137);
   __mac_tap(0);
   __mac_tap(126);
   __mac_tap(0);
   __mac_tap(-117);
   __mac_tap(0);
   __mac_tap(107);
   __mac_tap(0);
   __mac_tap(-100);
   __mac_tap(0);
   __mac_tap(91);
   __mac_tap(0);
   __mac_tap(-84);
   __mac_tap(0);
   __mac_tap(77);
   __mac_tap(0);
   __mac_tap(-71);
   __mac_tap(0);
   __mac_tap(64);
   __mac_tap(0);
   __mac_tap(-60);
   __mac_tap(0);
   __mac_tap(53);
   __mac_tap(0);
   __mac_tap(-50);
   __mac_tap(0);
   __mac_tap(44);
   __mac_tap(0);
   __mac_tap(-41);
   __mac_tap(0);
   __mac_tap(36);
   __mac_tap(0);
   __mac_tap(-33);
   __mac_tap(0);
   __mac_tap(29);
   __mac_tap(0);
   __mac_tap(-26);
   __mac_tap(0);
   __mac_tap(23);
   __mac_tap(0);
   __mac_tap(-21);
   __mac_tap(0);
   __mac_tap(17);
   __mac_tap(0);
   __mac_tap(-16);
   __mac_tap(0);
   __mac_tap(13);
   __mac_tap(0);
   __mac_tap(-12);
   __mac_tap(0);
   __mac_tap(9);
   return (int16_t)(acc >> 15);
}

static const int16_t lpf_fs4_i(const int16_t sample)
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

static const int16_t lpf_fs4_q(const int16_t sample)
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

static void sig_gen(int16_t b[], const uint32_t bl, const float f, const int32_t p, const int32_t a, const int32_t offset = 0)
{
   // fill the input buffer with a signal frequency f, phase p, and amplitude a
   for (uint32_t i = 0; i < bl; i++)
   {
      const int16_t sample = (int16_t)(a * cos(M_PI * p / 180.0 + f * 2.0 * M_PI * i / SAMPLERATE))+(int16_t)offset;
      b[i] = sample;
   }
}

static int16_t dc(const int16_t in)
{
   static int32_t s = 0;
   static int32_t x1 = 0;
   static int16_t y1 = 0;
   s -= x1;
   x1 = (int32_t)in << Q;
   s += x1 - ((int32_t)y1 << 8); // <<8 = 1/256
   return (y1 = s >> Q);
}

static const int16_t fs4lo(const int16_t s)
{
   static uint32_t lo = 0;
   int16_t v = 0;
   switch (lo & 0x3)
   {
      case 0: v = +s; break;
      case 1: v = 0;  break;
      case 2: v = -s; break;
      case 3: v = 0;  break;
   }
   lo++;
   return v;
}

static const int16_t fs4i(const int16_t s)
{
   static uint32_t lo = 0;
   int16_t v = 0;
   switch (lo & 0x3)
   {
      case 0: v = +s; break;
      case 1: v = 0;  break;
      case 2: v = -s; break;
      case 3: v = 0;  break;
   }
   lo++;
   return v;
}

static const int16_t fs4q(const int16_t s)
{
   static uint32_t lo = 0;
   int16_t v = 0;
   switch (lo & 0x3)
   {
      case 0: v = 0;  break;
      case 1: v = +s; break;
      case 2: v = 0;  break;
      case 3: v = -s; break;
   }
   lo++;
   return v;
}

static void loiq(const int16_t s,int16_t &i,int16_t q)
{
   static uint32_t lo = 0;
   int16_t v = 0;
   switch (lo & 0x3)
   {
      case 0: i = +s; q = 0;  break;
      case 1: i = 0;  q = +s; break;
      case 2: i = -s; q = 0;  break;
      case 3: i = 0;  q = -s; break;
   }
   lo++;
}

static void test_dc(void)
{
  // low pass filter
  static int16_t buf_in[BUFFER_SIZE];
  static int16_t buf_out[BUFFER_SIZE];

  const int32_t freq = 1000;
  const int32_t amp = 2000;
  const int32_t offset = 500;
  const auto phase = 0;
  // mic input is 12 bit ADC
  sig_gen(buf_in, BUFFER_SIZE, freq, phase, amp, offset);
  for (uint32_t i = 0; i < BUFFER_SIZE; i++)
  {
    buf_out[i] = dc(buf_in[i]);
  }

  printf("sig,dc\n");
  for (int16_t i = 0; i < BUFFER_SIZE; i++)
  {
    printf("%i,%i\n", buf_in[i], buf_out[i]);
  }
}

static void test_lp(void)
{
   // low pass filter
   static int16_t buf_in[BUFFER_SIZE];
   static int16_t buf_lpf_2400[BUFFER_SIZE];
   static int16_t buf_lo1[BUFFER_SIZE];
   static int16_t buf_fs4_1[BUFFER_SIZE];
   static int16_t buf_i[BUFFER_SIZE];
   static int16_t buf_q[BUFFER_SIZE];
   static int16_t buf_out_i[BUFFER_SIZE];
   static int16_t buf_out_q[BUFFER_SIZE];

   const int32_t freq = 1000;
   const int32_t amp = 1000;
   const int32_t offset = 400;
   const auto phase = 0;
   // mic input is 12 bit ADC
   sig_gen(buf_in, BUFFER_SIZE, freq, phase, amp, offset);
   for (uint32_t i = 0; i < BUFFER_SIZE; i++)
   {
      // LPF 2400
      buf_lpf_2400[i] = lpf_2400(dc(buf_in[i]));

      // FS/4 LO
      buf_lo1[i] = fs4lo(buf_lpf_2400[i]);

      // FS/4 LPF
      buf_fs4_1[i] = lpf_fs4_1(buf_lo1[i]); // *2

      // quadrature LO
      buf_i[i] = fs4i(buf_fs4_1[i]);
      buf_q[i] = fs4q(buf_fs4_1[i]);

      // FS/4 LPF
      buf_out_i[i] = lpf_fs4_i(buf_i[i]); // *2
      buf_out_q[i] = lpf_fs4_q(buf_q[i]);
   }

   printf("sig,lpf2400,lo1,lpf_fs4,out_i,out_q\n");
   for (int16_t i = 0; i < BUFFER_SIZE; i++)
   {
      printf("%i,%i,%i,%i,%i,%i\n", buf_in[i], buf_lpf_2400[i], buf_lo1[i], buf_fs4_1[i], buf_out_i[i], buf_out_q[i]);
   }
}

static void test_hp(void)
{
   // high pass filter
   static int16_t buf_in[BUFFER_SIZE];
   static int16_t buf_lpf_2400[BUFFER_SIZE];
   static int16_t buf_lo1[BUFFER_SIZE];
   static int16_t buf_fs4_1[BUFFER_SIZE];
   static int16_t buf_i[BUFFER_SIZE];
   static int16_t buf_q[BUFFER_SIZE];
   static int16_t buf_out_i[BUFFER_SIZE];
   static int16_t buf_out_q[BUFFER_SIZE];

   const int32_t freq = 1200;
   const int32_t amp = 1000;
   const int32_t offset = 500;
   const auto phase = 0;
   // mic input is 12 bit ADC
   sig_gen(buf_in, BUFFER_SIZE, freq, phase, amp, offset);
   for (uint32_t i = 0; i < BUFFER_SIZE; i++)
   {
      // LPF 2400
      buf_lpf_2400[i] = lpf_2400(dc(buf_in[i]));

      // FS/4 LO
      buf_lo1[i] = fs4lo(buf_lpf_2400[i]);

      // FS/4 HPF
      buf_fs4_1[i] = hpf_fs4(buf_lo1[i]);

      // quadrature LO
      buf_i[i] = fs4i(buf_fs4_1[i]);
      buf_q[i] = fs4q(buf_fs4_1[i]);

      // FS/4 LPF
      buf_out_i[i] = lpf_fs4_i(buf_i[i]);
      buf_out_q[i] = lpf_fs4_q(buf_q[i]);
   }

   printf("sig,lpf2400,lo1,hpf_fs4,out_i,out_q\n");
   for (int16_t i = 0; i < BUFFER_SIZE; i++)
   {
      printf("%i,%i,%i,%i,%i,%i\n", buf_in[i], buf_lpf_2400[i], buf_lo1[i], buf_fs4_1[i], buf_out_i[i], buf_out_q[i]);
   }
}

int main(void)
{
   //test_dc();
   test_lp();
   //test_hp();
   return 0;
}
