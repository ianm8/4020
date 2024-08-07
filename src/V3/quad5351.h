#ifndef QUAD5351_H
#define QUAD5351_H

static const uint32_t get_divisor(const uint32_t f)
{
  if (f < 6850000ul)
  {
    return 126;
  }
  if (f < 9500000ul)
  {
    return 88;
  }
  if (f < 13600000ul)
  {
    return 64;
  }
  if (f < 17500000ul)
  {
    return 44;
  }
  if (f < 25000000ul)
  {
    return 34;
  }
  if (f < 36000000ul)
  {
    return 24;
  }
  if (f < 45000000ul)
  {
    return 18;
  }
  if (f < 60000000ul)
  {
    return 14;
  }
  if (f < 80000000ul)
  {
    return 10;
  }
  if (f < 100000000ul)
  {
    return 8;
  }
  if (f < 146600000ul)
  {
    return 6;
  }
  return 4;
}

#endif