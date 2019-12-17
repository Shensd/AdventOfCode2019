#ifndef INTCODE_HPP
#define INTCODE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <stdexcept>


/* Uncomment these lines for debugging output */

// Outputs what memory addresses are requested and if applicable their value
//#define DEBUG_STACK_TRACE

// Outputs instruction calls and their associated parameters
//#define DEBUG_INSTRUCTIONS

namespace intcode {

    /**
     * Contains information about a given instruction
     */
    class Instruction {
    public:
        // max argument amount is 3
        int flags[3];
        unsigned int opcode;

        Instruction(unsigned int opcode, int flags[3]) : opcode(opcode) {
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
    class NumberStream {
    public:
        std::vector<long> contents;

        NumberStream(std::vector<long> contents) : contents(contents) {}
        long get(void) {
            long last = contents.back();
            contents.pop_back();
            return last;
        }
        void push(long value) {
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
        std::vector<long> output;
        unsigned int interrupt_reason;

        RunState(unsigned int opcode_position, std::vector<long> output, unsigned int interrupt_reason) : 
            opcode_position(opcode_position), output(output), interrupt_reason(interrupt_reason) {}
        RunState() : opcode_position(0), output({}), interrupt_reason(PROGRAM_BEGIN) {}
    };

    class InstructionBundle {
    public:
        long relative_base;
        std::vector<long>& tape;
        NumberStream& input;
        std::vector<long>& output;

        InstructionBundle(
            long relative_base, std::vector<long>& tape, NumberStream& input, std::vector<long>& output) :
            relative_base(relative_base), tape(tape), input(input), output(output) {}

        /**
         * Adjust the relative base for relative address calls
         * 
         * @param base offset for current base
         */
        void adjust_relative_base(long base) {
            relative_base += base;
        }

        /**
         * Expand the working memory of the program by the given amount
         * 
         * @param amount to increase memory by
         */
        void expand_memory(int amount) {
            int memory_size = tape.size();

            if(amount <= 0) return;

            #ifdef DEBUG_STACK_TRACE
            std::cout << "MEMORY TO EXPAND BY " << amount << " ADDRESSES, IS " << memory_size << " WILL BE " << (memory_size + amount + 2) << std::endl;
            #endif // DEBUG_STACK_TRACE

            // add 2 for write safety
            tape.resize(memory_size + amount + 2, 0L);
        }
    };

    Instruction parse_instruction(long instruction);
    std::vector<long> get_opcodes_from_file(std::string file_location);

    typedef int (*opcodefn)(int, InstructionBundle&); 

    long get_intended_value(Instruction instruction, int offset, int flag, InstructionBundle& bundle);
    int get_write_location(Instruction instruction, int offset, int flag, InstructionBundle& bundle, bool allow_negative_addresses = false);
    void print_instruction(Instruction instruction, std::string name, int offset, int values, InstructionBundle& bundle);

    /* BEGIN INSTRUCTION FUNCTIONS */
    int instr_add(int offset, InstructionBundle& bundle);
    int instr_multi(int offset, InstructionBundle& bundle);
    int instr_input(int offset, InstructionBundle& bundle);
    int instr_output(int offset, InstructionBundle& bundle);
    int instr_jump_true(int offset, InstructionBundle& bundle);
    int instr_jump_false(int offset, InstructionBundle& bundle);
    int instr_less_than(int offset, InstructionBundle& bundle);
    int instr_equals(int offset, InstructionBundle& bundle);
    int instr_adjust_base(int offset, InstructionBundle& bundle);
    /* END INSTRUCTION FUNCTIONS */

    RunState run_program(std::vector<long>& opcodes, std::vector<long> input, RunState state = RunState());
}


#endif // !INTCODE_HPP