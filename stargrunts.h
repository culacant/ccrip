#include "raylib.h"

#define RAYMATH_IMPLEMENTATION
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "enums.h"

Texture2D TEXTURES[NUMTEXTURES];
Vector3 FORMATIONPOSITION[] =
{
// relative position of figures in formation
  {0.0f,0.0f,1.0f}, // LINE
  {1.0f,0.0f,0.0f}, // COLLUMN
};
int ACTIONTIMERS[] =
{

  1000, // ACTION_CLOSECOMBAT
  1000, // ACTION_UNPANIC
  1000, // ACTION_UNSUPPRESS
  1000, // ACTION_FIRE
  1000, // ACTION_NOTARGET
  1000, // ACTION_NOAMMO
  1000, // ACTION_MOVE
  1000, // ACTION_MOVE_POSITION
  1000, // ACTION_RALLY
  1000, // ACTION_REGROUP
  1000, // ACTION_IDLE
 10000, // ACTION_CASUALTY
};

/*
map data struct:
tile:
  X,Y as ints
  cover hard/soft
  seethrough
.obj as graphics
alter:
  X,Y,type in data file
fixed typeid in array
  cover hard/soft
  seethrough
*/
typedef struct Tile Tile;
typedef struct Terraintile Terraintile;
typedef struct TileType TileType;
typedef struct Figure Figure;
typedef struct Unit Unit;
typedef struct Weapon Weapon;
struct Tile
{
  int cover;
  int visible;
  int walkable;
};
struct Terraintile
{
  int x;
  int y;
  int type;
};
struct TileType
{
  int tilecover;
  int tilevisible;
  int tilewalkable;
  Tile tile[TILERES][TILERES];
  Model model;
};
struct Weapon
{
  int FP;
  int IV;
  int flags;
  int burst;

  char name[MAXNAMELENGTH];
};
struct Figure
{
  int weapon;
  int FP;
  int IV;
  int wflags;
  int burst;
  int ammo;

  int movespeed;

  int AT;
  int flags;
  int LV;
  int quality;
  int confidence;

  int suppressed;
  int cover;

  Vector3 position;
  float facing;
  Model model;

  int action;
  int actiontimer;
  int doingcommand;

  char name[MAXNAMELENGTH];
};
struct Unit
{
  int team;
  int commandlevel;
  int flags;
  int figurecnt;
  Figure figures[MAXFIGURES];

  Vector3 goal;
  float facing;
  int formation;
  int spread;
  Unit *friendlytarget;
  Unit *enemytarget;

  int command;

  char name[MAXNAMELENGTH];
};

void DebugLog(int debuglvl,const char *format, ...);

int roll_die(int sides);
int roll_die_number(int sides, int target);
int roll_die_die(int sides1, int sides2);
int roll_die_dice(const int *sides1, int numdice, int sides2, int *total);

int closedshift(int sides,int dir);
void openshift(int sides1, int dir,int sides2,int *result);

//ticks
void tick_units(Unit *units,int deltatime);

// commands
int command_fire(Unit *unit1,Unit *unit2);
int command_move(Unit *unit,Vector3 position);
int command_idle(Unit *unit);

// unit actions
int do_action_rally(Unit unit1, Unit *unit2);
int do_action_regroup(Unit *unit1, Unit *unit2);

// figure actions
int do_action_move(Unit *unit,int figureid);

int do_action_closecombat(Figure *figure1,Figure *figure2);
int do_action_fire(Figure figure1,Figure *figure2);

int do_action_unpanic(Figure *figure);
int do_action_unsuppress(Figure *figure);
int do_action_move_in_position(Figure *figure);

int get_range_die(Figure figure1,Figure figure2);
int get_leader_id(Unit unit);
int get_cover(Figure figure);
int get_figure_can_do_action(Figure figure);
int get_figure_suppressed(Figure figure);

Vector3 get_position_formation(Unit unit, int figureid);

int get_unit_can_do_action(Unit unit);
int get_unit_confidence(Unit unit);
Vector3 get_unit_position(Unit unit);
int get_next_action_figure(Unit unit,int figureid);
Figure *get_enemy_target_figure(Unit *unit,int figureid);

void set_suppressed(Figure *figure);
void set_unsuppressed(Figure *figure);
void set_casualty(Figure *figure, int IV);

void set_confidence(Unit *unit, int dconfidence);
int set_replacement_leader(Unit *unit);

int test_confidence(Figure *figure, int threatlevel);
int test_reaction(Figure figure, int threatlevel);
int test_panic(Figure figure);
int test_figure_exists(Figure figure);
int test_figure_move_finished(Figure figure);

int test_figure_LOS(Figure figure1, Figure figure2);
int test_figure_selectable(const Figure *figure1,const Figure *figure2);
int test_unit_selectable(const Unit *unit1,const Unit *unit2);
int test_unit_LOS(Unit unit1,Unit unit2);
int test_unit_action_finished(Unit unit);

int test_communicate(Unit unit1,Unit unit2);
int test_facetoface(Unit unit1,Unit unit2);

void print_unit(Unit unit);
void print_teams();
Unit* read_units_file(const char *filename);

void read_weapontable_file(const char *filename);

Vector3 string_to_pos(const char *string);
int enum_string_to_int(const char *string);
int flag_string_to_int(const char *string);

void lv_enum_to_string(char *out,int LV);
void quality_enum_to_string(char *out,int quality);
void confidence_enum_to_string(char *out,int confidence);
void commandlevel_enum_to_string(char *out,int commandlevel);
void AT_enum_to_string(char *out,int AT);
void cover_enum_to_string(char *out, int cover);
void actions_enum_to_string(char *out, int action);
void flags_to_string(char *out,int flags);
void flags_to_status(char *out,int flags);
void flags_to_role(char *out,int uflags,int wflags);


void raylib_init();
Camera raylib_init_camera();
void raylib_init_textures();
void raylib_init_models(Unit *units);
void raylib_free_models(Unit *units);

Rectangle *UNIT_ICON_RECTANGLES;
/*
TODO:
rectangle anti-collision
*/
Rectangle ui_clamp_icon_rectangle(Rectangle rectangle);
Rectangle ui_get_icon_rectangle(Unit unit,Camera camera);
void ui_draw_unit_icons(Unit *units,Camera camera);
void ui_draw_selected_summary();
Unit *ui_select_unit(Unit *units);
Vector3 raylib_select_position(Camera camera);

void ui_init();
void ui_draw(Camera camera);
void ui_end();

void raylib_draw(Unit *units,Camera *camera);
void raylib_input(Unit *units,Camera camera);
void raylib_end();

void run_game(Unit *units);
// globals
Terraintile **read_map_file(const char *mapfile,const char *terrainfile);
void map_draw();
Tile get_tile_at(Vector3 position);

Terraintile **MAP;
/*
TileType
  int cover;
  bool visible;
*/
TileType TERRAINTILES[NUMTERRAINTYPES];
int MAPWIDTH;
int MAPHEIGHT;

Unit *selected;

int SCREENWIDTH;
int SCREENHEIGHT;

int CURRENTCOMMAND;

int NUMUNITS;
int NUMTEAMS;
int NUMWEAPONS;

Weapon *WEAPONTABLE;

int **TEAMIDS;
/*
[TEAMID]
  0: # of units in team
  x: unitid x
*/
