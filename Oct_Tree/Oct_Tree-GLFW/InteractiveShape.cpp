#include "InteractiveShape.h"
#include "InputManager.h"

InteractiveShape::InteractiveShape(Collider collider, GLint vao, GLsizei count, GLenum mode, Shader shader, glm::vec4 color)
{
	this->RenderShape::RenderShape(vao, count, mode, shader, color);
	_collider = collider;
	_mouseOver = false;
	_selected = false;
	_active = true;
}

InteractiveShape::~InteractiveShape(){}

void InteractiveShape::Update(float dt)
{
	RenderShape::Update(dt);

	// Constrain position to the screen
	_transform.position.x = _transform.position.x > 1.337f ? 1.337f : _transform.position.x;
	_transform.position.x = _transform.position.x < -1.337f ? -1.337f : _transform.position.x;
	_transform.position.y = _transform.position.y > 1.0f ? 1.0f : _transform.position.y;
	_transform.position.y = _transform.position.y < -1.0f ? -1.0f : _transform.position.y;
	_transform.position.z = _transform.position.z > -3.0f ? -3.0f : _transform.position.z;
	_transform.position.z = _transform.position.z < -5.0f ? -5.0f : _transform.position.z;
	if (_transform.position.x == 1.337f || _transform.position.x == -1.337f)
	{
		_transform.linearVelocity.x *= -1.0f;
	}
	if (_transform.position.y == 1.0f || _transform.position.y == -1.0f)
	{
		_transform.linearVelocity.y *= -1.0f;
	}
	if (_transform.position.z == -3.0f || _transform.position.z == -5.0f)
	{
		_transform.linearVelocity.z *= -1.0f;
	}

	_mouseOut = false;
	// Check the mouse
	glm::vec2 mousePos = InputManager::GetMouseCoords();
}

void InteractiveShape::Draw(const glm::mat4& viewProjMat)
{
	RenderShape::Draw(viewProjMat);
}

const Collider& InteractiveShape::collider()
{
	Collider ret = _collider;
	ret.x += _transform.position.x;
	ret.y += _transform.position.y;
	ret.z += _transform.position.z;
	return ret;
}

Collider InteractiveShape::MakeCollider(float width, float height, float depth, float x, float y, float z)
{
	Collider ret;
	ret.width = width;
	ret.height = height;
	ret.depth = depth;
	ret.x = x;
	ret.y = y;
	ret.z = z;
	return ret;
}

bool InteractiveShape::mouseOver() { return _mouseOver; }
bool InteractiveShape::mouseOut() { return _mouseOut; }