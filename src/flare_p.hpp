#pragma once
#include "Enemy.hpp"
//#include "Player.hpp"
#include "Box2D/Box2D.h"

class Flare_P : public Renderable
{
public:
	static Texture flare_p_textures[27];
	bool init(vec2 position, vec2 direction);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	bool collides_with(Enemy& enemy);
	vec2 get_position()const;
	void set_position(vec2 position);
	int get_duration();
	vec2 get_bounding_box()const;
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	bool shouldErase = false;
	bool hit = false;
	bool created = false;
	bool flared = false;

	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef flareBodyDef;
	b2PolygonShape flareShape;
	b2FixtureDef flareFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void setDef();
	/////////////////////////

private:
	int damage;
	bool can_damage;
	Texture skill_texture;
	float frametime_prev;
	int duration_countdown;
	float start_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
};
