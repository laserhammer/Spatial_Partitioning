#include "InputManager.h"
#include <iostream>

double InputManager::_mousePos[2];
bool InputManager::_leftMouseButton = false;
bool InputManager::_rightMouseButton = false;
bool InputManager::_prevLeftMouseButton = false;
bool InputManager::_prevRightMouseButton = false;
bool InputManager::_spaceKey = false;
bool InputManager::_prevSpaceKey = false;
GLFWwindow* InputManager::_window;
float InputManager::_aspectRatio = 0.0f;
int InputManager::_windowSize[2];

void InputManager::Init(GLFWwindow* window)
{
	_window = window;
	glfwGetWindowSize(window, &_windowSize[0], &_windowSize[1]);
	_aspectRatio = (float)_windowSize[0] / (float)_windowSize[1];
}

void InputManager::Update()
{
	_prevLeftMouseButton = _leftMouseButton;
	_prevRightMouseButton = _rightMouseButton;
	_leftMouseButton = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	_rightMouseButton = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	_prevSpaceKey = _spaceKey;
	_spaceKey = glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS;
	glfwGetCursorPos(_window, &_mousePos[0], &_mousePos[1]);
}

glm::vec2 InputManager::GetMouseCoords()
{
	glm::vec2 ret = glm::vec2();
	ret.x = (((float)_mousePos[0] / (float)_windowSize[0]) * 2.0f - 1.0f) * _aspectRatio;
	ret.y = -(((float)_mousePos[1] / (float)_windowSize[1]) * 2.0f - 1.0f);
	return ret;
}
bool InputManager::leftMouseButton(bool prev) { if (prev) return _prevLeftMouseButton; else return _leftMouseButton; }
bool InputManager::rightMouseButton(bool prev) { if (prev) return _prevRightMouseButton; else return _rightMouseButton; }
bool InputManager::spaceKey(bool prev) { if (prev) return _prevSpaceKey; else return _spaceKey; }