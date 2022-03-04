#include "aircraft_manager.hpp"

bool AircraftManager::update() {
    for (auto it = aircrafts.begin(); it != aircrafts.end();)
    {
        auto& item = *it;
        if (item->update())
        {
            ++it;
        }
        else
        {
            it = aircrafts.erase(it);
        }
    }
    return true;
}
void AircraftManager::add_aircraft(std::unique_ptr<Aircraft> aircraft)
{
    aircrafts.emplace_back(std::move(aircraft));
}
