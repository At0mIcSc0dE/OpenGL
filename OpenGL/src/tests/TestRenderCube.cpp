#include "TestRenderCube.h"
#include <array>
#include <iostream>

//Combines two arrays into one
void CombineArrays(float a1[4], float a2[4], float* result)
{
	for (short unsigned int i = 0; i < 4; ++i)
	{
		result[i] = a1[i];
	}

	for (short unsigned int i = 0; i < 4; ++i)
	{
		result[i + 4] = a2[i];
	}

}



namespace test {


	TestRenderCube::TestRenderCube(Renderer& renderer, Shader& shader)
		:m_Positions{ -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f }, m_Indices{ 0, 1, 2, 2, 3, 0 }, 
		vb(m_Positions, 4 * 2 * sizeof(float)), ib(m_Indices, 6), shader(shader), renderer(renderer), m_Color{ 0.8f, 0.3f, 0.8f, 1.0f }
	{
	
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		shader.Bind();
		shader.SetUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
	}


	TestRenderCube::~TestRenderCube()
	{

	}

	 
	void TestRenderCube::OnUpdate(float deltatime)
	{
		
	}
	
	
	void TestRenderCube::OnRender()
	{

		shader.Bind();
		shader.SetUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		renderer.Draw(va, ib, shader);
	}


	void TestRenderCube::OnImGuiRender()
	{
		ImGui::ColorEdit4("Edit Cube Color", m_Color);

		//for (int i = 0; i < 8; ++i)
		//	std::cout << m_Positions[i] << '\n';
	}


}

