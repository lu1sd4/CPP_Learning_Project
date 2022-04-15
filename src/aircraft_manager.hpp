#pragma once

#include <vector>

#include "aircraft.hpp"

class AircraftManager : public GL::DynamicObject
{
private:
    std::vector<std::unique_ptr<Aircraft>> aircrafts;
    int crashed_planes = 0;
public:
    bool update() override;
    void add_aircraft(std::unique_ptr<Aircraft> aircraft);
    int count_in_airline(const std::string& airline) const;
    int get_required_fuel() const;
    int get_crashed_planes() const;
};