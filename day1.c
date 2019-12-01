
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint64_t calculate_fuel(const uint64_t mass) {
        const int64_t fuel = mass / 3 - 2;
        if (fuel <= 0) return 0;
        return fuel + calculate_fuel(fuel);
}

int main(void) {
        char str[30];
        uint64_t fuel_sum = 0;
        while(fgets(str, 30, stdin)) {
                fuel_sum += calculate_fuel(atol(str));
        }

        printf("%lu\n", fuel_sum);
}
