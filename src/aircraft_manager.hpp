#pragma once

#include <vector>

#include "aircraft.hpp"

class AircraftManager : public GL::DynamicObject
{
private:
    std::vector<std::unique_ptr<Aircraft>> aircrafts;
public:
    bool update() override;
    void add_aircraft(std::unique_ptr<Aircraft> aircraft);
};