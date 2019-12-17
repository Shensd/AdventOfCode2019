#include "intcode.hpp"

#define INPUT_LOCATION "./input"

int main(int argc, char** argv) {

    std::string input_location = INPUT_LOCATION;

    if(argc > 1) input_location = argv[1];

    std::vector<long> opcodes = intcode::get_opcodes_from_file(input_location);

    intcode::RunState state = intcode::run_program(opcodes, {2L});

    std::cout << "OUTPUT ";
    for(auto i : state.output) std::cout << i << " ";
    std::cout << std::endl;

    return 0;
}