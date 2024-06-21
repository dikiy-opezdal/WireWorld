#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/call.h>
#include <shaders.h>

#define VERTEX_SHADER_PATH "src/shader.vert"
#define FRAGMENT_SHADER_PATH "src/shader.frag"

unsigned int SCR_WIDTH    = 800;
unsigned int SCR_HEIGHT   = 450;
unsigned int SCALE        = 400;

unsigned int shader_program;

void render(GLFWwindow *window) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}

void proccess_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
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

    glUniform1f(glGetUniformLocation(shader_program, "u_scale"), SCALE);
    glUniform2f(glGetUniformLocation(shader_program, "u_resolution"), (float)SCR_WIDTH, (float)SCR_HEIGHT);            
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    glUniform2f(glGetUniformLocation(shader_program, "u_resolution"), (float)width, (float)height);

    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

int init_window(GLFWwindow **window) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "WireWorld", NULL, NULL);
    if (*window == NULL) {
        printf("Failed to create GLFW window");
        return -1;
    }
    glfwMakeContextCurrent(*window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);

    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);

    return 1;
}

int main() {
    GLFWwindow *window = NULL;
    
    if (!init_window(&window)) {
        glfwTerminate();
        return -1;
    }

    shader_program = create_shader_program(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    glUseProgram(shader_program);

    init_buffers();

    glUniform2f(glGetUniformLocation(shader_program, "u_offset"), 0.0f, 0.0f);
    glUniform3f(glGetUniformLocation(shader_program, "u_color"), 1.0f, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        proccess_input(window);

        render(window);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}