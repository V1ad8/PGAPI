#include "hw1/building.h"

#include <algorithm>
#include <random>

using namespace hw1;

Shader *Building::shader = nullptr;

void Building::GenerateCurves()
{
	float totalH = Rand(9.f, 15.f);
	int sections = 3 + rand() % 3;
	float sectionH = totalH * Rand(0.55f, 0.75f) / sections;
	float r = Rand(1.3f, Building::MAX_RADIUS), y = 0.f;
	this->radius = r;

	for (int i = 0; i < sections; i++) {
		float y1 = y + sectionH * Rand(0.8f, 1.2f);
		float r1 = clampR(r + Rand(-0.6f, 0.5f));

		if (i == 0 || rand() % 2) {
			glm::vec2 c1(clampR(r + Rand(-0.8f, 0.8f)), y + (y1 - y) * Rand(0.2f, 0.5f));
			glm::vec2 c2(clampR(r1 + Rand(-0.8f, 0.8f)), y + (y1 - y) * Rand(0.5f, 0.8f));
			this->segs.push_back(Bezier({ r, y }, c1, c2, { r1, y1 }));
			this->radius = std::max(this->radius, std::max(c1.x, c2.x));
		} else {
			this->segs.push_back(Line({ r, y }, { r1, y1 }));
		}
		y = y1;
		r = r1;
		this->radius = std::max(this->radius, r);

		if (i < sections - 1 && rand() % 100 < 40) {
			float r2 = clampR(r - Rand(0.3f, 0.6f));
			this->segs.push_back(Line({ r, y }, { r2, y }));
			r = r2;
		}
	}

	// Crown: closes the top by going to r = 0
	float ch = std::max(totalH - y, 1.5f);
	switch (rand() % 3) {
	case 0: // dome
		this->segs.push_back(Bezier({ r, y }, { clampR(r * 1.2f), y + ch * 0.5f }, { r * 0.6f, y + ch * 0.9f }, { 0.f, y + ch }));
		y += ch;
		break;
	case 1: // spire
		ch += 2.f;
		this->segs.push_back(Bezier({ r, y }, { r * 0.7f, y + ch * 0.3f }, { r * 0.2f, y + ch * 0.6f }, { 0.f, y + ch }));
		y += ch;
		break;
	default: // flat roof
		this->segs.push_back(Line({ r, y }, { 0.f, y }));
		break;
	}
	this->height = y;
}

Mesh *Building::BuildMesh()
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	for (const Seg &seg : this->segs)
		for (int k = 0; k < 4; k++) {
			indices.push_back((unsigned int)vertices.size());
			vertices.push_back(VertexFormat(glm::vec3(seg.p[k], 0.f), glm::vec3(1)));
		}

	Mesh *m = new Mesh("building_profile");
	m->InitFromData(vertices, indices);
	m->SetDrawMode(GL_LINES_ADJACENCY);
	return m;
}

Building::Building(gfxc::Camera *aCamera, glm::vec3 aPosition, glm::vec3 aRotation, glm::vec3 aScale) : Element(aCamera, aPosition, aRotation, aScale)
{
	window_color = WINDOW_COLORS[rand() % WINDOW_COLORS->length()];
	wall_color = WALL_COLOR + WINDOW_COLOR_BLEND * window_color;
	windowSeed = Rand(0.f, 1000.f);

	GenerateCurves();
	radius *= aScale.x;
	height *= aScale.y;

	mesh = BuildMesh();

	if (!shader)
		shader = MakeShader("BuildingShader", "building", true);

	curve_samples = Rand(6, 12);

	params.material_ka = MATERIAL_KA;
	params.material_ke = 0;
	params.material_kd = 0.2f;
	params.material_ks = 0.1f;
	params.material_shininess = 30;
	params.instances = Rand(6, 12);
}

Building::~Building()
{
}

void Building::Render()
{
	glUseProgram(shader->program);

	glUniform1i(UniformLocation(shader, "curve_samples"), curve_samples);
	glUniform1i(UniformLocation(shader, "slices"), params.instances);
	glUniform1f(UniformLocation(shader, "seed"), windowSeed);
	glUniform1f(UniformLocation(shader, "window_density"), WINDOW_DENSITY);
	glUniform1f(UniformLocation(shader, "time"), (float)glfwGetTime());
	glUniform3fv(UniformLocation(shader, "window_color"), 1, glm::value_ptr(window_color));
	glUniform3fv(UniformLocation(shader, "wall_color"), 1, glm::value_ptr(wall_color));

	RenderMesh(mesh, shader, nullptr, &params);
}