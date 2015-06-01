#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "loader.h"
#include "camera.h"
#include "callbacks.h"

int main()
{
    GLenum glew_status; // Status of glewInit()
    GLFWwindow *window; // GLFW window
    GLuint vao, vbos[3]; // Vertex buffers
    GLuint prog, tex, u_trans, u_proj; // GLSL related variables

    Camera cam;
    Mesh model;
    int width, height;
    double mouse_x, mouse_y;

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        fputs("Failed to initialize GLFW", stderr);
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1920, 1080, "Hello World!", glfwGetPrimaryMonitor(), NULL);

    if (!window) {
        fputs("Failed to create GLFW window", stderr);
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, glfw_key_callback);

    glewExperimental = GL_TRUE;
    glew_status = glewInit();

    if (glew_status != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW: %s", glewGetErrorString(glew_status));
        exit(EXIT_FAILURE);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    prog = load_program("../res/vs.glsl", "../res/fs.glsl");
    tex = load_texture("../res/head.jpg");
    u_proj = glGetUniformLocation(prog, "m_proj");
    u_trans = glGetUniformLocation(prog, "m_trans");

    mesh_load(&model, "../res/head.obj");
    cam = camera_create(1.0f, 1280, 720, 0.1f, 100.0f);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(3, vbos);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, model.v_size, &model.v_data.data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, model.n_size, &model.n_data.data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, model.u_size, &model.u_data.data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(window)) {
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glBindVertexArray(vao);

        camera_update(&cam);
        camera_aspect(&cam, width, height);
        camera_rotation(&cam, mouse_y * 0.0005, mouse_x * 0.0005, 0.0f);
        camera_translation(&cam, 0, 0, -3.0f);
        glUniformMatrix4fv(u_proj, 1, GL_FALSE, &cam.m_pro[0][0]);
        glUniformMatrix4fv(u_trans, 1, GL_FALSE, &cam.m_mod[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, model.draw_count);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    exit(EXIT_SUCCESS);
}