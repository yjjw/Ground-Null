#pragma once
#include "Enemy.hpp"
#include "Player.hpp"

class Helper : public Renderable
{
public:
	static Texture helper_textures[3];
	bool init();
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	vec2 get_position()const;
	void set_position(vec2 position);
	void increase_pages();
	void decrease_pages();
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
private:
	Texture skill_texture;
	int page;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};