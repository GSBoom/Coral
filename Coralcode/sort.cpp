#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

int main() {
    // Open the input file containing numbers
    string inputfilepath=".\\ResultNew\\CDF\\UK-2002\\cdf-4000-4-1outflow_weight_info";
    std::ifstream inputFile(inputfilepath+".txt");
    if (!inputFile.is_open()) {
        std::cerr << "open failed" << std::endl;
        return 1;
    }

    std::vector<int> numbers;
    int number;
    // Read numbers from the input file
    while (inputFile >> number) {
        numbers.push_back(number);
    }
    inputFile.close();

    // Sort the read numbers.
    std::sort(numbers.begin(), numbers.end());

    // Open the output file for writing the sorted results.
    std::ofstream outputFile(inputfilepath+"-sort.txt");
    if (!outputFile.is_open()) {
        std::cerr << "output illegal！" << std::endl;
        return 1;
    }

    // Write the sorted numbers to the output file.
    for (int num : numbers) {
        outputFile << num << std::endl;
    }
    outputFile.close();

    std::cout << "success write " << std::endl;
    return 0;
}