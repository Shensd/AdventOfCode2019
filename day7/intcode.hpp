#ifndef INTCODE_HPP
#define INTCODE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

namespace intcode {

    /**
     * Contains information about a given instruction
     */
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
     * Used to control input to program in the form of a universal int stream
     */
    class IntStream {
    public:
        std::vector<int> contents;

        IntStream(std::vector<int> contents) : contents(contents) {}
        int get(void) {
            int last = contents.back();
            contents.pop_back();
            return last;
        }
        void push(int value) {
            contents.push_back(value);
        }
        int size(void) {
            return contents.size();
        }
    };

    enum {
        PROGRAM_FINISH,
        PROGRAM_BEGIN,
        OUT_OF_INSTRUCTIONS,
        INPUT_EMPTY,
        UNKNOWN_OPCODE
    };

    /**
     * Used for resuming execution from a given previous state
     */
    class RunState {
    public:
        unsigned int opcode_position;
        std::vector<int> output;
        unsigned int interrupt_reason;

        RunState(unsigned int opcode_position, std::vector<int> output, unsigned int interrupt_reason) : 
            opcode_position(opcode_position), output(output), interrupt_reason(interrupt_reason) {}
        RunState() : opcode_position(0), output({}), interrupt_reason(PROGRAM_BEGIN) {}
    };

    Instruction parse_instruction(int instruction);
    std::vector<int> get_opcodes_from_file(std::string file_location);

    typedef int (*opcodefnptr)(int, std::vector<int>&, IntStream&, std::vector<int>&); 

    /* BEGIN INSTRUCTION FUNCTIONS */
    int instr_add(int offset, std::vector<int>& tape, IntStream& input, std::vector<int>& output);
    int instr_multi(int offset, std::vector<int>& tape, IntStream& input, std::vector<int>& output);
    int instr_input(int offset, std::vector<int>& tape, IntStream& input, std::vector<int>& output);
    int instr_output(int offset, std::vector<int>& tape, IntStream& input, std::vector<int>& output);
    int instr_jump_true(int offset, std::vector<int>& tape, IntStream& input, std::vector<int>& output);
    int instr_jump_false(int offset, std::vector<int>& tape, IntStream& input, std::vector<int>& output);
    int instr_less_than(int offset, std::vector<int>& tape, IntStream& input, std::vector<int>& output);
    int instr_equals(int offset, std::vector<int>& tape, IntStream& input, std::vector<int>& output);
    /* END INSTRUCTION FUNCTIONS */

    RunState run_program(std::vector<int>& opcodes, std::vector<int> input, RunState state = RunState());
}


#endif // !INTCODE_HPP