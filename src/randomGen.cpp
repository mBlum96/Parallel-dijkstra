#include <iostream>
#include <random>

int main()
{
    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Set up distributions for the source, destination, and weight
    std::uniform_int_distribution<> src_dist(0, 199);
    std::uniform_int_distribution<> dest_dist(0, 199);
    std::uniform_int_distribution<> weight_dist(1, 1000);

    for (int i = 0; i < 8; i++)
    {
        // Generate random values for the source, destination, and weight
        int src = src_dist(gen);
        int dest = dest_dist(gen);
        int weight = weight_dist(gen);

        // Print the edge
        std::cout << src << " " << dest << " " << weight << std::endl;
    }

    return 0;
}