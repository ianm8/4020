/*
 * 4020 Version 2.0.225
 * Copyright 2024 Ian Mitchell VK7IAN
 * Licenced under the GNU GPL Version 3
 *
 * libraries
 *
 * https://github.com/etherkit/Si5351Arduino
 * https://github.com/brianlow/Rotary
 * https://github.com/datacute/Tiny4kOLED
 *
 */
#include <si5351.h>
#include <Tiny4kOLED.h>
#include "Rotary.h"
#include "agc.h"
#include "mic.h"
#include "menu.h"
#include "quad5351.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/vreg.h"

//#define YOUR_CALL "VK7IAN"

#define VERSION_STRING "  V2.0."
#define BAND_40M 0
#define BAND_20M 1
#define BAND_MIN BAND_40M 
#define BAND_MAX BAND_20M 
#define DEFAULT_FREQUENCY 14200000ul
#define DEFAULT_BAND BAND_20M
#define DEFAULT_MODE MODE_USB
#define DEFAULT_ATTENUATION false
#define DEFAULT_MIC_PROC 0u
#define DEFAULT_STEP 500ul
#define BUTTON_LONG_PRESS_TIME 1000ul

#define TCXO_FREQ 26000000ul
#define LM4875_MUTE 56u
#define LM4875_SHUTDOWN 0u
#define USB_FREQUENCY_LOW  7040000ul
#define USB_FREQUENCY_HIGH 7081000ul

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
#define PIN_ATT    16 // Attenuator relay
#define PIN_AGCOUT 17 // PWM
#define PIN_ENCBUT 20 // rotary
#define PIN_ENCA   18 // rotary
#define PIN_ENCB   19 // rotary
#define PIN_REG    23 // Pico regulator
#define PIN_MIC    26 // analog MIC
#define PIN_AGCIN  28 // analog AGC
#define PIN_UNUSED 27 // free pin

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

enum radio_setmode_t
{
  SETMODE_AUTO,
  SETMODE_LSB,
  SETMODE_USB
};

enum radio_setatten_t
{
  SET_ATTEN_AUTO,
  SET_ATTEN_ON,
  SET_ATTEN_OFF
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

auto_init_mutex(rotary_mutex);
volatile static uint32_t mic_i_p_pwm = 0;
volatile static uint32_t mic_i_n_pwm = 0;
volatile static uint32_t mic_q_p_pwm = 0;
volatile static uint32_t mic_q_n_pwm = 0;
volatile static bool setup_complete = false;

volatile static struct
{
  int32_t tune;
  uint32_t step;
  uint32_t frequency;
  uint32_t divisor;
  uint32_t band;
  radio_mode_t mode;
  radio_setmode_t set_mode;
  bool attenuation;
  radio_setatten_t set_attenuation;
  bool tx_enable;
  uint8_t mic_proc;
}
radio =
{
  0,
  DEFAULT_STEP,
  DEFAULT_FREQUENCY,
  0,
  DEFAULT_BAND,
  DEFAULT_MODE,
  SETMODE_AUTO,
  DEFAULT_ATTENUATION,
  SET_ATTEN_AUTO,
  false,
  DEFAULT_MIC_PROC
};

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

volatile static struct
{
  uint32_t frequency;
}
save_data[] =
{
  { 7105000UL},
  {14200000UL}
};

Si5351 si5351;
Rotary r = Rotary(PIN_ENCB,PIN_ENCA);

void init_adc(void)
{
  adc_init();
  adc_gpio_init(PIN_MIC);
  adc_gpio_init(PIN_AGCIN);
  adc_select_input(AGC_MUX);
  adc_fifo_setup(true, false, 4, false, false);
  adc_fifo_drain();
  adc_irq_set_enabled(true);
  irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_interrupt_handler);
  irq_set_enabled(ADC_IRQ_FIFO, true);
  adc_run(true);
}

void setup(void)
{
  // run DSP on core 0
  vreg_set_voltage(VREG_VOLTAGE_1_30);
  const uint32_t clksys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(PIN_ATT,OUTPUT);
  pinMode(PIN_BAND,OUTPUT);
  pinMode(PIN_TXBIAS,OUTPUT);
  pinMode(PIN_TX,OUTPUT);
  pinMode(PIN_TXN,OUTPUT);
  pinMode(PIN_RXN,OUTPUT);
  pinMode(PIN_PTT,INPUT_PULLUP);
  pinMode(PIN_ENCBUT,INPUT_PULLUP);
  pinMode(PIN_UNUSED,INPUT_PULLUP);
  pinMode(PIN_REG,OUTPUT);

  // pin defaults
  // set pico regulator to low noise
  digitalWrite(PIN_REG,HIGH);
  digitalWrite(PIN_TXBIAS,LOW);
  digitalWrite(PIN_TX,LOW);
  digitalWrite(PIN_TXN,HIGH);
  digitalWrite(PIN_RXN,LOW);
  digitalWrite(PIN_ATT,radio.attenuation?HIGH:LOW);
  digitalWrite(PIN_BAND,radio.band==BAND_20M?HIGH:LOW);
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
  radio.mic_proc = DEFAULT_MIC_PROC;

#ifdef DEBUG_LED
  pinMode(LED_BUILTIN,OUTPUT);
  for (int i=0;i<2;i++)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(10);
    digitalWrite(LED_BUILTIN,LOW);
    delay(250);
  }
#endif

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

#ifdef DEBUG_LED
  pinMode(LED_BUILTIN,OUTPUT);
  for (int i=0;i<2;i++)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN,LOW);
    delay(250);
  }
#endif

  init_adc();

  // disable Mic in RX mode
  pinMode(PIN_MIC,OUTPUT);
  digitalWrite(PIN_MIC,LOW);

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
  char sz_clksys[16] = "";
  memset(sz_clksys,0,sizeof(sz_clksys));
  ultoa(clksys,sz_clksys,10);
  sz_clksys[3] = '\0';
  oled.print(VERSION_STRING);
  oled.print(sz_clksys);
  oled.setCursor(0,1);
#ifdef YOUR_CALL
  oled.setFont(FONT8X16);
  oled.setCursor(0,2);
  oled.print(" ");
  oled.print(YOUR_CALL);
#else
  oled.setFont(FONT8X16);
  oled.setCursor(0,2);
  oled.print(" VK7IAN");
#endif
  oled.switchFrame();

  // splash delay
  delay(4000);

  setup_complete = true;
}

void setup1(void)
{
  // run UI on core 1
  // only go to loop1 when setup() has completed
  while (!setup_complete)
  {
    tight_loop_contents();
  }

#ifdef DEBUG_LED
  pinMode(LED_BUILTIN,OUTPUT);
  for (int i=0;i<5;i++)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
    delay(250);
  }
#endif
}

void update_display(const uint32_t signal_level = 0u)
{
    char sz_frequency[16] = "";
    memset(sz_frequency,0,sizeof(sz_frequency));
    ultoa(radio.frequency,sz_frequency,10);
    uint8_t freq_pos = 0u;
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
    if (radio.tx_enable)
    {
      const char *sz_tx = "TX";
      switch (radio.mic_proc)
      {
        case 1: sz_tx = "P1"; break;
        case 2: sz_tx = "P2"; break;
        case 3: sz_tx = "P3"; break;
      }
      oled.invertOutput(true);
      oled.print(sz_tx);
      oled.invertOutput(false);
    }
    else
    {
      oled.print(radio.attenuation?" *":"  ");
    }
    oled.print(radio.mode==MODE_LSB?"LSB":"USB");
    oled.setCursor(0,2);
    oled.print(radio.tx_enable?"-25-50-75-":"-3-5-7-9-+");
    const uint8_t sig = min(signal_level,63);
    oled.bitmap(0, 3, sig, 4, AGC::meter);
    oled.switchFrame();
}

static void display_menu(const uint8_t window,const uint8_t selection)
{
  oled.clear();
  oled.setFont(FONT6X8);
  for (uint8_t i=window,j=0;i<NUM_MENU_ITEMS && j<4;i++,j++)
  {
    char sz_menu_name[16] = "";
    memset(sz_menu_name,' ',sizeof(sz_menu_name));
    sz_menu_name[10] = '\0';
    for (int j=0;j<10;j++)
    {
      const char c = menu_options[i].menu_name[j];
      if (c=='\0') break;
      sz_menu_name[j] = c;
    }
    oled.setCursor(0,j);
    if (i==selection) oled.invertOutput(true);
    oled.print(sz_menu_name);
    if (i==selection) oled.invertOutput(false);
  }
  oled.switchFrame();
}

static void display_menu_option(const uint8_t menu_id,const uint8_t option_id,const uint8_t selection)
{
  oled.clear();
  if (menu_id<NUM_MENU_ITEMS)
  {
    const uint8_t num_options = menu_options[menu_id].num_options;
    if (option_id<num_options && selection<num_options)
    {
      oled.setFont(FONT6X8);
      oled.setCursor(0,0);
      oled.print(menu_options[menu_id].menu_name);
      for (uint8_t i=option_id,j=1;i<num_options && j<4;i++,j++)
      {
        char sz_option_name[16] = "";
        memset(sz_option_name,' ',sizeof(sz_option_name));
        sz_option_name[10] = '\0';
        for (int j=1;j<10;j++)
        {
          const char c = menu_options[menu_id].options[i].option_name[j-1];
          if (c=='\0') break;
          sz_option_name[j] = c;
        }
        oled.setCursor(0,j);
        if (i==selection) oled.invertOutput(true);
        oled.print(sz_option_name);
        if (i==selection) oled.invertOutput(false);
      }
    }
  }
  oled.switchFrame();
}

static const option_value_t process_menu(void)
{
  option_value_t option = OPTION_EXIT;
  uint8_t menu_window = 0;
  uint8_t menu_current = 0;
  display_menu(menu_window,menu_current);
  if (digitalRead(PIN_ENCBUT)==LOW)
  {
    delay(50);
    while (digitalRead(PIN_ENCBUT)==LOW)
    {
      delay(50);
    }
    delay(50);
  }
  for (;;)
  {
    mutex_enter_blocking(&rotary_mutex);
    const int32_t rotary_delta = radio.tune;
    radio.tune = 0;
    mutex_exit(&rotary_mutex);
    if (rotary_delta>0)
    {
      if (menu_current<NUM_MENU_ITEMS-1)
      {
        menu_current++;
        if (menu_current>menu_window+3)
        {
          if (menu_window<NUM_MENU_ITEMS-1)
          {
            menu_window++;
          }
        }
        display_menu(menu_window,menu_current);
      }
    }
    else if (rotary_delta<0)
    {
      if (menu_current>0)
      {
        menu_current--;
        if (menu_current<menu_window)
        {
          if (menu_window>0)
          {
            menu_window--;
          }          
        }
        display_menu(menu_window,menu_current);
      }
    }
    // button press?
    if (digitalRead(PIN_ENCBUT)==LOW)
    {
      delay(50);
      while (digitalRead(PIN_ENCBUT)==LOW)
      {
        delay(50);
      }
      delay(50);
      if (menu_options[menu_current].menu_value==MENU_EXIT)
      {
        return option;
      }
      // process menu options
      const uint8_t num_options = menu_options[menu_current].num_options;
      uint8_t option_window = 0;
      uint8_t option_current = 0;
      display_menu_option(menu_current,option_window,option_current);
      for (;;)
      {
        mutex_enter_blocking(&rotary_mutex);
        const int32_t rotary_delta = radio.tune;
        radio.tune = 0;
        mutex_exit(&rotary_mutex);
        if (rotary_delta>0)
        {
          if (option_current<num_options-1)
          {
            option_current++;
            if (option_current>option_window+2)
            {
              if (option_window<num_options-1)
              {
                option_window++;
              }
            }
            display_menu_option(menu_current,option_window,option_current);
          }
        }
        else if (rotary_delta<0)
        {
          if (option_current>0)
          {
            option_current--;
            if (option_current<option_window)
            {
              if (option_window>0)
              {
                option_window--;
              }          
            }
            display_menu_option(menu_current,option_window,option_current);
          }
        }
        // button press?
        if (digitalRead(PIN_ENCBUT)==LOW)
        {
          delay(50);
          while (digitalRead(PIN_ENCBUT)==LOW)
          {
            delay(50);
          }
          delay(50);
          option = menu_options[menu_current].options[option_current].option_value;
          return option;
        }
      }
    }
  }
  return option;
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
  if (adc_fifo_get_level()<4u)
  {
    return;
  }
  adc_raw += adc_fifo_get();
  adc_raw += adc_fifo_get();
  adc_raw += adc_fifo_get();
  adc_raw += adc_fifo_get();
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
        process_mic(adc_value,mic_i,mic_q,radio.mic_proc);
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
      pinMode(PIN_MIC,OUTPUT);
      digitalWrite(PIN_MIC,LOW);
      adc_select_input(AGC_MUX);
      tx = false;
    }
  }
  else
  {
    // RX, check if changed to TX
    if (radio.tx_enable)
    {
      // switch to TX
      adc_gpio_init(PIN_MIC);
      adc_select_input(MIC_MUX);
      tx = true;
    }
    else
    {
      if (adc_value_ready)
      {
        adc_value_ready = false;
        agc_adc_value = adc_value;
        agc_value_ready = true;
        // set TX output to zero in receive mode
        dac_value_i_p = -512;
        dac_value_i_n = -512;
        dac_value_q_p = -512;
        dac_value_q_n = -512;
      }
      // only process rotary in receive mode
      switch (r.process())
      {
        case DIR_CW:
        {
          mutex_enter_blocking(&rotary_mutex);
          radio.tune++;
          mutex_exit(&rotary_mutex);
          break;
        }
        case DIR_CCW:
        {
          mutex_enter_blocking(&rotary_mutex);
          radio.tune--;
          mutex_exit(&rotary_mutex);
          break;
        }
      }
    }
  }
}

void loop1(void)
{
  // run UI on core 1
  static uint32_t old_frequency = radio.frequency;
  static uint32_t old_divisor = radio.divisor;
  static mode_t old_mode = radio.mode;
  static uint32_t agc_continue = 0;
  static uint8_t saved_agc_level = 0;

  // process AGC
  if (agc_value_ready && millis()>agc_continue)
  {
    agc_value_ready = false;
    const uint8_t agc_level = AGC::agc_pwm(agc_adc_value);
    analogWrite(PIN_AGCOUT,agc_level);
    saved_agc_level = agc_level;
  }

  // process button press
  // short press: change band
  // long press: menu options
  enum button_state_t {BUTTON_IDLE,BUTTON_TEST_SHORT,BUTTON_WAIT_RELEASE};
  enum button_action_t {BUTTON_NO_PRESS,BUTTON_SHORT_PRESS,BUTTON_LONG_PRESS};
  static button_state_t button_state = BUTTON_IDLE;
  static uint32_t button_timer = 0;
  button_action_t button_action = BUTTON_NO_PRESS;
  if (!radio.tx_enable)
  {
    switch (button_state)
    {
      case BUTTON_IDLE:
      {
        if (digitalRead(PIN_ENCBUT)==LOW)
        {
          button_state = BUTTON_TEST_SHORT;
          button_timer = millis()+BUTTON_LONG_PRESS_TIME;
          delay(50);
        }
        break;
      }
      case BUTTON_TEST_SHORT:
      {
        const uint32_t now = millis();
        if (digitalRead(PIN_ENCBUT)==HIGH)
        {
          button_state = BUTTON_IDLE;
          if (now<button_timer)
          {
            button_action = BUTTON_SHORT_PRESS;
          }
          delay(50);
          break;
        }
        if (now>button_timer)
        {
          button_state = BUTTON_WAIT_RELEASE;
          button_action = BUTTON_LONG_PRESS;
        }
        break;
      }
      case BUTTON_WAIT_RELEASE:
      {
        if (digitalRead(PIN_ENCBUT)==HIGH)
        {
          button_state = BUTTON_IDLE;
          delay(50);
        }
        break;
      }
    }

    // process botton action
    switch (button_action)
    {
      case BUTTON_SHORT_PRESS:
      {
        switch (radio.band)
        {
          case BAND_40M:
          {
            // change to 20M
            save_data[radio.band].frequency = radio.frequency;
            radio.band = BAND_20M;
            if (radio.set_attenuation==SET_ATTEN_AUTO) radio.attenuation = false;
            radio.frequency = save_data[radio.band].frequency;
            break;
          }
          case BAND_20M:
          {
            //change to 40M
            save_data[radio.band].frequency = radio.frequency;
            radio.band = BAND_40M;
            if (radio.set_attenuation==SET_ATTEN_AUTO) radio.attenuation = true;
            radio.frequency = save_data[radio.band].frequency;
            break;
          }
        }
        // mute during band change
        analogWrite(PIN_AGCOUT,LM4875_MUTE);
        delay(50);
        digitalWrite(PIN_BAND,radio.band==BAND_20M?HIGH:LOW);
        digitalWrite(PIN_ATT,radio.attenuation?HIGH:LOW);
        delay(50);
        break;
      }
      case BUTTON_LONG_PRESS:
      {////
        const option_value_t option = process_menu();
        switch (option)
        {
          case OPTION_MODE_LSB: radio.set_mode = SETMODE_LSB; break;
          case OPTION_MODE_USB: radio.set_mode = SETMODE_USB; break;
          case OPTION_MODE_AUTO: radio.set_mode = SETMODE_AUTO; break;
          case OPTION_STEP_10: radio.step = 10U; break;
          case OPTION_STEP_100: radio.step = 100U; break;
          case OPTION_STEP_500: radio.step = 500U; break;
          case OPTION_STEP_1000: radio.step = 1000U; break;
          case OPTION_ATTEN_ON: radio.set_attenuation = SET_ATTEN_ON; break; 
          case OPTION_ATTEN_OFF: radio.set_attenuation = SET_ATTEN_OFF; break; 
          case OPTION_ATTEN_AUTO: radio.set_attenuation = SET_ATTEN_AUTO; break; 
          case OPTION_MICLEVEL_1: radio.mic_proc = 1u; break; 
          case OPTION_MICLEVEL_2: radio.mic_proc = 2u; break; 
          case OPTION_MICLEVEL_3: radio.mic_proc = 3u; break; 
          case OPTION_MICLEVEL_OFF: radio.mic_proc = 0u; break; 
        }
        // set attenuation based on band or as selected
        switch (radio.set_attenuation)
        {
          case SET_ATTEN_ON: radio.attenuation = true; break;
          case SET_ATTEN_OFF: radio.attenuation = false; break;
          case SET_ATTEN_AUTO: radio.attenuation = (radio.band==BAND_40M)?true:false; break;
        }
        digitalWrite(PIN_ATT,radio.attenuation?HIGH:LOW);
        break;
      }
    }

    // set the mode based on frequency or as selected
    if (radio.set_mode==SETMODE_AUTO)
    {
      radio.mode = MODE_USB;
      if (radio.frequency<10000000ul)
      {
        if (radio.frequency<USB_FREQUENCY_LOW)
        {
          radio.mode = MODE_LSB;
        }
        else if (radio.frequency>USB_FREQUENCY_HIGH)
        {
          radio.mode = MODE_LSB;
        }
      }
    }
    else if (radio.set_mode==SETMODE_LSB)
    {
      radio.mode = MODE_LSB;
    }
    else if (radio.set_mode==SETMODE_USB)
    {
      radio.mode = MODE_USB;
    } 

    // process main tuning
    mutex_enter_blocking(&rotary_mutex);
    const int32_t tuning_delta = radio.tune;
    radio.tune = 0;
    mutex_exit(&rotary_mutex);
    radio.frequency = radio.frequency+(tuning_delta * (int32_t)radio.step);
    radio.frequency = radio.frequency/radio.step;
    radio.frequency = radio.frequency*radio.step;
    radio.frequency = constrain(radio.frequency,bands[radio.band].lo,bands[radio.band].hi);
    if (radio.frequency!=old_frequency)
    {
      old_frequency = radio.frequency;
      radio.divisor = get_divisor(radio.frequency);
      const uint64_t f = radio.frequency*SI5351_FREQ_MULT;
      const uint64_t p = radio.frequency*radio.divisor*SI5351_FREQ_MULT;
      si5351.set_freq_manual(f,p,SI5351_CLK0);
      si5351.set_freq_manual(f,p,SI5351_CLK1);
    } // frequency different

    switch (radio.mode)
    {
      case MODE_LSB:
      {
        si5351.set_phase(SI5351_CLK0,0);
        si5351.set_phase(SI5351_CLK1,radio.divisor);
        break;
      }
      case MODE_USB:
      {
        si5351.set_phase(SI5351_CLK0,radio.divisor);
        si5351.set_phase(SI5351_CLK1,0);
        break;
      }
    }

    // reset pll if mode or large frequency change
    if (radio.divisor!=old_divisor || radio.mode!=old_mode)
    {
      old_divisor = radio.divisor;
      old_mode = radio.mode;
      analogWrite(PIN_AGCOUT,LM4875_MUTE);
      delay(50);
      si5351.pll_reset(SI5351_PLLA);
    }

  } // receive mode
 
  // update the display every 50ms
  static uint32_t next_update = 0;
  const uint32_t now = millis();
  if (now>next_update)
  {
    next_update = now + 50ul;
    update_display(AGC::smeter());
  }

  // check for PTT
  if (digitalRead(PIN_PTT)==LOW)
  {
    // 1. mute the receiver
    // 2. set TX/RX relay to TX
    // 3. enable MIC (DSP)
    // 4. enable TX bias

    digitalWrite(LED_BUILTIN,HIGH);

    // mute the receiver
    analogWrite(PIN_AGCOUT,LM4875_SHUTDOWN);
    delay(50);

    // prevent TX/RX feedback
    // *Note, there is an issue with the RP2040 ADC switching circuitry
    pinMode(PIN_AGCIN,OUTPUT);
    digitalWrite(PIN_AGCIN,LOW);

    // disable QSD
    digitalWrite(PIN_RXN,HIGH);

    // switch antenna to TX
    digitalWrite(PIN_TX,HIGH);
    delay(20);

    // enable MIC processing
    radio.tx_enable = true;
    update_display();
    delay(10);

    // enable QSE and TX bias
    digitalWrite(PIN_TXN,LOW);
    digitalWrite(PIN_TXBIAS,HIGH);
    delay(50);

    // wait for PTT release
    uint32_t mic_peak_level = 0;
    uint32_t mic_level_update = 0;
    uint32_t tx_display_update = 0;
    uint32_t mic_hangtime_update = 0;
    uint32_t tx_debounce = 0;
    enum tx_button_state_t
    {
      TX_BUTTON_IDLE,
      TX_BUTTON_WAIT_BEBOUNCE,
      TX_BUTTON_WAIT_RELEASE,
      TX_BUTTON_DEBOUNCE
    }
    tx_button_state = TX_BUTTON_IDLE;
    while (digitalRead(PIN_PTT)==LOW)
    {
      static const uint32_t MIC_LEVEL_DECAY_RATE = 50ul;
      static const uint32_t MIC_LEVEL_HANG_TIME = 1000ul;
      const uint32_t now = millis();
      const uint32_t mic_level = abs(adc_value)>>5;
      if (mic_level>mic_peak_level)
      {
        mic_peak_level = mic_level;
        mic_level_update = now + MIC_LEVEL_DECAY_RATE;
        mic_hangtime_update = now + MIC_LEVEL_HANG_TIME;
      }
      else
      {
        if (now>mic_hangtime_update)
        {
          if (now>mic_level_update)
          {
            if (mic_peak_level) mic_peak_level--;
            mic_level_update = now + MIC_LEVEL_DECAY_RATE;
          }
        }
      }
      if (now>tx_display_update)
      {
        update_display(mic_peak_level);
        tx_display_update = now + 50ul;
      }
      else
      {
        delayMicroseconds(32);
      }
      // set/disable mic processing
      const uint32_t tx_now = millis();
      switch (tx_button_state)
      {
        case TX_BUTTON_IDLE:
        {
          if (digitalRead(PIN_ENCBUT)==LOW)
          {
            // set up to debounce
            tx_debounce = tx_now+100ul;
            // update processor
            radio.mic_proc++;
            if (radio.mic_proc>3) radio.mic_proc = 0;
            update_display(mic_peak_level);
            // wait for button release
            tx_button_state = TX_BUTTON_WAIT_BEBOUNCE;
          }
          break;
        }
        case TX_BUTTON_WAIT_BEBOUNCE:
        {
          if (tx_now>tx_debounce)
          {
            tx_button_state = TX_BUTTON_WAIT_RELEASE;
          }
          break;
        }
        case TX_BUTTON_WAIT_RELEASE:
        {
          if (digitalRead(PIN_ENCBUT)==HIGH)
          {
            // wait for button release
            tx_debounce = tx_now+100ul;
            tx_button_state = TX_BUTTON_DEBOUNCE;
          }
          break;
        }
        case TX_BUTTON_DEBOUNCE:
        {
          if (tx_now>tx_debounce)
          {
            tx_button_state = TX_BUTTON_IDLE;
          }
          break;
        }
      }
    }

    // back to receive
    digitalWrite(PIN_TXBIAS,LOW);
    digitalWrite(PIN_TXN,HIGH);
    adc_gpio_init(PIN_AGCIN);
    radio.tx_enable = false;
    delay(50);
    digitalWrite(PIN_TX,LOW);
    delay(20);
    digitalWrite(PIN_RXN,LOW);
    digitalWrite(LED_BUILTIN,LOW);
    update_display();
    for (uint32_t ramp=LM4875_MUTE;ramp<saved_agc_level;ramp++)
    {
      // ramp up to AGC level
      analogWrite(PIN_AGCOUT,ramp);
      delay(5);
    }
    agc_continue = millis()+200ul;
  }
}