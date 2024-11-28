#pragma once
#include <stddef.h>
#include <stdint.h>

#include <sound.hpp>

#include "emu2413.h"
namespace rdmtracker {
class OPLSound;

struct OPLNote {
  enum Key : uint8_t { _None, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B, C, X };
  Key key;
  uint8_t octave;
  uint8_t cmd;
};

struct OPLBlock {
  OPLNote notes[0x3f][8];
};

struct OPLSong {
  int num_orders;
  int num_blocks;
  int time;  // increments of 1/60
};

class OPL {
  OPLL* opll;
  rdm::SoundEmitter* emitter;
  size_t blabla;
  OPLSound* sound;
  int time_wait;

  int currentOrder;
  int currentRegion;
  int currentNote;

  OPLSong* song;
  int* songOrder;
  OPLBlock* songBlocks;

 public:
  OPL(rdm::SoundManager* manager);
  ~OPL();

  static uint16_t calcFNum(float freq, int block);
  static uint16_t calcFNum(OPLNote::Key n);

  void readSong(const char* file);

  void play();
  void upload();
  void imgui();

  std::string formattedLocation();

  OPLL* getOpll() { return opll; }
};
}  // namespace rdmtracker
