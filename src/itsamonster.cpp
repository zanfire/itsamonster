#include "Monster.hpp"

#include "monsters/YoungGoldDragon.hpp"
#include "monsters/Yochlol.hpp"
#include "monsters/AwakenedPlants.hpp"
#include "monsters/Larvae.hpp"
#include "Match.hpp"

#include <iostream>
#include <random>
#include "Dice.hpp"
#include <ctime>
#include <iomanip>

using namespace itsamonster;

int main(int argc, char **argv) {
    int total_simulations = 1000;
    unsigned int seed = static_cast<unsigned int>(std::time(nullptr));
    bool in_darkness = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            Logger::Instance().SetVerbose(true);
        } else if ((arg == "-n" || arg == "--iterations") && i + 1 < argc) {
            total_simulations = std::stoi(argv[++i]);
        } else if ((arg == "-s" || arg == "--seed") && i + 1 < argc) {
            seed = static_cast<unsigned int>(std::stoul(argv[++i]));
        } else if (arg == "-d" || arg == "--darkness") {
            in_darkness = true;
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: itsamonster [-v|--verbose] [-n|--iterations <num>] [-s|--seed <seed>]" << std::endl;
            std::cout << "  -v, --verbose        Show detailed output for a single fight" << std::endl;
            std::cout << "  -n, --iterations N   Number of simulations to run (default: 1000)" << std::endl;
            std::cout << "  -s, --seed SEED      Set RNG seed (default: current time)" << std::endl;
            std::cout << "  -d, --darkness       Run the match in darkness (default: false)" << std::endl;
            std::cout << "  -h, --help           Show this help message" << std::endl;
            return 0;
        }
    }
    std::cout << "Seed: " << seed << "\n";

    //Match<Larvae, AwakenedPlants> match(in_darkness);
    Match<YoungGoldDragon, Yochlol> match(in_darkness);
    match.Go(total_simulations, seed);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << match.GetMonster1Name() << " win rate: " << match.GetMonster1Wins() << " / " << total_simulations << " (" << (match.GetMonster1Wins() * 100.0 / total_simulations) << "%)\n";
    std::cout << match.GetMonster2Name() << " win rate: " << match.GetMonster2Wins() << " / " << total_simulations << " (" << (match.GetMonster2Wins() * 100.0 / total_simulations) << "%)\n";
    std::cout << "Average rounds per fight: " << match.GetAverageRounds(total_simulations) << "\n";

    return 0;
}