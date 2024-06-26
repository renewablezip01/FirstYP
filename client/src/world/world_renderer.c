#include "../entity/mob.h"
#include "../projectile/projectile.h"
#include "../ui/sprite.h"
#include "../ui/spritesheet.h"
#include "../ui/health.h"
#include "../ui/element.h"
#include "../ui/text.h"
#include "../ui/canvas.h"
#include "../ui/health.h"
#include "../ui/game_over.h"
#include "SDL2/SDL_render.h"
#include "level_common.h"
#include "world.h"
#include <stdio.h>

typedef struct {
  SDL_Renderer *sdl_renderer;
  SpriteSheet *player_sprite_sheet;
  SpriteSheet *mob_sprite_sheet;
  Sprite *projectile_sprite;
  HealthBar *health_bar;
  GameOverScreen *game_over_screen;
} WorldRenderer;

WorldRenderer *world_renderer_create(SDL_Renderer *renderer) {
  WorldRenderer *world_renderer = malloc(sizeof(WorldRenderer));
  world_renderer->sdl_renderer = renderer;
  world_renderer->player_sprite_sheet =
      spritesheet_create(renderer, "assets/sprites/player.png", 64, 64);
  world_renderer->mob_sprite_sheet =
      spritesheet_create(renderer, "assets/sprites/mob.png", 96 / 3, 128 / 4);
  world_renderer->projectile_sprite =
      sprite_create(renderer, "assets/sprites/projectile.png");
    world_renderer->health_bar = health_bar_create(renderer);
    world_renderer->game_over_screen = game_over_screen_create(renderer, "You suck tomatoes!", "assets/fonts/sans.ttf", 48);
  return world_renderer;
}

void world_renderer_destroy(WorldRenderer *world_renderer) {
  spritesheet_destroy(world_renderer->player_sprite_sheet);
  health_bar_destroy(world_renderer->health_bar);
  game_over_screen_destroy(world_renderer->game_over_screen);
  free(world_renderer);
}

SDL_Renderer *world_renderer_get_renderer(WorldRenderer *world_renderer) {
  return world_renderer->sdl_renderer;
}

void world_renderer_project_coord(SDL_Rect *rect, SDL_Point pivot) {
  rect->x -= pivot.x;
  rect->y -= pivot.y;
}

void world_renderer_tile_coord(SDL_Rect *rect, SDL_Point pivot, int x, int y,
                               int w, int h) {
  rect->x = x * LEVEL_TILE_SIZE;
  rect->y = y * LEVEL_TILE_SIZE;
  rect->w = w;
  rect->h = h;
  world_renderer_project_coord(rect, pivot);
}

void world_renderer_render_level_tile(WorldRenderer *renderer, Level *level,
                                      int x, int y, SDL_Point pivot) {
  Tileset *tileset = level_get_tileset(level);
  int tile = level_get_tile(level, x, y);
  SDL_Rect rect;
  world_renderer_tile_coord(&rect, pivot, x, y, LEVEL_TILE_SIZE,
                            LEVEL_TILE_SIZE);
  tileset_draw_tile(tileset, renderer->sdl_renderer, tile, &rect);
}

void world_renderer_render_level(WorldRenderer *renderer, Level *level,
                                 SDL_Point pivot) {
  unsigned int width = level_get_width(level);
  unsigned int height = level_get_height(level);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      world_renderer_render_level_tile(renderer, level, x, y, pivot);
    }
  }
}

void world_renderer_render_player(WorldRenderer *renderer, Player *player,
                                  SDL_Point pivot) {
  SDL_Point coord = player_get_coord(player);
  SDL_Rect rect = {
      .x = coord.x,
      .y = coord.y,
      .w = spritesheet_get_cellwidth(renderer->player_sprite_sheet),
      .h = spritesheet_get_cellheight(renderer->player_sprite_sheet)};
  world_renderer_project_coord(&rect, pivot);
  int draw_frame = player_get_draw_frame_id(player);
  int cell_x =
      draw_frame % spritesheet_get_width(renderer->player_sprite_sheet);
  int cell_y =
      draw_frame / spritesheet_get_width(renderer->player_sprite_sheet);
  spritesheet_draw(renderer->sdl_renderer, renderer->player_sprite_sheet,
                   cell_x, cell_y, &rect);
}

void world_renderer_render_players(WorldRenderer *renderer, World *world,
                                   SDL_Point pivot) {
  EntityList *player_list = world_get_player_list(world);
  for (int i = 0; i < entity_list_size(player_list); i++) {
    Player *player = (Player *)entity_list_get(player_list, i);
    world_renderer_render_player(renderer, player, pivot);
  }
  Player *self = world_get_self_player(world);
  world_renderer_render_player(renderer, self, pivot);
}

void world_renderer_render_mob(WorldRenderer *renderer, Mob *mob,
                               SDL_Point pivot) {
  SDL_Point coord = mob_get_coord(mob);
  SDL_Rect rect = {.x = coord.x, .y = coord.y, .w = 17 * 2, .h = 28 * 2};
  world_renderer_project_coord(&rect, pivot);
  int draw_frame = mob_get_draw_frame_id(mob);
  int cell_x = draw_frame % spritesheet_get_width(renderer->mob_sprite_sheet);
  int cell_y = draw_frame / spritesheet_get_width(renderer->mob_sprite_sheet);
  spritesheet_draw(renderer->sdl_renderer, renderer->mob_sprite_sheet, cell_x,
                   cell_y, &rect);
}

void world_renderer_render_mobs(WorldRenderer *renderer, World *world,
                                SDL_Point pivot) {
  EntityList *mob_list = world_get_mob_list(world);
  for (int i = 0; i < entity_list_size(mob_list); i++) {
    Mob *mob = (Mob *)entity_list_get(mob_list, i);
    world_renderer_render_mob(renderer, mob, pivot);
  }
}

void world_render_projectile(WorldRenderer *world_renderer,
                             Projectile *projectile, SDL_Point pivot) {
  Collider *collider = projectile_get_collider(projectile);
  Entity *entity = collider_get_entity(collider);
  SDL_Point coord = entity_get_coord(entity);
  SDL_Point size = collider_get_size(collider);

  SDL_Rect projected = (SDL_Rect){coord.x, coord.y, size.x * 2, size.y * 2};
  world_renderer_project_coord(&projected, pivot);
  sprite_draw(world_renderer->sdl_renderer, world_renderer->projectile_sprite,
              &(SDL_Rect){0, 0, size.x * 2, size.y * 2}, &projected);
}

void world_renderer_render_projectiles(WorldRenderer *world_renderer,
                                       World *world, SDL_Point pivot) {
  EntityList *projectiles = world_get_projectiles_list(world);
  int projectiles_size = entity_list_size(projectiles);
  for (int i = 0; i < projectiles_size; i++) {
    Projectile *projectile = entity_list_get(projectiles, i);
    world_render_projectile(world_renderer, projectile, pivot);
  }
}

void world_renderer_render(WorldRenderer *renderer, World *world,
                           SDL_Point pivot) {
  Level *level = world_get_level(world);
  if (level)
    world_renderer_render_level(renderer, level, pivot);
  world_renderer_render_projectiles(renderer, world, pivot);
  world_renderer_render_mobs(renderer, world, pivot);
  world_renderer_render_players(renderer, world, pivot);

  Player *self_player = world_get_self_player(world);
  health_bar_update(renderer->health_bar, player_get_health(self_player));
  health_bar_draw(renderer->health_bar, renderer->sdl_renderer);

  if(player_get_health(self_player) <= 0) {
    game_over_screen_draw(renderer->game_over_screen);
  }
}
