#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <render.h>
#include <logic.h>

int main() {
    if (!init_renderer()) {
        terminate_renderer();
        return -1;
    }

    set_scale(16.0f);
    set_camera_pos(1.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        add_instance(-1.0f, 0.0f, 3);
        add_instance(0.0f, 0.0f, 3);
        add_instance(1.0f, 0.0f, 3);
        add_instance(0.0f, 1.0f, 3);
        add_instance(0.0f, 2.0f, 2);

        render();
    }

    terminate_renderer();
    return 0;
}