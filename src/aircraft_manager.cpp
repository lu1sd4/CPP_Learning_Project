#include "aircraft_manager.hpp"

#include <algorithm>

bool AircraftManager::update() {
    std::sort(
        aircrafts.begin(),
        aircrafts.end(),
        [](const std::unique_ptr<Aircraft>& a1, const std::unique_ptr<Aircraft>& a2)
        {
            if (a1->has_terminal()) return false; // if a1 has terminal it's not ahead of a2
            return a1->fuel < a2->fuel; // if a1 has less fuel than a2 it's ahead of a2
        }
    );

    aircrafts.erase(
        std::remove_if(
            aircrafts.begin(),
            aircrafts.end(),
            [](auto& a) {
                try {
                    bool result = a->update();
                    return !result;
                } catch (const AircraftCrash &e) {
                    std::cerr << e.what() << std::endl;
                    return true;
                }
            }),
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
int AircraftManager::get_required_fuel() const
{
    int sum = 0;
    for (const auto& aircraft: aircrafts) {
        if (aircraft->is_low_on_fuel() && !aircraft->has_been_serviced) {
            sum += (3000 - aircraft->fuel);
        }
    }
    return sum;
}
