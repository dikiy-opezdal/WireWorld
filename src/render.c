#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/call.h>

#include <shaders.h>

#include <render.h>

#define VERTEX_SHADER_PATH "src/shader.vert"
#define FRAGMENT_SHADER_PATH "src/shader.frag"

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 450;
float SCALE = 0.0f;

float camera_pos[] = {0.0f, 0.0f};

GLFWwindow *window = NULL;

unsigned int shader_program;

int instd_len;
float *instance_data;

void terminate_renderer() {
    glfwTerminate();
    free(instance_data);
}

void proccess_input() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void render() {
    glfwPollEvents();
    proccess_input();

    glClear(GL_COLOR_BUFFER_BIT);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0, instd_len / 5);

    glfwSwapBuffers(window);
}

void add_instance(float x, float y, int type) {
    float r = 0.0f, g = 0.0f, b = 0.0f;
    switch (type) {
        case 1:
            b = 1.0f;
            break;
        case 2:
            r = 1.0f;
            break;
        case 3:
            r = 1.0f;
            g = 1.0f;
            break;
        default:
            break;
    }

    instance_data = realloc(instance_data, (instd_len + 5) * sizeof(float));

    instance_data[instd_len] = x;
    instance_data[instd_len+1] = y;
    instance_data[instd_len+2] = r;
    instance_data[instd_len+3] = g;
    instance_data[instd_len+4] = b;

    instd_len += 5;

    glBufferData(GL_ARRAY_BUFFER, instd_len * sizeof(float), instance_data, GL_DYNAMIC_DRAW);
}

void set_camera_pos(float x, float y) {
    camera_pos[0] = x;
    camera_pos[1] = y;

    glUniform2f(glGetUniformLocation(shader_program, "u_camera_pos"), x, y);
}

void set_scale(float val) {
    SCALE = val;
    glUniform1f(glGetUniformLocation(shader_program, "u_scale"), val);
}

void init_buffers() {
    char vertex_data[] = {
        0,  0,
        0, -2,
        2,  0,
        2, -2,
    };
    unsigned char index_data[] = {
        0, 2, 1,
        2, 3, 1, 
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_BYTE, GL_FALSE, 2 * sizeof(char), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, instd_len * sizeof(float), instance_data, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    glUniform1f(glGetUniformLocation(shader_program, "u_scale"), SCALE);
    glUniform2f(glGetUniformLocation(shader_program, "u_camera_pos"), camera_pos[0], camera_pos[1]);
    glUniform2f(glGetUniformLocation(shader_program, "u_resolution"), (float)SCR_WIDTH, (float)SCR_HEIGHT);  
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    glUniform2f(glGetUniformLocation(shader_program, "u_resolution"), (float)width, (float)height);

    SCR_WIDTH = (float)width;
    SCR_HEIGHT = (float)height;
}

int init_window() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "WireWorld", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window");
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return 1;
}

int init_renderer() {
    if (!init_window()) {
        return -1;
    }

    shader_program = create_shader_program(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    glUseProgram(shader_program);

    instd_len = 0;
    instance_data = (float*)calloc(0, sizeof(float));

    init_buffers();

    return 1;
}