#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define byte unsigned char

#define CHUNK_SIZE 16
#define MAP_SIZE 128

enum CELLS {
    EMPT,
    HEAD,
    TAIL,
    COND,
};

typedef struct
{
    byte cells[CHUNK_SIZE * CHUNK_SIZE];
    unsigned int update_queue[CHUNK_SIZE * CHUNK_SIZE]; // #TODO: dynamic array
    unsigned int queue_length; 
} chunk_t;

typedef struct
{
    chunk_t chunks[MAP_SIZE * MAP_SIZE]; // #TODO: dynamic array
    unsigned int update_queue[MAP_SIZE * MAP_SIZE]; // #TODO: dynamic array
    unsigned int queue_length;
} map_t;

map_t map;
chunk_t buffer;

byte count_neighbours(chunk_t *chunk, byte cell) { // #FIXME: chunk borders
    byte neighbours = 0;
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (chunk->cells[cell + (y*CHUNK_SIZE + x)] == HEAD) {
                neighbours++;
            }
        }
    }

    return neighbours;
}

void chunk_draw(chunk_t chunk) {
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++) {
        if (i % CHUNK_SIZE == 0) printf("\n");
        printf("%d", chunk.cells[i]);
    }
}

void cell_update(chunk_t *buffer_chunk, chunk_t *chunk, byte cell) { // #FIXME: chunk borders
    switch (buffer_chunk->cells[cell]) {
        case HEAD:
            chunk->cells[cell] = TAIL;
            break;
        case TAIL:
            chunk->cells[cell] = COND;
            break;
        case COND:
            byte neighbours = count_neighbours(buffer_chunk, cell);
            if (neighbours == 1 || neighbours == 2) {
                chunk->cells[cell] = HEAD;
            }
            break;
        default:
            chunk->cells[cell] = EMPT;
    }
}

void map_update() {
    for (int i = 0; i < map.queue_length; i++) {
        memcpy(&buffer, &map.chunks[map.update_queue[i]], sizeof(chunk_t));
        for (int j = 0; j < buffer.queue_length; j++) {
            cell_update(&buffer, &map.chunks[map.update_queue[i]], buffer.update_queue[j]);
        }
    }
}

void set_cell(int x, int y, byte type) { // #FIXME: can already be in queue
    int chunk_id = (int)(y / MAP_SIZE) * MAP_SIZE + (int)(x / MAP_SIZE);
    int cell_id = y % CHUNK_SIZE * CHUNK_SIZE + x % CHUNK_SIZE;

    map.chunks[chunk_id].cells[cell_id] = type;
    if (type != EMPT) {
        map.chunks[chunk_id].update_queue[map.chunks[chunk_id].queue_length] = cell_id;
        map.chunks[chunk_id].queue_length++;
    }
}

void map_init() {
    map.update_queue[map.queue_length] = 0;
    map.queue_length++;
    
    set_cell(8, 8, HEAD);

    set_cell(5, 8, COND);
    set_cell(6, 8, COND);
    set_cell(7, 8, COND);

    set_cell(9, 8, COND);
    set_cell(10, 8, COND);
    set_cell(11, 8, COND);

    set_cell(8, 5, COND);
    set_cell(8, 6, COND);
    set_cell(8, 7, COND);

    set_cell(8, 9, COND);
    set_cell(8, 10, COND);
    set_cell(8, 11, COND);
}

int main() {
    map_init();
    chunk_draw(map.chunks[0]);
    printf("\n");

    map_update();
    chunk_draw(map.chunks[0]);
    printf("\n");

    map_update();
    chunk_draw(map.chunks[0]);
    printf("\n");

    return 0;
}