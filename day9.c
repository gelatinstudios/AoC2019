
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <gmp.h>

typedef enum {POSITION, IMMEDIATE, RELATIVE} mode;

mpz_t *param(const mode m, const long relative_base, mpz_t Intcode[], const size_t i) {
        switch (m) {
                case POSITION:
                        return &Intcode[mpz_get_ui(Intcode[i])];
                case IMMEDIATE:
                        return &Intcode[i];
                case RELATIVE:
                        return &Intcode[relative_base + mpz_get_si(Intcode[i])];
                default:
                        puts("something went wrong");
                        exit(EXIT_FAILURE);
        }
}

void exec(mpz_t Intcode[], const long input) {
        long relative_base = 0;
        for (size_t i = 0; mpz_cmp_si(Intcode[i], 99); ) {
                const long instruction = mpz_get_ui(Intcode[i]);
                const long opcode = instruction % 100;

                const mode param_mode_1 = instruction % 1000 / 100;
                const mode param_mode_2 = instruction % 10000 / 1000;
                const mode param_mode_3 = instruction / 10000;

                mpz_t *const param1 = param(param_mode_1, relative_base, Intcode, i+1);
                mpz_t *const param2 = param(param_mode_2, relative_base, Intcode, i+2);
                mpz_t *const param3 = param(param_mode_3, relative_base, Intcode, i+3);

                switch (opcode) {
                        case 1:
                                mpz_add(*param3, *param1, *param2);
                                i += 4;
                                break;
                        case 2:
                                mpz_mul(*param3, *param1, *param2);
                                i += 4;
                                break;
                        case 3:
                                assert(param_mode_1 != IMMEDIATE);
                                mpz_set_si(*param1, input);
                                i += 2;
                                break;
                        case 4:
                                gmp_printf("%Zd ", *param1);
                                i += 2;
                                break;
                        case 5:
                                if (mpz_sgn(*param1)) i = mpz_get_si(*param2);
                                else i += 3;
                                break;
                        case 6:
                                if (!mpz_sgn(*param1)) i = mpz_get_si(*param2);
                                else i += 3;
                                break;
                        case 7:
                                assert(param_mode_3 != IMMEDIATE);
                                if (mpz_cmp(*param1, *param2) < 0)
                                        mpz_set_si(*param3, 1);
                                else
                                        mpz_set_si(*param3, 0);
                                i += 4;
                                break;
                        case 8:
                                assert(param_mode_3 != IMMEDIATE);
                                if (!mpz_cmp(*param1, *param2))
                                        mpz_set_si(*param3, 1);
                                else
                                        mpz_set_si(*param3, 0);
                                i += 4;
                                break;
                        case 9:
                                relative_base += mpz_get_si(*param1);
                                i += 2;
                                break;
                        default:
                                printf("Invalid opcode %ld\n", opcode);
                                return;
                }
        }
        putchar('\n');
}

int main(void) {
        char *str = 0;
        size_t size;
        getdelim(&str, &size, EOF, stdin);

        const size_t IntcodeSize = size * 100;
        const size_t IntcodeBytes = IntcodeSize * sizeof(mpz_t);
        mpz_t *Intcode = malloc(IntcodeBytes);
        for (size_t i = 0; i < IntcodeSize; ++i) {
                mpz_init_set_si(Intcode[i], 0);
        }
        size_t count = 0;
        char *token = strtok(str, ",");
        while(token) {
                mpz_set_si(Intcode[count], atol(token));
                ++count;
                token = strtok(0, ",");
        }
        free(str);

        mpz_t *IntcodeCopy = malloc(IntcodeBytes);

        memcpy(IntcodeCopy, Intcode, IntcodeBytes);

        printf("Part One: ");
        exec(Intcode, 1);

        printf("Part Two: ");
        exec(IntcodeCopy, 2);

        free(Intcode);
        free(IntcodeCopy);
}
