#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define CHUNK_SIZE 16
#define MAP_SIZE 16

#define TPS 1

#define SCR_H 32
#define SCR_W 128

typedef struct {
    int y, x;
} pos_t;

typedef struct {
    uint8_t type;
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
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            map->chunks[y][x].pos = (pos_t){y, x};
        }   
    }
}

void queue_add(queue_t *queue, chunk_t *chunk) {
    queue->len++;
    queue->chunks = realloc(queue->chunks, sizeof(chunk_t**) * queue->len);
    queue->chunks[queue->len-1] = chunk;
}

void load_map(char *filepath, map_t *map) {
    FILE *file = fopen(filepath, "rb");

    uint16_t chunk_count;
    fread(&chunk_count, sizeof(uint16_t), 1, file);
    for (int i = 0; i < chunk_count; i++) {
        chunk_t chunk = {0};

        int16_t x, y;
        fread(&x, sizeof(int16_t), 1, file);
        fread(&y, sizeof(int16_t), 1, file);
        chunk.pos = (pos_t){y, x};

        uint8_t cell_count, pos, type;
        fread(&cell_count, sizeof(uint8_t), 1, file);
        for (int i = 0; i < cell_count; i++) {
            fread(&pos, sizeof(uint8_t), 1, file);
            fread(&type, sizeof(uint8_t), 1, file);
            chunk.cells[pos / CHUNK_SIZE][pos % CHUNK_SIZE].type = type;
        }

        map->chunks[chunk.pos.y][chunk.pos.x] = chunk;
        queue_add(&map->queue, &map->chunks[chunk.pos.y][chunk.pos.x]);
    }
    fclose(file);
}

void save_map(char *filepath, map_t *map) {
    FILE *file = fopen(filepath, "wb");

    uint16_t chunk_count = (uint16_t)map->queue.len;
    fwrite(&chunk_count, sizeof(uint16_t), 1, file);
    for (int i = 0; i < chunk_count; i++) {
        chunk_t chunk = *map->queue.chunks[i];

        int16_t x = (int16_t)chunk.pos.x;
        int16_t y = (int16_t)chunk.pos.y;
        fwrite(&x, sizeof(int16_t), 1, file);
        fwrite(&y, sizeof(int16_t), 1, file);

        uint8_t cell_count;
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                if (chunk.cells[y][x].type > 0 && chunk.cells[y][x].type < 4) {
                    cell_count++;
                }
            }
        }
        fwrite(&cell_count, sizeof(uint8_t), 1, file);

        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                if (chunk.cells[y][x].type == 0) continue;
                uint8_t pos = (uint8_t)(y*CHUNK_SIZE+x);
                uint8_t type = (uint8_t)chunk.cells[y][x].type;
                fwrite(&pos, sizeof(uint8_t), 1, file);
                fwrite(&type, sizeof(uint8_t), 1, file);
            }
        }
    }
    fclose(file);
}

int sign(int num) {
    return (num > 0) - (num < 0);
}

int count_nbs(map_t map, pos_t chunk, pos_t cell) {
    int nbs = 0;
    for (int y = cell.y-1; y < cell.y+2; y++) {
        for (int x = cell.x-1; x < cell.x+2; x++) {
            if (y == cell.y && x == cell.x) continue;
            pos_t pos = {(y+CHUNK_SIZE)%CHUNK_SIZE, (x+CHUNK_SIZE)%CHUNK_SIZE};
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
                if (buffer[i].cells[y][x].type == 0) continue;
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
    load_map("map.wwm", &map);

    while (1) {
        printm(map, cam);
        update_map(&map);

        sleep(1000.0/TPS);
    }
    
    return 0;
}