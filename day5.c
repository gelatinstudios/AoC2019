
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void exec(int Intcode[], const int input) {
        for (size_t i = 0; Intcode[i] != 99; ) {
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
                                Intcode[Intcode[i+1]] = input;
                                i += 2;
                                break;
                        case 4:
                                printf("%d ", param1);
                                i += 2;
                                break;
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
                        default:
                                printf("Invalid opcode %d\n", opcode);
                                return;
                }
        }
        putchar('\n');
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
        while(token) {
                Intcode[count] = atoi(token);
                ++count;
                token = strtok(0, ",");
        }
        free(str);

        int *IntcodeCopy = malloc(IntcodeBytes);

        memcpy(IntcodeCopy, Intcode, IntcodeBytes);

        puts("Part One:");
        exec(Intcode, 1);

        memcpy(Intcode, IntcodeCopy, IntcodeBytes);

        puts("Part Two:");
        exec(Intcode, 5);
        free(Intcode);
        free(IntcodeCopy);
}
