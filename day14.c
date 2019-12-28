
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct reaction reaction;

typedef struct {
        size_t required;
        reaction *reac_ptr;
} ingredient;

struct reaction {
        size_t makes;
        char name[6];
        ingredient ingredients[16];
};

reaction *fuel;

reaction *add_or_get_reaction(reaction *reactions, const char *name) {
        reaction *r = reactions;
        while (r->name[0] && strcmp(name, r->name)) ++r;
        strcpy(r->name, name);
        if (!strcmp(name, "FUEL")) fuel = r;
        return r;
}

char *get_result_str(char *c) {
        while (*c != '=') ++c;
        *c = 0;
        return c + 3;
}

typedef struct {
        const char *names[256];
        size_t counts[256];
} leftovers;

size_t get_leftovers(leftovers *left, const char *name, const size_t need) {
        for (size_t i = 0; i < 256; ++i) {
                if (name == left->names[i]) {
                        if (need > left->counts[i]) {
                                const size_t n = left->counts[i];
                                left->counts[i] = 0;
                                return n;
                        } else {
                                left->counts[i] -= need;
                                return need;
                        }
                }
        }

        return 0;
}

void add_leftovers(leftovers *left, const char *name, size_t count) {
        size_t i = 0;
        while (left->counts[i] && name != left->names[i]) ++i;

        left->names[i] = name;
        left->counts[i] += count;
}

size_t do_reaction(reaction *reac, leftovers *left, size_t reac_times) {
        if (!strcmp(reac->name, "ORE")) return reac_times;

        size_t ore = 0;
        for (ingredient *ing = reac->ingredients; ing->required; ++ing) {
                const char *ing_name = ing->reac_ptr->name;

                size_t need = reac_times * ing->required;
                need -= get_leftovers(left, ing_name, need);
                if (!need) continue;

                const size_t can_make = ing->reac_ptr->makes;
                const size_t times = (need + can_make - 1) / can_make; // fast integer ceil
                const size_t making = times * can_make;
                const size_t leftover = making - need;

                if (leftover) add_leftovers(left, ing_name, leftover);

                ore += do_reaction(ing->reac_ptr, left, times);
        }

        return ore;
}

int main(void) {
        reaction reactions[256] = {0};

        char *line = 0;
        size_t length = 0;
        while (getline(&line, &length, stdin) != -1) {
                char *result_str = get_result_str(line);

                size_t makes;
                char name[6];
                sscanf(result_str, "%zu %s", &makes, name);
                reaction *reac = add_or_get_reaction(reactions, name);
                reac->makes = makes;

                ingredient *ing = reac->ingredients;
                char *token = strtok(line, ",");
                while (token) {
                        char ing_name[6];
                        sscanf(token, "%zu %s", &ing->required, ing_name);
                        ing->reac_ptr = add_or_get_reaction(reactions, ing_name);

                        ++ing;
                        token = strtok(0, ",");
                }
        }
        free(line);

        for (reaction *r = reactions; r->name[0]; ++r) {
                if (!strcmp(r->name, "ORE")) {
                        r->makes = 1;
                        break;
                }
        }

        leftovers left = {0};
        size_t ore = do_reaction(fuel, &left, 1);
        printf("Part One: %zu\n", ore);
        memset(&left, 0, sizeof left);


        // this is very, very, very slow :(
        size_t max_fuel = 0;
        ore = 0;
        while (ore < 1000000000000) {
                ore += do_reaction(fuel, &left, 1);
                ++max_fuel;
        }

        printf("Part Two: %zu\n", max_fuel-1);
}
