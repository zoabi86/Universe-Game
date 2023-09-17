#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

struct WindowSizeT {
    int width;
    int height;
};

class Planet;

constexpr WindowSizeT WINDOW_DEFAULT_SIZE {640, 480};

class OpenGLRenderer {


public:
    OpenGLRenderer();

    ~OpenGLRenderer();

    void checkGLErrors();
    void render();

    void updateTransform();

    // callbacks:
    void handleFramebufferResize(int width, int height);

private:
    void initializeShaders();

    void setupBuffers();

    void calculateFPS();

private:
    GLFWwindow* _window;
        // Vertex data setup
    GLuint _VBO; //for index buffer object
    GLuint _VAO; //for index buffer object
    GLuint _IBO; //for index buffer object
    GLuint _shaderProgram;

    // Sphere points generated with python
    std::vector<GLfloat> _vertices;
    std::vector<GLuint> _indices;

//     std::vector<GLfloat> _vertices = {
//         0.000f,  1.000f,  0.000f,
//         0.894f,  0.447f,  0.000f,
//         0.276f,  0.447f,  0.851f,
//         -0.724f,  0.447f,  0.526f,
//         -0.724f,  0.447f, -0.526f,
//         0.276f,  0.447f, -0.851f,
//         0.724f, -0.447f,  0.526f,
//         -0.276f, -0.447f,  0.851f,
//         -0.894f, -0.447f,  0.000f,
//         -0.276f, -0.447f, -0.851f,
//         0.724f, -0.447f, -0.526f,
//         0.000f, -1.000f,  0.000f
//     };

//     std::vector<GLuint> _indices = {

//     0,  2,  1,
//     0,  3,  2,
//     0,  4,  3,
//     0,  5,  4,
//     0,  1,  5,

//     1,  2,  6,
//     2,  3,  7,
//     3,  4,  8,
//     4,  5,  9,
//     5,  1, 10,

//     2, 6, 7,
//     3, 7, 8,
//     4, 8, 9,
//     5, 9, 10,
//     6, 10, 11,

//     6,  7, 11,
//     7,  8, 11,
//     8,  9, 11,
//     9, 10, 11,
//     10,  6, 11
// };

    // std::vector<GLuint> _indices = {
    //     0,  1,  2,
    //     0,  2,  3,
    //     0,  3,  4,
    //     0,  4,  5,
    //     0,  5,  1,

    //     1,  6,  2,
    //     2,  7,  3,
    //     3,  8,  4,
    //     4,  9,  5,
    //     5, 10,  1,

    //     6, 11,  7,
    //     7, 11,  8,
    //     8, 11,  9,
    //     9, 11, 10,
    //     10, 11,  6
    // };
    
    // std::vector<GLuint> _indices = {
    //     0,  1,  2,
    //     0,  2,  3,
    //     0,  3,  4,
    //     0,  4,  5,
    //     0,  5,  1,
    //     1,  6,  2,
    //     2,  7,  3,
    //     3,  8,  4,
    //     4,  9,  5,
    //     5, 10,  1,
    //     6, 11,  7,
    //     7, 11,  8,
    //     8, 11,  9,
    //     9, 11, 10,
    //     10, 11,  6
    // };

    int _width, _height;

    // Transform matrix
    glm::mat4 _transform = glm::mat4(1.0f);
    glm::mat4 _projection = glm::mat4(1.0f);

    glm::vec3 _transformV3{0.0f, 0.0f, 0.0f};
    glm::vec3 _rotateV3{0.0f, 0.0f, 1.0f};
    glm::vec3 _scaleV3{0.3f, 0.3f, 0.3f};

    float _rotationAngle{0.0f};

    float _moveSpeed = 0.005f;
    float _translation = 0.0f;

    // Variables for FPS calculation
    double _previousTime;
    int _frameCount;
    double _fps; // Store the FPS value

    // Shaders source code: TBD - move into files
    // const char* _vertexShaderSource = R"(
    // #version 330 core

    // layout(location = 0) in vec3 inPosition;
    // uniform mat4 model;
    // uniform mat4 view;
    // uniform mat4 projection;

    // void main() {
    //     gl_Position = projection * view * model * vec4(inPosition, 1.0);
    // }

    // )";


    // const char* _fragmentShaderSource = R"(
    // #version 330 core

    // out vec4 color;

    // void main() {
    //     color = vec4(1.0, 0.0, 0.0, 1.0);  // Red color for testing
    // }

    // )";


    const char* _vertexShaderSource = R"(
    #version 330 core

    layout(location = 0) in vec3 inPosition;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 fragPos;  // Fragment position for lighting calculations

    void main() {
        fragPos = vec3(model * vec4(inPosition, 1.0));
        gl_Position = projection * view * model * vec4(inPosition, 1.0);
    }
    )";


    const char* _fragmentShaderSource = R"(
    #version 330 core

    out vec4 color;

    uniform vec3 lightPos;  // Position of the light source

    in vec3 fragPos;

    void main() {
        vec3 lightDir = normalize(lightPos - fragPos);
        float diff = max(dot(lightDir, normalize(-fragPos)), 0.0);
        vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);  // White light

        color = vec4(diffuse, 1.0);
    }

    )";
    const float dw = WINDOW_DEFAULT_SIZE.width / 2.0f;
    const float dh = WINDOW_DEFAULT_SIZE.height / 2.0f;

    std::unique_ptr<Planet> _planet;
};

