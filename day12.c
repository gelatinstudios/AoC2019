
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef struct {
        int x, y, z;
} iv3;

void gravity(int *a, int *b, const int a_pos, const int b_pos) {
        const int n = (a_pos > b_pos) - (a_pos < b_pos);
        *a -= n;
        *b += n;
}

size_t axis_sim(int pos[], const int orig_pos[]) {
        int vel[4] = {0};
        size_t step = 0;
        bool match = false;
        while (!match) {
                // gravity
                for (size_t i = 0; i < 3; ++i) {
                        for (size_t j = i+1; j < 4; ++j) {
                                gravity(&vel[i], &vel[j], pos[i], pos[j]);
                        }
                }

                //velocity
                for (size_t i = 0; i < 4; ++i) {
                        pos[i] += vel[i];
                }

                match = true;
                for (size_t i = 0; i < 4; ++i) {
                        if (pos[i] != orig_pos[i]) {
                                match = false;
                                break;
                        }
                }
                ++step;
        }
        return step+1;
}

size_t gcd(size_t a, size_t b) {
        if (a < b) {
                const size_t tmp = a;
                a = b;
                b = tmp;
        }

        size_t rem;
        while (rem = a % b) {
                a = b;
                b = rem;
        }

        return b;
}

size_t lcm(size_t a, size_t b) {
        return a * b / gcd(a, b);
}

int main(void) {
        iv3 moon_pos[4];
        iv3 moon_vel[4] = {0};

        char line[32];
        for (size_t i = 0; fgets(line, 32, stdin); ++i) {
                sscanf(line, "%*3c%d%*4c%d%*4c%d", &moon_pos[i].x, &moon_pos[i].y, &moon_pos[i].z);
        }

        const iv3 orig_pos[4] = {moon_pos[0], moon_pos[1], moon_pos[2], moon_pos[3]};

        for (size_t step = 0; step < 1000; ++step) {
                // gravity
                for (size_t i = 0; i < 3; ++i) {
                        for (size_t j = i+1; j < 4; ++j) {
                                gravity(&moon_vel[i].x, &moon_vel[j].x, moon_pos[i].x, moon_pos[j].x);
                                gravity(&moon_vel[i].y, &moon_vel[j].y, moon_pos[i].y, moon_pos[j].y);
                                gravity(&moon_vel[i].z, &moon_vel[j].z, moon_pos[i].z, moon_pos[j].z);
                        }
                }

                //velocity
                for (size_t i = 0; i < 4; ++i) {
                        moon_pos[i].x += moon_vel[i].x;
                        moon_pos[i].y += moon_vel[i].y;
                        moon_pos[i].z += moon_vel[i].z;
                }
        }

        unsigned sum_total = 0;
        for (size_t i = 0; i < 4; ++i) {
                const unsigned potential_energy = abs(moon_pos[i].x) + abs(moon_pos[i].y) + abs(moon_pos[i].z);
                const unsigned kinetic_energy = abs(moon_vel[i].x) + abs(moon_vel[i].y) + abs(moon_vel[i].z);
                sum_total += potential_energy * kinetic_energy;
        }

        printf("Part One: %u\n", sum_total);

        for (size_t i = 0; i < 4; ++i) {
                moon_pos[i] = orig_pos[i];
                moon_vel[i] = (iv3){0, 0, 0};
        }

        int x_pos[] = {moon_pos[0].x, moon_pos[1].x, moon_pos[2].x, moon_pos[3].x};
        int y_pos[] = {moon_pos[0].y, moon_pos[1].y, moon_pos[2].y, moon_pos[3].y};
        int z_pos[] = {moon_pos[0].z, moon_pos[1].z, moon_pos[2].z, moon_pos[3].z};

        int orig_x_pos[] = {orig_pos[0].x, orig_pos[1].x, orig_pos[2].x, orig_pos[3].x};
        int orig_y_pos[] = {orig_pos[0].y, orig_pos[1].y, orig_pos[2].y, orig_pos[3].y};
        int orig_z_pos[] = {orig_pos[0].z, orig_pos[1].z, orig_pos[2].z, orig_pos[3].z};

        size_t x_cycle = axis_sim(x_pos, orig_x_pos);
        size_t y_cycle = axis_sim(y_pos, orig_y_pos);
        size_t z_cycle = axis_sim(z_pos, orig_z_pos);

        const size_t n = lcm(x_cycle, y_cycle);
        const size_t cycle = lcm(n, z_cycle);

        printf("Part Two: %zu\n", cycle);
}
