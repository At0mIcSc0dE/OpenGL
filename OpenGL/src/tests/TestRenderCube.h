#pragma once

#include "Test.hpp"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"


namespace test {

	class TestRenderCube : public Test
	{
	private:
		float m_Positions[8];
		float m_Color[4];

		unsigned int m_Indices[6];

		VertexArray va;
		VertexBuffer vb;
		VertexBufferLayout layout;
		IndexBuffer ib;
		Shader& shader;
		Renderer& renderer;
	public:
		TestRenderCube(Renderer& renderer, Shader& shader);
		~TestRenderCube();

		void OnUpdate(float deltatime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};

}