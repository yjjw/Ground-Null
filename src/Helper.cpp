// stlib
#include <vector>
#include <string>
#include <algorithm>
#include "Helper.hpp"

Texture Helper::helper_textures[3];

bool Helper::init()
{
	char texPath[] = character_shield;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	skill_texture.load_from_file(buffer);


	float wr = skill_texture.width*0.04f;
	float hr = skill_texture.height*0.04f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[1].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[0].texcoord = { 1.f, 1.f, };
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[3].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.01f };
	vertices[2].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values
	//m_scale.x = -35.f;
	//m_scale.y = 35.f;
	m_position = vec2({0.f, 0.f});
	m_scale.x = 1.f;
	m_scale.y = 1.f;
	page = 0;

	return true;
}

void Helper::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	//
	effect.release();
}

void Helper::increase_pages() {
	if (page < 2) page++;
}

void Helper::decrease_pages() {
	if (page > 0) page--;
}

int Helper::getEntityType()
{
	return ET_helper;
}

void Helper::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
}

void Helper::update(float ms)
{
}

void Helper::draw(const mat3& projection, mat3& view_2d)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint view_uloc = glGetUniformLocation(effect.program, "view");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));


	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, helper_textures[page].id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Helper::get_position()const
{
	return m_position;
}

void Helper::set_position(vec2 position)
{
	m_position = position;
}

