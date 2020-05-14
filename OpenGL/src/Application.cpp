#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "Timer.h"

#include <GLFW/glfw3.h>
   
#include <string>
#include <fstream>
#include <sstream>


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
    //window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
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
        //VertexPositions + Texture coordinates
        float positions[] = {
            -0.5f, -0.5f, 0.0f, 0.0f,   //0
             0.5f, -0.5f, 1.0f, 0.0f,   //1
             0.5f,  0.5f, 1.0f, 1.0f,   //2
            -0.5f,  0.5f, 0.0f, 1.0f    //3
        };

        //Index buffer
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        //For texture rendering
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        //Vertex Array (Default provided with glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE))
        VertexArray va;
        //Generate VertexBuffer
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2); //Drawing 2 triangles
        layout.Push<float>(2); //for the Texture cooridinates
        va.AddBuffer(vb, layout);


        //Assigning indexBuffer to avoid copying some vertices
        IndexBuffer ib(indices, 6);


        //Parsing shader from CPU to GPU
        Shader shader("res/shader/Basic.shader");
        shader.Bind();

        //Accessing variable in shader from CPU
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        //Adding the texture
        Texture texture("res/textures/TestImage.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);


        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        float r = 0.0f;
        float rIncrement = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            renderer.Draw(va, ib, shader);

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

    }

    glfwTerminate();
    return 0;
}