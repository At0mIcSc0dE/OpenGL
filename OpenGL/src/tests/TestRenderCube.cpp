#include "TestRenderCube.h"
#include <array>


float CombineArrays(float a1[4], float a2[4])
{
	float result[8];
	for (short unsigned int i = 0; i < 4; ++i)
	{
		result[i] = a1[i];
	}

	for (short unsigned int i = 4; i < 8; ++i)
	{
		result[i] = a2[i];
	}

	return result;
}



namespace test {


	TestRenderCube::TestRenderCube()
		:m_Positions1{ -0.5f, -0.5f, 0.5f, -0.5f }, m_Positions2{ 0.5f, 0.5f, -0.5f, 0.5f }
	{

	}


	TestRenderCube::~TestRenderCube()
	{

	}


	void TestRenderCube::OnUpdate(float deltatime)
	{
		
	}
	
	
	void TestRenderCube::OnRender()
	{

	}


	void TestRenderCube::OnImGuiRender()
	{
		ImGui::DragFloat4("Pos1", m_Positions1, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat4("Pos2", m_Positions2, 0.1f, -1.0f, 1.0f);
	}


}

