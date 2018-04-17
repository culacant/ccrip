#include "raylib.h"

#define RAYMATH_IMPLEMENTATION
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "enums.h"
/*
███████ ████████ ██████  ██    ██  ██████ ████████ ███████
██         ██    ██   ██ ██    ██ ██         ██    ██
███████    ██    ██████  ██    ██ ██         ██    ███████
     ██    ██    ██   ██ ██    ██ ██         ██         ██
███████    ██    ██   ██  ██████   ██████    ██    ███████
*/
typedef struct Tile Tile;
typedef struct Terraintile Terraintile;
typedef struct TileType TileType;
typedef struct Weapon Weapon;
typedef struct Figure Figure;
typedef struct Unit Unit;
typedef struct Team Team;
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
  int spottingtimer;
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
struct Team
{
  Figure **spots;
  int *unitids;
  int unitcnt;
};

/*
██████  ███████ ██████  ██    ██  ██████
██   ██ ██      ██   ██ ██    ██ ██
██   ██ █████   ██████  ██    ██ ██   ███
██   ██ ██      ██   ██ ██    ██ ██    ██
██████  ███████ ██████   ██████   ██████
*/
void DebugLog(int debuglvl,const char *format, ...);

/*
██████  ██  ██████ ███████
██   ██ ██ ██      ██
██   ██ ██ ██      █████
██   ██ ██ ██      ██
██████  ██  ██████ ███████
*/
int roll_die(int sides);
int roll_die_number(int sides, int target);
int roll_die_die(int sides1, int sides2);
int roll_die_dice(const int *sides1, int numdice, int sides2, int *total);

int closedshift(int sides,int dir);
void openshift(int sides1, int dir,int sides2,int *result);

/*
████████ ██  ██████ ██   ██ ███████
   ██    ██ ██      ██  ██  ██
   ██    ██ ██      █████   ███████
   ██    ██ ██      ██  ██       ██
   ██    ██  ██████ ██   ██ ███████
*/
void tick_units(Unit *units,int deltatime);

/*
 ██████  ██████  ███    ███ ███    ███  █████  ███    ██ ██████  ███████
██      ██    ██ ████  ████ ████  ████ ██   ██ ████   ██ ██   ██ ██
██      ██    ██ ██ ████ ██ ██ ████ ██ ███████ ██ ██  ██ ██   ██ ███████
██      ██    ██ ██  ██  ██ ██  ██  ██ ██   ██ ██  ██ ██ ██   ██      ██
 ██████  ██████  ██      ██ ██      ██ ██   ██ ██   ████ ██████  ███████
*/
int command_fire(Unit *unit1,Unit *unit2);
int command_move(Unit *unit,Vector3 position);
int command_idle(Unit *unit);

/*
██    ██ ███    ██ ██ ████████      █████   ██████ ████████ ██  ██████  ███    ██ ███████
██    ██ ████   ██ ██    ██        ██   ██ ██         ██    ██ ██    ██ ████   ██ ██
██    ██ ██ ██  ██ ██    ██        ███████ ██         ██    ██ ██    ██ ██ ██  ██ ███████
██    ██ ██  ██ ██ ██    ██        ██   ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
 ██████  ██   ████ ██    ██        ██   ██  ██████    ██    ██  ██████  ██   ████ ███████
*/
int do_action_rally(Unit unit1, Unit *unit2);
int do_action_regroup(Unit *unit1, Unit *unit2);

/*
███████ ██  ██████  ██    ██ ██████  ███████      █████   ██████ ████████ ██  ██████  ███    ██ ███████
██      ██ ██       ██    ██ ██   ██ ██          ██   ██ ██         ██    ██ ██    ██ ████   ██ ██
█████   ██ ██   ███ ██    ██ ██████  █████       ███████ ██         ██    ██ ██    ██ ██ ██  ██ ███████
██      ██ ██    ██ ██    ██ ██   ██ ██          ██   ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
██      ██  ██████   ██████  ██   ██ ███████     ██   ██  ██████    ██    ██  ██████  ██   ████ ███████
*/
int do_action_move(Unit *unit,int figureid);

int do_action_spot_figure(Figure spotter,Figure defender);
int do_action_spot(Unit unit, int spotterid);

// TODO
int do_action_closecombat(Figure *attacker,Figure *defender);
int do_action_fire(Figure attacker,Figure *defender);
int do_action_fire_position(Figure attacker,Vector3 position);
int do_action_unpanic(Figure *figure);
int do_action_unsuppress(Figure *figure);
int do_action_move_in_position(Figure *figure);

/*
███████ ██  ██████  ██    ██ ██████  ███████      ██████  ███████ ████████ ███████ ███████ ████████
██      ██ ██       ██    ██ ██   ██ ██          ██       ██         ██    ██      ██         ██
█████   ██ ██   ███ ██    ██ ██████  █████       ██   ███ █████      ██    ███████ █████      ██
██      ██ ██    ██ ██    ██ ██   ██ ██          ██    ██ ██         ██         ██ ██         ██
██      ██  ██████   ██████  ██   ██ ███████      ██████  ███████    ██    ███████ ███████    ██
*/
Vector3 get_position_formation(Unit unit, int figureid);
int get_range_die(Figure attacker,Figure defender,int cover);
int get_leader_id(Unit unit);
int get_cover(Figure attacker, Figure defender);
int get_cover_position(Vector3 position, Figure defender);
int get_figure_can_do_action(Figure figure);
int get_figure_suppressed(Figure figure);

void set_suppressed(Figure *figure);
void set_unsuppressed(Figure *figure);
void set_casualty(Figure *figure, int IV,int cover);

void set_spotted_figure(int teamid,Figure *defender);
void remove_spotted_figure(int teamid,Figure *defender);
int is_spotted_figure(int teamid,Figure *defender);
/*
██    ██ ███    ██ ██ ████████      ██████  ███████ ████████ ███████ ███████ ████████
██    ██ ████   ██ ██    ██        ██       ██         ██    ██      ██         ██
██    ██ ██ ██  ██ ██    ██        ██   ███ █████      ██    ███████ █████      ██
██    ██ ██  ██ ██ ██    ██        ██    ██ ██         ██         ██ ██         ██
 ██████  ██   ████ ██    ██         ██████  ███████    ██    ███████ ███████    ██
*/
int get_unit_can_do_action(Unit unit);
int get_unit_confidence(Unit unit);
Vector3 get_unit_position(Unit unit);
int get_next_action_figure(Unit unit,int figureid);
Figure *get_enemy_target_figure(Unit *unit,int figureid);

Unit *get_enemy_target_unit_spot(Unit *unit,int figureid);
void set_confidence(Unit *unit, int dconfidence);
int set_replacement_leader(Unit *unit);

/*
███████ ██  ██████  ██    ██ ██████  ███████     ████████ ███████ ███████ ████████ ███████
██      ██ ██       ██    ██ ██   ██ ██             ██    ██      ██         ██    ██
█████   ██ ██   ███ ██    ██ ██████  █████          ██    █████   ███████    ██    ███████
██      ██ ██    ██ ██    ██ ██   ██ ██             ██    ██           ██    ██         ██
██      ██  ██████   ██████  ██   ██ ███████        ██    ███████ ███████    ██    ███████
*/
int test_confidence(Figure *figure, int threatlevel);
int test_reaction(Figure figure, int threatlevel);
int test_panic(Figure figure);
int test_figure_exists(Figure figure);
// TODO
int test_figure_move_finished(Figure figure);
int test_figure_LOS(Figure figure1, Figure figure2);
int test_figure_selectable(const Figure *figure1,const Figure *figure2);
int figure_in_cover_from_attacker(Figure attacker,Figure defender);
int figure_in_cover_from_position(Vector3 position,Figure defender);

/*
██    ██ ███    ██ ██ ████████     ████████ ███████ ███████ ████████ ███████
██    ██ ████   ██ ██    ██           ██    ██      ██         ██    ██
██    ██ ██ ██  ██ ██    ██           ██    █████   ███████    ██    ███████
██    ██ ██  ██ ██ ██    ██           ██    ██           ██    ██         ██
 ██████  ██   ████ ██    ██           ██    ███████ ███████    ██    ███████
*/
int test_unit_selectable(const Unit *unit1,const Unit *unit2);
int test_unit_LOS(Unit unit1,Unit unit2);
// TODO
int test_unit_action_finished(Unit unit);
int test_communicate(Unit unit1,Unit unit2);
int test_facetoface(Unit unit1,Unit unit2);

/*
██████  ██████  ██ ███    ██ ████████ ███████
██   ██ ██   ██ ██ ████   ██    ██    ██
██████  ██████  ██ ██ ██  ██    ██    ███████
██      ██   ██ ██ ██  ██ ██    ██         ██
██      ██   ██ ██ ██   ████    ██    ███████
*/
void print_unit(Unit unit);
void print_teams();
void print_weaponstable();

/*
██████  ███████  █████  ██████  ███████
██   ██ ██      ██   ██ ██   ██ ██
██████  █████   ███████ ██   ██ ███████
██   ██ ██      ██   ██ ██   ██      ██
██   ██ ███████ ██   ██ ██████  ███████
*/
Unit* read_units_file(const char *filename);
void read_weapontable_file(const char *filename);
Terraintile **read_map_file(const char *mapfile,const char *terrainfile);

/*
███████ ████████ ██████  ██ ███    ██  ██████  ███████
██         ██    ██   ██ ██ ████   ██ ██       ██
███████    ██    ██████  ██ ██ ██  ██ ██   ███ ███████
     ██    ██    ██   ██ ██ ██  ██ ██ ██    ██      ██
███████    ██    ██   ██ ██ ██   ████  ██████  ███████
*/
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
void loglevel_to_string(char *out,int loglvl);

/*
██████   █████  ██    ██ ██      ██ ██████
██   ██ ██   ██  ██  ██  ██      ██ ██   ██
██████  ███████   ████   ██      ██ ██████
██   ██ ██   ██    ██    ██      ██ ██   ██
██   ██ ██   ██    ██    ███████ ██ ██████
*/
void raylib_init();
Camera raylib_init_camera();
void raylib_init_textures();
void raylib_init_models(Unit *units);
void raylib_free_models(Unit *units);

void raylib_draw(Unit *units,Camera *camera);
void raylib_input(Unit *units,Camera camera);
void raylib_end();

/*
██    ██ ██
██    ██ ██
██    ██ ██
██    ██ ██
 ██████  ██
*/
/*
TODO:
rectangle anti-collision
*/

void ui_init();
void ui_draw(Camera camera);
void ui_end();

void ui_draw_unit_icons(Unit *units,Camera camera);
void ui_draw_selected_summary();

Rectangle ui_clamp_icon_rectangle(Rectangle rectangle);
Rectangle ui_get_icon_rectangle(Unit unit,Camera camera);

Unit *ui_select_unit(Unit *units);
Vector3 raylib_select_position(Camera camera);

/*
███    ███  █████  ██████
████  ████ ██   ██ ██   ██
██ ████ ██ ███████ ██████
██  ██  ██ ██   ██ ██
██      ██ ██   ██ ██
*/
void map_draw();
Vector2 get_tile_xy_at(Vector3 position);
int get_map_line_value(Vector2 start,Vector2 end,int value);

/*
██████  ██    ██ ███    ██
██   ██ ██    ██ ████   ██
██████  ██    ██ ██ ██  ██
██   ██ ██    ██ ██  ██ ██
██   ██  ██████  ██   ████
*/
void run_game(Unit *units);

/*
 ██████  ██       ██████  ██████   █████  ██      ███████
██       ██      ██    ██ ██   ██ ██   ██ ██      ██
██   ███ ██      ██    ██ ██████  ███████ ██      ███████
██    ██ ██      ██    ██ ██   ██ ██   ██ ██           ██
 ██████  ███████  ██████  ██████  ██   ██ ███████ ███████
*/
int MAPWIDTH;
int MAPHEIGHT;

int SCREENWIDTH;
int SCREENHEIGHT;

Terraintile **MAP;
TileType TERRAINTILES[NUMTERRAINTYPES];

int CURRENTCOMMAND;

int NUMUNITS;
int NUMTEAMS;
int NUMWEAPONS;

Unit *units;
Unit *selected;
//int **TEAMIDS;
Team *TEAMS;
int PLAYERTEAM = 0;

Weapon *WEAPONTABLE;

Rectangle *UNIT_ICON_RECTANGLES;
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
  1000, // ACTION_SPOT
};

/*
████████ ███████ ███████ ████████
   ██    ██      ██         ██
   ██    █████   ███████    ██
   ██    ██           ██    ██
   ██    ███████ ███████    ██
*/
Model blastmarker;
Vector3 blastpos;

// projectile stuff
typedef struct Projectile Projectile;
typedef struct ProjectileType ProjectileType;
struct Projectile
{
  Vector3 position;
  Vector3 target;
  Vector3 velocity;
  int TTL;
  int type;
};
struct ProjectileType
{
  Model model;
  Vector3 basevelocity;
  float speed;
};
void initprojectiletypes();
void addprojectile(Vector3 position,Vector3 target,int type);
void deleteprojectile(int id);
void endprojectiles();
void drawprojectiles();
void tickprojectiles(int deltatime);
#define MAXPROJECTILES 25
#define NUMPROJECTILETYPES 2
Projectile PROJECTILES[MAXPROJECTILES];
ProjectileType *PROJECTILETYPES;
/*
projectiles:
spawn with speed and model at firer position and TTL
3 animations: fire,flight, impact
*/
