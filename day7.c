
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

bool stop_execution = false;

typedef struct {
        int output;
        size_t pc;
} program_state;

program_state exec(int Intcode[], const int input1, const int input2, const size_t pc) {
        bool input1_used = pc;
        for (size_t i = pc; !stop_execution; ) {
                const int instruction = Intcode[i];
                const int opcode = instruction % 100;

                const int param_mode_1 = instruction % 1000 / 100;
                const int param_mode_2 = instruction % 10000 / 1000;
                const int param_mode_3 = instruction / 10000;

                assert(!param_mode_3); // as per warning

                const int param1 = param_mode_1 ? Intcode[i+1] : Intcode[Intcode[i+1]];
                const int param2 = param_mode_2 ? Intcode[i+2] : Intcode[Intcode[i+2]];

                switch (opcode) {
                        case 1:
                                Intcode[Intcode[i+3]] = param1 + param2;
                                i += 4;
                                break;
                        case 2:
                                Intcode[Intcode[i+3]] = param1 * param2;
                                i += 4;
                                break;
                        case 3:
                                if (!input1_used) {
                                        Intcode[Intcode[i+1]] = input1;
                                        input1_used = true;
                                } else Intcode[Intcode[i+1]] = input2;

                                i += 2;
                                break;
                        case 4:
                                i += 2;
                                return (program_state){param1, i};
                        case 5:
                                if (param1) i = param2;
                                else i += 3;
                                break;
                        case 6:
                                if (!param1) i = param2;
                                else i += 3;
                                break;
                        case 7:
                                if (param1 < param2) Intcode[Intcode[i+3]] = 1;
                                else Intcode[Intcode[i+3]] = 0;
                                i += 4;
                                break;
                        case 8:
                                if (param1 == param2) Intcode[Intcode[i+3]] = 1;
                                else Intcode[Intcode[i+3]] = 0;
                                i += 4;
                                break;

                        case 99:
                                stop_execution = true;
                                break;
                        default:
                                printf("Invalid opcode %d\n", opcode);
                                exit(EXIT_FAILURE);
                }
        }
        return (program_state){input2, -1};
}

bool permute(int seq[5]) {
        int k = -1;
        for (int i = 0; i < 4; ++i) {
                if (seq[i] < seq[i+1] && i > k)
                        k = i;
        }
        if (k == -1) return true;

        int l = -1;
        for (int i = 0; i < 5; ++i) {
                if (seq[k] < seq[i] && i > l)
                        l = i;
        }
        assert(l > -1);

        const int tmp = seq[k];
        seq[k] = seq[l];
        seq[l] = tmp;

        for (size_t low = k+1, high = 4; low < high; ++low, --high) {
                const int temp = seq[low];
                seq[low] = seq[high];
                seq[high] = temp;
        }

        return false;
}

int main(void) {
        char *str = 0;
        size_t size;
        getdelim(&str, &size, EOF, stdin);

        const size_t IntcodeSize = size / 2;
        const size_t IntcodeBytes = IntcodeSize * sizeof(int);
        int *Intcode = calloc(IntcodeSize, sizeof(int));
        size_t count = 0;
        char *token = strtok(str, ",");
        while (token) {
                Intcode[count] = atoi(token);
                ++count;
                token = strtok(0, ",");
        }
        free(str);

        int *IntcodeCopy = malloc(IntcodeBytes);
        memcpy(IntcodeCopy, Intcode, IntcodeBytes);

        int sequence[] = {0, 1, 2, 3, 4};
        int largest = 0;
        bool quit = false;
        while (!quit) {
                program_state ps = {0};
                for (size_t i = 0; i < 5; ++i) {
                        ps = exec(Intcode, sequence[i], ps.output, 0);
                        memcpy(Intcode, IntcodeCopy, IntcodeBytes);
                }
                if (ps.output > largest)
                        largest = ps.output;

                quit = permute(sequence);
        }

        printf("Part One: %d\n", largest);

        int *IntcodeCopies[5] = {0};
        for (size_t i = 0; i < 5; ++i) {
                IntcodeCopies[i] = malloc(IntcodeBytes);
                memcpy(IntcodeCopies[i], IntcodeCopy, IntcodeBytes);
        }

        int sequence2[] = {5, 6, 7, 8, 9};
        largest = 0;
        quit = false;
        while (!quit) {
                stop_execution = false;
                program_state ps = {0};
                size_t pcs[5] = {0};
                while (!stop_execution) {
                        for (size_t i = 0; i < 5; ++i) {
                                ps = exec(IntcodeCopies[i], sequence2[i], ps.output, pcs[i]);
                                pcs[i] = ps.pc;
                        }
                }

                if (ps.output > largest)
                        largest = ps.output;

                for (size_t i = 0; i < 5; ++i) {
                        memcpy(IntcodeCopies[i], IntcodeCopy, IntcodeBytes);
                }

                quit = permute(sequence2);
        }

        printf("Part Two: %d\n", largest);

        for (size_t i = 0; i < 5; ++i) {
                free(IntcodeCopies[i]);
        }
        free(Intcode);
        free(IntcodeCopy);
}
