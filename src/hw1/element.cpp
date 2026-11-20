#include "hw1/element.h"

using namespace hw1;
using namespace std;

std::vector<light_t> Element::lights;

Shader *Element::MakeShader(const char *name, const char *dir, bool geometry)
{
	Shader *s = new Shader(name);
	s->AddShader(PATH_JOIN(SOURCE_PATH::HW1, "shaders", dir, "VertexShader.glsl"), GL_VERTEX_SHADER);
	if (geometry)
		s->AddShader(PATH_JOIN(SOURCE_PATH::HW1, "shaders", dir, "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
	s->AddShader(PATH_JOIN(SOURCE_PATH::HW1, "shaders", dir, "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
	s->CreateAndLink();
	return s;
}

void Element::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D *texture, AdditionalParams *param)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	glUseProgram(shader->program);
	glm::mat4 fullMatrix = baseMatrix * modelMatrix;

	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(fullMatrix));

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);
	}

	int eyePos = glGetUniformLocation(shader->program, "eye_position");
	if (eyePos != -1) {
		glm::vec3 eyePosition = camera->m_transform->GetWorldPosition();
		glUniform3f(eyePos, eyePosition.x, eyePosition.y, eyePosition.z);
	}

	if (param) {
		int ka = glGetUniformLocation(shader->program, "material_ka");
		if (ka != -1)
			glUniform1f(ka, param->material_ka);

		int ke = glGetUniformLocation(shader->program, "material_ke");
		if (ke != -1)
			glUniform1f(ke, param->material_ke);

		int kd = glGetUniformLocation(shader->program, "material_kd");
		if (kd != -1)
			glUniform1f(kd, param->material_kd);

		int ks = glGetUniformLocation(shader->program, "material_ks");
		if (ks != -1)
			glUniform1f(ks, param->material_ks);

		int shininess =
			glGetUniformLocation(shader->program, "material_shininess");
		if (shininess != -1)
			glUniform1i(shininess, param->material_shininess);

		int color = glGetUniformLocation(shader->program, "color");
		if (color != -1)
			glUniform3f(color, param->color.x, param->color.y, param->color.z);

		//int light_num_location = glGetUniformLocation(shader->program, "num_lights");
		//glUniform1i(light_num_location, num_lights);

		//for (int i = 0; i < MAX_LIGHTS; i++) {
		//	std::string base = "lights[" + std::to_string(i) + "].";

		//	glUniform1i(glGetUniformLocation(shader->program, (base + "spotlight").c_str()), light_sources[i].spotlight);
		//	glUniform3fv(glGetUniformLocation(shader->program, (base + "position").c_str()), 1, glm::value_ptr(light_sources[i].position));
		//	glUniform3fv(glGetUniformLocation(shader->program, (base + "color").c_str()), 1, glm::value_ptr(light_sources[i].color));
		//	glUniform3fv(glGetUniformLocation(shader->program, (base + "direction").c_str()), 1, glm::value_ptr(light_sources[i].direction));
		//	glUniform1f(glGetUniformLocation(shader->program, (base + "cut_off_angle").c_str()), light_sources[i].cut_off_angle);
		//	glUniform1f(glGetUniformLocation(shader->program, (base + "intensity").c_str()), light_sources[i].intensity);
		//	glUniform1i(glGetUniformLocation(shader->program, (base + "cookie").c_str()), light_sources[i].cookie);
		//}
	}

	std::string shaderName = shader->GetName();


	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);

	if (param && param->instances > 1)
		glDrawElementsInstanced(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()),GL_UNSIGNED_INT, (void *)0, param->instances);
	else
		glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

}
