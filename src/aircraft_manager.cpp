#include "aircraft_manager.hpp"

#include <algorithm>

bool AircraftManager::update() {
    aircrafts.erase(
        std::remove_if(
            aircrafts.begin(),
            aircrafts.end(),
            [](auto& a) { return !a->update(); }),
    aircrafts.end());
    return true;
}

void AircraftManager::add_aircraft(std::unique_ptr<Aircraft> aircraft)
{
    aircrafts.emplace_back(std::move(aircraft));
}
