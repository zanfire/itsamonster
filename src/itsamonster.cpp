
#include "monsters/MermaidWarrior.hpp"
#include "monsters/SharkyWarrior.hpp"
#include "Match.hpp"

#include <iostream>
#include <ctime>
#include <iomanip>

using namespace itsamonster;

int main(int argc, char **argv) {
    int total_simulations = 1;
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

    using TeamFrogs = std::array<MermaidWarrior, 2>;
    using TeamFish = std::array<SharkyWarrior, 1>;
    Match<TeamFrogs, TeamFish> match(in_darkness);
    match.AddSpawnPosition({ 200, 10, 0 });
    match.AddSpawnPosition({ 180, 30, 0 });
    match.AddSpawnPosition({ 200, 90, 0 });

    match.Go(total_simulations, seed);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << match.GetMonster1Name() << " win rate: " << match.GetMonster1Wins() << " / " << total_simulations << " (" << (match.GetMonster1Wins() * 100.0 / total_simulations) << "%)\n";
    std::cout << match.GetMonster2Name() << " win rate: " << match.GetMonster2Wins() << " / " << total_simulations << " (" << (match.GetMonster2Wins() * 100.0 / total_simulations) << "%)\n";
    std::cout << "Average rounds per fight: " << match.GetAverageRounds(total_simulations) << "\n";

    return 0;
}