#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHUNK_SIZE 16
#define MAP_SIZE 16

#define SCR_H 32
#define SCR_W 128

#define TPS 1

typedef struct {
    int y, x;
} pos_t;

typedef struct {
    int type;
} cell_t;

typedef struct {
    cell_t cells[CHUNK_SIZE][CHUNK_SIZE];
    pos_t pos;
} chunk_t;

typedef struct {
    int len;
    chunk_t **chunks;
} queue_t;

typedef struct {
    chunk_t chunks[MAP_SIZE][MAP_SIZE];
    queue_t queue;
} map_t;

void init_map(map_t *map) {
    *map = (map_t){0};
    for (int cy = 0; cy < MAP_SIZE; cy++) {
        for (int cx = 0; cx < MAP_SIZE; cx++) {
            map->chunks[cy][cx].pos = (pos_t){cy, cx};
        }   
    }
}

void set_cell(map_t *map, pos_t pos, int type) {
    map->chunks[pos.y / CHUNK_SIZE][pos.x / CHUNK_SIZE].cells[pos.y % CHUNK_SIZE][pos.x % CHUNK_SIZE].type = type;
}

void load_map(map_t *map) {
    map->queue.chunks = calloc(4, sizeof(chunk_t));
    map->queue.len = 4;

    map->queue.chunks[0] = &map->chunks[0][0];
    map->queue.chunks[1] = &map->chunks[0][1];
    map->queue.chunks[2] = &map->chunks[1][0];
    map->queue.chunks[3] = &map->chunks[1][1];

    set_cell(map, (pos_t){13, 14}, 2);
    set_cell(map, (pos_t){13, 15}, 1);
    set_cell(map, (pos_t){13, 16}, 3);
    set_cell(map, (pos_t){13, 17}, 3);
    set_cell(map, (pos_t){14, 18}, 3);
    set_cell(map, (pos_t){15, 18}, 3);
    set_cell(map, (pos_t){16, 18}, 3);
    set_cell(map, (pos_t){17, 18}, 3);
    set_cell(map, (pos_t){18, 17}, 3);
    set_cell(map, (pos_t){18, 16}, 3);
    set_cell(map, (pos_t){18, 15}, 3);
    set_cell(map, (pos_t){18, 14}, 3);
    set_cell(map, (pos_t){17, 13}, 3);
    set_cell(map, (pos_t){16, 13}, 3);
    set_cell(map, (pos_t){15, 13}, 3);
    set_cell(map, (pos_t){14, 13}, 3);
}

int sign(int num) {
    return (num > 0) - (num < 0);
}

int count_nbs(map_t map, pos_t chunk, pos_t cell) {
    int nbs = 0;
    for (int y = cell.y-1; y < cell.y+2; y++) {
        for (int x = cell.x-1; x < cell.x+2; x++) {
            if (y == cell.y && x == cell.x) continue;
            pos_t pos = {(y+CHUNK_SIZE)%CHUNK_SIZE, (x+CHUNK_SIZE)%CHUNK_SIZE}; // real cell position
            if (map.chunks[chunk.y+sign(y-pos.y)][chunk.x+sign(x-pos.x)].cells[pos.y][pos.x].type == 1) {
                nbs++;
            }
        }
    }
    return nbs;
}

void update_cell(map_t map, pos_t chunk, pos_t cell_pos, cell_t *cell) {
    switch (cell->type) {
        case 1:
            cell->type = 2;
            break;
        case 2:
            cell->type = 3;
            break;
        case 3:
            int nbs = count_nbs(map, chunk, cell_pos);
            if (nbs == 1 || nbs == 2) cell->type = 1;
            break;
    }
}

void update_map(map_t *map) {
    chunk_t *buffer = calloc(map->queue.len, sizeof(chunk_t));
    for (int i = 0; i < map->queue.len; i++) {
        buffer[i] = *map->queue.chunks[i];
        for (int y = 0; y < CHUNK_SIZE; y++) {
           for (int x = 0; x < CHUNK_SIZE; x++) {
                update_cell(*map, buffer[i].pos, (pos_t){y, x}, &buffer[i].cells[y][x]);
            }   
        }
    }
    for (int i = 0; i < map->queue.len; i++) {
        *map->queue.chunks[i] = buffer[i];
    }
}

char type2char(int type) {
    switch (type) {
        case 0: return ' ';
        case 1: return '@';
        case 2: return '.';
        case 3: return 'O';
        default: return '?';
    }
}

void printm(map_t map, pos_t cam) {
    char buffer[SCR_H*SCR_W + 1];
    for (int y = 0; y < SCR_H; y++) {
        for (int x = 0; x < SCR_W; x++) {
            buffer[y*SCR_W + x] = type2char(map.chunks[(y+cam.y)/CHUNK_SIZE][(x+cam.x)/CHUNK_SIZE].cells[(y+cam.y)%CHUNK_SIZE][(x+cam.x)%CHUNK_SIZE].type);
        }
    }
    buffer[SCR_H*SCR_W] = '\0';

    printf("%s", buffer);
}

void sleep(int msec) {
    struct timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

int main() {
    map_t map;
    pos_t cam = {0};

    init_map(&map);
    load_map(&map);

    while (1) {
        printm(map, cam);
        update_map(&map);

        sleep(1000.0/TPS);
    }
    
    return 0;
}