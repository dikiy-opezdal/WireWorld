#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <logic.h>

#define byte unsigned char
#define uint unsigned int

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
    int cells[CHUNK_SIZE * CHUNK_SIZE];
    int update_queue[CHUNK_SIZE * CHUNK_SIZE]; // #TODO: dynamic array
    int queue_length; 
} chunk_t;

typedef struct
{
    chunk_t chunks[MAP_SIZE * MAP_SIZE]; // #TODO: dynamic array
    int update_queue[MAP_SIZE * MAP_SIZE]; // #TODO: dynamic array
    int queue_length;
} map_t;

map_t map;
map_t buffer;

byte count_neighbours(map_t *buffer, int chunk_id, int cell) { // #FIXME: chunk borders
    int neighbours = 0;
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (buffer->chunks[chunk_id].cells[cell + (y*CHUNK_SIZE + x)] == HEAD) {
                neighbours++;
            }
        }
    }

    return neighbours;
}

void chunk_draw(int chunk_id) {
    chunk_t chunk = map.chunks[chunk_id];
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++) {
        if (i % CHUNK_SIZE == 0) printf("\n");
        printf("%d", chunk.cells[i]);
    }
}

void cell_update(map_t *buffer, chunk_t *chunk, int chunk_id, byte cell) { // #FIXME: chunk borders
    switch (buffer->chunks[chunk_id].cells[cell]) {
        case HEAD:
            chunk->cells[cell] = TAIL;
            break;
        case TAIL:
            chunk->cells[cell] = COND;
            break;
        case COND:
            byte neighbours = count_neighbours(buffer, chunk_id, cell);
            if (neighbours == 1 || neighbours == 2) {
                chunk->cells[cell] = HEAD;
            }
            break;
        default:
            break;
    }
}

void map_update() {
    memcpy(&buffer, &map, sizeof(chunk_t));
    for (int i = 0; i < map.queue_length; i++) {
        chunk_t *chunk = &map.chunks[map.update_queue[i]];
        for (int j = 0; j < chunk->queue_length; j++) {
            cell_update(&buffer, chunk, map.update_queue[i], chunk->update_queue[j]);
        }
    }
}

int binarySearch(int *arr, int low, int high, int t) {
    if (low < high) {
        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (arr[mid] == t) return mid;
            if (arr[mid] < t) low = mid + 1;
            else high = mid - 1;
        }
    }

    return -1;
}

chunk_t *get_chunk(map_t *map) {

}

void set_cell(int x, int y, int type) {
    int chunk_id = (int)(y / MAP_SIZE) * MAP_SIZE + (int)(x / MAP_SIZE);
    chunk_t *chunk = &map.chunks[chunk_id];
    int cell_id = y % CHUNK_SIZE * CHUNK_SIZE + x % CHUNK_SIZE;
    int past_type = chunk->cells[cell_id];

    if (past_type == type) return;
    chunk->cells[cell_id] = type;

    if (past_type != EMPT && type != EMPT) return;
    else if (past_type != EMPT && type == EMPT) {
        int id = binarySearch(chunk->update_queue, 0, chunk->queue_length, cell_id);
        if (id < 0) return;
        chunk->queue_length--;
        chunk->update_queue[id] = chunk->update_queue[chunk->queue_length];

        if (chunk->queue_length < 1) {
            id = binarySearch(map.update_queue, 0, map.queue_length, chunk_id);
            if (id < 0) return;
            map.queue_length--;
            map.update_queue[id] = map.update_queue[map.queue_length];
        }
    }
    else {
        chunk->update_queue[chunk->queue_length] = cell_id;
        chunk->queue_length++;

        int id = binarySearch(map.update_queue, 0, map.queue_length, chunk_id);
        if (id >= 0) return;
        map.update_queue[map.queue_length] = chunk_id;
        map.queue_length++;
    }
}

void map_init() {
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