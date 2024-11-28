#include <game.hpp>
#include <settings.hpp>

#include "tracker.hpp"
int main(int argc, char** argv) {
  rdm::Settings::singleton()->parseCommandLine(argv, argc);
  rdmtracker::Tracker tracker;
  tracker.mainLoop();
  rdm::Settings::singleton()->save();
}
