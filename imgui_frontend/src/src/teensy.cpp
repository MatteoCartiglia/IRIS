#include "teensy.h"


std::vector<int> readbias_file(const std::string& path) {
    std::stringstream ss;
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        std::cout << "Could not open the file - '"
             << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    ss << input_file.rdbuf();
    std::string value;
    std::vector<int> biases;

    while(std::getline(ss,value,'\n')){
        biases.push_back(stoi(value));
    }
    return biases;
}


