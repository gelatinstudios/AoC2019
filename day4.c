
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

typedef struct {
        uint8_t n[6];
} digits;

static inline digits to_digits(size_t n) {
        digits d;
        size_t denom = 100000;
        for (uint8_t i = 0; i < 6; ++i) {
                d.n[i] = n / denom;
                n %= denom;
                denom /= 10;
        }
        return d;
}

int main(void) {
        size_t begin, end;
        scanf("%zu-%zu", &begin, &end);
        size_t count_one = 0;
        size_t count_two = 0;
        for (size_t i = begin; i < end; ++i) {
                const digits d = to_digits(i);
                bool repeat = false;
                bool repeat_two = false;
                bool incr = true;
                for (uint8_t j = 0; j < 5; ++j) {
                        if (d.n[j] > d.n[j+1]) {
                                incr = false;
                                break;
                        }
                        if (d.n[j] == d.n[j+1]) {
                                repeat = true;
                                if ((j == 4 && d.n[j] != d.n[j-1]) ||
                                    (j < 4 && d.n[j] != d.n[j+2] && d.n[j] != d.n[j-1]))
                                        repeat_two = true;
                        }
                }
                if (repeat && incr) ++count_one;
                if (repeat_two && incr) ++count_two;
        }

        printf("Part one: %zu\n", count_one);
        printf("Part two: %zu\n", count_two);
}
