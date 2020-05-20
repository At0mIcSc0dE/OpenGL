#include "TestRenderTexture.h"
#include "Renderer.h"
#include <iostream>


namespace test {

	TestRenderTexture::TestRenderTexture()
		:m_Buffer{ 0 }, m_Texture("")
	{

	}


	TestRenderTexture::~TestRenderTexture()
	{

	}


	void TestRenderTexture::OnUpdate(float deltatime)
	{

	}


	void TestRenderTexture::OnRender()
	{
		m_Texture = Texture(m_Buffer);
		m_Texture.Bind(5);
	}


	void TestRenderTexture::OnImGuiRender()
	{
		ImGui::InputText("Path for File", m_Buffer, sizeof(m_Buffer));
		std::cout << m_Buffer << '\n';
		
	}


}



