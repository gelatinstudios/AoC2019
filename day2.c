
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reset(int Intcode[], int IntcodeCopy[], const size_t count) {
        for (size_t i = 0; i < count; ++i) {
                Intcode[i] = IntcodeCopy[i];
        }
}

void exec(int Intcode[], const int noun, const int verb) {
        Intcode[1] = noun;
        Intcode[2] = verb;

        for (size_t i = 0; Intcode[i] != 99; i += 4) {
                switch (Intcode[i]) {
                        case 1:
                                Intcode[Intcode[i+3]] = Intcode[Intcode[i+1]] + Intcode[Intcode[i+2]];
                                break;
                        case 2:
                                Intcode[Intcode[i+3]] = Intcode[Intcode[i+1]] * Intcode[Intcode[i+2]];
                                break;
                        default:
                                puts("something went wrong");
                                return;
                }
        }
}

int main(void) {
        char *str = 0;
        size_t size;
        getline(&str, &size, stdin);

        int Intcode[size / 2];
        size_t count = 0;
        char *token = strtok(str, ",");
        while(token) {
                Intcode[count] = atoi(token);
                ++count;
                token = strtok(0, ",");
        }
        free(str);

        //printf("%zu instructions parsed\n", count);

        int IntcodeCopy[count];
        for (size_t i = 0; i < count; ++i) {
                IntcodeCopy[i] = Intcode[i];
        }

        exec(Intcode, 12, 2);

        printf("First part:\t%d\n", Intcode[0]);

        reset(Intcode, IntcodeCopy, count);

        int noun = 0;
        int verb;
        while (noun <= 99) {
                verb = 0;
                while (verb <= 99) {
                        exec(Intcode, noun, verb);
                        if (Intcode[0] == 19690720) {
                                goto END;
                        }
                        reset(Intcode, IntcodeCopy, count);
                        ++verb;
                }
                ++noun;
        }
END:
        printf("Second part:\t%d\n", 100 * noun + verb);
}
