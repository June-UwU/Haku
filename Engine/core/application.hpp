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
 
  const Status initialize() const;
  const Status exec();
private:
};

