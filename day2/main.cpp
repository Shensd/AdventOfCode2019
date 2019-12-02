/*
The relevant challenge for this portion was far too long to place here, but 
it was effectively the specification for a very simple opcode language.

For the second part of the challenge, if done smart, required no code to be 
written. The most obvious choice is to use a brute force solution, but by 
chaning the noun and verb and looking at how it changed the output, by more 
or less binary search tree guessing the output could be obtained in about
8 or so guesses.
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#define INPUT_LOCATION "./input"

typedef unsigned int u_int;

/**
 * Get the file contents of the file at the given location.
 * 
 * Exits program with error message if file cannot be read.
 * 
 * @param location file location to read from
 * @returns string content of file
 */
std::string get_file_content(std::string location) {
    std::ifstream input(location);

    if(!input.is_open()) {
        std::cout << "Unable to open given input file." << std::endl;
        exit(-1);
    }

    std::string content = "";
    while(!input.eof()) {
        char buffer[1024];
        input.getline(buffer, 1024);
        content += buffer;
        content += "\n";
    }

    return content;
}

/**
 * Given a string source, parse the opcodes into a unsigned int vector.
 * 
 * @param source opcode source 
 * @returns vector of unsigned integers representing opcodes
 */
std::vector<u_int> parse_opcodes(std::string source) {
    std::vector<u_int> opcodes;
    std::stringstream sstream(source);

    while(!sstream.eof()) {
        char buffer[1024];
        // opcodes are command delimited
        sstream.getline(buffer, 1024, ',');

        opcodes.push_back(
            std::stoi(buffer)
        );
    }

    return opcodes;
}   

#define OPCODE_ADD 1
#define OPCODE_MUL 2
#define OPCODE_END 99

/**
 * Run the program given by a vector of opcodes, the program is run in place and 
 * modifies the vector given
 * 
 * @param opcodes a vector of opcodes to work as program instructions
 */
void run_program(std::vector<u_int>& opcodes) {
    for(int i = 0; i < opcodes.size(); i++) {

        u_int current_opcode = opcodes[i];
        u_int left = opcodes[opcodes[i+1]];
        u_int right = opcodes[opcodes[i+2]];
        u_int location = opcodes[i+3];

        switch(current_opcode) {

            case OPCODE_ADD:
                opcodes[location] = left + right;
                // skip next 4 instructions since they were eaten
                i+=3;
                break;
            case OPCODE_MUL:
                opcodes[location] = left * right;
                // skip next 4 instructions since they were eaten
                i+=3;
                break;
            case OPCODE_END:
                return;
            default:
                std::cout << "Unexpected instruction '" << current_opcode << "'" << std::endl;
                exit(-1);
                break;
        }

    }

}

int main(int argc, char* argv[]) {
    std::string content = get_file_content(INPUT_LOCATION);
    std::vector<unsigned int> opcodes = parse_opcodes(content);

    run_program(opcodes);

    // output only the meaningful parts of the end program
    std::cout << "NOUN   : " << opcodes[1] << std::endl;
    std::cout << "VERB   : " << opcodes[2] << std::endl;
    std::cout << "OUTPUT : " << opcodes[0] << std::endl;
}