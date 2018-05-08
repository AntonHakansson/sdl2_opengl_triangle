#include <stdio.h>
#include <iostream>

#include <SDL2/SDL.h>
#define GLEW_STATIC
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

static const GLuint WIDTH = 512;
static const GLuint HEIGHT = 512;
static const GLchar* vertex_shader_source =
    "#version 330 core\n"
    "layout(location = 0) in vec2 in_position;\n "
    "void main() {\n"
    "    gl_Position = vec4(in_position, 0, 1);\n"
    "}\n";
static const GLchar* fragment_shader_source =
    "#version 330 core\n"
    "out vec4 color;\n"
    "void main() {\n"
    "    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";
static GLfloat vertices[] = {
     0.0,  0.8,
    -0.8, -0.8,
     0.8, -0.8,
};
#define DEBUG_OPENGL \
    { \
      GLenum error = glGetError(); \
      if(error) { \
        const GLubyte* error_string = gluErrorString(error); \
        printf("ERROR [opengl]: %s, %i, %s\n", __FILE__, __LINE__, error_string); \
      } \
    }

GLuint common_get_shader_program(
    const char *vertex_shader_source,
    const char *fragment_shader_source
) {
    GLchar *log = NULL;
    GLint log_length, success;
    GLuint fragment_shader, program, vertex_shader;

    /* Vertex shader */
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
    log = (GLchar*)malloc(log_length);
    if (log_length > 0) {
        glGetShaderInfoLog(vertex_shader, log_length, NULL, log);
        printf("vertex shader log:\n\n%s\n", log);
    }
    if (!success) {
        printf("vertex shader compile error\n");
        exit(EXIT_FAILURE);
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
        log = (GLchar*)realloc(log, log_length);
        glGetShaderInfoLog(fragment_shader, log_length, NULL, log);
        printf("fragment shader log:\n\n%s\n", log);
    }
    if (!success) {
        printf("fragment shader compile error\n");
        exit(EXIT_FAILURE);
    }

    /* Link shaders */
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
        log = (GLchar*)realloc(log, log_length);
        glGetProgramInfoLog(program, log_length, NULL, log);
        printf("shader link log:\n\n%s\n", log);
    }
    if (!success) {
        printf("shader link error");
        exit(EXIT_FAILURE);
    }

    /* Cleanup. */
    free(log);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}
void PrintSDL_GL_Attributes()
{
	int value = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION: " << value << std::endl;

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MINOR_VERSION: " << value << std::endl;
}
int main(void) {
    GLuint program, vbo, vao;
    SDL_Event event;
    SDL_GLContext gl_context;
    SDL_Window *window;

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
      std::cout << "failed to init sdl" << std::endl;
      exit(1); // or handle the error in a nicer way
    }

    // Use OpenGL core
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );

    window = SDL_CreateWindow(__FILE__, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    if (!window) {
      std::cout << "Error: [SDL] failed to open window" << std::endl;
      exit(1); // or handle the error in a nicer way
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
      std::cout << "Error: [GL] failed to get GL context from window" << std::endl;
      exit(1); // or handle the error in a nicer way
    }

    PrintSDL_GL_Attributes();

    // glewExperimental = true;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
      std::cout << "failed to init glew" << std::endl;
      exit(1); // or handle the error in a nicer way
    }

    std::cout << "OpenGL Version" << glGetString(GL_VERSION) << std::endl;

    /* Shader setup. */
    program = common_get_shader_program(vertex_shader_source, fragment_shader_source);

    /* Buffer setup. */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);

    /* Global draw state */
    glUseProgram(program);
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* Main loop. */
    while (1) {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);

        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;

        DEBUG_OPENGL
    }

    /* Cleanup. */
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
