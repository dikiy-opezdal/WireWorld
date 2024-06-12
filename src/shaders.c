#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shaders.h>

char *read_shader_source(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("Failed to open shader source %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *shader_source = (char*)malloc(file_size + 1);
    if (shader_source == NULL) {
        printf("Failed to allocate memory for shader source");
        free(shader_source);
        fclose(file);
        return NULL;
    }

    fread(shader_source, 1, file_size, file);
    shader_source[file_size] = '\0';

    fclose(file);
    return shader_source;
}

unsigned int create_shader(const char *shader_source, unsigned int shader_type) {
    unsigned int shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("%s", info_log);
    }

    return shader;
}

unsigned int create_shader_program(const char *vertex_shader_path, const char *fragment_shader_path) {
    char *vertex_shader_source = read_shader_source(vertex_shader_path);
    char *fragment_shader_source = read_shader_source(fragment_shader_path);

    unsigned int vertex_shader = create_shader(vertex_shader_source, GL_VERTEX_SHADER);
    unsigned int fragment_shader = create_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    int success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf("%s", info_log);
    }

    free(vertex_shader_source);
    free(fragment_shader_source);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}
