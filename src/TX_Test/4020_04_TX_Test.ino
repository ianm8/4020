
/*
 * libraries
 *
 * https://github.com/etherkit/Si5351Arduino
 * https://github.com/brianlow/Rotary
 * https://github.com/datacute/Tiny4kOLED
 *
 */

#include <si5351.h>
#include <Rotary.h>
#include <Tiny4kOLED.h>c:\Users\ianm\OneDrive - University of Tasmania\Documents\Arduino\libraries\Rotary-master\Rotary.cpp
#include "agc.h"
#include "mic.h"
#include "quad5351.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

//#define YOUR_CALL "VK7IAN"

#define BAND_40M 0
#define BAND_20M 1
#define BAND_MIN BAND_40M 
#define BAND_MAX BAND_20M 
//#define DEFAULT_FREQUENCY 7123000ul
//#define DEFAULT_BAND BAND_40M
//#define DEFAULT_MODE MODE_LSB
#define DEFAULT_FREQUENCY 14123000ul
#define DEFAULT_BAND BAND_20M
#define DEFAULT_MODE MODE_USB

#define TCXO_FREQ 26000000ul

#define PIN_PTT     0 // Mic PTT (active low)
#define PIN_TXN     1 // Enable TX mixer (active low)
#define PIN_RXN     2 // Enable RX mixer (active low)
#define PIN_TXBIAS  3 // enable TX bias
#define PIN_SDA     4 // I2C
#define PIN_SCL     5 // I2C
#define PIN_TX000H  6 // PWM
#define PIN_TX000L  7 // PWM
#define PIN_TX180H  8 // PWM
#define PIN_TX180L  9 // PWM
#define PIN_TX090H 10 // PWM
#define PIN_TX090L 11 // PWM
#define PIN_TX270H 12 // PWM
#define PIN_TX270L 13 // PWM
#define PIN_TX     14 // TX Relay
#define PIN_BAND   15 // Band select
#define PIN_ATT    16 // Attenuation relay
#define PIN_AGC    17 // PWM
#define PIN_ENCBUT 18 // rotary
#define PIN_ENCA   19 // rotary
#define PIN_ENCB   20 // rotary
#define PIN_REG    23 // Pico regulator
#define PIN_MIC    26 // analog MIC
#define PIN_AGCIN  28 // analog AGC

#define PIN_UNUSED 27

#if PIN_MIC == 26U
#define MIC_MUX 0U
#elif PIN_MIC == 27U
#define MIC_MUX 1U
#elif PIN_MIC == 28U
#define MIC_MUX 2U
#elif PIN_MIC == 29U
#define MIC_MUX 3U
#endif

#if PIN_AGCIN == 26U
#define AGC_MUX 0U
#elif PIN_AGCIN == 27U
#define AGC_MUX 1U
#elif PIN_AGCIN == 28U
#define AGC_MUX 2U
#elif PIN_AGCIN == 29U
#define AGC_MUX 3U
#endif

enum radio_mode_t
{
  MODE_NONE,
  MODE_LSB,
  MODE_USB
};

// the PWM simulates a 10 bit DAC using both channels
// resistors on the pin pairs have a ratio of 32:1 (eg 33k and 1k)
// assuming a pin resistance (on average) of 50 ohms (33050/1050 = 31.48)
// since the PWM operates in phase correct mode
// the PWM frequency will be processor clock / 32 / 2 (eg 225Mhz/32/2 = ~3.5MHz)
// v is signed, channel A is the high 5 bits, channel B is the low 5 bits
// need to convert to unsigned and extract the high and low 5 bits
// PWM A is Pin 2 (high bits - 1k resistor)
// PWM B is Pin 3 (low bits - 33k resistor)

#define DAC(pwm,v) pwm_set_both_levels(pwm,((((uint32_t)(v+512UL))>>5)&0x1f),(((uint32_t)(v+512UL))&0x1f))

////
//auto_init_mutex(display_mutex);
volatile static uint32_t mic_i_p_pwm = 0;
volatile static uint32_t mic_i_n_pwm = 0;
volatile static uint32_t mic_q_p_pwm = 0;
volatile static uint32_t mic_q_n_pwm = 0;
volatile static bool setup_complete = false;

volatile static struct
{
  uint32_t frequency;
  uint32_t divisor;
  uint32_t band;
  radio_mode_t mode;
  bool rx_enable;
  bool tx_enable;
} radio = {DEFAULT_FREQUENCY,0,DEFAULT_BAND,DEFAULT_MODE,true,false};

static struct
{
  const uint32_t lo;
  const uint32_t hi;
  uint32_t frequency;
}
bands[] =
{
  { 7000000UL,  7300000UL,  7105000UL},
  {14000000UL, 14350000UL, 14200000UL}
};

Si5351 si5351;
Rotary r = Rotary(PIN_ENCA,PIN_ENCB);

void init_adc(void)
{
////
// adc_set_round_robin(uint mask);
// value between 0 and 0x1f (bit 0 to bit 3 for GPIO 26 to 29)
  adc_init();
  adc_gpio_init(PIN_MIC);
  adc_gpio_init(PIN_AGC);
  adc_select_input(AGC_MUX);
  adc_fifo_setup(true, false, 4, false, false);
  adc_irq_set_enabled(true);
  irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_interrupt_handler);
  irq_set_enabled(ADC_IRQ_FIFO, true);
  adc_run(true);
}

void setup(void)
{
  // run DSP on core 0
////
  // pin directions
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(PIN_ATT,OUTPUT);
  pinMode(PIN_BAND,OUTPUT);
  pinMode(PIN_TXBIAS,OUTPUT);
  pinMode(PIN_TX,OUTPUT);
  pinMode(PIN_TXN,OUTPUT);
  pinMode(PIN_RXN,OUTPUT);
  pinMode(PIN_UNUSED,INPUT_PULLUP);
  pinMode(PIN_REG,OUTPUT);

  // pin defaults
  // set pico regulator to low noise
  digitalWrite(PIN_REG,HIGH);
  digitalWrite(PIN_TXBIAS,LOW);
  digitalWrite(PIN_TX,LOW);
  digitalWrite(PIN_TXN,HIGH);
  digitalWrite(PIN_RXN,LOW);
  digitalWrite(PIN_ATT,LOW);
  digitalWrite(PIN_BAND,LOW);
  delay(40);
  digitalWrite(LED_BUILTIN,LOW);

  // set pin function to PWM
  gpio_set_function(PIN_TX000H,GPIO_FUNC_PWM); //  6  PWM
  gpio_set_function(PIN_TX000L,GPIO_FUNC_PWM); //  7  PWM
  gpio_set_function(PIN_TX180H,GPIO_FUNC_PWM); //  8  PWM
  gpio_set_function(PIN_TX180L,GPIO_FUNC_PWM); //  9  PWM
  gpio_set_function(PIN_TX090H,GPIO_FUNC_PWM); // 10  PWM
  gpio_set_function(PIN_TX090L,GPIO_FUNC_PWM); // 11  PWM
  gpio_set_function(PIN_TX270H,GPIO_FUNC_PWM); // 12  PWM
  gpio_set_function(PIN_TX270L,GPIO_FUNC_PWM); // 13  PWM

  // get PWM slice connected to each pin pair
  mic_i_p_pwm = pwm_gpio_to_slice_num(PIN_TX000H);
  mic_i_n_pwm = pwm_gpio_to_slice_num(PIN_TX180H);
  mic_q_p_pwm = pwm_gpio_to_slice_num(PIN_TX090H);
  mic_q_n_pwm = pwm_gpio_to_slice_num(PIN_TX270H);

  // set period of 32 cycles
  pwm_set_wrap(mic_i_p_pwm,31);
  pwm_set_wrap(mic_i_n_pwm,31);
  pwm_set_wrap(mic_q_p_pwm,31);
  pwm_set_wrap(mic_q_n_pwm,31);

  // switch to phase correct
  pwm_set_phase_correct(mic_i_p_pwm,true);
  pwm_set_phase_correct(mic_i_n_pwm,true);
  pwm_set_phase_correct(mic_q_p_pwm,true);
  pwm_set_phase_correct(mic_q_n_pwm,true);

  // initialise to zero (low)
  DAC(mic_i_p_pwm,-512L);
  DAC(mic_i_n_pwm,-512L);
  DAC(mic_q_p_pwm,-512L);
  DAC(mic_q_n_pwm,-512L);

  // set each PWM running
  pwm_set_enabled(mic_i_p_pwm,true);
  pwm_set_enabled(mic_i_n_pwm,true);
  pwm_set_enabled(mic_q_p_pwm,true);
  pwm_set_enabled(mic_q_n_pwm,true);

  // init rotary
  r.begin();

  // init PLL and set default frequency
  radio.frequency = DEFAULT_FREQUENCY;
  radio.divisor = get_divisor(radio.frequency);
  radio.band = DEFAULT_BAND;
  radio.mode = DEFAULT_MODE;
////
////
////
pinMode(LED_BUILTIN,OUTPUT);
for (int i=0;i<2;i++)
{
  digitalWrite(LED_BUILTIN,HIGH);
  delay(10);
  digitalWrite(LED_BUILTIN,LOW);
  delay(250);
}

  Wire.setSDA(PIN_SDA);
  Wire.setSCL(PIN_SCL);
  Wire.setClock(400000ul);
  si5351.init(SI5351_CRYSTAL_LOAD_0PF,TCXO_FREQ,0);
  si5351.drive_strength(SI5351_CLK0,SI5351_DRIVE_8MA);
  si5351.drive_strength(SI5351_CLK1,SI5351_DRIVE_8MA);
  const uint64_t f = radio.frequency*SI5351_FREQ_MULT;
  const uint64_t p = radio.frequency*radio.divisor*SI5351_FREQ_MULT;
  si5351.set_freq_manual(f,p,SI5351_CLK0);
  si5351.set_freq_manual(f,p,SI5351_CLK1);
  if (radio.mode==MODE_LSB)
  {
    si5351.set_phase(SI5351_CLK0,0);
    si5351.set_phase(SI5351_CLK1,radio.divisor);
  }
  else
  {
    si5351.set_phase(SI5351_CLK0,radio.divisor);
    si5351.set_phase(SI5351_CLK1,0);
  }
  si5351.pll_reset(SI5351_PLLA);

  pinMode(LED_BUILTIN,OUTPUT);
  for (int i=0;i<2;i++)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN,LOW);
    delay(250);
  }

  init_adc();
////
digitalWrite(PIN_BAND,HIGH); // 20m
digitalWrite(PIN_TX,HIGH);
delay(50);
digitalWrite(PIN_TXN,LOW);
digitalWrite(PIN_RXN,HIGH);
//digitalWrite(PIN_TXBIAS,HIGH);
digitalWrite(PIN_TXBIAS,LOW);
setup_complete = true;
return;

  // O2 57076 O3 59660
  // OLED bright and rotated
  // note: include file declares the oled object
  oled.begin(64,32,sizeof(tiny4koled_init_64x32br),tiny4koled_init_64x32br);
  oled.enableChargePump();
  oled.setRotation(0);
  oled.setInternalIref(true);
  oled.clear();
  oled.on();
  oled.switchRenderFrame();

  // intro screen
  oled.clear();
  oled.setFont(FONT6X8);
  oled.setCursor(0,0);
  oled.print(" 4020 TRX");
  oled.setCursor(0,1);
#ifdef YOUR_CALL
  oled.print("  hello");
  oled.setFont(FONT8X16);
  oled.setCursor(0,2);
  oled.print(" ");
  oled.print(YOUR_CALL);
#else
  oled.print("    by");
  oled.setFont(FONT8X16);
  oled.setCursor(0,2);
  oled.print(" VK7IAN");
#endif
  oled.switchFrame();

  // splash delay
  delay(2000);

  setup_complete = true;
}

void setup1(void)
{
  // run UI on core 1
  // only go to loop1 when setup() has completed
  while (!setup_complete)
  {
    ;
  }

  pinMode(LED_BUILTIN,OUTPUT);
  for (int i=0;i<5;i++)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
    delay(250);
  }

}

volatile static int16_t adc_value = 0;
volatile static int16_t agc_adc_value = 0;
volatile static int32_t dac_value_i_p = 0;
volatile static int32_t dac_value_i_n = 0;
volatile static int32_t dac_value_q_p = 0;
volatile static int32_t dac_value_q_n = 0;
volatile static bool adc_value_ready = false;
volatile static bool agc_value_ready = false;

void adc_interrupt_handler(void)
{
  volatile static uint32_t counter = 0;
  volatile static uint32_t adc_raw = 0;
  adc_raw += adc_fifo_get();
  adc_raw += adc_fifo_get();
  adc_raw += adc_fifo_get();
  adc_raw += adc_fifo_get();
  //digitalWrite(LED_BUILTIN,counter&1?HIGH:LOW);
  if (counter==4)
  {
    DAC(mic_i_p_pwm,dac_value_i_p);
    DAC(mic_i_n_pwm,dac_value_i_n);
    DAC(mic_q_p_pwm,dac_value_q_p);
    DAC(mic_q_n_pwm,dac_value_q_n);
    adc_value = (int16_t)(adc_raw>>4)-2048;
    adc_value_ready = true;
    adc_raw = 0;
    counter = 0;
  }
  counter++;
}

void loop(void)
{
  // run DSP on core 0
  static bool tx = false;
  if (tx)
  {
    // TX, check if changed to RX
    if (radio.tx_enable)
    {
      if (adc_value_ready)
      {
        adc_value_ready = false;
        int16_t mic_i = 0;
        int16_t mic_q = 0;
        process_mic(adc_value,mic_i,mic_q);
        mic_i = constrain(mic_i,-512,+511);
        mic_q = constrain(mic_q,-512,+511);
        dac_value_i_p = +mic_i;
        dac_value_i_n = -mic_i;
        dac_value_q_p = +mic_q;
        dac_value_q_n = -mic_q;
        agc_adc_value = 0;
      }
    }
    else
    {
      // switched to RX
      adc_select_input(AGC_MUX);
      adc_value_ready = false;
      tx = false;
    }
  }
  else
  {
    // RX, check if changed to TX
    if (radio.tx_enable)
    {
      adc_select_input(MIC_MUX);
      adc_value_ready = false;
      tx = true;
    }
    else
    {
      if (adc_value_ready)
      {
        adc_value_ready = false;
        agc_adc_value = adc_value;
        agc_value_ready = true;
        // output TX zero in receive mode
        dac_value_i_p = -512;
        dac_value_i_n = -512;
        dac_value_q_p = -512;
        dac_value_q_n = -512;
      }
    }
  }
}

void loop1(void)
{

  pinMode(PIN_AGCIN,OUTPUT);
  digitalWrite(PIN_AGCIN,LOW);

// TX
//digitalWrite(PIN_RXN,LOW);

delay(5000);
digitalWrite(LED_BUILTIN,HIGH);
digitalWrite(PIN_RXN,HIGH);
digitalWrite(PIN_TXN,LOW);
radio.tx_enable = true;
digitalWrite(PIN_TXBIAS,HIGH);
delay(5000);

// RX
digitalWrite(LED_BUILTIN,LOW);
digitalWrite(PIN_TXBIAS,LOW);
radio.tx_enable = false;
digitalWrite(PIN_RXN,LOW);
digitalWrite(PIN_TXN,HIGH);

return;

digitalWrite(LED_BUILTIN,HIGH);

// mute the receiver, disable QSD
analogWrite(PIN_AGC,0u);
digitalWrite(PIN_RXN,HIGH);

// switch antenna to TX
digitalWrite(PIN_TX,HIGH);

// enable the MIC
radio.tx_enable = true;
delay(20);
delay(10);

// enable QSE
digitalWrite(PIN_TXN,LOW);
digitalWrite(PIN_TXBIAS,HIGH);

////
delay(5000);

// RX
radio.tx_enable = false;
digitalWrite(PIN_TXBIAS,LOW);
digitalWrite(PIN_TXN,HIGH);
delay(50);
digitalWrite(PIN_TX,LOW);
delay(20);
digitalWrite(PIN_RXN,LOW);
digitalWrite(LED_BUILTIN,LOW);

return;

  // run UI on core 1
  static uint32_t old_frequency = radio.frequency;
  static uint32_t old_divisor = radio.divisor;

  // process AGC
  if (agc_value_ready)
  {
    agc_value_ready = false;
    const uint8_t agc_level = AGC::agc_pwm(agc_adc_value);
    analogWrite(PIN_AGC,agc_level);
  }

  // process the main tuning
  if (!radio.tx_enable)
  {
    switch (r.process())
    {
      case DIR_CW:
      {
        radio.frequency += 1000;
/*
        digitalWrite(LED_BUILTIN,HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN,LOW);
*/
        break;
      }
      case DIR_CCW:
      {
        radio.frequency -= 1000;
/*
        digitalWrite(LED_BUILTIN,HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN,LOW);
*/
        break;
      }
    }
    radio.frequency = radio.frequency/1000;
    radio.frequency = radio.frequency*1000;
    radio.frequency = constrain(radio.frequency,bands[radio.band].lo,bands[radio.band].hi);
    if (radio.frequency!=old_frequency)
    {
      old_frequency = radio.frequency;
      radio.divisor = get_divisor(radio.frequency);
      const uint64_t f = radio.frequency*SI5351_FREQ_MULT;
      const uint64_t p = radio.frequency*radio.divisor*SI5351_FREQ_MULT;
      si5351.set_freq_manual(f,p,SI5351_CLK0);
      si5351.set_freq_manual(f,p,SI5351_CLK1);
      switch (radio.band)
      {
        case BAND_40M:
        {
          // LSB
          si5351.set_phase(SI5351_CLK0,0);
          si5351.set_phase(SI5351_CLK1,radio.divisor);
          radio.mode = MODE_LSB;
          digitalWrite(PIN_ATT,HIGH);
          break;
        }
        case BAND_20M:
        {
          // USB
          si5351.set_phase(SI5351_CLK0,radio.divisor);
          si5351.set_phase(SI5351_CLK1,0);
          radio.mode = MODE_USB;
          digitalWrite(PIN_ATT,LOW);
          break;
        }
      }
      if (radio.divisor!=old_divisor)
      {
        old_divisor = radio.divisor;
        si5351.pll_reset(SI5351_PLLA);
      }
    }
  }
 
  // update the display every 20ms
  static uint32_t next_update = 0;
  const uint32_t now = millis();
  if (next_update<now)
  {
    next_update = now + 20ul;
    uint8_t freq_pos = 0u;
    char sz_frequency[16] = "";
    memset(sz_frequency,0,sizeof(sz_frequency));
    ultoa(radio.frequency,sz_frequency,10);
    if (radio.frequency<10000000u)
    {
      // 7 digits
      // 3555000 -> 3.555.000
      // 0123456    012345678
      sz_frequency[8] = sz_frequency[6];
      sz_frequency[7] = sz_frequency[5];
      sz_frequency[6] = sz_frequency[4];
      sz_frequency[5] = '.';
      sz_frequency[4] = sz_frequency[3];
      sz_frequency[3] = sz_frequency[2];
      sz_frequency[2] = sz_frequency[1];
      sz_frequency[1] = '.';
      freq_pos = 3u;
    }
    else
    {
      // 8 digits
      // 14222000 -> 14.222.000
      // 01234567    0123456789
      sz_frequency[9] = sz_frequency[7];
      sz_frequency[8] = sz_frequency[6];
      sz_frequency[7] = sz_frequency[5];
      sz_frequency[6] = '.';
      sz_frequency[5] = sz_frequency[4];
      sz_frequency[4] = sz_frequency[3];
      sz_frequency[3] = sz_frequency[2];
      sz_frequency[2] = '.';
    }
    oled.clear();
    oled.setFont(FONT6X8);
    oled.setCursor(freq_pos,0);
    oled.print(sz_frequency);
    oled.setCursor(0,1);
    oled.print("Mode:");
    oled.print(radio.tx_enable?"TX":"  ");
    oled.print(radio.mode==MODE_LSB?"LSB":"USB");
    oled.setCursor(0,2);
    oled.print("-3-5-7-9-+");
  ////
    const uint32_t signal_level = AGC::smeter();
    const uint8_t sig = min(signal_level,63);
    oled.bitmap(0, 3, sig, 4, AGC::meter);
    oled.switchFrame();
  }

//#define PIN_TXN     // Enable TX mixer (active low)
//#define PIN_RXN     2 // Enable RX mixer (active low)
//#define PIN_TXBIAS  3 // enable TX bias
//#define PIN_TX  // relay

  // check for PTT
  if (digitalRead(PIN_PTT)==LOW)
  {
    // 1. set TX/RX relay to TX
    // 2. enable MIC (DSP)
    // 3. enable TX bias
    
    // enable the MIC
    radio.tx_enable = true;
    digitalWrite(LED_BUILTIN,HIGH);

    // mute the receiver
    analogWrite(PIN_AGC,0u);
    delay(10);

    // enable TX
    digitalWrite(PIN_RXN,HIGH);
    digitalWrite(PIN_TXN,LOW);
    digitalWrite(PIN_TX,HIGH);
    delay(10);
    digitalWrite(PIN_TXBIAS,HIGH);

    // wait for PTT release
    while (digitalRead(PIN_PTT)==LOW)
    {
      delay(10);
    }

    // back to receive
    radio.tx_enable = false;
    digitalWrite(PIN_TXBIAS,LOW);
    delay(10);
    digitalWrite(PIN_TX,LOW);
    digitalWrite(PIN_TXN,HIGH);
    digitalWrite(PIN_RXN,LOW);
    delay(40);
    digitalWrite(LED_BUILTIN,LOW);
  }
}
