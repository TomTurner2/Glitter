// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

float vertices[] = {
    // positions // colors
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
    0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f // top
};

unsigned int indices[] = {
    0, 1, 2, // first triangle
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class ShaderFile
{
public:
    static constexpr const char* VertexShader_6_3 = "VertexShader-6-3.vert";
    static constexpr const char* FragmentShader_6_3 = "FragmentShader-6-3.frag";

    static constexpr const char* VertexShader_6_4 = "VertexShader-6-4.vert";
    static constexpr const char* FragmentShader_6_4 = "FragmentShader-6-4.frag";
};


const std::string* LoadShader(const char* filePath)
{
    std::ifstream t(filePath);  // Only the file name is needed if in the same directory
    if (!t.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return nullptr;
    }

    std::stringstream buffer;
    buffer << t.rdbuf();
    return new std::string(buffer.str());
}


unsigned int CreateShader(const char* vertexSource, const char* fragmentSource)
{
    int success;
    char infoLog[512];

    // Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Shader Program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

int main(int argc, char* argv[])
{
    // Initialize GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr)
    {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    glViewport(0, 0, mWidth, mHeight);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Create VAO and VBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Load and Compile Shaders
    auto fragShader = LoadShader(ShaderFile::FragmentShader_6_4);
    auto vertShader = LoadShader(ShaderFile::VertexShader_6_4);

    // Check if shaders were loaded correctly
    if (!fragShader || !vertShader) 
    {
        return EXIT_FAILURE;
    }

    // Print the shader source code for debugging
    std::cout << "Vertex Shader Source:\n" << *vertShader << std::endl;
    std::cout << "Fragment Shader Source:\n" << *fragShader << std::endl;

    unsigned int shaderProgram = CreateShader(vertShader->c_str(), fragShader->c_str());

    // Clean up shader strings
    delete fragShader;
    fragShader = nullptr;

    delete vertShader;
    vertShader = nullptr;

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Rendering Loop
    while (!glfwWindowShouldClose(mWindow))
    {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(mWindow, true);
        }

        // Update colour
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

        if (vertexColorLocation != -1)
        {
            float timeValue = glfwGetTime();
            float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
            glUseProgram(shaderProgram);
            glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        }
        
        // Clear screen
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render with the shader program
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap buffers
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
