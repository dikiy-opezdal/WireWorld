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

void load_map(char *filepath, map_t *map) {
    FILE *file = fopen(filepath, "rb");

    unsigned short chcount;
    fread(&chcount, sizeof(short), 1, file);
    if (chcount > 0) {
        printf("chcount: %d\n", chcount);
        for (int i = 0; i < chcount; i++) {
            printf("chunk %d\n", i);
            chunk_t chunk = {0};

            short x, y;
            fread(&x, sizeof(short), 1, file);
            fread(&y, sizeof(short), 1, file);
            chunk.pos.x = (int)x;
            chunk.pos.y = (int)y;
            printf("\tpos: %d | %d\n", chunk.pos.x, chunk.pos.y);

            unsigned char ccount;
            fread(&ccount, sizeof(char), 1, file);
            printf("\tccount: %d\n", ccount);
            unsigned char pos, type;
            for (int i = 0; i < ccount; i++) {
                printf("\tcell %d\n", i);
                fread(&pos, sizeof(char), 1, file);
                fread(&type, sizeof(char), 1, file);
                chunk.cells[pos/16][pos%16].type = type;
                printf("\t\tpos | type: %d | %d\n", pos, chunk.cells[pos/16][pos%16].type);
            }
            ccount = 0;

            map->chunks[chunk.pos.y][chunk.pos.x] = chunk;
            map->queue.chunks = realloc(map->queue.chunks, sizeof(chunk_t **)*(map->queue.len+1));
            map->queue.chunks[map->queue.len] = &map->chunks[chunk.pos.y][chunk.pos.x];
            map->queue.len++;
        }
        printf("Loaded %s map\n", filepath);
    }
    else {
        printf("Failed to load %s map, loading default\n", filepath);
        set_cell(map, (pos_t){13, 14}, 2); set_cell(map, (pos_t){13, 15}, 1); set_cell(map, (pos_t){13, 16}, 3); set_cell(map, (pos_t){13, 17}, 3); set_cell(map, (pos_t){14, 18}, 3); set_cell(map, (pos_t){15, 18}, 3); set_cell(map, (pos_t){16, 18}, 3); set_cell(map, (pos_t){17, 18}, 3); set_cell(map, (pos_t){18, 17}, 3); set_cell(map, (pos_t){18, 16}, 3); set_cell(map, (pos_t){18, 15}, 3); set_cell(map, (pos_t){18, 14}, 3); set_cell(map, (pos_t){17, 13}, 3); set_cell(map, (pos_t){16, 13}, 3); set_cell(map, (pos_t){15, 13}, 3); set_cell(map, (pos_t){14, 13}, 3);
        map->queue.chunks = calloc(4, sizeof(chunk_t));
        map->queue.len = 4;

        map->queue.chunks[0] = &map->chunks[0][0];
        map->queue.chunks[1] = &map->chunks[0][1];
        map->queue.chunks[2] = &map->chunks[1][0];
        map->queue.chunks[3] = &map->chunks[1][1];
    }
    fclose(file);
}

void save_map(char *filepath, map_t *map) {
    FILE *file = fopen(filepath, "wb");
    unsigned short chcount = (short)map->queue.len;
    fwrite(&chcount, sizeof(short), 1, file);
    printf("chcount: %d\n", chcount);

    for (int i = 0; i < map->queue.len; i++) {
        chunk_t chunk = *map->queue.chunks[i];
        printf("chunk %d\n", i);

        short x = (short)chunk.pos.x, y = (short)chunk.pos.y;
        fwrite(&x, sizeof(short), 1, file);
        fwrite(&y, sizeof(short), 1, file);
        printf("\tpos: %d | %d\n", x, y);

        unsigned char ccount;
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                if (chunk.cells[y][x].type > 0 && chunk.cells[y][x].type < 4) ccount++;
            }
        }
        fwrite(&ccount, sizeof(char), 1, file);
        printf("\tccount: %d\n", ccount);
        ccount = 0;

        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                if (chunk.cells[y][x].type == 0) continue;
                printf("\tcell %d\n", y*CHUNK_SIZE+x);
                unsigned char pos = (unsigned char)(y*CHUNK_SIZE+x);
                fwrite(&pos, sizeof(char), 1, file);
                unsigned char type = (unsigned char)chunk.cells[y][x].type;
                fwrite(&type, sizeof(char), 1, file);
                printf("\t\tpos | type: %d | %d\n", pos, chunk.cells[y][x].type);
            }
        }
    }
    fclose(file);

    printf("Saved %s map\n", filepath);
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
    load_map("map.wwm", &map);
    //save_map("map.wwm", &map);

    while (1) {
        printm(map, cam);
        update_map(&map);

        sleep(1000.0/TPS);
    }
    
    return 0;
}