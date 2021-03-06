#pragma once
#include <GLEW\GL\glew.h>
#include <GLM\gtc\matrix_transform.hpp>
#include <vector>

struct Transform;
struct Collider;
struct Shader;
class RenderShape;
class InteractiveShape;

class RenderManager
{
public:

	static void GenerateShapes(Shader shader, GLuint vao, int numShapes, GLenum type, GLsizei count);

	static void AddShape(Shader shader, GLuint vao, GLenum type, GLsizei count, glm::vec4 color, Transform transform, Collider collider);

	static void AddShape(Shader shader, GLuint vao, GLenum type, GLsizei count, glm::vec4 color, Transform transform);
	
	static void AddShape(RenderShape* shape);

	static void Update(float dt);

	static void Draw();

	static void DumpData();

	static const std::vector<InteractiveShape*>& interactiveShapes();

private:

	static std::vector<RenderShape*> _shapes;
	static std::vector<InteractiveShape*> _interactiveShapes;

	static glm::mat4 _projMat;

};
