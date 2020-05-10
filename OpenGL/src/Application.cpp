#include <GL/glew.h>
#include <GLFW/glfw3.h>
   
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <chrono>
#include <functional>


#define ASSERT(x)   if(!(x)) __debugbreak();
#define GLCall(x)   GLClearError();\
                    x;\
                    ASSERT(GLLogCall(#x, __FILE__, __LINE__))


class Timer {
public:
    Timer(std::function<void(std::chrono::time_point<std::chrono::steady_clock>& startTime, std::chrono::time_point<std::chrono::steady_clock>& endTime)> func) {
        this->func = func;
    
        startTime = std::chrono::high_resolution_clock::now();
    }

    ~Timer() {
        endTime = std::chrono::high_resolution_clock::now();
        func(startTime, endTime);
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    std::chrono::time_point<std::chrono::steady_clock> endTime;
    std::function<void(std::chrono::time_point<std::chrono::steady_clock>& startTime, std::chrono::time_point<std::chrono::steady_clock>& endTime)> func;
};


inline void PrintTimer(std::chrono::time_point<std::chrono::steady_clock>& startTime, std::chrono::time_point<std::chrono::steady_clock>& endTime){
    std::chrono::duration<float> duration;
    
    duration = endTime - startTime;
    float us = duration.count() * 1000 * 1000;

    std::cout << "Timer took " << us << "us\n";

}


static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);

}


static bool GLLogCall(const char* function, const char* file, unsigned int line) {

    while (GLenum error = glGetError()) {

        std::cout << "[OpenGL Error] (" << error << ") " << function << " " << file << ": " << line << '\n';
        return false;
    }
    return true;
}


struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};


static ShaderProgramSource ParseShader(const std::string& filepath) {

    enum class ShaderType {
        None = -1, VertexShader = 0, FragmentShader = 1
    };

    std::fstream stream(filepath);
    
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::None;

    while (getline(stream, line)) {
        //Check if line contains shader token
        if (line.find("#shader") != std::string::npos) {  //if line.find() is not a invaid position

            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VertexShader;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FragmentShader;
            }

        }
        else {
            ss[(int)type] << line << '\n';
        }

    }

    return { ss[0].str(), ss[1].str() };

}



static unsigned int CompileShader(unsigned int type, const char* source) {

    unsigned int id = glCreateShader(type);
    GLCall(glShaderSource(id, 1, &source, nullptr));
    GLCall(glCompileShader(id));

    //Error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE) {
        int length;
        //Gets length of log message which is to be allocated on memory later on
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        //Allocated memory on the stack
        char* message = (char*)alloca(length * sizeof(char));
        //Fills the message with the actual error message
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << "shader!\n";
        std::cout << message << "\n";
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}


static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {

    unsigned int program = glCreateProgram();
    
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader.c_str());  //or &vertexShader[0] == vertexShader.c_str();
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader.c_str());

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));
    
    GLCall(glDetachShader(program, vs)); //Unimplemented by TheCherno
    GLCall(glDetachShader(program, fs)); //Unimplemented by TheCherno

    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Activates VSync
    glfwSwapInterval(1);

    if (GLEW_OK != glewInit()) {
        return -1;

    }

    //VertexPositions
    float positions[12] = {
        -0.5f, -0.5f, //0
         0.5f, -0.5f, //1
         0.5f,  0.5f, //2
        -0.5f,  0.5f, //3
    };

    //Index buffer
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    //Vertex Array (Default provided with glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE))
    unsigned int vao; //Vertex-Array-Object
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));


    //Generate VertexBuffer
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    //Assigning indexBuffer to avoid copying some vertices
    unsigned int ibo; //Index-Buffer-Object
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    //Parsing shader from CPU to GPU
    ShaderProgramSource source = ParseShader("res/shader/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    //Accessing variable in shader from CPU
    GLCall(unsigned int uniformLocation = glGetUniformLocation(shader, "u_Color"));
    ASSERT(uniformLocation != -1);


    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


    float r = 0.0f;
    float rIncrement = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(uniformLocation, r, 0.3f, 0.8f, 1.0f));
        
        GLCall(glBindVertexArray(vao));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
        
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); //Count = amount of indices to draw //Buffer is already bound, because of that: nullptr

        if (r > 1.0f)
            rIncrement = -0.05f;
        else if (r < 0.0f)
            rIncrement = 0.05f;
        r += rIncrement;


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}