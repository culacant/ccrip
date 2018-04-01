#define MAXFIGURES 12
#define NUMTEXTURES 4

#define TYPEHEIGHT 32
#define TYPEWIDTH 64
#define ICONHEIGHT 32
#define ICONWIDTH  32

#define SUMMARYWIDTH 256
#define SUMMARYHEADERHEIGHT 50+2
#define SUMMARYFIGUREHEIGHT 32

#define NUMFORMATIONS 2

#define MAXNAMELENGTH 25

#define FACETOFACEDISTANCE 5
#define SELECTDISTANCE 5
#define MOVEDISTANCE 0.5

enum LOGLEVELS
{
  LOGROLLS       = 1<<0,
  LOGACTIONS     = 1<<1,
  LOGCONTROLS    = 1<<2,
  LOGINFO        = 1<<3,
  LOGTEST        = 1<<4,
};
#define LOGLEVEL (LOGTEST | LOGROLLS)

enum TEXTUREFILES
{
  TEXTURE_FLAGS     = 0,
  TEXTURE_TYPES     = 1,
  TEXTURE_SUMMARY   = 2,
  TEXTURE_WEAPONS   = 3,
  TEXTURE_FORMATION = 4,
};
enum ICON
{
  ICON_CONFIDENT  = 0,
  ICON_STEADY     = 1,
  ICON_SHAKEN     = 2,
  ICON_BROKEN     = 3,
  ICON_ROUTED     = 4,
  ICON_HEALED     = 5,
  ICON_DEAD       = 6,
  ICON_CASUALTY   = 7,
  ICON_SUPPRESSED = 8,

  ICON_INFANTRY   = 0,
};

enum DIE
{
  D4  = 4,
  D6  = 6,
  D8  = 8,
  D10 = 10,
  D12 = 12,
};

enum MISSION_MOTIVATION
{
  LOW     = 0,
  MEDIUM  = 1,
  HIGH    = 2,
};
enum FATIGUE_LEVEL
{
  EXHAUSTED = 0,
  TIRED     = 1,
  FRESH     = 2,
};

enum COMMANDLEVEL
{
  BATTALION = 0,
  COMPANY   = 1,
  PLATOON   = 2,
  SQUAD     = 3,
};

enum LEADERSHIP
{
  BEST    = 1,
  AVERAGE = 2,
  POOR    = 3,
};

enum QUALITY
{
  ELITE     = 12,
  VETERAN   = 10,
  REGULAR   = 8,
  GREEN     = 6,
  UNTRAINED = 4,
};

enum CONFIDENCE
{
  CONFIDENT = 12,
  STEADY    = 10,
  SHAKEN    = 8,
  BROKEN    = 6,
  ROUTED    = 4,
};

enum COVER
{
  NONE        = 1<<0,
  IN_POSITION = 1<<1,
  SOFT        = 1<<2,
  HARD        = 1<<3,
};

enum ARMOR_TYPE
{
  BASIC         = 4,
  PARTIAL_LIGHT = 6,
  FULL_LIGHT    = 8,
  POWER_LIGHT   = 10,
  POWER_HEAVY   = 12,
};

enum FLAGS
{
  EXISTS      = 1<<0,
  CASUALTY    = 1<<1,
  DEAD        = 1<<2,
  SUPPRESSED  = 1<<3,
  PANICKED    = 1<<4,
  SUPPORT     = 1<<5,
  CLOSERANGE  = 1<<6,
  TERROR      = 1<<7,
  LEADER      = 1<<8,

  DETACHED,
  LASTSTAND,

};

enum FORMATIONS
{
  LINE    = 0,
  COLLUMN = 1,
};

enum ACTIONS
{
  ACTION_CLOSECOMBAT    =  0,
  ACTION_UNPANIC        =  1,
  ACTION_UNSUPPRESS     =  2,
  ACTION_FIRE           =  3,
  ACTION_NOTARGET       =  4,
  ACTION_NOAMMO         =  5,
  ACTION_MOVE           =  6,
  ACTION_MOVE_POSITION  =  7,
  ACTION_RALLY          =  8,
  ACTION_REGROUP        =  9,
  ACTION_IDLE           = 10,
  ACTION_CASUALTY       = 11,
};
enum COMMANDS
{
  COMMAND_IDLE          = 0,
  COMMAND_FIRE          = 1,
  COMMAND_MOVE          = 2,
// COMMAND_INTERRUPTED ?
};

#define TILERES 10
#define TERRAINRES 10
#define NUMTERRAINTYPES 1
enum TERRAINTYPESENUM
{
  TERRAIN_FLAT          = 0,
};
