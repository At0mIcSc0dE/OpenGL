#pragma once

#include "Test.hpp"
#include "Renderer.h"


namespace test {

	class TestUniform : public Test
	{
	public:
		TestUniform();
		~TestUniform();

		void TestUniform4f();
		
		void OnUpdate(float deltatime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};

}