#include "Monster.hpp"

#include "YoungGoldDragon.hpp"
#include "Yochlol.hpp"
#include "Match.hpp"

#include <iostream>
#include <random>
#include <ctime>
#include <iomanip>

using namespace itsamonster;
bool g_verbose = false; // global verbose flag used by Monster methods


int main(int argc, char **argv) {
    int total_simulations = 1000;
    unsigned int seed = static_cast<unsigned int>(std::time(nullptr));
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            g_verbose = true;
        } else if ((arg == "-n" || arg == "--iterations") && i + 1 < argc) {
            total_simulations = std::stoi(argv[++i]);
        } else if ((arg == "-s" || arg == "--seed") && i + 1 < argc) {
            seed = static_cast<unsigned int>(std::stoul(argv[++i]));
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: itsamonster [-v|--verbose] [-n|--iterations <num>] [-s|--seed <seed>]" << std::endl;
            std::cout << "  -v, --verbose        Show detailed output for a single fight" << std::endl;
            std::cout << "  -n, --iterations N   Number of simulations to run (default: 1000)" << std::endl;
            std::cout << "  -s, --seed SEED      Set RNG seed (default: current time)" << std::endl;
            std::cout << "  -h, --help           Show this help message" << std::endl;
            return 0;
        }
    }
    std::cout << "Seed: " << seed << "\n";

    // If verbose, show a single illustrative fight first
    if (g_verbose) {
        //Battle(1, rng); // one verbose fight
    }

    Match<YoungGoldDragon, Yochlol> match;
    match.Go(total_simulations, seed);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << match.GetMonster1Name() << " win rate: " << match.GetMonster1Wins() << " / " << total_simulations << " (" << (match.GetMonster1Wins() * 100.0 / total_simulations) << "%)\n";
    std::cout << match.GetMonster2Name() << " win rate: " << match.GetMonster2Wins() << " / " << total_simulations << " (" << (match.GetMonster2Wins() * 100.0 / total_simulations) << "%)\n";

    return 0;
}