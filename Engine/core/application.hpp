#pragma once
#include "defines.hpp"

class HakuEngine 
{
public:
  HakuEngine ();
  HakuEngine (HakuEngine  &&) = default;
  HakuEngine (const HakuEngine  &) = default;
  HakuEngine  &operator=(HakuEngine  &&) = default;
  HakuEngine  &operator=(const HakuEngine  &) = default;
  ~HakuEngine ();
 
  Status initialize() const;
  Status exec();
private:
};

