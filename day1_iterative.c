
//alternative iterative solution

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
        char str[30];
        uint64_t fuel_sum = 0;
        while(fgets(str, 30, stdin)) {
                int64_t fuel = atoi(str) / 3 - 2;
                while (fuel > 0) {
                        fuel_sum += fuel;
                        fuel = fuel / 3 - 2;
                }
        }

        printf("%lu\n", fuel_sum);
}
