#include "io_helper.h"

IoHelperCommandLine::IoHelperCommandLine() {
    
}

IoHelperCommandLine::~IoHelperCommandLine() {
    
}

std::string IoHelperCommandLine::get_input() {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void IoHelperCommandLine::print_output(const std::string& output) {
    std::cout << output << std::endl;
}