#include <GL/glew.h>
#include <GLFW/glfw3.h>
   
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <chrono>
#include <functional>

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
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    //Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        //Gets length of log message which is to be allocated on memory later on
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        //Allocated memory on the stack
        char* message = (char*)alloca(length * sizeof(char));
        //Fills the message with the actual error message
        glGetShaderInfoLog(id, length, &length, message);
        
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << "shader!\n";
        std::cout << message << "\n";
        glDeleteShader(id);
        return 0;
    }

    return id;
}


static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {

    Timer(std::function<void(std::chrono::time_point<std::chrono::steady_clock>& startTime, std::chrono::time_point<std::chrono::steady_clock>& endTime)>(PrintTimer));

    unsigned int program = glCreateProgram();
    
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader.c_str());  //or &vertexShader[0] == vertexShader.c_str();
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader.c_str());

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    
    glDetachShader(program, vs); //Unimplemented by TheCherno
    glDetachShader(program, fs); //Unimplemented by TheCherno

    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit()) {
        return -1;

    }

    float positions[12] = {
        -0.5f, -0.5f, //0
         0.5f, -0.5f, //1
         0.5f,  0.5f, //2
        -0.5f,  0.5f, //3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    //Generate VertexBuffer
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);


    unsigned int ibo; //Index-Buffer-Object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    ShaderProgramSource source = ParseShader("res/shader/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); //Count = amount of indices to draw //Buffer is already bound, because of that: nullptr

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}