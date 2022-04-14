#include "tower_sim.hpp"

#include "GL/opengl_interface.hpp"
#include "aircraft.hpp"
#include "aircraft_manager.hpp"
#include "airport.hpp"
#include "config.hpp"
#include "img/image.hpp"
#include "img/media_path.hpp"

#include <cassert>
#include <cstdlib>
#include <ctime>

using namespace std::string_literals;

TowerSimulation::TowerSimulation(int argc, char** argv) :
    help { (argc > 1) && (std::string { argv[1] } == "--help"s || std::string { argv[1] } == "-h"s) }
{
    MediaPath::initialize(argv[0]);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    GL::init_gl(argc, argv, "Airport Tower Simulation");

    create_keystrokes();
}

void TowerSimulation::create_random_aircraft() const
{
    assert(airport); // make sure the airport is initialized before creating aircraft
    aircraft_manager->add_aircraft(aircraft_factory->with_airport(airport.get()));
}

void TowerSimulation::create_keystrokes() const
{
    GL::keystrokes.emplace('x', []() { GL::exit_loop(); });
    GL::keystrokes.emplace('q', []() { GL::exit_loop(); });
    GL::keystrokes.emplace('c', [this]() { create_random_aircraft(); });
    GL::keystrokes.emplace('+', []() { GL::change_zoom(0.95f); });
    GL::keystrokes.emplace('-', []() { GL::change_zoom(1.05f); });
    GL::keystrokes.emplace('>', []() { GL::increase_fps(); });
    GL::keystrokes.emplace('<', []() { GL::decrease_fps(); });
    GL::keystrokes.emplace('f', []() { GL::toggle_fullscreen(); });
    GL::keystrokes.emplace('p', []() { GL::toggle_pause(); });
    for (int i = 0; i <= 7; ++i)
    {
        GL::keystrokes.emplace('0'+i, [this, i]() { display_count_airline(i); });
    }
}

void TowerSimulation::display_count_airline(const int& airline_index) const
{
    const auto airline_name = aircraft_factory->airlines[airline_index];
    std::cout << aircraft_manager->count_in_airline(airline_name) << " aircrafts in " << airline_name << std::endl;
}

void TowerSimulation::display_help() const
{
    std::cout << "This is an airport tower simulator" << std::endl
              << "the following keysstrokes have meaning:" << std::endl;

    for (const auto& [ key, action ] : GL::keystrokes)
    {
        std::cout << key << ' ';
    }

    std::cout << std::endl;
}

void TowerSimulation::init_airport()
{
    aircraft_factory = std::make_unique<AircraftFactory>();
    aircraft_manager = std::make_unique<AircraftManager>();

    airport = std::make_unique<Airport>( one_lane_airport, Point3D { 0, 0, 0 },
                            new img::Image { one_lane_airport_sprite_path.get_full_path() }, aircraft_manager.get());

    GL::move_queue.emplace(airport.get());
    GL::move_queue.emplace(aircraft_manager.get());
}

void TowerSimulation::launch()
{
    if (help)
    {
        display_help();
        return;
    }

    init_airport();

    GL::loop();
}
