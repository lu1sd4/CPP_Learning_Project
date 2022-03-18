#pragma once

#include "aircraft_manager.hpp"
#include "aircraft_factory.hpp"
class Airport;
struct AircraftType;

class TowerSimulation
{
private:
    bool help        = false;
    std::unique_ptr<Airport> airport = nullptr;
    std::unique_ptr<AircraftManager> aircraft_manager = nullptr;
    std::unique_ptr<AircraftFactory> aircraft_factory = nullptr;

    TowerSimulation(const TowerSimulation&) = delete;
    TowerSimulation& operator=(const TowerSimulation&) = delete;

    void create_random_aircraft() const;
    void display_count_airline(const int& airline_index) const;

    void create_keystrokes() const;
    void display_help() const;

    void init_airport();

public:
    TowerSimulation(int argc, char** argv);
    ~TowerSimulation() = default;

    void launch();
};
