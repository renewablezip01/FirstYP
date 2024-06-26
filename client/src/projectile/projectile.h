#ifndef _PROJECTILE_
#define _PROJECTILE_

#include "../collider/collider.h"
#include "../entity/entity.h"
#include "projectile_common.h"
#include <SDL2/SDL_rect.h>
typedef struct Projectile Projectile;

Projectile *projectile_create(float speed, float damage,
                              SDL_Point collision_size, SDL_Point coord,
                              void *entity, ProjectileType type,
                              SDL_FPoint direction,
                              ProjectileDestroyFunc destroy);
void projectile_destroy(Projectile *projectile);
void projectile_update(Projectile *projectile, float dt);

SDL_Point projectile_get_start_coordinate(Projectile *projectile);
SDL_Point projectile_get_coord(Projectile *projectile);
float projectile_get_speed(Projectile *projectile);
float projectile_get_damage(Projectile *projectile);
SDL_FPoint projectile_get_direction(Projectile *projectile);
ProjectileType projectile_get_type(Projectile *projectile);
Collider *projectile_get_collider(Projectile *projectile);
void projectile_set_coord(Projectile *projectile, SDL_Point coord);

#endif // !_PROJECTILE_
