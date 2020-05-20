#pragma once
#include "imgui/imgui.h"


namespace test {

	enum TestType {
		NONE = 0, ClearColor, SetUniform4f, RenderTexture
	};


	class Test {
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltatime) = 0;
		virtual void OnRender() = 0;
		virtual void OnImGuiRender() = 0;
	};


}
