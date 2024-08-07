#ifndef MENU_H
#define MENU_H

/*

 Step
  10
  100
  500
  1000
  Exit

 Mode
   LSB
   USB
   Auto
   Exit

  Atten
    On
    Off
    Auto
    Exit

  Mic Proc
    Level 1
    Level 2
    Level 3
    Exit

  Exit
    Exit

*/

#define NUM_MENU_ITEMS 5U

enum menu_top_t
{
  MENU_STEP,
  MENU_MODE,
  MENU_ATTEN,
  MENU_MIC,
  MENU_EXIT
};

enum option_value_t
{
  OPTION_STEP_10,
  OPTION_STEP_100,
  OPTION_STEP_500,
  OPTION_STEP_1000,
  OPTION_MODE_USB,
  OPTION_MODE_LSB,
  OPTION_MODE_AUTO,
  OPTION_ATTEN_ON,
  OPTION_ATTEN_OFF,
  OPTION_ATTEN_AUTO,
  OPTION_MICLEVEL_1,
  OPTION_MICLEVEL_2,
  OPTION_MICLEVEL_3,
  OPTION_MICLEVEL_OFF,
  OPTION_EXIT
};

struct options_t
{
  option_value_t option_value;
  const char* option_name;
};

volatile static struct
{
  const menu_top_t menu_value;
  const char *menu_name;
  const uint8_t num_options;
  const options_t options[5];
}
menu_options[] =
{
  {
    MENU_STEP,
    "Step",
    5U,
    {
      {OPTION_STEP_10,"10"},
      {OPTION_STEP_100,"100"},
      {OPTION_STEP_500,"500"},
      {OPTION_STEP_1000,"1000"},
      {OPTION_EXIT,"Exit"}
    }
  },
  {
    MENU_MODE,
    "Mode",
    4U,
    {
      {OPTION_MODE_LSB,"LSB"},
      {OPTION_MODE_USB,"USB"},
      {OPTION_MODE_AUTO,"AUTO"},
      {OPTION_EXIT,"Exit"},
      {OPTION_EXIT,"Exit"}
    }
  },
  {
    MENU_ATTEN,
    "Atten",
    4U,
    {
      {OPTION_ATTEN_ON,"On"},
      {OPTION_ATTEN_OFF,"Off"},
      {OPTION_ATTEN_AUTO,"Auto"},
      {OPTION_EXIT,"Exit"},
      {OPTION_EXIT,"Exit"}
    }
  },
  {
    MENU_MIC,
    "Processor",
    5U,
    {
      {OPTION_MICLEVEL_1,"Level 1"},
      {OPTION_MICLEVEL_2,"Level 2"},
      {OPTION_MICLEVEL_3,"Level 3"},
      {OPTION_MICLEVEL_OFF,"Off"},
      {OPTION_EXIT,"Exit"}
    }
  },
  {
    MENU_EXIT,
    "Exit",
    1U,
    {
      {OPTION_EXIT,"Exit"},
      {OPTION_EXIT,"Exit"},
      {OPTION_EXIT,"Exit"},
      {OPTION_EXIT,"Exit"},
      {OPTION_EXIT,"Exit"}
    }
  }
};

#endif