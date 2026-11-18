#include "hw1/building.h"

#include <algorithm>
#include <random>

using namespace hw1;

Shader *Building::shader = nullptr;

Seg Bezier(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d)
{
	return { { a, b, c, d } };
}

Seg Line(glm::vec2 a, glm::vec2 b)
{
	return Bezier(a, glm::mix(a, b, 1.f / 3.f), glm::mix(a, b, 2.f / 3.f), b);
}

static float RandF(std::mt19937 &g, float a, float b)
{
	return std::uniform_real_distribution<float>(a, b)(g);
}

Profile GenerateProfile(unsigned int seed)
{
	std::mt19937 g(seed);
	const float MIN_R = Building::MIN_RADIUS, MAX_R = Building::MAX_RADIUS;
	auto clampR = [&](float r) { return glm::clamp(r, MIN_R, MAX_R); };

	Profile P;
	float totalH = RandF(g, 9.f, 15.f);
	int sections = 3 + g() % 3;
	float sectionH = totalH * RandF(g, 0.55f, 0.75f) / sections;
	float r = RandF(g, 1.3f, MAX_R), y = 0.f;
	P.radius = r;

	for (int i = 0; i < sections; i++) {
		float y1 = y + sectionH * RandF(g, 0.8f, 1.2f);
		float r1 = clampR(r + RandF(g, -0.6f, 0.5f));

		if (i == 0 || g() % 2) {
			glm::vec2 c1(clampR(r + RandF(g, -0.8f, 0.8f)), y + (y1 - y) * RandF(g, 0.2f, 0.5f));
			glm::vec2 c2(clampR(r1 + RandF(g, -0.8f, 0.8f)), y + (y1 - y) * RandF(g, 0.5f, 0.8f));
			P.segs.push_back(Bezier({ r, y }, c1, c2, { r1, y1 }));
			P.radius = std::max(P.radius, std::max(c1.x, c2.x));
		} else {
			P.segs.push_back(Line({ r, y }, { r1, y1 }));
		}
		y = y1;
		r = r1;
		P.radius = std::max(P.radius, r);

		if (i < sections - 1 && g() % 100 < 40) {
			float r2 = clampR(r - RandF(g, 0.3f, 0.6f));
			P.segs.push_back(Line({ r, y }, { r2, y }));
			r = r2;
		}
	}

	// Crown: closes the top by going to r = 0
	float ch = std::max(totalH - y, 1.5f);
	switch (g() % 3) {
	case 0: // dome
		P.segs.push_back(Bezier({ r, y }, { clampR(r * 1.2f), y + ch * 0.5f }, { r * 0.6f, y + ch * 0.9f }, { 0.f, y + ch }));
		y += ch;
		break;
	case 1: // spire
		ch += 2.f;
		P.segs.push_back(Bezier({ r, y }, { r * 0.7f, y + ch * 0.3f }, { r * 0.2f, y + ch * 0.6f }, { 0.f, y + ch }));
		y += ch;
		break;
	default: // flat roof
		P.segs.push_back(Line({ r, y }, { 0.f, y }));
		break;
	}
	P.height = y;
	return P;
}

Mesh* BuildProfileMesh(const Profile &profile)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	for (const Seg &seg : profile.segs)
		for (int k = 0; k < 4; k++) {
			indices.push_back((unsigned int)vertices.size());
			vertices.push_back(
				VertexFormat(glm::vec3(seg.p[k], 0.f), glm::vec3(1)));
		}

	Mesh *m = new Mesh("building_profile");
	m->InitFromData(vertices, indices);
	m->SetDrawMode(GL_LINES_ADJACENCY);
	return m;
}

Building::Building(gfxc::Camera *aCamera, glm::vec3 aPosition,
				   glm::vec3 aRotation, glm::vec3 aScale)
	: Element(aCamera, aPosition, aRotation, aScale)
{
	scale = aScale.x;

	type = rand() % TYPE_COUNT;
	window_color = WINDOW_COLORS[rand() % 5];
	wall_color = WALL_COLOR + WINDOW_COLOR_BLEND * window_color;
	seed = (float)(rand() % 1000) / 1000.f;

	Profile profile = GenerateProfile(rand());
	mesh = BuildProfileMesh(profile);
	radius = profile.radius * scale;
	height = profile.height * scale;

	if (!shader) {
		shader = new Shader("BuildingShader");
		shader->AddShader(PATH_JOIN(SOURCE_PATH::HW1, "shaders", "buildingShader", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(SOURCE_PATH::HW1, "shaders", "buildingShader", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
		shader->AddShader(PATH_JOIN(SOURCE_PATH::HW1, "shaders", "buildingShader", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
	}

	curve_samples = rand() % 4 + 6;

	params.material_ka = MATERIAL_KA;
	params.material_ke = 0;
	params.material_kd = 0.2f;
	params.material_ks = 0.1f;
	params.material_shininess = 30;
	params.color = wall_color;
	params.instances = rand() % 6 + 8;
}

Building::~Building()
{
}

float Building::Radius() const
{
	return radius;
}
float Building::Height() const
{
	return height;
}

void Building::Render()
{
	// Uniforms must be set AFTER the program is bound.
	glUseProgram(shader->program);

	auto loc = [&](const char *n) {
		return glGetUniformLocation(shader->program, n);
	};
	glUniform1i(loc("curve_samples"), curve_samples);
	glUniform1i(loc("slices"), params.instances);
	glUniform1f(loc("seed"), seed);
	glUniform1f(loc("window_density"), WINDOW_DENSITY);
	glUniform1f(loc("time"), (float)glfwGetTime());
	glUniform3fv(loc("window_color"), 1, glm::value_ptr(window_color));
	glUniform3fv(loc("wall_color"), 1, glm::value_ptr(wall_color));

	// One instance per angular slice.
	RenderMesh(mesh, shader, baseMatrix, nullptr, &params);
}