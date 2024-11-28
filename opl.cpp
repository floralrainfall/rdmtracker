#include "opl.hpp"

#include <AL/al.h>
#include <imgui.h>

#include <format>

#include "emu2413.h"
#include "filesystem.hpp"

#define MSX_CLK 357945
#define SAMPLERATE 48000

namespace rdmtracker {
class OPLSound : public rdm::Sound {
  OPL* opl;

 public:
  OPLSound(OPL* opl) : opl(opl) {}

  virtual bool uploadData(ALuint buffer, bool loop) {
    std::vector<int16_t> read;
    read.resize(128);

    opl->upload();

    for (int i = 0; i < read.size(); i++) {
      read[i] = OPLL_calc(opl->getOpll()) * 8;
    }

    alBufferData(buffer, AL_FORMAT_MONO16, &read.front(),
                 read.size() * sizeof(int16_t), SAMPLERATE);

    return true;
  }

  virtual rdm::Sound::LoadType getLoadType() { return rdm::Sound::Stream; }
};

uint16_t OPL::calcFNum(float freq, int block) {
  float f = (freq * (pow(2.f, 18.f) / (MSX_CLK / 72.f))) / pow(2, block - 1);
  return (uint16_t)floorf(f);
}

uint16_t OPL::calcFNum(OPLNote::Key n) {
  uint16_t freqs[] = {0, 181, 192, 204, 216, 229, 242, 257, 288, 305, 323, 343};
  return freqs[n];
}

OPL::OPL(rdm::SoundManager* manager) {
  opll = OPLL_new(MSX_CLK, SAMPLERATE);
  OPLL_reset(opll);

  blabla = 1;

  emitter = manager->newEmitter();
  OPLL_writeReg(opll, 0x30, 0x40);
  // OPLL_writeReg(opll, 0x31, 0xf0);
  sound = new OPLSound(this);
  emitter->setGain(100.0);
  emitter->setPitch(1.0);

  play();
  emitter->play(sound);

  song = 0;
  currentNote = 0;
  currentOrder = 0;
  currentRegion = 0;
  time_wait = 0;

  readSong("test.rmd");
}

void OPL::upload() {}

void OPL::readSong(const char* file) {
  common::FileIO* io =
      common::FileSystem::singleton()->getFileIO(file, "r").value();

  song = new OPLSong;
  io->read(song, sizeof(OPLSong));
  songOrder = (int*)malloc(sizeof(int) * song->num_orders);
  io->read(songOrder, sizeof(int) * song->num_orders);
  songBlocks = (OPLBlock*)malloc(sizeof(OPLBlock) * song->num_blocks);
  io->read(songBlocks, sizeof(OPLBlock) * song->num_blocks);
}

void OPL::imgui() {
  ImGui::Begin("OPL");
  ImGui::Text("Order: %i, Region: %i, Note: %i", currentOrder, currentRegion,
              currentNote);
  ImGui::End();
}

std::string OPL::formattedLocation() {
  return std::format("{}.{}.{}", currentOrder, currentRegion, currentNote);
}

void OPL::play() {
  if (song) {
    // play note

    if (time_wait == 0) {
      time_wait = song->time;
    } else {
      time_wait--;
      return;
    }
    int nextNote = currentNote + 1;

    for (int chan = 0; chan < 8; chan++) {
      OPLNote note = songBlocks[currentRegion].notes[currentNote][chan];

      if (note.cmd & 0x01) {
        switch (note.cmd & 0x01) {
          default:
            rdm::Log::printf(rdm::LOG_WARN, "%s(%i): unknown command %i",
                             formattedLocation().c_str(), chan,
                             note.cmd & 0x01);
            break;  // do nothing
          case 1:   // set instrument
            OPLL_writeReg(opll, 0x30 + chan, (note.cmd & 0xf0));
            rdm::Log::printf(rdm::LOG_DEBUG, "%s(%i): SETINSTRUMENT = %02x",
                             formattedLocation().c_str(), chan,
                             (note.cmd & 0xf0));
            break;
          case 2:  // set note
            nextNote = (note.cmd & 0xfe) >> 1;
            rdm::Log::printf(rdm::LOG_DEBUG, "%s(%i): SETNOTE = %02x",
                             formattedLocation().c_str(), chan, nextNote);
            break;
        }
      }

      if (note.key == OPLNote::X) {
        OPLL_writeReg(opll, 0x20 + chan, 0);  // note off
      } else if (note.key == OPLNote::_None) {
        // do nothing
      } else {
        uint16_t fNum = calcFNum(note.key);
        uint8_t fNumL = fNum & 0xff;
        uint8_t fNumH = (fNum >> 8) & 0b1;

        uint8_t octave = (note.octave & 0b111) << 1;

        OPLL_writeReg(opll, 0x10 + chan, (char)fNumL);
        OPLL_writeReg(opll, 0x20 + chan, 0b00010000 | octave | fNumH);
      }
    }

    currentNote = nextNote;

    if (currentNote >= 0x40) {
      currentNote = 0;
      currentOrder++;
      if (currentOrder >= song->num_orders) {
        currentOrder = 0;
      }
      currentRegion = songOrder[currentOrder];
    }
  }
  /* */
}

OPL::~OPL() { OPLL_delete(opll); }
};  // namespace rdmtracker
