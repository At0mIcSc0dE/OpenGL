#include "TestUniform.h"



namespace test {

	TestUniform::TestUniform()
	{

	}
	TestUniform::~TestUniform()
	{
	
	}

	void TestUniform::TestUniform4f()
	{
		glUniform4f(0, 1.0f, 0.4f, 1.8f, 1.0f);
	}



	void TestUniform::OnRender()
	{

	}


	void TestUniform::OnUpdate(float deltatime)
	{

	}


	void TestUniform::OnImGuiRender()
	{

	}

}
