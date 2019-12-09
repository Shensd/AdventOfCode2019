#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

typedef int (*opcodefnptr)(int, std::vector<int>&); 

class Instruction {
public:
    // max argument amount is 3
    bool flags[3];
    unsigned int opcode;

    Instruction(unsigned int opcode, bool flags[3]) : opcode(opcode) {
        std::copy(flags, flags+3, Instruction::flags);
    }

    std::string to_string(void) {
        char buffer[32];
        sprintf(buffer, "OPCODE : %u, FLAGS : %d%d%d", opcode, flags[0], flags[1], flags[2]);
        return buffer;
    }
};

/**
 * Given an intruction, parse it into an Instruction object, with flags and opcode
 * 
 * @param instruction integer instruction to parse
 * @returns Instruction object representing corresponding object
 * 
 */
Instruction parse_instruction(int instruction) {
    std::string opcode_string = std::to_string(instruction);

    // pad from with 0s if they are missing, as non present flags default to 0
    if(opcode_string.size() != 5) {
        opcode_string.insert(0, 5 - opcode_string.size(), '0');
    }

    // opcode is last two digits of number
    int opcode = std::stoi(opcode_string.substr(3, 2));

    // flags are in reverse order
    bool flags[] = {
        std::stoi(opcode_string.substr(2, 1)),
        std::stoi(opcode_string.substr(1, 1)),
        std::stoi(opcode_string.substr(0, 1))
    };

    return Instruction(opcode, flags);
}

/**
 * Given a file location, grab opcodes from file
 * 
 * @param file_location location of file
 * @returns vector of unsigned integers representing opcodes
 */
std::vector<int> get_opcodes_from_file(std::string file_location) {
    std::vector<int> opcodes;
    std::ifstream input_file(file_location);

    while(!input_file.eof()) {
        char buffer[1024];
        // opcodes are command delimited
        input_file.getline(buffer, 1024, ',');

        opcodes.push_back(
            std::stoi(buffer)
        );
    }

    return opcodes;
}   

/** ####################### **/
/** BEGIN INSTRUCTION BLOCK **/
/** ####################### **/

/**
 * Opcode  : 1
 * Operands: 3
 * 
 * Adds the first two operands and writes the result to the third
 */
int instr_add(int offset, std::vector<int>& tape) {
    Instruction instruction = parse_instruction(tape[offset]);
    // 0 is address mode, 1 is immediate mode, grab values accordingly
    int left = (instruction.flags[0]) ? tape[offset+1] : tape[tape[offset+1]];
    int right = (instruction.flags[1]) ? tape[offset+2] : tape[tape[offset+2]];
    // locations are never in immediate mode
    int location = tape[offset+3];

    if(location > tape.size()) {
        std::cout << "Attempted write to out of bounds address " << location << std::endl;
        exit(-1);
    }

    tape[location] = left + right;

    // jump ahead 4 places
    return offset+4;
}

/**
 * Opcode  : 2
 * Operands: 3
 * 
 * Multiplies the first two operands and writes the result to the third
 */
int instr_multi(int offset, std::vector<int>& tape) {
    Instruction instruction = parse_instruction(tape[offset]);
    // 0 is address mode, 1 is immediate mode, grab values accordingly
    int left = (instruction.flags[0]) ? tape[offset+1] : tape[tape[offset+1]];
    int right = (instruction.flags[1]) ? tape[offset+2] : tape[tape[offset+2]];
    // locations are never in immediate mode
    int location = tape[offset+3];

    if(location > tape.size()) {
        std::cout << "Attempted write to out of bounds address " << location << std::endl;
        exit(-1);
    }

    tape[location] = left * right;

    // jump ahead 4 places
    return offset+4;
}

/**
 * Opcode  : 3
 * Operands: 1
 * 
 * Takes a user given number and writes it to the given location
 */
int instr_input(int offset, std::vector<int>& tape) {
    // locations are never in immedate mode
    int location = tape[offset+1];
    
    if(location > tape.size()) {
        std::cout << "Attempted write to out of bounds address " << location << std::endl;
        exit(-1);
    }

    // prompt for input
    std::cout << "? ";

    std::string buffer;
    std::getline(std::cin, buffer);

    int input_value = std::stoi(buffer);

    tape[location] = input_value;

    // jump ahead 3 places
    return offset+2;
}

/**
 * Opcode  : 4
 * Operands: 1
 * 
 * Outputs the given operand to the command ine
 */
int instr_output(int offset, std::vector<int>& tape) {
    Instruction instruction = parse_instruction(tape[offset]);
    // 0 is address mode, 1 is immediate mode, grab values accordingly
    int output = (instruction.flags[0]) ? tape[offset+1] : tape[tape[offset+1]];

    std::cout << output << std::endl;

    // jump ahead 3 places
    return offset+2;
}

/**
 * Opcode  : 5
 * Operands: 2
 * 
 * If the first operand is true, jump to the location given by the second
 */
int instr_jump_true(int offset, std::vector<int>& tape) {
    Instruction instruction = parse_instruction(tape[offset]);
    int test_value = (instruction.flags[0]) ? tape[offset+1] : tape[tape[offset+1]];
    // location in this instance is not an output but a jump location, so it can 
    // be in either immediate or address mode
    int location = (instruction.flags[1]) ? tape[offset+2] : tape[tape[offset+2]];

    if(location > tape.size() && test_value != 0) {
        std::cout << "Attempted to jump to out of bounds address " << location << std::endl;
        exit(-1);
    }

    // jump to given location if test value is non zero, otherwise ahead 3
    return (test_value != 0) ? location : offset + 3;

}

/**
 * Opcode  : 6
 * Operands: 2
 * 
 * If the first operand is false, jump to the location given by the second
 */
int instr_jump_false(int offset, std::vector<int>& tape) {
    Instruction instruction = parse_instruction(tape[offset]);
    int test_value = (instruction.flags[0]) ? tape[offset+1] : tape[tape[offset+1]];
    // location in this instance is not an output but a jump location, so it can 
    // be in either immediate or address mode
    int location = (instruction.flags[1]) ? tape[offset+2] : tape[tape[offset+2]];

    if(location > tape.size() && test_value == 0) {
        std::cout << "Attempted to jump to out of bounds address " << location << std::endl;
        exit(-1);
    }

    // jump to given location if test value is zero, otherwise ahead 3
    return (test_value == 0) ? location : offset + 3;
}

/**
 * Opcode  : 7
 * Operands: 3
 * 
 * If the first operand is less than the second one, write 1 to the location 
 * supplied by the third operand
 */
int instr_less_than(int offset, std::vector<int>& tape) {
    Instruction instruction = parse_instruction(tape[offset]);
    int left = (instruction.flags[0]) ? tape[offset+1] : tape[tape[offset+1]];
    int right = (instruction.flags[1]) ? tape[offset+2] : tape[tape[offset+2]];
    // locations are never in immedate mode
    int location = tape[offset+3];

    if(location > tape.size()) {
        std::cout << "Attempted write to out of bounds address " << location << std::endl;
        exit(-1);
    }

    tape[location] = (left < right);

    return offset + 4;
}

/**
 * Opcode  : 8
 * Operands: 2
 * 
 * If the first operand is equalto the second one, write 1 to the location 
 * supplied by the third operand
 */
int instr_equals(int offset, std::vector<int>& tape) {
    Instruction instruction = parse_instruction(tape[offset]);
    int left = (instruction.flags[0]) ? tape[offset+1] : tape[tape[offset+1]];
    int right = (instruction.flags[1]) ? tape[offset+2] : tape[tape[offset+2]];
    // locations are never in immedate mode
    int location = tape[offset+3];

    if(location > tape.size()) {
        std::cout << "Attempted write to out of bounds address " << location << std::endl;
        exit(-1);
    }

    tape[location] = (left == right);

    return offset + 4;
}

/** ##################### **/
/** END INSTRUCTION BLOCK **/
/** ##################### **/


/**
 * Run the program given by a vector of opcodes, the program is run in place and 
 * modifies the vector given
 * 
 * @param opcodes a vector of opcodes to work as program instructions
 */ 
void run_program(std::vector<int>& opcodes) {
    opcodefnptr operations[10] = {0};
    operations[1] = &instr_add;
    operations[2] = &instr_multi;
    operations[3] = &instr_input;
    operations[4] = &instr_output;
    operations[5] = &instr_jump_true;
    operations[6] = &instr_jump_false;
    operations[7] = &instr_less_than;
    operations[8] = &instr_equals;

    for(int i = 0; i < opcodes.size();) {
        Instruction current_instruction = parse_instruction(opcodes[i]);
        // std::cout << current_instruction.to_string() << std::endl;

        if(current_instruction.opcode == 99) return;
        
        opcodefnptr opcode_handler = operations[current_instruction.opcode];

        if(opcode_handler != 0) {
            i = (*opcode_handler)(i, opcodes);
        } else {
            std::cout << "Unknow opcode " << current_instruction.to_string() << std::endl;
            return;
        }
    }

}

#define INPUT_LOCATION "./input"

int main(int argc, char* argv[]) {
    std::vector<int> opcodes = get_opcodes_from_file(INPUT_LOCATION);

    run_program(opcodes);
}