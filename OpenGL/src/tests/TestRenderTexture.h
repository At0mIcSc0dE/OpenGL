#pragma once

#include "Test.hpp"
#include "Texture.h"


namespace test {

	class TestRenderTexture : public Test
	{
	private:
		char m_Buffer[64];
		Texture m_Texture;
	public:
		TestRenderTexture();
		~TestRenderTexture();

		void OnUpdate(float deltatime) override;
		void OnRender() override;
		void OnImGuiRender() override;

		const char* GetString() const { return m_Buffer; }
	};

}