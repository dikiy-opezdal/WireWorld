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
map_t buffer;

byte count_neighbours(chunk_t *chunk, byte cell) { // #FIXME: chunk borders
    byte neighbours = 0;

    for (int i = -4; i < 5; i++) {
        if (chunk->cells[cell + i] == HEAD) neighbours++;
    }

    return neighbours;
}

void chunk_draw(chunk_t chunk) {
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++) {
        if (i % CHUNK_SIZE == 0) printf("\n");
        printf("%d", chunk.cells[i]);
    }
}

void cell_update(chunk_t *chunk, byte cell) { // #FIXME: chunk borders
    switch (chunk->cells[cell]) {
    case HEAD:
        chunk->cells[cell] = TAIL;
        break;
    case TAIL:
        chunk->cells[cell] = COND;
        break;
    case COND:
        byte neighbours = count_neighbours(chunk, cell);
        printf("%d\n", neighbours);
        if (neighbours == 1 || neighbours == 2) {
            chunk->cells[cell] = HEAD;
        }
        break;
    }
}

void map_update() {
    memcpy(&buffer, &map, sizeof(map));
    for (int i = 0; i < buffer.queue_length; i++) {
        for (int j = 0; j < buffer.chunks[buffer.update_queue[i]].queue_length; j++) {
            cell_update(&map.chunks[buffer.update_queue[i]], buffer.chunks[buffer.update_queue[i]].update_queue[j]);
        }
    }
}

void set_cell(int x, int y, byte type) { // #FIXME: can be already in queue
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
    set_cell(5, 5, HEAD);
}

int main() {
    map_init();
    chunk_draw(map.chunks[0]);

    map_update();
    chunk_draw(map.chunks[0]);

    map_update();
    chunk_draw(map.chunks[0]);

    return 0;
}