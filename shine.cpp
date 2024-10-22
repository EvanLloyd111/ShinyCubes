/*TO RUN EXECUTE WITH: g++ shine.cpp -o shine -lGL -lGLEW -lglfw -lfreetype -I/usr/include/freetype2
If libaries not found then:
MAC-----
brew install freetype
export LDFLAGS="-L/usr/local/opt/freetype/lib"
export CPPFLAGS="-I/usr/local/opt/freetype/include"

OTHERS----
sudo apt update
sudo apt install libfreetype6-dev
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Vertex Shader Source
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float shininess;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

// Cube vertices with normals (full cube)
float vertices[] = {
    // positions          // normals
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,         
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,         
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,        
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,         
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,         
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,         
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,         
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,         
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,         
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,         
    // Left face
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,         
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,         
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,         
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,         
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,         
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,         
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,         
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,         
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,         
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,         
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,         
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,         
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,         
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,         
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,         
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,         
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,         
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,         
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,         
     0.5f,  0.5f , 0.5f,  0.0f,  1.0f,  0.0f,         
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,         
     0.5f,  0.5f , 0.5f,  0.0f,  1.0f,  0.0f,         
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,         
    -0.5f,  0.5f , 0.5f,   0.0f,  1.0f,  0.0f          
};

// Function to compile shader
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        std::cerr << "Error compiling shader: " << infoLog << std::endl;
    }
    return id;
}

// Function to create shader program
unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Error linking program: " << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Specular Cubes", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Create VAO, VBO for the cube
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind and set cube VAO and VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Array of shininess values
    float shininessValues[8] = {2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f, 256.0f};

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Clear buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use cube shader program
        glUseProgram(shaderProgram);

        // Set up camera and projection
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -7.0f));
        glm::mat4 projection = glm::perspective(glm::radians(50.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        // Pass matrices to shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // Set view position
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 7.0f)));
        // Set light color and object color
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(0.8f, 0.3f, 0.3f)));

        // Draw cubes with different shininess values
        for (int i = 0; i < 8; i++) {
            // Calculate cube position
            float spacing = 1.2f;
            glm::vec3 position = glm::vec3((i % 4) * spacing - 1.8f, -(i / 4) * spacing + 0.6f, 0.0f);
            glm::mat4 cubeModel = glm::translate(model, position);
            // Rotate cube
            cubeModel = glm::rotate(cubeModel, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            // Set model matrix
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
            // Set shininess
            glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininessValues[i]);
            // Set individual light position
            glm::vec3 cubeLightPos = position + glm::vec3(-1.0f, 0.0f, 2.0f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(cubeLightPos));
            // Draw cube
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

