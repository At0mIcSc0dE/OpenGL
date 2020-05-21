#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "Texture.h"

#include "tests/TestClearColor.h"
#include "tests/TestUniform.h"
#include "tests/TestRenderTexture.h"
#include "tests/TestRenderCube.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Timer.h"

#include <GLFW/glfw3.h>
   

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
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    //window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
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

    {

        //For texture rendering
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        Renderer renderer;
        Shader shader("res/shader/Basic.shader");
        Texture texture("res/textures/TestImage.png");
        shader.Bind();
        texture.Bind();

        //IMGUI
        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();


        test::TestClearColor ClearColorTest;
        test::TestUniform Uniform4fTest;
        test::TestRenderTexture RenderImageTest;
        test::TestRenderCube RenderCubeTest;
        test::TestType TestType = test::NONE;


        while (!glfwWindowShouldClose(window))
        {

            renderer.Clear();

            ImGui_ImplGlfwGL3_NewFrame();

            //Buttons for TestFramework
            if (ImGui::Button("ClearColorTest"))
            {
                TestType = test::ClearColor;
            }
            else if (ImGui::Button("SetUniform4f"))
            {
                TestType = test::SetUniform4f;
            }
            else if (ImGui::Button("RenderCube"))
            {
                TestType = test::RenderCube;
            }
            else if (ImGui::Button("RenderTexture"))
            {
                TestType = test::RenderTexture;
            }

            switch (TestType) {
            case test::ClearColor:
                ClearColorTest.OnUpdate(0);
                ClearColorTest.OnRender();
                ClearColorTest.OnImGuiRender();
                break;

            case test::SetUniform4f:
                Uniform4fTest.OnImGuiRender();
                break;

            case test::RenderCube:
                RenderCubeTest.OnUpdate(0);
                RenderCubeTest.OnImGuiRender();
                break;

            case test::RenderTexture:
                RenderImageTest.OnUpdate(0);
                RenderImageTest.OnImGuiRender();

                if (ImGui::Button("SelectPath")) 
                {
                    const char* imagePath = RenderImageTest.GetString();
                    std::cout << imagePath << '\n';

                    //RenderImageTest.OnRender();

                    texture = Texture(imagePath);
                    texture.Bind(5);

                    shader.SetUniform1i("u_Texture", 5);
                    //TestType = test::NONE;
                }

                break;
            }


            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);

            glfwPollEvents();
        }

    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();



    glfwTerminate();
    return 0;
}
