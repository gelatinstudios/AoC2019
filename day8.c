
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

int main(void) {
        uint8_t image[25*6];
        for (size_t i = 0; i < 25*6; ++i)
                image[i] = 2;

        size_t current_zeroes = 0;
        size_t min_zeroes = 25 * 6;
        size_t zero_count = 0;
        size_t one_count = 0;
        size_t two_count = 0;
        size_t result = 0;

        size_t i = 0;
        // oh yeah it's K & R time
        int c;
        while ((c = getchar()) != EOF && c != '\n') {
                const uint8_t digit = c - '0';
                if (!digit) ++zero_count;
                else if (digit == 1) ++one_count;
                else if (digit == 2) ++two_count;
                else {
                        puts("something went wrong");
                        return EXIT_FAILURE;
                }

                if (image[i] == 2) image[i] = digit;

                ++i;
                if (i == 25*6) {
                        if (zero_count < min_zeroes) {
                                min_zeroes = zero_count;
                                result = one_count * two_count;

                        }

                        i = 0;
                        zero_count = 0;
                        one_count = 0;
                        two_count = 0;
                }
        }

        printf("Part One: %zu\n", result);

        printf("Part Two:\n");
        for (size_t j = 0, z = 0; j < 6; ++j) {
                for (size_t k = 0; k < 25; ++k) {
                        if (image[z]) putchar('#');
                        else putchar(' ');
                        ++z;
                }
                putchar('\n');
        }
}
