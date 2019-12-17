#include "intcode.hpp"

namespace intcode {

/**
 * Given an intruction, parse it into an Instruction object, with flags and opcode
 * 
 * @param instruction integer instruction to parse
 * @returns Instruction object representing corresponding object
 * 
 */
Instruction parse_instruction(long instruction) {
    std::string opcode_string = std::to_string(instruction);

    // pad from with 0s if they are missing, as non present flags default to 0
    if(opcode_string.size() != 5) {
        opcode_string.insert(0, 5 - opcode_string.size(), '0');
    }

    // opcode is last two digits of number
    int opcode = std::stoi(opcode_string.substr(3, 2));

    // flags are in reverse order
    int flags[] = {
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
std::vector<long> get_opcodes_from_file(std::string file_location) {
    std::vector<long> opcodes;
    std::ifstream input_file(file_location);

    while(!input_file.eof()) {
        char buffer[1024];
        // opcodes are command delimited
        input_file.getline(buffer, 1024, ',');

        opcodes.push_back(
            std::stol(buffer)
        );
    }

    return opcodes;
}   

/**
 * Get the intended value of a flag from an instruction
 * 
 * @param instruction the instruction being called with assoicated parameter modes
 * @param offset instruction call offset
 * @param flag 0 indexed requested flag value
 * @param bundle instruction bundle with tape values
 * @returns intended value of flag
 */
long get_intended_value(Instruction instruction, int offset, int flag,
     InstructionBundle& bundle) {

    int flag_offset = flag + 1;
    long flag_value = bundle.tape[offset+flag_offset]; 

    long location, value;
    switch(instruction.flags[flag]) {
        // address mode
        default:
        case 0:
            location = flag_value; 

            // if the location is out of bounds then expand available memory
            if(location > bundle.tape.size()) {
                // location is 0 indexed and size is 1 indexed so add 1 to account
                // for offset
                bundle.expand_memory((location + 1) - bundle.tape.size());
            }

            value = bundle.tape[location];
            #ifdef DEBUG_STACK_TRACE
            std::cout << "ADDRESS " << location << " ACCESSED WITH VALUE " << value << std::endl;
            #endif // DEBUG_STACK_TRACE

            return value;
            break;
        // immediate mode
        case 1:
            return flag_value;
            break;
        // relative mode
        case 2:
            location = bundle.relative_base + flag_value; 

            // if the location is out of bounds then expand available memory
            if(location > bundle.tape.size()) {
                // location is 0 indexed and size is 1 indexed so add 1 to account
                // for offset
                bundle.expand_memory((location + 1) - bundle.tape.size());
            }

            value = bundle.tape[location];
            #ifdef DEBUG_STACK_TRACE
            std::cout << "RELATIVE ADDRESS " << location << " ACCESSED WITH VALUE " << value << std::endl;
            #endif // DEBUG_STACK_TRACE

            return bundle.tape[location];
            break;
    }
}   

/**
 * Get the write location of a given flag, accounts for the fact that write 
 * locations are never in immediate mode
 * 
 * @param instruction instruction with associated flags
 * @param offset instruction offset 
 * @param flag 0 indexed flag number
 * @param bundle instruction bundle containing tape values
 * @param allow_negative_addresses (default = False) if this is set to true then
 * no error will be thrown when a negative output address is requested 
 * @returns output location
 */
int get_write_location(Instruction instruction, int offset, int flag, 
    InstructionBundle& bundle, bool allow_negative_addresses) {

    int flag_face_value = bundle.tape[offset + (flag + 1)];
    int flag_mode = instruction.flags[flag];

    int location;
    switch(flag_mode) {
        // relative mode
        case 2:
            
            location = bundle.relative_base + flag_face_value;

            break;
        // address mode/immediate mode, immediate mode is not possible for output
        // locations so the reqest is defaulted to address instead
        case 0:
        case 1:
        default:

            location = flag_face_value;

            break;
    }

    // disallow negative addresses
    if(location < 0 && !allow_negative_addresses) {
        throw new std::runtime_error("Attempted write to out of bounds address " + location);
    }

    // expand memory if address outside of bounds is requested
    if(location > bundle.tape.size()) {
        int change = location - bundle.tape.size();
        bundle.expand_memory(change);
    }

    #ifdef DEBUG_STACK_TRACE
    std::cout << "LOCATION " << location << " REQUESTED" << std::endl;
    #endif // DEBUG_STACK_TRACE

    return location;
}

#ifdef DEBUG_INSTRUCTIONS
/**
 * Debug only function used to print the value and associated parameters of an 
 * instruction
 * 
 * @param instruction instruction with associated flags
 * @param name mnemonic device for instruction
 * @param offset instruction offset 
 * @param values number of parameters the instruction accepts
 * @param bundle instruction with assoicated tape values
 */
void print_instruction(Instruction instruction, std::string name, int offset, int values, InstructionBundle& bundle) {
    std::cout << "(" << instruction.opcode << ") " << name << " ";
    for(int i = 0; i < values; i++) {
        std::cout << "(" << instruction.flags[i] << ")" << bundle.tape[offset+(i+1)];

        if(i < values-1) std::cout << ", ";
    }
    std::cout << std::endl;
}
#endif // DEBUG_INSTRUCTIONS

/** ####################### **/
/** BEGIN INSTRUCTION BLOCK **/
/** ####################### **/

/**
 * Opcode  : 1
 * Operands: 3
 * 
 * ADD [012 LEFT OPERAND] [012 RIGHT OPERAND] [02 WRITE_LOCATION]
 * 
 * Adds the first two operands and writes the result to the third
 */
int instr_add(int offset, InstructionBundle& bundle) {
    Instruction instruction = parse_instruction(bundle.tape[offset]);
    long left = get_intended_value(instruction, offset, 0, bundle);
    long right = get_intended_value(instruction, offset, 1, bundle);
    // locations are never in immediate mode
    long location = get_write_location(instruction, offset, 2, bundle);

    #ifdef DEBUG_INSTRUCTIONS
    print_instruction(instruction, "ADD", offset, 3, bundle);
    #endif // DEBUG_STACK_TRACE

    bundle.tape[location] = left + right;

    return offset+4;
}

/**
 * Opcode  : 2
 * Operands: 3
 * 
 * MULT [012 LEFT OPERAND] [012 RIGHT OPERAND] [02 WRITE_LOCATION]
 * 
 * Multiplies the first two operands and writes the result to the third
 */
int instr_multi(int offset, InstructionBundle& bundle) {
    Instruction instruction = parse_instruction(bundle.tape[offset]);
    long left = get_intended_value(instruction, offset, 0, bundle);
    long right = get_intended_value(instruction, offset, 1, bundle);
    // locations are never in immediate mode
    long location = get_write_location(instruction, offset, 2, bundle);

    #ifdef DEBUG_INSTRUCTIONS
    print_instruction(instruction, "MULTI", offset, 3, bundle);
    #endif // DEBUG_STACK_TRACE

    bundle.tape[location] = left * right;

    return offset+4;
}

/**
 * Opcode  : 3
 * Operands: 1
 * 
 * INPUT [02 WRITE_LOCATION]
 * 
 * Takes a user given number and writes it to the given location
 */
int instr_input(int offset, InstructionBundle& bundle) {
    Instruction instruction = parse_instruction(bundle.tape[offset]);
    // locations are never in immedate mode
    long location = get_write_location(instruction, offset, 0, bundle);

    #ifdef DEBUG_INSTRUCTIONS
    print_instruction(instruction, "INPUT", offset, 1, bundle);
    #endif // DEBUG_STACK_TRACE
    
    if(bundle.input.size() < 1) {
        std::cout << "Attempted to read input but none was available" << std::endl;
        exit(-1);
    }

    long input_value = bundle.input.get();
    bundle.tape[location] = input_value;

    return offset+2;
}

/**
 * Opcode  : 4
 * Operands: 1
 * 
 * OUTPUT [012 INPUT_LOCATION]
 * 
 * Outputs the given operand to the command ine
 */
int instr_output(int offset, InstructionBundle& bundle) {
    Instruction instruction = parse_instruction(bundle.tape[offset]);
    long output_value = get_intended_value(instruction, offset, 0, bundle);

    #ifdef DEBUG_INSTRUCTIONS
    print_instruction(instruction, "OUTPUT", offset, 1, bundle);
    #endif // DEBUG_STACK_TRACE

    bundle.output.push_back(output_value);

    return offset+2;
}

/**
 * Opcode  : 5
 * Operands: 2
 * 
 * JUMP_TRUE [012 TEST_VALUE] [012 JUMP_LOCATION]
 * 
 * If the first operand is true, jump to the location given by the second
 */
int instr_jump_true(int offset, InstructionBundle& bundle) {
    Instruction instruction = parse_instruction(bundle.tape[offset]);
    long test_value = get_intended_value(instruction, offset, 0, bundle);
    // location in this instance is not an output but a jump location, so it can 
    // be in either immediate or address mode
    long location = get_intended_value(instruction, offset, 1, bundle);

    #ifdef DEBUG_INSTRUCTIONS
    print_instruction(instruction, "JMP_TRUE", offset, 2, bundle);
    #endif // DEBUG_STACK_TRACE

    if(location < 0 && test_value != 0) {
        throw new std::runtime_error("Attempted to jump to out of bounds address " + location);
    }

    #ifdef DEBUG_STACK_TRACE
    if(test_value != 0) std::cout << "JUMP PASSED, JUMPING TO " << location << std::endl;
    #endif // DEBUG_STACK_TRACE

    // jump to given location if test value is non zero, otherwise ahead 3
    return (test_value != 0) ? location : offset + 3;

}

/**
 * Opcode  : 6
 * Operands: 2
 * 
 * JUMP_FALSE [012 TEST_VALUE] [012 JUMP_LOCATION]
 * 
 * If the first operand is false, jump to the location given by the second
 */
int instr_jump_false(int offset, InstructionBundle& bundle) {
    Instruction instruction = parse_instruction(bundle.tape[offset]);
    long test_value = get_intended_value(instruction, offset, 0, bundle);
    // location in this instance is not an output but a jump location, so it can 
    // be in either immediate or address mode
    long location = get_intended_value(instruction, offset, 1, bundle);

    #ifdef DEBUG_INSTRUCTIONS
    print_instruction(instruction, "JMP_FALSE", offset, 2, bundle);
    #endif // DEBUG_STACK_TRACE

    if(location < 0 && test_value == 0) {
        throw new std::runtime_error("Attempted to jump to out of bounds address " + location);
    }

    #ifdef DEBUG_STACK_TRACE
    if(test_value == 0) std::cout << "JUMP PASSED, JUMPING TO " << location << std::endl;
    #endif // DEBUG_STACK_TRACE

    // jump to given location if test value is zero, otherwise ahead 3
    return (test_value == 0) ? location : offset + 3;
}

/**
 * Opcode  : 7
 * Operands: 3
 * 
 * LESS_THAN [012 LEFT_VALUE] [012 RIGHT_VALUE] [02 WRITE_LOCATION]
 * 
 * If the first operand is less than the second one, write 1 to the location 
 * supplied by the third operand
 */
int instr_less_than(int offset, InstructionBundle& bundle) {
    Instruction instruction = parse_instruction(bundle.tape[offset]);
    long left = get_intended_value(instruction, offset, 0, bundle);
    long right = get_intended_value(instruction, offset, 1, bundle);
    // locations are never in immedate mode
    long location = get_write_location(instruction, offset, 2, bundle);

    #ifdef DEBUG_INSTRUCTIONS
    print_instruction(instruction, "LESS_THAN", offset, 3, bundle);
    #endif // DEBUG_STACK_TRACE

    bundle.tape[location] = (left < right);

    return offset + 4;
}

/**
 * Opcode  : 8
 * Operands: 3
 * 
 * EQUALS [012 LEFT_VALUE] [012 RIGHT_VALUE] [02 OUTPUT_LOCATION]
 * 
 * If the first operand is equalto the second one, write 1 to the location 
 * supplied by the third operand
 */
int instr_equals(int offset, InstructionBundle& bundle) {
    Instruction instruction = parse_instruction(bundle.tape[offset]);
    long left = get_intended_value(instruction, offset, 0, bundle);
    long right = get_intended_value(instruction, offset, 1, bundle);
    // locations are never in immedate mode
    long location = get_write_location(instruction, offset, 2, bundle);

    #ifdef DEBUG_INSTRUCTIONS
    print_instruction(instruction, "EQUALS", offset, 3, bundle);
    #endif // DEBUG_STACK_TRACE

    bundle.tape[location] = (left == right);

    return offset + 4;
}

/**
 * Opcode: 9
 * Operands: 1
 * 
 * ADJUST_BASE [012 ADJUSTMENT_VALUE]
 * 
 * Adjusts the relative base for the program
 */
int instr_adjust_base(int offset, InstructionBundle& bundle) {
    Instruction instruction = parse_instruction(bundle.tape[offset]);
    long base = get_intended_value(instruction, offset, 0, bundle);

    #ifdef DEBUG_INSTRUCTIONS
    print_instruction(instruction, "ADJUST_BASE", offset, 1, bundle);
    #endif // DEBUG_STACK_TRACE

    bundle.adjust_relative_base(base);

    #ifdef DEBUG_STACK_TRACE
    std::cout << "\t" << "ABSOLUTE BASE CHANGED TO " << bundle.relative_base << std::endl;
    #endif // DEBUG_STACK_TRACE

    return offset + 2;
}

/** ##################### **/
/** END INSTRUCTION BLOCK **/
/** ##################### **/

/**
 * Run the program given by a vector of opcodes, the program is run in place and 
 * modifies the vector given
 * 
 * @param opcodes a vector of opcodes to work as program instructions
 * @param input a vector of integers to use as input
 * @param state a run state to resume running from, default argument starts from 
 *  beginning of program
 * @returns a run state holding the state of the program
 */ 
RunState run_program(std::vector<long>& opcodes, std::vector<long> input, RunState state) {
    opcodefn operations[16] = {0};
    operations[1] = &instr_add;
    operations[2] = &instr_multi;
    operations[3] = &instr_input;
    operations[4] = &instr_output;
    operations[5] = &instr_jump_true;
    operations[6] = &instr_jump_false;
    operations[7] = &instr_less_than;
    operations[8] = &instr_equals;
    operations[9] = &instr_adjust_base;

    // reverse input since we read from back, not front, in NumberStream
    std::reverse(input.begin(), input.end());

    NumberStream input_stream(input);

    std::vector<long> output = state.output;

    InstructionBundle bundle(0, opcodes, input_stream, output);

    for(int i = state.opcode_position; i < opcodes.size();) {
        Instruction current_instruction = parse_instruction(opcodes[i]);

        if(current_instruction.opcode == 99) return RunState(i, output, PROGRAM_FINISH);
        
        opcodefn opcode_handler = operations[current_instruction.opcode];

        if(opcode_handler != 0) {

            #ifdef DEBUG_STACK_TRACE
            std::map<int, std::string> ops = {
                {1, "ADD"},
                {2, "MULTI"},
                {3, "INPUT"},
                {4, "OUTPUT"},
                {5, "JUMP_TRUE"},
                {6, "JUMP_FALSE"},
                {7, "LESS_THAN"},
                {8, "EQUALS"},
                {9, "ADJUST_BASE"},
            };
            
            std::cout << current_instruction.to_string() << " (" << ops.at(current_instruction.opcode) << ")" << std::endl;
            #endif // DEBUG_STACK_TRACE

            // special case, input read on empty input stream returns broken state
            if(current_instruction.opcode == 3 && input_stream.size() < 1) {
                return RunState(i, output, INPUT_EMPTY);
            }

            i = (*opcode_handler)(i, bundle);

            #ifdef DEBUG_STACK_TRACE
            std::cout << std::endl;
            #endif // DEBUG_STACK_TRACE
        } else {
            std::cout << "Unknow opcode " << current_instruction.to_string() << std::endl;
            return RunState(i, output, UNKNOWN_OPCODE);
        }
    }

    return RunState(opcodes.size(), output, OUT_OF_INSTRUCTIONS);
}

}