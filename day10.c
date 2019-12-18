
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <float.h>
#include <stdbool.h>
#include <assert.h>

bool float_eq(float x, float y) {
   float diff = x - y;
   return (diff < FLT_EPSILON) && (-diff < FLT_EPSILON);
}

typedef struct {
        float x, y;
} v2;

size_t count_asteroids(const int x, const int y, const size_t line_count, char *lines[]) {
        size_t asteroid_count = 0;
        v2 *vectors = calloc(line_count * 256, sizeof(*vectors));
        for (int check_y = 0; check_y < line_count; ++check_y) {
                for (int check_x = 0; lines[check_y][check_x] != '\n'; ++check_x) {
                        if (lines[check_y][check_x] == '.' ||
                            (check_x == x && check_y == y))
                                continue;

                        assert(lines[check_y][check_x] == '#');

                        const float vx = check_x - x;
                        const float vy = check_y - y;

                        const float norm = sqrt(vx*vx + vy*vy);
                        const v2 v = {vx / norm, vy / norm};

                        v2 *v_check = vectors;
                        bool match = false;
                        while (v_check->x || v_check->y) {
                                if (float_eq(v_check->x, v.x) &&
                                    float_eq(v_check->y, v.y)) {
                                        match = true;
                                        break;
                                }
                                ++v_check;
                        }
                        if (!match) {
                                *v_check = v;
                                ++asteroid_count;
                        }
                }
        }
        free(vectors);
        return asteroid_count;
}

typedef struct {
        int x, y;
} iv2;

typedef struct {
        iv2 iv;
        v2 v;
        float norm, angle;
        bool vaporized;
} asteroid;

int dist_cmp(const void *a, const void *b) {
        if (float_eq(((asteroid *) a)->norm , ((asteroid *) b)->norm)) return 0;
        if (((asteroid *) a)->norm < ((asteroid *) b)->norm) return -1;
        else return 1;
}

int angle_cmp(const void *a, const void *b) {
        if (float_eq(((asteroid *) a)->angle , ((asteroid *) b)->angle)) return 0;
        if (((asteroid *) a)->angle > ((asteroid *) b)->angle) return -1;
        else return 1;
}

int main(void) {
        char *lines[64] = {0};

        size_t line_count = 0;
        size_t size = 0;
        while (getline(&lines[line_count], &size, stdin) != -1) {
                ++line_count;
                size = 0;
        }

        iv2 station;
        size_t largest_asteroid_count = 0;
        for (size_t y = 0; y < line_count; ++y) {
                for (size_t x = 0; lines[y][x] != '\n'; ++x) {
                        if (lines[y][x] != '#') continue;

                        const size_t asteroid_count = count_asteroids(x, y, line_count, lines);
                        if (asteroid_count > largest_asteroid_count) {
                                largest_asteroid_count = asteroid_count;
                                station.x = x;
                                station.y = y;
                        }
                }
        }

        printf("Part One: %zu\n", largest_asteroid_count);
        //printf("Station: (%d, %d)\n", station.x , station.y);

        asteroid *asteroids = calloc(line_count * 64, sizeof(*asteroids));
        size_t asteroid_count = 0;
        for (int y = 0; y < line_count; ++y) {
                for (int x = 0; lines[y][x] != '\n'; ++x) {
                        if (lines[y][x] != '#' ||
                            (y == station.y && x == station.x))
                                continue;

                        asteroid *const a = &asteroids[asteroid_count];

                        const float ivx = x - station.x;
                        const float ivy = y - station.y;

                        a->iv.x = x;
                        a->iv.y = y;

                        a->norm = sqrt(ivx*ivx + ivy*ivy);
                        a->v.x = ivx / a->norm;
                        a->v.y = ivy / a->norm;
                        a->angle = atan2(-a->v.y, a->v.x) - M_PI / 2;
                        if (a->angle < 0.00001) a->angle += 2 * M_PI;
                        // else if (a->angle > 2 * M_PI) a->angle -= 2 * M_PI;

                        ++asteroid_count;
                }
        }

        qsort(asteroids, asteroid_count, sizeof(*asteroids), dist_cmp);
        qsort(asteroids, asteroid_count, sizeof(*asteroids), angle_cmp);

        iv2 asteroid200;
        bool found = false;
        size_t vaporized_asteroids = 0;
        while (!found) {
                float *angles = calloc(line_count * 64, sizeof(*angles));
                size_t angle_count = 0;

                for (size_t i = 0; i < asteroid_count; ++i) {
                        if (asteroids[i].vaporized) continue;

                        bool match = false;
                        for (size_t j = 0; j < angle_count; ++j) {
                                if (float_eq(asteroids[i].angle, angles[j])) {
                                    match = true;
                                    break;
                                }
                        }

                        if (!match) {
                                asteroids[i].vaporized = true;
                                ++vaporized_asteroids;

                                if (vaporized_asteroids == 200) {
                                        found = true;
                                        asteroid200 = asteroids[i].iv;
                                        break;
                                }

                                angles[angle_count] = asteroids[i].angle;
                                ++angle_count;
                        }
                }
                free(angles);
        }

        printf("Part Two: %d\n", asteroid200.x * 100 + asteroid200.y);

        for (size_t i = 0; i < 64; ++i) {
                free(lines[i]);
        }
        free(asteroids);
}
