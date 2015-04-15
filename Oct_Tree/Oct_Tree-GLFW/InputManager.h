#pragma once
#include <GLEW\GL\glew.h>
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>

class InputManager
{
public:
	static void Init(GLFWwindow* window);

	static void Update();

	static glm::vec2 GetMouseCoords();
	static bool leftMouseButton(bool prev = false);
	static bool rightMouseButton(bool prev = false);
	static bool spaceKey(bool prev = false);

private:

	static double _mousePos[2];
	static bool _leftMouseButton;
	static bool _prevLeftMouseButton;
	static bool _rightMouseButton;
	static bool _prevRightMouseButton;
	static bool _spaceKey;
	static bool _prevSpaceKey;
	static GLFWwindow* _window;
	static float _aspectRatio;
	static int _windowSize[2];
};
