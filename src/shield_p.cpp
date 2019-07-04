// stlib
#include <vector>
#include <string>
#include <algorithm>
#include <Shield_P.hpp>

Texture Shield_P::shield_p_textures[10];

bool Shield_P::init(vec2 position, vec2 direction, Player* player)
{
	char texPath[] = character_shield;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	skill_texture.load_from_file(buffer);

	damage = 2;
	duration_countdown = 50;
	start_time = 0.0;
	tex_num = 0;
	player->set_shield(true);
	this->player = player;

	float wr = skill_texture.width*0.016f;
	float hr = skill_texture.height*0.016f;

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
	indicess = { indices[0],indices[1],indices[2],indices[3],indices[4],indices[5] };
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
	m_position = position;
	m_direction = direction;
	m_scale.x = 35.f;
	m_scale.y = 35.f;

	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	return true;
}

void Shield_P::destroy()
{
	if(player)	player->set_shield(false);
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	//
	effect.release();
}

void Shield_P::update(float ms)
{
	start_time += ms;
	if (start_time >= Gloabal_animation_CountDown) {
		tex_num = (tex_num + 1) % 10;
		duration_countdown--;
		start_time = 0.0;
	}
	if (player) m_position = player->get_position();

	b2Vec2 b2position = b2Vec2((float32)(m_position.x * pixeltob2), (float32)(m_position.y* pixeltob2));
	if (rigidbody) { rigidbody->SetTransform(b2position, 0); }

}

void Shield_P::draw(const mat3& projection, mat3& view_2d)
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
	glBindTexture(GL_TEXTURE_2D, shield_p_textures[tex_num].id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Shield_P::get_position()const
{
	return m_position;
}

void Shield_P::set_position(vec2 position)
{
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the enemy 
vec2 Shield_P::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * 0.4f*skill_texture.width, std::fabs(m_scale.y) * 0.4f*skill_texture.height };
}

int Shield_P::getEntityType()
{
	return ET_player_shield;
}

void Shield_P::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (((r2EntityType % 100) / 10) == 1) {

		collides_with(*((Enemy*)r2));
	}
}

void Shield_P::setDef()
{
	shieldBodyDef.type = b2_dynamicBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	shieldBodyDef.position.Set(x, y);
    shieldBodyDef.angle = 0;
	//setting box2d shape and fixture def
	float32 width = abs(m_scale.x* skill_texture.width * pixeltob2 * 0.025);
	float32 height = abs(m_scale.y* skill_texture.height * pixeltob2 * 0.025);
	shieldShape.SetAsBox(width / 2, height / 2);
	shieldFixtureDef.shape = &shieldShape;
	shieldFixtureDef.density = 1;
	shieldFixtureDef.friction = 0.3;
}

bool Shield_P::collides_with(Enemy& enemy)
{   /*
	float dx = m_position.x - enemy.get_position().x;
	float dy = m_position.y - enemy.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(enemy.get_bounding_box().x - 30.f, enemy.get_bounding_box().y - 30.f);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 1.5f;
	if (d_sq < r * r) {
		enemy.attacked(damage);
		return true;
	}
	return false;*/

	shouldErase = true;
	hit = true;
	enemy.attacked(damage);
	return true;
}

int Shield_P::get_duration() {
	return duration_countdown;
}