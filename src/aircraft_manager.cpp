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
int AircraftManager::count_in_airline(const std::string& airline) const
{
    return std::count_if(aircrafts.begin(), aircrafts.end(), [airline](const auto& a) {
         return a->get_flight_num().substr(0,2) == airline;
    });
}
