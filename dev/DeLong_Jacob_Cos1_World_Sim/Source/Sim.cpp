#include "Sim.h"
#include <algorithm>

Sim::Sim() : currentTick(0), OverseerEnergy(100), maxOverseerEnergy(100) {}

void Sim::AdvanceTicks(int tickCount) {
	if (tickCount <= 0) {
		return;
	}
	for (int i = 0; i < tickCount; ++i) {
		StepSingleTick();
	}
}

void Sim::StepSingleTick() {
	++currentTick;
}