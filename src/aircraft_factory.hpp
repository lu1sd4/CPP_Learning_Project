#pragma once

#include <set>
#include <string>

#include "aircraft.hpp"
#include "airport.hpp"

class AircraftFactory
{
public:
    std::unique_ptr<Aircraft> with_airport(Airport* airport)
    {
        std::string flight_number;
        do {
            flight_number = airlines[std::rand() % 8] + std::to_string(1000 + (rand() % 9000));
        } while(flight_numbers.find(flight_number) != flight_numbers.end());
        const float angle       = (rand() % 1000) * 2 * 3.141592f / 1000.f; // random angle between 0 and 2pi
        const Point3D start     = Point3D { std::sin(angle), std::cos(angle), 0 } * 3 + Point3D { 0, 0, 2 };
        const Point3D direction = (-start).normalize();
        return std::make_unique<Aircraft>(*(aircraft_types[rand() % 3]), flight_number, start, direction, airport->get_tower());
    }
private:
    const std::string airlines[8] = { "AF", "LH", "EY", "DL", "KL", "BA", "AY", "EY" };
    const AircraftType* aircraft_types[3] = {
        new AircraftType { .02f, .05f, .02f, MediaPath { "l1011_48px.png" } },
        new AircraftType { .02f, .05f, .02f, MediaPath { "b707_jat.png" } },
        new AircraftType { .04f, .1f, .04f, MediaPath { "concorde_af.png" } }
    };
    std::set<std::string> flight_numbers;
};
