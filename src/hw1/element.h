#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "components/simple_scene.h"
#include "components/transform.h"
#include <glm/gtc/constants.hpp>

#define MATERIAL_KA 0.2f

namespace hw1
{
struct Seg {
	glm::vec2 p[4];
};

struct light_t {
	bool spotlight;
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 direction;
	float cut_off_angle;
	float intensity;
	float range;
};

struct AdditionalParams {
	float material_ka, material_ke, material_kd, material_ks, material_shininess;
	glm::vec3 color;
	int instances = 1;
};

class Element {
  public:
	Element(gfxc::Camera *aCamera, glm::vec3 aPosition, glm::vec3 aRotation, glm::vec3 aScale)
	{
		this->camera = aCamera;

		baseMatrix = glm::mat4(1);
		baseMatrix = glm::translate(baseMatrix, aPosition);
		baseMatrix = glm::rotate(baseMatrix, glm::radians(aRotation.x), glm::vec3(1, 0, 0));
		baseMatrix = glm::rotate(baseMatrix, glm::radians(aRotation.y), glm::vec3(0, 1, 0));
		baseMatrix = glm::rotate(baseMatrix, glm::radians(aRotation.z), glm::vec3(0, 0, 1));
		baseMatrix = glm::scale(baseMatrix, aScale);
	}

	virtual ~Element() {}

	glm::vec3 Position() const { return glm::vec3(baseMatrix[3]); }
	static std::vector<light_t> &Lights() { return lights; }

	float Rand(float min, float max) const { return min + (max - min) * ((float)rand() / RAND_MAX); }
	int Rand(int min, int max) const { return rand() % (max - min + 1) + min; }

	virtual void Render() {}
	virtual void Update(float dt) {}

  private:
	gfxc::Camera *camera;

  protected:
	void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D *texture, AdditionalParams *param = nullptr);
	void RenderMesh(Mesh* mesh, Shader* shader, Texture2D* texture, AdditionalParams* param = nullptr) {
		RenderMesh(mesh, shader, glm::mat4(1.0f), texture, param);
	}

	static Shader *MakeShader(const char *name, const char *dir, bool geometry = false);
	static GLint UniformLocation(Shader *s, const char *name) { return glGetUniformLocation(s->program, name); }

	Seg Bezier(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d) { return { { a, b, c, d } }; }
	Seg Line(glm::vec2 a, glm::vec2 b) { return Bezier(a, glm::mix(a, b, 1.f / 3.f), glm::mix(a, b, 2.f / 3.f), b); }

	glm::mat4 baseMatrix;
	AdditionalParams params;
	static std::vector<light_t> lights;
};
} // namespace hw1