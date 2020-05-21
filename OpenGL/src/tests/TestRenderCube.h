#pragma once

#include "Test.hpp"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Renderer.h"


namespace test {

	class TestRenderCube : public Test
	{
	private:
		float m_Positions1[4];
		float m_Positions2[4];
	public:
		TestRenderCube();
		~TestRenderCube();

		void OnUpdate(float deltatime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};

}