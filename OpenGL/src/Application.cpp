#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "Timer.h"

#include <GLFW/glfw3.h>
   
#include <string>
#include <fstream>
#include <sstream>


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

    std::cout << glGetString(GL_VERSION) << '\n';

    {   //In its seperate scope to garantee that vb and ib are deconstructed before glfwTerminate();
        //VertexPositions
        float positions[] = {
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
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

        //Assigning indexBuffer to avoid copying some vertices
        IndexBuffer ib(indices, 6);


        //Parsing shader from CPU to GPU
        ShaderProgramSource source = ParseShader("res/shader/Basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));

        //Accessing variable in shader from CPU
        GLCall(unsigned int uniformLocation = glGetUniformLocation(shader, "u_Color"));
        ASSERT(uniformLocation != -1);
        GLCall(glUniform4f(uniformLocation, 0.8f, 0.3f, 0.8f, 1.0f));


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
            ib.Bind();

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
    }

    glfwTerminate();
    return 0;
}