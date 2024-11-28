#include "tracker.hpp"
namespace rdmtracker {
void Tracker::initialize() { startClient(); }

void Tracker::initializeClient() {
  world->setTitle("RDMTracker");
  opl.reset(new OPL(soundManager.get()));

  world->stepped.listen([this] { opl->play(); });
  gfxEngine->renderStepped.listen([this] { opl->imgui(); });
}
};  // namespace rdmtracker
