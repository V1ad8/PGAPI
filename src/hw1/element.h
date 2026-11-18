#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "components/simple_scene.h"
#include "components/transform.h"

#define MATERIAL_KA 0.2f
#define MAX_LIGHTS 30
#define MAX_COOKIES 6

namespace hw1
{
struct light_t {
	bool spotlight;
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 direction;
	float cut_off_angle;
	float intensity;
	int cookie;
};

struct AdditionalParams {
	float material_ka, material_ke, material_kd, material_ks, material_shininess;
	glm::vec3 color;

	int instances;
};

class Element{
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

	virtual ~Element()
	{
	}

	virtual void Render() = 0;

  private:
	gfxc::Camera *camera;

  protected:
	void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D *texture, AdditionalParams *param = nullptr);

	glm::mat4 baseMatrix;
	AdditionalParams params;

	static light_t light_sources[MAX_LIGHTS];
	static int num_lights;
};
} // namespace hw1
