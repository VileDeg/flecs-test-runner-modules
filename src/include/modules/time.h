#pragma once

#include <flecs.h>

namespace waiting {
  struct Timer {
    int time;
  };

  struct TimeStep{
    int step;
  };
  
  struct module {
    module(flecs::world& world);
  };
}