#include "tower_sim.hpp"

int main(int argc, char** argv)
{
    TowerSimulation simulation { argc, argv };
    simulation.launch();
//    test_generic_points();
    return 0;
}