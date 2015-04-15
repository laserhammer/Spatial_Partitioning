#pragma once
#include "RenderShape.h"

struct Collider
{
	float width;
	float height;
	float depth;
	float x;
	float y;
	float z;
};

class InteractiveShape : public RenderShape
{
public:
	InteractiveShape(Collider collider, GLint vao = 0, GLsizei count = 0, GLenum mode = 0, Shader shader = Shader(), glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	~InteractiveShape();

	void Update(float dt);

	void Draw(const glm::mat4& viewProjMat);

	static Collider MakeCollider(float width, float height, float depth, float x, float y, float z);

	const Collider& collider();
	bool mouseOver();
	bool mouseOut();

private:

	bool _selected;
	bool _mouseOver;
	bool _mouseOut;
	Collider _collider;
};
