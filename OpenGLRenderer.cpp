#include "OpenGLRenderer.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Planet.h"

constexpr WindowSizeT WINDOW_MIN_SIZE {300, 300};
constexpr WindowSizeT WINDOW_MAX_SIZE {2024, 1080};

constexpr float BALL_MAX_BOUNCE_HEIGHT {100.0f};

OpenGLRenderer::OpenGLRenderer()
    :_planet {std::make_unique<Planet>()},
    _width {WINDOW_DEFAULT_SIZE.width},
    _height {WINDOW_DEFAULT_SIZE.height}
{
    // Initialization
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
    _window = glfwCreateWindow(_width, _height, "OpenGL Renderer", NULL, NULL);
    if (!_window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    glfwWindowHint(GLFW_SAMPLES, 4);  // Request 4x MSAA

    glDepthFunc(GL_LESS);

    glfwMakeContextCurrent(_window);

    glfwSetWindowSizeLimits(_window, WINDOW_MIN_SIZE.width, WINDOW_MIN_SIZE.height, WINDOW_MAX_SIZE.width, WINDOW_MAX_SIZE.height);


    // Set this object with window context so it can use linked callback
    glfwSetWindowUserPointer(_window, this);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set the background color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Set the initial viewport size
    glfwGetFramebufferSize(_window, &_width, &_height);
    glViewport(0.0f, 0.0f, _width, _height);

    // _projection = glm::ortho(0.0f, static_cast<float>(_width), 0.0f, static_cast<float>(_height), -1.0f, 1.0f);
    float fov = glm::radians(45.0f); // Field of view - 45 degrees is typical
    float aspect = static_cast<float>(_width) / static_cast<float>(_height); // Aspect ratio of the window
    float nearPlane = 0.1f; // Near clipping plane
    float farPlane = 100.0f; // Far clipping plane

    _projection = glm::perspective(fov, aspect, nearPlane, farPlane);


    // _transform = glm::translate(_transform, glm::vec3(100.0f, 400.0f, 0.0f));

    // Call the Python script to generate UV sphere data
   std::ignore = system("python3 ../python/uv_sphere_generator.py 64"); // Example resolution of 64

    // Load vertex and index data from generated files
    std::ifstream vertexFile("sphere_vertices.dat");
    std::ifstream indexFile("sphere_indices.dat");
    GLfloat vertexValue;
    GLuint indexValue;

    while (vertexFile >> vertexValue) {
        _vertices.push_back(vertexValue);
    }

    // std::cout << "_vertices: ";
    // for (const auto &val : _vertices) {
    //     std::cout << val << ' ';
    // }

    while (indexFile >> indexValue) {
        _indices.push_back(indexValue);
    }
    // std::cout << std::endl <<  "_indices: ";
    // for (const auto &val : _indices) {
    //     std::cout << val << ' ';
    // }
    std::cout << "Loaded " << _vertices.size() / 3 << " vertices and " << _indices.size() << " indices." << std::endl;


    initializeShaders();

    setupBuffers();
    checkGLErrors();

    // Set the resize callback
    glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* window, int width, int height) {
        OpenGLRenderer* renderer = static_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(window));
        renderer->handleFramebufferResize(width, height);
    });
}

OpenGLRenderer::~OpenGLRenderer()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glfwDestroyWindow(_window);
    glfwTerminate();
}

// Public methods:
///////////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::checkGLErrors()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
}

void OpenGLRenderer::render()
{
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glFrontFace(GL_CCW);  // This is the default
    glEnable(GL_MULTISAMPLE);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    // glShadeModel(GL_SMOOTH); 

    // glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST); 
    // glEnable(GL_POLYGON_SMOOTH);

    // glEnable( GL_ALPHA_TEST ); 

    // glClearColor(1.0f, 0.4f, 0.0f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // std::cout << "render begin" << std::endl;
    while (!glfwWindowShouldClose(_window)) 
    {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        checkGLErrors();

        // Use our shader program
        glUseProgram(_shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        
        // glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),  // Camera positioned 10 units away on the Z-axis
                             glm::vec3(0.0f, 0.0f, 0.0f),  // Camera looks at the origin
                             glm::vec3(0.0f, 1.0f, 0.0f)); // Up vector

        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), _width / static_cast<float>(_height), 0.1f, 100.0f);

        GLuint modelLoc = glGetUniformLocation(_shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(_shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(_shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(_projection));

        glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
        GLuint lightPosLoc = glGetUniformLocation(_shaderProgram, "lightPos");
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

        // _transformV3.y = screenYOffset;

        checkGLErrors();

        // updateTransform();

        // Set the translation uniform
        // glUniform3f(glGetUniformLocation(_shaderProgram, "translation"), _translation, 0.0f, 0.0f);

        // Bind VAO and draw the sphere
        glBindVertexArray(_VAO);
        // glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
        glDrawElements(GL_TRIANGLE_STRIP, _indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        checkGLErrors();
        calculateFPS();

        // std::cout << "fps: " << _fps << std::endl;

        glfwSwapBuffers(_window);
        glfwPollEvents();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
        checkGLErrors();
    }
    checkGLErrors();
}

void OpenGLRenderer::updateTransform()
{
    // Apply translation
    _transform = glm::translate(_transform, _transformV3);

    // Apply rotation
    _transform = glm::rotate(_transform, glm::radians(_rotationAngle), _rotateV3);

    // Apply scaling
    _transform = glm::scale(_transform, _scaleV3);
    _scaleV3 = {1.0f, 1.0f, 1.0f};
}

// Public Callbacks:
///////////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::handleFramebufferResize(int width, int height)
{
    _width = width;
    _height = height;

    // glViewport(0, 0, _width, _height); // (0,0) Top Left
    // Update dimenstions
    // glfwGetFramebufferSize(_window, &_width, &_height);
}

// Private methods:
///////////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::initializeShaders()
{
    // Shaders setup
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &_vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &_fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    _shaderProgram = glCreateProgram();
    glAttachShader(_shaderProgram, vertexShader);
    glAttachShader(_shaderProgram, fragmentShader);
    glLinkProgram(_shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    checkGLErrors();
}

void OpenGLRenderer::setupBuffers()
{
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_IBO); // Generate index buffer

    glBindVertexArray(_VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    if (!_VBO)
    {
        std::cerr << "Error binding _VBO" << std::endl;
    }
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
    if (!_IBO)
    {
        std::cerr << "Error binding _IBO" << std::endl;
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLRenderer::calculateFPS()
{
    double currentTime = glfwGetTime(); // Get current time
    _frameCount++; 

    double elapsedTime = currentTime - _previousTime;
    if (elapsedTime >= 1.0) { // If one second has passed
        _fps = static_cast<double>(_frameCount) / elapsedTime; // Calculate FPS
        _frameCount = 0; // Reset frame count
        _previousTime = currentTime; // Reset previous time
    }
}
