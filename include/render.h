#ifndef RENDER_H_
#define RENDER_H_

extern float SCALE;
extern float camera_pos[]; 
extern GLFWwindow *window;

int init_renderer();
void clear_instance_data();
void add_instance(float x, float y, int type);
void set_camera_pos(float x, float y);
void set_scale(float val);
void render();
void terminate_renderer();

#endif