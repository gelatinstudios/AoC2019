
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct object object;
struct object {
        char name[4];
        object *orbited_by[256];
};

object *com = 0;
size_t orbits = 0;

object *insert(object *tree, char name[4]) {
        object *ptr = tree;
        while (ptr->name[0] && strcmp(ptr->name, name)) ++ptr;
        strcpy(ptr->name, name);
        if (!strcmp(name, "COM")) com = ptr;
        return ptr;
}

void orbit(object *orbitee, object *orbiter) {
        size_t i = 0;
        while(orbitee->orbited_by[i]) {
                ++i;
                assert(i < 256);
        }
        orbitee->orbited_by[i] = orbiter;
}

void traverse(object *ptr, size_t level) {
        ++level;
        for (size_t i = 0; ptr->orbited_by[i]; ++i) {
                assert(i < 256);
                orbits += level;
                traverse(ptr->orbited_by[i], level);
        }
}

size_t find_santa(object *ptr, object *list[], size_t n, const char *str) {
        list[n] = ptr;

        if (!strcmp(ptr->name, str)) {
                return n;
        }

        ++n;
        for(size_t i = 0 ; ptr->orbited_by[i]; ++i) {
                const size_t count = find_santa(ptr->orbited_by[i], list, n, str);
                if (count) return count;
        }
}

int main(void) {
        char *str = 0;
        size_t size;
        getdelim(&str, &size, EOF, stdin);

        size_t line_count = 0;
        for (size_t i = 0; i < size; ++i) {
                if (str[i] == '\n') ++line_count;
        }

        object *tree = calloc(line_count, sizeof(*tree));

        char *line = strtok(str, "\n");
        while(line) {
                char orbitee[4] = {0};
                char orbiter[4] = {0};
                sscanf(line, "%c%c%c%*c%c%c%c",
                        &orbitee[0], &orbitee[1], &orbitee[2],
                        &orbiter[0], &orbiter[1], &orbiter[2]);

                object *orbitee_ptr = insert(tree, orbitee);
                object *orbiter_ptr = insert(tree, orbiter);
                orbit(orbitee_ptr, orbiter_ptr);

                line = strtok(0, "\n");
        }

        assert(com);
        traverse(com, 0);

        printf("Part One: %zu\n", orbits);

        object *san_list[1024];
        const size_t san_count = find_santa(com, san_list, -1, "SAN");
        object *you_list[1024];
        const size_t you_count = find_santa(com, you_list, -1, "YOU");

        size_t n = 0;
        while(!strcmp(san_list[n]->name, you_list[n]->name)) ++n;

        const size_t transfers = san_count + you_count - 2*n;
        printf("Part Two: %zu\n", transfers);

        free(str);
        free(tree);
}
