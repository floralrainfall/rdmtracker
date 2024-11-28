#pragma once
#include <game.hpp>

#include "opl.hpp"
namespace rdmtracker {
class Tracker : public rdm::Game {
  std::unique_ptr<OPL> opl;

 public:
  virtual void initialize();
  virtual void initializeClient();
};
};  // namespace rdmtracker
