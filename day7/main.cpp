#include <vector>
#include <algorithm>
#include <array>

#include "intcode.hpp"

typedef std::pair<std::vector<int>, intcode::RunState> Amplifier;

/**
 * Returns an 5 length array of numbers between 0 and 4 inclusive, corresponding
 * to the given sequence number
 * 
 * @param sequence number for corresponding sequence
 * @returns array of numbers 
 */
std::array<unsigned int, 5> get_base_sequence(unsigned int sequence) {
    std::array<unsigned int, 5> digits;

    // effectively convert the number to base 4, and then take the number at 
    // each digit position
    for(int i = 0; i < 5; i++) {
        if(sequence <= 0) {
            digits[i] = 0;
            continue;
        }

        int temp = sequence % 5;
        sequence = (sequence - temp) / 5;
        digits[i] = temp;
    }

    return digits;
}

/**
 * Returns a reloop sequeunce of numbers, a 5 length array of numbers between
 * 5 and 9 inclusive
 * 
 * @param sequence number corresponding to sequence
 * @returns array of numbers
 */
std::array<unsigned int, 5> get_reloop_sequence(unsigned int sequence) {
    std::array<unsigned int, 5> sequence_nums = get_base_sequence(sequence);

    // add 5 to each number to bring into range 5-9
    for(int i = 0; i < 5; i++) sequence_nums[i] += 5;

    return sequence_nums;
}

/**
 * Returns if the given sequence is a valid sequence, aka, each number only 
 * appears once in the sequence
 * 
 * @param sequence a 5 length array of numbers between 0 and 4 inclusive
 * @returns true if a valid sequence
 */
bool is_valid_sequence(const std::array<unsigned int, 5>& sequence) {
    std::array<unsigned int, 5> test_sequence{0, 1, 2, 3, 4};

    // if the given sequence is a permutation of the above set of numbers, then
    // each number only appears a single time
    return std::is_permutation(sequence.begin(), sequence.end(), test_sequence.begin());
}

/**
 * Return if the given sequence is a valid reloop sequence, aka each number only 
 * appears once in the sequence
 *
 * @param sequence a 5 length array of numbers between 5 and 9 inclusive
 * @returns true if a valid reloop sequence
 */
bool is_valid_reloop_sequence(const std::array<unsigned int, 5>& sequence) {
    std::array<unsigned int, 5> test_sequence{5, 6, 7, 8, 9};

    // if the given sequence is a permutation of the above set of numbers, then
    // each number only appears a single time
    return std::is_permutation(sequence.begin(), sequence.end(), test_sequence.begin());
}

/**
 * Run the given amplifier with the given phase and input signal, and return
 * the modified amplifier with its new state
 * 
 * @param amplifier amplifier to run sequence on
 * @param phase_setting initialization phase setting
 * @param input_signal input signal from previous amplifier
 * @returns state modified amplifier
 */
Amplifier run_amplifier(Amplifier amplifier, int phase_setting, int input_signal) {
    std::vector<int> inputs{phase_setting, input_signal};

    intcode::RunState program_state = intcode::run_program(
        amplifier.first, // opcodes
        inputs, 
        amplifier.second // run state
    );

    // feed new run state into amp
    amplifier.second = program_state;

    return amplifier;
}

/**
 * Run the given amplifier for a reloop cycle, execute from previous position
 * with new input signal
 * 
 * @param amplifier amplifier to run reloop on
 * @param input_signal new input value to feed to amp
 * @returns state modified amplifier
 */
Amplifier run_reloop_amplifier(Amplifier amplifier, int input_signal) {
    intcode::RunState program_state = intcode::run_program(
        amplifier.first, // opcodes
        {input_signal}, 
        amplifier.second // run state
    );

    // feed new run state into amp
    amplifier.second = program_state;

    return amplifier;
}

/**
 * Initialize a set of aplifiers with a given phase sequence, this method runs 
 * in place and modifies the amplifies directly
 * 
 * @param amps a 5 length array of amplifiers
 * @param phases phase numbers to feed to corresponding amps
 * @return output from last amplifier
 */
int init_amplifiers(std::array<Amplifier, 5>& amps, std::array<unsigned int, 5> phases) {
    int previous_output = 0;

    for(int i = 0; i < amps.size(); i++) {
        Amplifier current_amp = run_amplifier(amps[i], phases[i], previous_output);
        previous_output = current_amp.second.output[0];
        amps[i] = current_amp;
    }

    return previous_output;
}

/**
 * Get the total reloop value of a set of amps on a given phase sequence
 * 
 * @param amps list of 5 amplifiers 
 * @param phases phase sequence to use for amplifier initialization
 * @returns final output value from last amplifier
 */
int get_reloop_value(std::array<Amplifier, 5> amps, std::array<unsigned int, 5> phases) {
    int previous_output = init_amplifiers(amps, phases);

    // exit when the last amplifier has an exit condition of program end, 
    // if the last amp returns input empty then it is signaling to loop again
    while(amps.back().second.interrupt_reason != intcode::PROGRAM_FINISH) {

        // reloop all amps with output of last run
        for(int i = 0; i < amps.size(); i++) {
            Amplifier current_amp = run_reloop_amplifier(amps[i], previous_output);
            previous_output = current_amp.second.output.back();
            amps[i] = current_amp;
        }

    }

    return previous_output;
}

/**
 * Find the maximum phase sequence and output the result to stdout
 * 
 * @param amplifiers a list of 5 amplifiers to run test on
 */
void do_max_sequence_test(const std::array<Amplifier, 5>& amplifiers) {
    std::array<unsigned int, 5> max_setting;
    int max_setting_output = 0;
    
    const int MAX_SEQUENCE = (5*5*5*5*5);
    for(int i = 0; i < MAX_SEQUENCE; i++) {
        // get current sequence, if invalid skip to next sequence
        std::array<unsigned int, 5> phase_settings = get_base_sequence(i);
        if(!is_valid_sequence(phase_settings)) continue;

        // feed amps into each other to get final output
        int previous_output = 0;
        for(int i = 0; i < amplifiers.size(); i++) {
            Amplifier current_amp = run_amplifier(amplifiers[i], phase_settings[i], previous_output);
            previous_output = current_amp.second.output[0];
        }

        // save largest output
        if(previous_output > max_setting_output) {
            max_setting_output = previous_output;
            max_setting = phase_settings;
        }
    }

    std::cout << "MAX SETTING OUTPUT : " << max_setting_output << std::endl;
    std::cout << "MAX SETTINGS : ";
    for(auto i : max_setting) std::cout << i << " ";
    std::cout << std::endl; 
}

/**
 * Find the maximum reloop sequence and output the result to stdout
 * 
 * @param amplifiers a list of 5 amplifiers to run test on
 */
void do_max_reloop_test(const std::array<Amplifier, 5> amplifiers) {
    std::array<unsigned int, 5> max_reloop_setting; 
    int max_reloop_output = 0;

    const int MAX_SEQUENCE = (5*5*5*5*5);
    for(int i = 0; i < MAX_SEQUENCE; i++) {
        std::array<unsigned int, 5> phase_settings = get_reloop_sequence(i);
        if(!is_valid_reloop_sequence(phase_settings)) continue;

        int current_reloop = get_reloop_value(amplifiers, phase_settings);

        if(current_reloop > max_reloop_output) {
            max_reloop_output = current_reloop;
            max_reloop_setting = phase_settings;
        }
    }

    std::cout << "MAX RELOOP OUTPUT : " << max_reloop_output << std::endl;
    std::cout << "MAX RELOOP : ";

    for(auto i : max_reloop_setting) std::cout << i << " ";

    std::cout << std::endl;
}

#define INPUT_LOCATION "./input"

int main(int argc, char** argv) {

    std::vector<int> opcodes = intcode::get_opcodes_from_file(INPUT_LOCATION);

    // 5 amps in circuit
    std::array<Amplifier, 5> amplifiers{
        Amplifier(opcodes, intcode::RunState()),
        Amplifier(opcodes, intcode::RunState()), 
        Amplifier(opcodes, intcode::RunState()), 
        Amplifier(opcodes, intcode::RunState()), 
        Amplifier(opcodes, intcode::RunState()), 
    };

    // part 1
    do_max_sequence_test(amplifiers);
    // part 2
    do_max_reloop_test(amplifiers);

    return 0;
}