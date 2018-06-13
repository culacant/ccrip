#include "stargrunts.h"
#include "keybinds.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <math.h>

/*
██████  ███████ ██████  ██    ██  ██████
██   ██ ██      ██   ██ ██    ██ ██
██   ██ █████   ██████  ██    ██ ██   ███
██   ██ ██      ██   ██ ██    ██ ██    ██
██████  ███████ ██████   ██████   ██████
*/
void DebugLog(int loglvl,const char *format, ...)
{
  va_list args;
  va_start(args, format);
  if(LOGLEVEL & loglvl)
  {
    char logstr[MAXNAMELENGTH];
    loglevel_to_string(logstr,loglvl);
    printf("%s",logstr);
    vprintf(format,args);
  }
  va_end(args);
}

/*
███    ███  █████  ████████ ██   ██
████  ████ ██   ██    ██    ██   ██
██ ████ ██ ███████    ██    ███████
██  ██  ██ ██   ██    ██    ██   ██
██      ██ ██   ██    ██    ██   ██
*/
float Vector3DistanceSqr(Vector3 v1, Vector3 v2)
{
  float dx = v2.x - v1.x;
  float dy = v2.y - v1.y;
  float dz = v2.z - v1.z;
  float result = dx*dx + dy*dy + dz*dz;
  return result;
}
float Vector3LengthSqr(const Vector3 v)
{
  float result = v.x*v.x + v.y*v.y + v.z*v.z;
  return result;
}

/*
██████  ██  ██████ ███████
██   ██ ██ ██      ██
██   ██ ██ ██      █████
██   ██ ██ ██      ██
██████  ██  ██████ ███████
*/
int roll_die(int sides)
{
  int roll = rand()%sides+1;
  DebugLog(LOGROLLS,"d%i rolled: %i\n",sides,roll);
  return roll;
}
int roll_die_number(int sides, int target)
{
  int roll = roll_die(sides);
  if(roll > target)
  {
    if(roll >= 2*target)
    {
      DebugLog(LOGROLLS,"target %i critical success\n",target);
      return 2;
    }
    DebugLog(LOGROLLS,"target %i success\n",target);
    return 1;
  }
  if(roll*2 <= target)
  {
    DebugLog(LOGROLLS,"target %i critical fail\n",target);
    return -1;
  }
  DebugLog(LOGROLLS,"target %i fail\n",target);
  return 0;
}
int roll_die_die(int sides1, int sides2)
{
  int roll1 = roll_die(sides1);
  int roll2 = roll_die(sides2);
  if(roll1 > roll2)
  {
    if(roll1 >= 2*roll2)
    {
      return 2;
    }
    return 1;
  }
  return 0;
}
int roll_die_dice(const int *sides1, int numdice, int sides2, int *total)
{
  int numwins = 0;
  int die1;
  int die2 = roll_die(sides2);
  if(total)
    *total = 0;
  for(int i=0;i<numdice;i++)
  {
    die1 = roll_die(sides1[i]);
    if(total)
      *total += die1;
    if(die1 > die2)
      numwins++;
  }
  if(numwins >= 2)
    return 2;
  return numwins;
}

int closedshift(int sides,int dir)
{
  DebugLog(LOGROLLS,"closed shift: %i ",sides);
  sides += 2 *dir;
  if(sides < 4)
    sides = 4;
  if(sides > 12)
    sides = 12;
  DebugLog(LOGROLLS,"is now : %i\n",sides);
  return sides;
}
void openshift(int sides1, int dir, int sides2, int *result)
{
  DebugLog(LOGROLLS,"open shift: %i, %i ",sides1,sides2);
  sides1 += 2 *dir;
  if(sides1 < 4)
  {
    sides1 = 4;
    sides2 = closedshift(sides2,+1);
  }
  if(sides1 > 12)
  {
    sides2 = closedshift(sides2,-1);
    sides1 = 12;
  }
  result[0] = sides1;
  result[1] = sides2;
  DebugLog(LOGROLLS,"are now: %i, %i\n",result[0],result[1]);
}

/*
████████ ██  ██████ ██   ██ ███████
   ██    ██ ██      ██  ██  ██
   ██    ██ ██      █████   ███████
   ██    ██ ██      ██  ██       ██
   ██    ██  ██████ ██   ██ ███████
*/
void units_tick(Unit *units,int deltatime)
{
  Figure *figure = NULL;
  Figure *figuretarget = NULL;
  Unit *unit = NULL;
  for(int i=0;i<NUMUNITS;i++)
  {
    unit = &units[i];
    if(get_unit_can_do_action(*unit))
    {
      for(int j=0;j<MAXFIGURES;j++)
      {
        figure = &unit->figures[j];
        if(test_figure_exists(*figure))
        {
// rotation and movement
          figure->model.transform = MatrixRotateY(figure->facing);
          figure->actiontimer -= deltatime;
          figure->spottingtimer -= deltatime;
          if(figure->spottingtimer< 0)
          {
            do_action_spot(*unit,j);
            figure->spottingtimer = ACTIONTIMERS[ACTION_SPOT];
          }
          if(figure->actiontimer < 0)
          {
            switch(figure->action)
            {
              case ACTION_CLOSECOMBAT:
                break;
              case ACTION_UNPANIC:
                do_action_unpanic(figure);
                break;
              case ACTION_UNSUPPRESS:
                do_action_unsuppress(figure);
                break;
              case ACTION_FIRE:
              case ACTION_NOTARGET:
                if(figure->enemytarget)
                {
                  figure->ammo -= figure->burst;
                  projectile_add(figure->position,figure->enemytarget->position,figure->projectiletype);
                  if(figure->wflags&BLAST)
                    do_action_fire_position(*figure,figure->enemytarget->position);
                  else
                    do_action_fire(*figure,figure->enemytarget);
                }
                break;
              case ACTION_MOVE:
                figure->doingcommand = do_action_move(unit,j);
                DebugLog(LOGACTIONS,"%i\n",figure->doingcommand);
                break;
              case ACTION_GO_PRONE:
                do_action_go_prone(figure);
                break;
              case ACTION_RALLY:
                do_action_rally(*unit,unit->friendlytarget);
                break;
              case ACTION_REGROUP:
                do_action_regroup(unit,unit->friendlytarget);
                break;
              case ACTION_IDLE:
                break;
            }
            figure->action = get_next_action_figure(*unit,j);

            if(figure->action == ACTION_MOVE)
              figure->actiontimer = figure->movespeed;
            else
            {
              figure->actiontimer = ACTIONTIMERS[figure->action];
              switch(figure->action)
              {
                case ACTION_FIRE:
                  if(figure->ammo <= 0)
                  {
                    figure->ammo = 0;
                    DebugLog(LOGACTIONS,"ACTION_NOAMMO\n");
                    figure->action = ACTION_NOAMMO;
                    break;
                  }
                  figure->enemytarget = get_enemy_target_figure(unit,j);
                  if(figure->enemytarget == NULL)
                  {
                    DebugLog(LOGACTIONS,"ACTION_NOTARGET\n");
                    figure->action = ACTION_NOTARGET;
                  }
                  break;
              }
            }
          }
        }
      }
    }
  }
}
void projectiles_tick(int deltatime)
{
  for(int i=0;i<MAXPROJECTILES;i++)
  {
    if(PROJECTILES[i].TTL > 0)
    {
      Vector3 velocity = Vector3Multiply(PROJECTILES[i].velocity,deltatime);
      PROJECTILES[i].position = Vector3Add(PROJECTILES[i].position,velocity);
      PROJECTILES[i].TTL -= Vector3Length(velocity);
    }
  }
}

/*
 ██████  ██████  ███    ███ ███    ███  █████  ███    ██ ██████  ███████
██      ██    ██ ████  ████ ████  ████ ██   ██ ████   ██ ██   ██ ██
██      ██    ██ ██ ████ ██ ██ ████ ██ ███████ ██ ██  ██ ██   ██ ███████
██      ██    ██ ██  ██  ██ ██  ██  ██ ██   ██ ██  ██ ██ ██   ██      ██
 ██████  ██████  ██      ██ ██      ██ ██   ██ ██   ████ ██████  ███████
*/
int command_fire(Unit *unit1,Unit *unit2)
{
  unit1->command = COMMAND_FIRE;
  unit1->enemytarget = unit2;
  for(int i=0;i<MAXFIGURES;i++)
    unit1->figures[i].doingcommand = 1;
  return 1;
}
int command_move(Unit *unit,Vector3 position)
{
  unit->goal = position;
  unit->command = COMMAND_MOVE;
  unit->facing = atan2f(unit->goal.z-get_unit_position(*unit).z,unit->goal.x-get_unit_position(*unit).x);
  DebugLog(LOGACTIONS,"unit->command = %i\n",unit->command);
  for(int i=0;i<MAXFIGURES;i++)
    unit->figures[i].doingcommand = 1;
  return 1;
}
int command_idle(Unit *unit)
{
  unit->command = COMMAND_IDLE;
  return 1;
}

/*
██████  ██████   ██████       ██ ███████  ██████ ████████ ██ ██      ███████ ███████
██   ██ ██   ██ ██    ██      ██ ██      ██         ██    ██ ██      ██      ██
██████  ██████  ██    ██      ██ █████   ██         ██    ██ ██      █████   ███████
██      ██   ██ ██    ██ ██   ██ ██      ██         ██    ██ ██      ██           ██
██      ██   ██  ██████   █████  ███████  ██████    ██    ██ ███████ ███████ ███████
*/
void projectiles_init()
{
  PROJECTILETYPES = malloc(sizeof(ProjectileType)*NUMPROJECTILETYPES);
  PROJECTILETYPES[0].model = LoadModel("resources/bullet.obj");
  PROJECTILETYPES[0].basevelocity = (Vector3){0.0f,0.0f,0.0f};
  PROJECTILETYPES[0].speed = 1;
  for(int i=0;i<MAXPROJECTILES;i++)
  {
    PROJECTILES[i].TTL = -1;
  }
}
void projectiles_end()
{
  free(PROJECTILETYPES);
}
void projectiles_draw()
{
  for(int i=0;i<MAXPROJECTILES;i++)
  {
    if(PROJECTILES[i].TTL > 0)
    {
      DrawModel(PROJECTILETYPES[PROJECTILES[i].type].model,PROJECTILES[i].position,1.0f,YELLOW);
    }
  }
}
void projectile_add(Vector3 position,Vector3 target,int type)
{
  for(int i=0;i<MAXPROJECTILES;i++)
  {
    if(PROJECTILES[i].TTL < 0)
    {
      PROJECTILES[i].position = position;
      PROJECTILES[i].target = target;
      float angle = atan2f(position.z-target.z,position.x-target.x);
      float speed = PROJECTILETYPES[PROJECTILES[i].type].speed;
      PROJECTILES[i].velocity.x = cosf(angle)*speed*-1/10;
      PROJECTILES[i].velocity.y = 0;
      PROJECTILES[i].velocity.z  = sinf(angle)*speed*-1/10;
      PROJECTILES[i].TTL = Vector3Distance(position,target);
      PROJECTILES[i].type = type;
      break;
    }
  }
}

/*
██    ██ ███    ██ ██ ████████      █████   ██████ ████████ ██  ██████  ███    ██ ███████
██    ██ ████   ██ ██    ██        ██   ██ ██         ██    ██ ██    ██ ████   ██ ██
██    ██ ██ ██  ██ ██    ██        ███████ ██         ██    ██ ██    ██ ██ ██  ██ ███████
██    ██ ██  ██ ██ ██    ██        ██   ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
 ██████  ██   ████ ██    ██        ██   ██  ██████    ██    ██  ██████  ██   ████ ███████
*/
int do_action_rally(Unit unit1, Unit *unit2)
{
  DebugLog(LOGROLLS,"do_action_rally\n");
  if(!test_communicate(unit1,*unit2))
    return 0;
  int leaderid1 = get_leader_id(unit1);
  int leaderid2 = get_leader_id(*unit2);
  int totalLV = unit1.figures[leaderid1].LV + unit2->figures[leaderid2].LV;
  DebugLog(LOGROLLS,"total LV: %i\n",totalLV);
  int result = roll_die_number(unit2->figures[leaderid2].quality,totalLV);
  if(result > 0)
  {
    set_confidence(unit2,1);
    DebugLog(LOGROLLS,"success\n");
    return 1;
  }
  DebugLog(LOGROLLS,"failure\n");
  return 0;
}
int do_action_regroup(Unit *unit1,Unit *unit2)
{
  DebugLog(LOGROLLS,"do_action_regroup\n");
// set new leader
  int leaderid1 = get_leader_id(*unit1);
  int leaderid2 = get_leader_id(*unit2);
  if(unit1->figures[leaderid1].LV > unit2->figures[leaderid2].LV)
  {
    unit2->figures[leaderid2].flags &= ~LEADER;
    DebugLog(LOGROLLS,"Unit 2 leader is new leader\n");
  }
  else
  {
    unit2->figures[leaderid2].flags &= ~LEADER;
    DebugLog(LOGROLLS,"Unit 1 leader is new leader\n");
  }

  for(int i=0;i<MAXFIGURES;i++)
  {
    if(test_figure_exists(unit2->figures[i]))
    {
      for(int j=0;j<MAXFIGURES;j++)
      {
        if(!test_figure_exists(unit1->figures[j]))
        {
          unit1->figures[j] = unit2->figures[i];
        }
      }
    }
  }
  unit1->figurecnt += unit2->figurecnt;
  DebugLog(LOGROLLS,"success, new unit count: %i\n",unit1->figurecnt);
  return 1;
}

/*
███████ ██  ██████  ██    ██ ██████  ███████      █████   ██████ ████████ ██  ██████  ███    ██ ███████
██      ██ ██       ██    ██ ██   ██ ██          ██   ██ ██         ██    ██ ██    ██ ████   ██ ██
█████   ██ ██   ███ ██    ██ ██████  █████       ███████ ██         ██    ██ ██    ██ ██ ██  ██ ███████
██      ██ ██    ██ ██    ██ ██   ██ ██          ██   ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
██      ██  ██████   ██████  ██   ██ ███████     ██   ██  ██████    ██    ██  ██████  ██   ████ ███████
*/
int do_action_move(Unit *unit,int figureid)
{
  Figure *figure = &unit->figures[figureid];
  if(get_figure_can_do_action(*figure))
  {
    Vector3 goalpos = get_position_formation(*unit,figureid);
    figure->facing = atan2f(goalpos.z-figure->position.z,goalpos.x-figure->position.x);
    Vector3 movepos;
    movepos.x = cosf(figure->facing);
    movepos.z = sinf(figure->facing);
    movepos.y = 0.0f;
    figure->position = Vector3Add(figure->position,movepos);
    if(Vector3DistanceSqr(figure->position,goalpos) < MOVEDISTANCE*MOVEDISTANCE)
    {
      return 0;
    }
    return 1;
  }
  else
    return 0;
}

int do_action_spot_figure(Figure spotter,Figure defender)
{
  if(!test_figure_LOS(spotter,defender))
    return 0;

  int cover = get_cover(spotter,defender);
  int rangedie = get_range_die(spotter, defender,cover);

  int numdice = 2;
  int *Adice = malloc(sizeof(int)*numdice);
  int total;
  Adice[0] = spotter.quality;
// sensor die
  Adice[1] = 4;
  int result = roll_die_dice(Adice,numdice,rangedie,&total);
  free(Adice);
// if not idle (i e busy), result must be 2 to win
  if(spotter.action != ACTION_IDLE)
  {
    result = result/2;
  }
  return result;
}
int do_action_spot(Unit unit, int spotterid)
{
  float angle;
  float angled;
  int retval = 0;
  int alreadyspotted = 0;
  Figure spotter = unit.figures[spotterid];
  Figure *defender = NULL;
  for(int i=0;i<NUMTEAMS;i++)
  {
    if(i!=unit.team)
    {
      for(int j=0;j<TEAMS[i].unitcnt;j++)
      {
        for(int h=0;h<MAXFIGURES;h++)
        {
          defender = &units[TEAMS[i].unitids[j]].figures[h];
          angle = atan2f(spotter.position.z-defender->position.z,spotter.position.x-defender->position.z);
          angled = fmodf(spotter.facing-angle+PI+2*PI,2*PI)-PI;
          //if(angled<=45&&angled>=-45&&test_figure_LOS(spotter,*defender))
          if(test_figure_LOS(spotter,*defender))
          {
            if(!is_spotted_figure(unit.team,defender))
            {
              if(do_action_spot_figure(spotter,*defender))
              {
                set_spotted_figure(unit.team,defender);
                retval = 1;
              }
            }
          }
          else
            remove_spotted_figure(unit.team,defender);
        }
      }
    }
  }
  return retval;
}

int do_action_closecombat(Figure *attacker,Figure *defender)
{
//TODO
}
int do_action_fire(Figure attacker,Figure *defender)
{
  DebugLog(LOGROLLS,"do_action_fire\n");
  int numdice = 2;
  int *Adice = malloc(sizeof(int)*numdice);
  Adice[0] = attacker.quality;
  Adice[1] = attacker.FP;
  int cover = get_cover(attacker,*defender);
  int rangedie = get_range_die(attacker,*defender,cover);

  int total;

  int result = roll_die_dice(Adice,numdice,rangedie,&total);
  switch(result)
  {
    case 0:
      break;
    case 1:
      set_suppressed(defender);
      break;
    case 2:
      set_casualty(defender,attacker.IV,cover);
      break;
  }
  free(Adice);
  return result;
}
int do_action_fire_position(Figure attacker,Vector3 position)
{
  int direction = roll_die(360);
// inaccurate deviation?
  int deviation = roll_die(8);
  Vector3 finalpos = position;
  finalpos.x += cosf(direction)*deviation;
  finalpos.z += sinf(direction)*deviation;

//test
  blastpos = finalpos;
//endtest

// resolve
  for(int i=0;i<NUMUNITS;i++)
  {
    for(int j=0;j<MAXFIGURES;j++)
    {
      if(test_figure_exists(units[i].figures[j]))
      {
        if(Vector3Distance(finalpos,units[i].figures[j].position)<attacker.FP)
        {
          int cover = get_cover_position(finalpos,units[i].figures[j]);
          set_suppressed(&units[i].figures[j]);
          set_casualty(&units[i].figures[j],attacker.IV,cover);
        }
      }
    }
  }
  return 0;
}
int do_action_unpanic(Figure *figure)
{
  int roll = roll_die(figure->quality);
  if(roll == 1)
  {
    figure->confidence = closedshift(figure->confidence,-1);
    return -1;
  }
  if(roll <= figure->LV)
    return 0;
  figure->confidence = closedshift(figure->confidence,+1);
  figure->flags &= ~PANICKED;
  return 1;
}
int do_action_unsuppress(Figure *figure)
{
  int result = roll_die_number(figure->quality,figure->LV);
  if(result > 0)
  {
    set_unsuppressed(figure);
    return 1;
  }
  return 0;
}
int do_action_go_prone(Figure *figure)
{
  int threatlevel = 2;
//  if(get_cover(*figure) > 0)
//    threatlevel = 0;
  if(test_reaction(*figure,threatlevel) == 0)
    return 0;
  figure->cover |= PRONE;
  return 1;
}

/*
███████ ██  ██████  ██    ██ ██████  ███████      ██████  ███████ ████████ ███████ ███████ ████████
██      ██ ██       ██    ██ ██   ██ ██          ██       ██         ██    ██      ██         ██
█████   ██ ██   ███ ██    ██ ██████  █████       ██   ███ █████      ██    ███████ █████      ██
██      ██ ██    ██ ██    ██ ██   ██ ██          ██    ██ ██         ██         ██ ██         ██
██      ██  ██████   ██████  ██   ██ ███████      ██████  ███████    ██    ███████ ███████    ██
*/
Vector3 get_position_formation(Unit unit, int figureid)
{
    Vector3 position = Vector3Multiply(FORMATIONPOSITION[unit.formation],unit.spread*figureid);
    position = Vector3Transform(position,MatrixRotateY(unit.facing));
    position = Vector3Add(unit.goal,position);
    return position;
}
int get_range_die(Figure attacker,Figure defender,int cover)
{
  int range_die = 4;
  int shifts = cover;
  float range = Vector3Distance(attacker.position,defender.position);
  shifts += (int)(range/attacker.quality);

  for(int i=0;i<shifts;i++)
    range_die += 2;
  DebugLog(LOGINFO,"range: %f\nrange die: D%i\n",range,range_die);
  return range_die;
}
int get_leader_id(Unit unit)
{
  for(int i=0;i<MAXFIGURES;i++)
  {
    if(unit.figures[i].flags & LEADER)
      return i;
  }
  return 0;
}
int get_cover(Figure attacker, Figure defender)
{
  int cover = 0;
  if(defender.cover & PRONE)
    cover++;
  cover += figure_in_cover_from_attacker(attacker,defender);
  return cover;
}
int get_cover_position(Vector3 position, Figure defender)
{
  int cover = 0;
  if(defender.cover & PRONE)
    cover++;
  cover += figure_in_cover_from_position(position,defender);
  return cover;
}
int get_figure_can_do_action(Figure figure)
{
  if((figure.flags & EXISTS)&&!(figure.flags &CASUALTY)&&!(figure.flags & PANICKED)&&!(figure.flags&SUPPRESSED))
    return 1;
  return 0;
}
int get_figure_suppressed(Figure figure)
{
  if((figure.flags & EXISTS)&&!(figure.flags &CASUALTY)&&!(figure.flags & PANICKED)&&!(figure.flags&SUPPRESSED))
    return 1;
  return 0;
}

void set_suppressed(Figure *figure)
{
  figure->suppressed++;
  if(figure->suppressed > MAXSUPPRESSION)
    figure->suppressed = MAXSUPPRESSION;
  figure->flags |= SUPPRESSED;
}
void set_unsuppressed(Figure *figure)
{
  figure->suppressed--;
  if(figure->suppressed <= 0)
  {
    figure->suppressed = 0;
    figure->flags &= ~SUPPRESSED;
  }
}
void set_casualty(Figure *figure, int IV,int cover)
{
  DebugLog(LOGTEST,"set_casualty\n");
  int AT = figure->AT;
  for(int i=0;i<cover;i++)
    AT = closedshift(AT,+1);
  int result = roll_die_die(IV,AT);
  if(result == 1)
  {
    if(figure->flags & CASUALTY)
    {
      DebugLog(LOGTEST,"DEAD\n");
      figure->flags |= DEAD;
    }
    else
    {
      figure->flags |= CASUALTY;
      DebugLog(LOGTEST,"CASUALTY\n");
    }
  }
  else if(result == 2)
  {
    figure->flags |= CASUALTY;
    figure->flags |= DEAD;
    DebugLog(LOGTEST,"DEAD\n");
  }
  else
    DebugLog(LOGTEST,"NONE\n");
}

void set_spotted_figure(int teamid, Figure *defender)
{
  int firstempty = -1;
  for(int i=0;i<MAXSPOTS;i++)
  {
    if(TEAMS[teamid].spots[i] == defender)
      break;
    if(TEAMS[teamid].spots[i] == NULL && firstempty == -1)
      firstempty = i;
  }
  if(firstempty > -1)
    TEAMS[teamid].spots[firstempty] = defender;
}
void remove_spotted_figure(int teamid, Figure *defender)
{
  for(int i=0;i<MAXSPOTS;i++)
  {
    if(TEAMS[teamid].spots[i] == defender)
    {
      TEAMS[teamid].spots[i] = NULL;
      break;
    }
  }
}
int is_spotted_figure(int teamid,Figure *defender)
{
  if(!defender->flags & EXISTS)
    return 0;
  for(int i=0;i<MAXSPOTS;i++)
  {
    if(TEAMS[teamid].spots[i] == defender)
    {
      return 1;
    }
  }
  return 0;
}

/*
██    ██ ███    ██ ██ ████████      ██████  ███████ ████████ ███████ ███████ ████████
██    ██ ████   ██ ██    ██        ██       ██         ██    ██      ██         ██
██    ██ ██ ██  ██ ██    ██        ██   ███ █████      ██    ███████ █████      ██
██    ██ ██  ██ ██ ██    ██        ██    ██ ██         ██         ██ ██         ██
 ██████  ██   ████ ██    ██         ██████  ███████    ██    ███████ ███████    ██
*/
int get_unit_can_do_action(Unit unit)
{
  if(!unit.flags & EXISTS)
    return 0;
  return 1;
}
int get_unit_confidence(Unit unit)
{
  int confidence = 0;
  for(int i=0;i<MAXFIGURES;i++)
  {
    if(test_figure_exists(unit.figures[i]))
      confidence += unit.figures[i].confidence;
  }
  confidence = (int)(confidence/unit.figurecnt);
  confidence += confidence%2;
  return confidence;
}
Vector3 get_unit_position(Unit unit)
{
  Vector3 position = (Vector3){0.0f,0.0f,0.0f};
  for(int i=0;i<MAXFIGURES;i++)
  {
    if(test_figure_exists(unit.figures[i]))
    {
      position = Vector3Add(position,unit.figures[i].position);
    }
  }
  position.x /= unit.figurecnt;
  position.y /= unit.figurecnt;
  position.z /= unit.figurecnt;
  return position;
}
int get_next_action_figure(Unit unit,int figureid)
{
// TODO
  if(unit.figures[figureid].flags & CASUALTY)
  {
    DebugLog(LOGACTIONS,"ACTION_CASUALTY\n");
    return ACTION_CASUALTY;
  }
  if(unit.figures[figureid].flags & PANICKED)
  {
    DebugLog(LOGACTIONS,"ACTION_UNPANIC\n");
    return ACTION_UNPANIC;
  }
  if(unit.figures[figureid].flags & SUPPRESSED)
  {
    DebugLog(LOGACTIONS,"ACTION_UNSUPPRESS\n");
    return ACTION_UNSUPPRESS;
  }
  if(unit.figures[figureid].doingcommand)
  {
    switch(unit.command)
    {
      case COMMAND_MOVE:
        DebugLog(LOGACTIONS,"ACTION_MOVE\n");
        return ACTION_MOVE;
        break;
      case COMMAND_FIRE:
        DebugLog(LOGACTIONS,"ACTION_FIRE\n");
        return ACTION_FIRE;
// NOTARGET and NOAMMO handled in tick_units
        break;
    }
  }

  DebugLog(LOGACTIONS,"ACTION_IDLE\n");
  return ACTION_IDLE;
}
Figure *get_enemy_target_figure(Unit *unit,int figureid)
{
  Unit *unittarget = unit->enemytarget;
  int *targetids = malloc(sizeof(int) * (unittarget->figurecnt+1));
  targetids[0] = 0;
  for(int i=0;i<MAXFIGURES;i++)
  {
    if(test_figure_selectable(&unit->figures[figureid],&unittarget->figures[i]))
    {
      targetids[0]++;
      targetids[targetids[0]] = i;
    }
  }
  if(targetids[0] == 0)
  {
    free(targetids);
    return NULL;
  }
  int targetid = targetids[roll_die(targetids[0])];
  DebugLog(LOGTEST,"targetid: %i\n",targetid);
  free(targetids);
  return &unittarget->figures[targetid];
}

void set_confidence(Unit *unit, int dconfidence)
{
  for(int i=0;i<MAXFIGURES;i++)
  {
    if(test_figure_exists(unit->figures[i]))
      unit->figures[i].confidence = closedshift(unit->figures[i].confidence,dconfidence);
  }
}
int set_replacement_leader(Unit *unit)
{
  int highestLV = 0;
  int newleaderid = -1;
  for(int i=0;i<MAXFIGURES;i++)
  {
    if(test_figure_exists(unit->figures[i]))
    {
      if(unit->figures[i].LV > highestLV)
      {
        newleaderid = i;
        highestLV = unit->figures[i].LV;
      }
    }
  }
  if(newleaderid >= 0)
  {
    unit->figures[newleaderid].flags |= LEADER;
    return 1;
  }
  return 0;
}

/*
███████ ██  ██████  ██    ██ ██████  ███████     ████████ ███████ ███████ ████████ ███████
██      ██ ██       ██    ██ ██   ██ ██             ██    ██      ██         ██    ██
█████   ██ ██   ███ ██    ██ ██████  █████          ██    █████   ███████    ██    ███████
██      ██ ██    ██ ██    ██ ██   ██ ██             ██    ██           ██    ██         ██
██      ██  ██████   ██████  ██   ██ ███████        ██    ███████ ███████    ██    ███████
*/
int test_confidence(Figure *figure, int threatlevel)
{
  int result = roll_die_number(figure->quality,figure->LV+threatlevel);
  if(result>0)
    return figure->confidence;
  return closedshift(figure->confidence,result-1);
}
int test_reaction(Figure figure, int threatlevel)
{
  int result = roll_die_number(figure.quality,figure.LV+threatlevel);
  if(result>0)
    return 1;
  return 0;
}
int test_panic(Figure figure)
{
  int result = test_reaction(figure,0);
  return result;
}
int test_figure_exists(Figure figure)
{
  return figure.flags & EXISTS;
}
int test_figure_move_finished(Figure figure)
{
// TODO
}
int test_figure_LOS(Figure figure1, Figure figure2)
{
  if(!(figure1.flags&EXISTS) || !(figure2.flags&EXISTS))
    return 0;
  int los = get_map_line_value(get_tile_xy_at(figure1.position),get_tile_xy_at(figure2.position),VISIBLE);
  if(los > 0)
    return 0;
  return 1;
}
int test_figure_selectable(const Figure *figure1,const Figure *figure2)
{
  if(figure2 == NULL)
    return 0;
  if(!test_figure_exists(*figure2))
    return 0;
  if(figure2->flags & CASUALTY)
    return 0;
  if(!test_figure_LOS(*figure1,*figure2))
    return 0;
  return 1;
}
int figure_in_cover_from_attacker(Figure attacker,Figure defender)
{
  Vector3 maxcoverpos = Vector3Subtract(attacker.position,defender.position);
  maxcoverpos = Vector3Normalize(maxcoverpos);
  maxcoverpos = Vector3Multiply(maxcoverpos,COVERDISTANCE);
  maxcoverpos = Vector3Add(maxcoverpos,defender.position);
  return get_map_line_value(get_tile_xy_at(defender.position),get_tile_xy_at(maxcoverpos),COVER);
}
int figure_in_cover_from_position(Vector3 position,Figure defender)
{
  Vector3 maxcoverpos = Vector3Subtract(position,defender.position);
  maxcoverpos = Vector3Normalize(maxcoverpos);
  maxcoverpos = Vector3Multiply(maxcoverpos,COVERDISTANCE);
  maxcoverpos = Vector3Add(maxcoverpos,defender.position);
  return get_map_line_value(get_tile_xy_at(defender.position),get_tile_xy_at(maxcoverpos),COVER);
}

/*
██    ██ ███    ██ ██ ████████     ████████ ███████ ███████ ████████ ███████
██    ██ ████   ██ ██    ██           ██    ██      ██         ██    ██
██    ██ ██ ██  ██ ██    ██           ██    █████   ███████    ██    ███████
██    ██ ██  ██ ██ ██    ██           ██    ██           ██    ██         ██
 ██████  ██   ████ ██    ██           ██    ███████ ███████    ██    ███████
*/
int test_unit_selectable(const Unit *unit1,const Unit *unit2)
{
  if(unit2 == NULL)
    return 0;
  if(!(unit2->flags & EXISTS))
    return 0;
  return 1;
}
int test_unit_LOS(Unit unit1, Unit unit2)
{
  for(int i=0;i<MAXFIGURES;i++)
  {
    for(int j=0;j<MAXFIGURES;j++)
    {
      if(test_figure_LOS(unit1.figures[i],unit2.figures[j]))
        return 1;
    }
  }
  return 0;
}
int test_unit_action_finished(Unit unit)
{
// TODO
}
int test_communicate(Unit unit1,Unit unit2)
{
  DebugLog(LOGROLLS,"test_communicate\n");
  if(test_facetoface(unit1,unit2))
  {
    DebugLog(LOGROLLS,"face to face: success\n");
    return 1;
  }
  int leaderid1 = get_leader_id(unit1);
  int leaderid2 = get_leader_id(unit2);
  int lowestLV = unit1.figures[leaderid1].LV;
  if(lowestLV > unit2.figures[leaderid2].LV)
  {
    lowestLV = unit2.figures[leaderid2].LV;
    DebugLog(LOGROLLS,"Unit 2 LV lower\n");
  }
  else
    DebugLog(LOGROLLS,"Unit 1 LV lower\n");
  int commandleveldiff = abs(unit1.commandlevel - unit2.commandlevel -1);
  DebugLog(LOGROLLS,"command level difference: %i\n",commandleveldiff);
  if(commandleveldiff>0)
    lowestLV = closedshift(lowestLV,commandleveldiff);

  int result = roll_die_number(unit1.figures[leaderid1].quality,lowestLV);
  if(result > 0)
  {
    DebugLog(LOGROLLS,"success\n");
    return 1;
  }
  DebugLog(LOGROLLS,"failure\n");
  return 0;
}
int test_facetoface(Unit unit1,Unit unit2)
{
  float distance = Vector3Distance(get_unit_position(unit1),get_unit_position(unit2));
  if(distance < FACETOFACEDISTANCE)
    return 1;
  return 0;
}

/*
██████  ██████  ██ ███    ██ ████████ ███████
██   ██ ██   ██ ██ ████   ██    ██    ██
██████  ██████  ██ ██ ██  ██    ██    ███████
██      ██   ██ ██ ██  ██ ██    ██         ██
██      ██   ██ ██ ██   ████    ██    ███████
*/
void print_unit(Unit unit)
{
  if(!(unit.flags & EXISTS))
  {
    printf("unit does not exist\n");
    return;
  }
  char LV[25];
  char quality[25];
  char confidence[25];
  char commandlevel[25];
  char AT[25];
  char flags[50];
  commandlevel_enum_to_string(commandlevel,unit.commandlevel);
  flags_to_string(flags,unit.flags);
  printf("Unit X:\n"
         "team: %i\n"
         "commandlevel: %s\n"
         "flags: %s\n"
         "figure count: %i\n"
        ,unit.team,commandlevel,flags,unit.figurecnt);
  for(int i=0; i< MAXFIGURES;i++)
  {
    if(test_figure_exists(unit.figures[i]))
    {
      AT_enum_to_string(AT,unit.figures[i].AT);
      lv_enum_to_string(LV,unit.figures[i].LV);
      quality_enum_to_string(quality,unit.figures[i].quality);
      confidence_enum_to_string(confidence,unit.figures[i].confidence);
      flags_to_string(flags,unit.figures[i].flags);
      printf("figure %i:\n"
             "\tweapon: %i\n"
             "\tLV: %s\n"
             "\tquality: %s\n"
             "\tconfidence: %s\n"
             "\tflags: %s\n"
            ,i+1,unit.figures[i].weapon,AT,LV,quality,confidence,flags);
    }
  }
}
void print_teams()
{
  for(int i=0;i<NUMTEAMS;i++)
  {
    printf("team: %i\n",i);
    printf("ids: ");
    for(int j=0;j<TEAMS[i].unitcnt;j++)
      printf("%i, ",TEAMS[i].unitids[j]);
    printf("\nspots: ");
    for(int j=0;j<MAXSPOTS;j++)
      printf("%i, ",TEAMS[i].spots[j]);
    printf("\n");
  }
}
void print_weaponstable()
{
  char flags[256];
  for(int i=0;i<NUMWEAPONS;i++)
  {
    if(WEAPONTABLE[i].name != 0)
    {
      printf("weapon: %i\n",i+1);
      printf("name: %s\n",WEAPONTABLE[i].name);
      printf("FP: %i\n",WEAPONTABLE[i].FP);
      printf("IV: %i\n",WEAPONTABLE[i].IV);
      flags_to_string(flags,WEAPONTABLE[i].flags);
      printf("flags: %s\n",flags);
      printf("\n");
    }
  }
}

/*
██████  ███████  █████  ██████  ███████
██   ██ ██      ██   ██ ██   ██ ██
██████  █████   ███████ ██   ██ ███████
██   ██ ██      ██   ██ ██   ██      ██
██   ██ ███████ ██   ██ ██████  ███████
*/
Unit* read_units_file(const char *filename)
{
  FILE *file = fopen(filename,"r");
  char varname[256];
  char varvalue[256];
  Unit *units;
  int unitid = 0;
  int figureid = 0;

  int *teamcounter;

  while(!feof(file))
  {
    fscanf(file,"%s %s\n",varname,varvalue);
    if(strcmp("unitcnt",varname) == 0)
    {
      NUMUNITS = atoi(varvalue);
      units = malloc(sizeof(Unit) * NUMUNITS);
    }
    if(strcmp("teamcnt",varname) == 0)
    {
      NUMTEAMS = atoi(varvalue);
      TEAMS = malloc(sizeof(Team)*NUMTEAMS);
      for(int i=0;i<NUMTEAMS;i++)
      {
        TEAMS[i].spots = malloc(sizeof(Figure*)*MAXSPOTS);
        for(int j=0;j<MAXSPOTS;j++)
          TEAMS[i].spots[j] = 0;
      }
      teamcounter = malloc(sizeof(int) * NUMTEAMS);
      for(int i=0;i<NUMTEAMS;i++)
        teamcounter[i] = 0;
    }
    if(strcmp("teamunitcnt",varname) == 0)
    {
      int teamnr;
      int unitcnt;
      sscanf(varvalue,"%i,%i",&teamnr,&unitcnt);
      TEAMS[teamnr-1].unitids = malloc(sizeof(int)*(unitcnt));
      TEAMS[teamnr-1].unitcnt = unitcnt;
    }

    if(strcmp("unit",varname) == 0)
    {
      unitid = atoi(varvalue)-1;
      units[unitid].formation = LINE;
      units[unitid].spread = 2;
      units[unitid].friendlytarget = NULL;
      units[unitid].enemytarget = NULL;
      units[unitid].command = COMMAND_IDLE;
      units[unitid].facing = 0.0f;
      for(int i=0;i<MAXFIGURES;i++)
      {
        units[unitid].figures[i].flags = 0;
        units[unitid].figures[i].cover = 0;
        units[unitid].figures[i].suppressed = 0;
      }
    }
    if(strcmp("team",varname) == 0)
    {
      int teamnr = atoi(varvalue) -1;
      units[unitid].team = teamnr;
      TEAMS[teamnr].unitids[teamcounter[teamnr]] = unitid;
      teamcounter[teamnr]++;
    }
    if(strcmp("uname",varname) == 0)
    {
      strncpy(units[unitid].name,varvalue,MAXNAMELENGTH);
    }
    if(strcmp("commandlevel",varname) == 0)
      units[unitid].commandlevel = enum_string_to_int(varvalue);
    if(strcmp("uflags",varname) == 0)
      units[unitid].flags = flag_string_to_int(varvalue);
    if(strcmp("position",varname) == 0)
      units[unitid].goal = string_to_pos(varvalue);
    if(strcmp("figurecnt",varname) == 0)
      units[unitid].figurecnt = atoi(varvalue);

    if(strcmp("figure",varname) == 0)
    {
      figureid = atoi(varvalue)-1;
      Vector3 relposition = Vector3Multiply(FORMATIONPOSITION[units[unitid].formation],figureid*units[unitid].spread);
      Vector3 finalpos = Vector3Add(units[unitid].goal,relposition);
      units[unitid].figures[figureid].position = finalpos;
      units[unitid].figures[figureid].spottingtimer = unitid*100 + figureid*10;
      units[unitid].figures[figureid].enemytarget = NULL;
      units[unitid].figures[figureid].doingcommand = 0;
    }
    if(strcmp("fname",varname) == 0)
    {
      strncpy(units[unitid].figures[figureid].name,varvalue,MAXNAMELENGTH);
    }
    if(strcmp("weapon",varname) == 0)
    {
      units[unitid].figures[figureid].weapon = atoi(varvalue)-1;
      units[unitid].figures[figureid].FP = WEAPONTABLE[atoi(varvalue)-1].FP;
      units[unitid].figures[figureid].IV = WEAPONTABLE[atoi(varvalue)-1].IV;
      units[unitid].figures[figureid].wflags = WEAPONTABLE[atoi(varvalue)-1].flags;
      units[unitid].figures[figureid].burst = WEAPONTABLE[atoi(varvalue)-1].burst;
      units[unitid].figures[figureid].projectiletype = WEAPONTABLE[atoi(varvalue)-1].projectiletype;
    }
    if(strcmp("ammo",varname) == 0)
      units[unitid].figures[figureid].ammo = atoi(varvalue);
    if(strcmp("movespeed",varname) == 0)
      units[unitid].figures[figureid].movespeed = atoi(varvalue);
    if(strcmp("AT",varname) == 0)
      units[unitid].figures[figureid].AT = enum_string_to_int(varvalue);
    if(strcmp("LV",varname) == 0)
      units[unitid].figures[figureid].LV = enum_string_to_int(varvalue);
    if(strcmp("quality",varname) == 0)
      units[unitid].figures[figureid].quality = enum_string_to_int(varvalue);
    if(strcmp("confidence",varname) == 0)
      units[unitid].figures[figureid].confidence = enum_string_to_int(varvalue);
    if(strcmp("fflags",varname) == 0)
      units[unitid].figures[figureid].flags = flag_string_to_int(varvalue);
  }
  fclose(file);
  free(teamcounter);
  return units;
}
void read_weapontable_file(const char *filename)
{
  FILE *file = fopen(filename,"r");
  char varname[256];
  char varvalue[256];
  int weaponid = 0;
  while(!feof(file))
  {
    fscanf(file,"%s %s\n",varname,varvalue);
    if(strcmp("weaponcnt",varname) == 0)
    {
      NUMWEAPONS = atoi(varvalue);
      WEAPONTABLE = malloc(sizeof(Weapon) * NUMWEAPONS);
      for(int i=0;i<NUMWEAPONS;i++)
      {
        strcpy(WEAPONTABLE[i].name,"0");
      }
    }
    if(strcmp("weapon",varname) == 0)
    {
      weaponid = atoi(varvalue)-1;
    }
    if(strcmp("name",varname) == 0)
    {
      strncpy(WEAPONTABLE[weaponid].name,varvalue,MAXNAMELENGTH);
    }
    if(strcmp("FP",varname) == 0)
      WEAPONTABLE[weaponid].FP = atoi(varvalue);
    if(strcmp("IV",varname) == 0)
      WEAPONTABLE[weaponid].IV = atoi(varvalue);
    if(strcmp("flags",varname) == 0)
      WEAPONTABLE[weaponid].flags = flag_string_to_int(varvalue);
    if(strcmp("burst",varname) == 0)
      WEAPONTABLE[weaponid].burst= atoi(varvalue);
    if(strcmp("projectiletype",varname) == 0)
      WEAPONTABLE[weaponid].projectiletype= atoi(varvalue);
  }
  fclose(file);
}
Terraintile **read_map_file(const char *mapfile,const char *terrainfile)
{
// map
  Terraintile **map;
  FILE *file = fopen(mapfile,"r");
  char in[25];
  fscanf(file,"%i,%i\n",&MAPWIDTH,&MAPHEIGHT);
  map = malloc(sizeof(Terraintile)*MAPHEIGHT);
  for(int i=0;i<MAPWIDTH;i++)
    map[i] = malloc(sizeof(Terraintile)*MAPWIDTH);
  while(!feof(file))
  {
    for(int y=0;y<MAPHEIGHT;y++)
    {
      for(int x=0;x<MAPWIDTH-1;x++)
      {
        fscanf(file,"%i,",&map[x][y].type);
      }
      fscanf(file,"%i\n",&map[MAPWIDTH-1][y].type);
    }
  }
  fclose(file);
// TERRAINTILES
  int tileid = 0;
  int hasvalue = 0;
  file = fopen(terrainfile,"r");
  while(!feof(file))
  {
    fscanf(file,"%s\n",in);
    if(strcmp(in,"}") == 0)
      tileid++;
    if(strcmp(in,"model:") == 0)
    {
      char *path;
      fscanf(file,"%s\n",in);
// 11 = length of "resources/"
      path = malloc(sizeof(char) * (strlen(in)+11));
      strcpy(path,"resources/");
      strcat(path,in);
      TERRAINTILES[tileid].model = LoadModel(path);
      free(path);
    }
    if(strcmp(in,"texture:") == 0)
    {
      char *path;
      fscanf(file,"%s\n",in);
// 11 = length of "resources/"
      path = malloc(sizeof(char) * (strlen(in)+11));
      strcpy(path,"resources/");
      strcat(path,in);
      TERRAINTILES[tileid].model.material.maps[MAP_DIFFUSE].texture = LoadTexture(path);
      free(path);
    }
    if(strcmp(in,"cover:") == 0)
    {
      for(int y=0;y<TILERES;y++)
      {
        for(int x=0;x<TILERES-1;x++)
        {
          fscanf(file,"%i ",&hasvalue);
          TERRAINTILES[tileid].tile[x][y].cover = hasvalue;
          TERRAINTILES[tileid].tilecover += hasvalue;
        }
        fscanf(file,"%i\n",&hasvalue);
        TERRAINTILES[tileid].tile[TILERES-1][y].cover = hasvalue;
        TERRAINTILES[tileid].tilecover += hasvalue;
      }
    }
    if(strcmp(in,"visible:") == 0)
    {
      for(int y=0;y<TILERES;y++)
      {
        for(int x=0;x<TILERES-1;x++)
        {
          fscanf(file,"%i ",&hasvalue);
          TERRAINTILES[tileid].tile[x][y].visible = hasvalue;
          TERRAINTILES[tileid].tilevisible += hasvalue;
        }
        fscanf(file,"%i\n",&hasvalue);
        TERRAINTILES[tileid].tile[TILERES-1][y].visible = hasvalue;
        TERRAINTILES[tileid].tilevisible += hasvalue;
      }
    }
    if(strcmp(in,"walkable:") == 0)
    {
      for(int y=0;y<TILERES;y++)
      {
        for(int x=0;x<TILERES-1;x++)
        {
          fscanf(file,"%i ",&hasvalue);
          TERRAINTILES[tileid].tile[x][y].walkable = hasvalue;
          TERRAINTILES[tileid].tilewalkable += hasvalue;
        }
        fscanf(file,"%i\n",&hasvalue);
        TERRAINTILES[tileid].tile[TILERES-1][y].walkable = hasvalue;
        TERRAINTILES[tileid].tilewalkable += hasvalue;
      }
    }
  }
  fclose(file);
  return map;
}

/*
███████ ████████ ██████  ██ ███    ██  ██████  ███████
██         ██    ██   ██ ██ ████   ██ ██       ██
███████    ██    ██████  ██ ██ ██  ██ ██   ███ ███████
     ██    ██    ██   ██ ██ ██  ██ ██ ██    ██      ██
███████    ██    ██   ██ ██ ██   ████  ██████  ███████
*/
Vector3 string_to_pos(const char *string)
{
  Vector3 position;
  sscanf(string,"%f,%f,%f",&position.x,&position.y,&position.z);
  return position;
}
int enum_string_to_int(const char *string)
{
// MISSION_MOTIVATION
  if(strcmp("LOW",string) == 0)
    return LOW;
  if(strcmp("MEDIUM",string) == 0)
    return MEDIUM;
  if(strcmp("HIGH",string) == 0)
    return HIGH;
// FATIGUE_LEVEL
  if(strcmp("EXHAUSTED",string) == 0)
    return EXHAUSTED;
  if(strcmp("TIRED",string) == 0)
    return TIRED;
  if(strcmp("FRESH",string) == 0)
    return FRESH;
// COMMANDLEVEL
  if(strcmp("BATTALION",string) == 0)
    return BATTALION;
  if(strcmp("COMPANY",string) == 0)
    return COMPANY;
  if(strcmp("PLATOON",string) == 0)
    return PLATOON;
  if(strcmp("SQUAD",string) == 0)
    return SQUAD;
// LEADERSHIP
  if(strcmp("BEST",string) == 0)
    return BEST;
  if(strcmp("AVERAGE",string) == 0)
    return AVERAGE;
  if(strcmp("POOR",string) == 0)
    return POOR;
// QUALITY
  if(strcmp("ELITE",string) == 0)
    return ELITE;
  if(strcmp("VETERAN",string) == 0)
    return VETERAN;
  if(strcmp("REGULAR",string) == 0)
    return REGULAR;
  if(strcmp("GREEN",string) == 0)
    return GREEN;
  if(strcmp("UNTRAINED",string) == 0)
    return UNTRAINED;
// CONFIDENCE
  if(strcmp("CONFIDENT",string) == 0)
    return CONFIDENT;
  if(strcmp("STEADY",string) == 0)
    return STEADY;
  if(strcmp("SHAKEN",string) == 0)
    return SHAKEN;
  if(strcmp("BROKEN",string) == 0)
    return BROKEN;
  if(strcmp("ROUTED",string) == 0)
    return ROUTED;
// ARMOR_TYPE
  if(strcmp("BASIC",string) == 0)
    return BASIC;
  if(strcmp("PARTIAL_LIGHT",string) == 0)
    return PARTIAL_LIGHT;
  if(strcmp("FULL_LIGHT",string) == 0)
    return FULL_LIGHT;
  if(strcmp("POWER_LIGHT",string) == 0)
    return POWER_LIGHT;
  if(strcmp("POWER_HEAVY",string) == 0)
    return POWER_HEAVY;

  return 0;
}
int flag_string_to_int(const char *string)
{
  int flags = 0;
  char str[256];
  strcpy(str,string);
  char *tok;
  tok = strtok(str,",");
  while(tok!=NULL)
  {
    if(strcmp("EXISTS",tok) == 0)
      flags |= EXISTS;
    if(strcmp("CASUALTY",tok) == 0)
      flags |= CASUALTY;
    if(strcmp("DEAD",tok) == 0)
      flags |= DEAD;
    if(strcmp("SUPPRESSED",tok) == 0)
      flags |= SUPPRESSED;
    if(strcmp("PANICKED",tok) == 0)
      flags |= PANICKED;
    if(strcmp("SUPPORT",tok) == 0)
      flags |= SUPPORT;
    if(strcmp("CLOSERANGE",tok) == 0)
      flags |= CLOSERANGE;
    if(strcmp("TERROR",tok) == 0)
      flags |= TERROR;
    if(strcmp("LEADER",tok) == 0)
      flags |= LEADER;
    if(strcmp("HEAVY",tok) == 0)
      flags |= HEAVY;
    if(strcmp("BLAST",tok) == 0)
      flags |= BLAST;
    tok = strtok(NULL,",");
  }
  return flags;
}

void lv_enum_to_string(char *out,int LV)
{
  switch(LV)
  {
    case BEST:
      strcpy(out,"BEST");
      break;
    case AVERAGE:
      strcpy(out,"AVERAGE");
      break;
    case POOR:
      strcpy(out,"POOR");
      break;
    default:
      sprintf(out,"%i",LV);
      break;
  }
}
void quality_enum_to_string(char *out,int quality)
{
  switch(quality)
  {
    case ELITE:
      strcpy(out,"ELITE");
      break;
    case VETERAN:
      strcpy(out,"VETERAN");
      break;
    case REGULAR:
      strcpy(out,"REGULAR");
      break;
    case GREEN:
      strcpy(out,"GREEN");
      break;
    case UNTRAINED:
      strcpy(out,"UNTRAINED");
      break;
    default:
      sprintf(out,"%i",quality);
      break;
  }
}
void confidence_enum_to_string(char *out,int confidence)
{
  switch(confidence)
  {
    case CONFIDENT:
      strcpy(out,"CONFIDENT");
      break;
    case STEADY:
      strcpy(out,"STEADY");
      break;
    case SHAKEN:
      strcpy(out,"SHAKEN");
      break;
    case BROKEN:
      strcpy(out,"BROKEN");
      break;
    case ROUTED:
      strcpy(out,"ROUTED");
      break;
    default:
      sprintf(out,"%i",confidence);
      break;
  }
}
void commandlevel_enum_to_string(char *out,int commandlevel)
{
  switch(commandlevel)
  {
    case BATTALION:
      strcpy(out,"BATTALION");
      break;
    case COMPANY:
      strcpy(out,"COMPANY");
      break;
    case PLATOON:
      strcpy(out,"PLATOON");
      break;
    case SQUAD:
      strcpy(out,"SQUAD");
      break;
    default:
      sprintf(out,"%i",commandlevel);
      break;
  }
}
void AT_enum_to_string(char *out,int AT)
{
  switch(AT)
  {
    case BASIC:
      strcpy(out,"BASIC");
      break;
    case PARTIAL_LIGHT:
      strcpy(out,"PARTIAL_LIGHT");
      break;
    case FULL_LIGHT:
      strcpy(out,"FULL_LIGHT");
      break;
    case POWER_LIGHT:
      strcpy(out,"POWER_LIGHT");
      break;
    case POWER_HEAVY:
      strcpy(out,"POWER_HEAVY");
      break;
    default:
      sprintf(out,"%i",AT);
      break;
  }
}
void cover_enum_to_string(char *out, int cover)
{
  strcpy(out,"");
  if(cover & HARD)
    strcat(out,"HARD");
  else if(cover & SOFT)
    strcat(out,"SOFT");
  else if(cover & NONE)
    strcat(out,"NONE");
  if(cover & PRONE)
    strcat(out,"PRONE");
}
void actions_enum_to_string(char *out, int action)
{
  switch(action)
  {
    case ACTION_CLOSECOMBAT:
      strcpy(out,"close combat");
      break;
    case ACTION_UNPANIC:
      strcpy(out,"unpanicking");
      break;
    case ACTION_UNSUPPRESS:
      strcpy(out,"unsuppressing");
      break;
    case ACTION_FIRE:
      strcpy(out,"firing");
      break;
    case ACTION_NOTARGET:
      strcpy(out,"no target");
      break;
    case ACTION_NOAMMO:
      strcpy(out,"out of ammo");
      break;
    case ACTION_MOVE:
      strcpy(out,"moving");
      break;
    case ACTION_GO_PRONE:
      strcpy(out,"taking cover");
      break;
    case ACTION_RALLY:
      strcpy(out,"rallying");
      break;
    case ACTION_REGROUP:
      strcpy(out,"regrouping");
      break;
    case ACTION_IDLE:
      strcpy(out,"idle");
      break;
    case ACTION_CASUALTY:
      strcpy(out,"casualty");
      break;
    default:
      sprintf(out,"%i",action);
      break;
  }
}
void flags_to_string(char *out,int flags)
{
  strcpy(out,"");
  if(flags & EXISTS)
    strcat(out,"EXISTS,");
  if(flags & CASUALTY)
    strcat(out,"CASUALTY,");
  if(flags & DEAD)
    strcat(out,"DEAD,");
  if(flags & SUPPRESSED)
    strcat(out,"SUPPRESSED,");
  if(flags & PANICKED)
    strcat(out,"PANICKED,");
  if(flags & SUPPORT)
    strcat(out,"SUPPORT,");
  if(flags & CLOSERANGE)
    strcat(out,"CLOSERANGE,");
  if(flags & TERROR)
    strcat(out,"TERROR,");
  if(flags & LEADER)
    strcat(out,"LEADER,");
  if(flags & HEAVY)
    strcat(out,"HEAVY,");
  if(flags & BLAST)
    strcat(out,"BLAST,");
}
void flags_to_status(char *out,int flags)
{
  if(flags & DEAD)
    strcpy(out,"dead");
  else if(flags & CASUALTY)
    strcpy(out,"wounded");
  else if(flags & SUPPRESSED)
    strcpy(out,"suppressed");
  else if(flags & PANICKED)
    strcpy(out,"panicked");
  else
    strcpy(out,"healthy");
}
void flags_to_role(char *out,int uflags,int wflags)
{
  if(uflags & LEADER)
    strcpy(out,"leader");
  else if(wflags & SUPPORT)
    strcpy(out,"gunner");
  else
    strcpy(out,"soldier");
}
void loglevel_to_string(char *out,int loglvl)
{
  if(loglvl & LOGROLLS)
    strcpy(out,"ROLL: ");
  if(loglvl & LOGACTIONS)
    strcpy(out,"ACTION: ");
  if(loglvl & LOGCONTROLS)
    strcpy(out,"CONTROL: ");
  if(loglvl & LOGINFO)
    strcpy(out,"INFO: ");
  if(loglvl & LOGTEST)
    strcpy(out,"TEST: ");
}

/*
██████   █████  ██    ██ ██      ██ ██████
██   ██ ██   ██  ██  ██  ██      ██ ██   ██
██████  ███████   ████   ██      ██ ██████
██   ██ ██   ██    ██    ██      ██ ██   ██
██   ██ ██   ██    ██    ███████ ██ ██████
*/
void raylib_init()
{
  SCREENHEIGHT = 800;
  SCREENWIDTH = 1280;
  InitWindow(SCREENWIDTH,SCREENHEIGHT,"ccrip");
  SetTargetFPS(60);
}
Camera raylib_init_camera()
{
  Camera camera;
  camera.position = (Vector3){10.0f,10.0f,10.0f};
  camera.target = (Vector3){0.0f,0.0f,0.0f};
  camera.up = (Vector3){0.0f,1.0f,0.0f};
  camera.fovy = 45.0f;
  camera.type = CAMERA_PERSPECTIVE;
  SetCameraMode(camera,CAMERA_FREE);
  return camera;
}
void raylib_init_textures()
{
  TEXTURES[TEXTURE_FLAGS] = LoadTexture("resources/flags.png");
  TEXTURES[TEXTURE_TYPES] = LoadTexture("resources/types.png");
  TEXTURES[TEXTURE_SUMMARY] = LoadTexture("resources/summary.png");
  TEXTURES[TEXTURE_WEAPONS] = LoadTexture("resources/weapons.png");
}
void raylib_init_models(Unit *units)
{
  for(int i=0;i<NUMUNITS;i++)
  {
    if(units[i].flags & EXISTS)
    {
      for(int j=0;j<MAXFIGURES;j++)
      {
        if(test_figure_exists(units[i].figures[j]))
        {
          units[i].figures[j].model = LoadModel("resources/infantry1.obj");
        }
      }
    }
  }
}
void raylib_free_models(Unit *units)
{

  for(int i=0;i<NUMUNITS;i++)
  {
    if(units[i].flags & EXISTS)
    {
      for(int j=0;j<MAXFIGURES;j++)
      {
        if(test_figure_exists(units[i].figures[j]))
          UnloadModel(units[i].figures[j].model);
      }
    }
  }
  for(int i=0;i<NUMTERRAINTYPES;i++)
  {
    UnloadTexture(TERRAINTILES[i].model.material.maps[MAP_DIFFUSE].texture);
    UnloadModel(TERRAINTILES[i].model);
  }
}

void raylib_draw(Unit *units,Camera *camera)
{
  UpdateCamera(camera);
  BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(*camera);
// figures
      Color unitcolor;
      for(int i=0;i<NUMUNITS;i++)
      {
        if(units[i].flags & EXISTS)
        {
          switch(units[i].team)
          {
            case 0:
              unitcolor = BLUE;
              break;
            default:
              unitcolor = RED;
              break;
          }
// goal
          DrawCube(units[i].goal, 0.5f, 0.5f, 0.5f, RAYGREEN);
// test
DrawModel(blastmarker,blastpos,1.0f,ORANGE);
// endtest

// projectiles
          projectiles_draw();
          for(int j=0;j<MAXFIGURES;j++)
          {
            if(test_figure_exists(units[i].figures[j]))
            {
              if(units[i].team!=PLAYERTEAM)
              {
                if(!is_spotted_figure(PLAYERTEAM,&units[i].figures[j]))
                  unitcolor = GRAY;
              }
              DrawModel(units[i].figures[j].model,units[i].figures[j].position,1.0f,unitcolor);
            }
          }
        }
      }
    map_draw();
    EndMode3D();
    ui_draw(*camera);
// icons
    ui_draw_unit_icons(units,*camera);
    ui_draw_selected_summary();
  EndDrawing();
}
void raylib_input(Unit *units,Camera camera)
{

  if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    Unit *unittarget = NULL;
    Vector3 positiontarget;
    switch(CURRENTCOMMAND)
    {
      case COMMAND_FIRE:
        unittarget = ui_select_unit(units);
        if(selected)
        {
          if(unittarget)
            command_fire(selected,unittarget);
        }
        break;
      case COMMAND_MOVE:
        if(selected)
        {
          positiontarget = raylib_select_position(camera);
          command_move(selected,positiontarget);
        }
        break;
      case COMMAND_IDLE:
        unittarget = ui_select_unit(units);
        if(unittarget)
          selected = unittarget;
        break;
      case COMMAND_TEST:
        if(selected)
          do_action_spot(*selected, 0);
        break;
    }
    CURRENTCOMMAND = COMMAND_IDLE;
  }
  if(IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
  {
    CURRENTCOMMAND = COMMAND_IDLE;
  }
  if(IsKeyReleased(CONTROL_FIRE))
  {
    if(selected)
    {
      CURRENTCOMMAND = COMMAND_FIRE;
    }
  }
  if(IsKeyReleased(CONTROL_TEST))
  {
    if(selected)
    {
      CURRENTCOMMAND = COMMAND_TEST;
    }
  }
  if(IsKeyReleased(CONTROL_TEST_2))
  {
    Vector3 terrainhit = raylib_select_position(camera);
    Vector2 pos = get_tile_xy_at(terrainhit);
  }
  if(IsKeyReleased(CONTROL_MOVE))
  {
    if(selected)
    {
      CURRENTCOMMAND = COMMAND_MOVE;
    }
  }
}
void raylib_end()
{
  CloseWindow();
}

/*
██    ██ ██
██    ██ ██
██    ██ ██
██    ██ ██
 ██████  ██
*/
void ui_init()
{
  UNIT_ICON_RECTANGLES = malloc(sizeof(Rectangle)*NUMUNITS);
}
void ui_draw(Camera camera)
{
// target line
  if(selected)
  {
    if(CURRENTCOMMAND != COMMAND_IDLE)
    {
      Color linecolor = RAYWHITE;
      switch(CURRENTCOMMAND)
      {
        case COMMAND_FIRE:
          linecolor = RED;
          break;
        case COMMAND_MOVE:
          linecolor = BLUE;
          break;
        case COMMAND_TEST:
          linecolor = PURPLE;
          break;
      }
      Rectangle unitrectangle = ui_get_icon_rectangle(*selected,camera);
      Vector2 unitposition = (Vector2){unitrectangle.x+(unitrectangle.width/2),unitrectangle.y+(unitrectangle.height/2)};
      DrawLineEx(unitposition,GetMousePosition(),5.0f,linecolor);
    }
  }
}
void ui_end()
{
  free(UNIT_ICON_RECTANGLES);
}

void ui_draw_unit_icons(Unit *units,Camera camera)
{
  for(int i=0;i<NUMUNITS;i++)
  {
    if(units[i].flags & EXISTS)
    {
      UNIT_ICON_RECTANGLES[i] = ui_get_icon_rectangle(units[i],camera);
      Color unitcolor;
      switch(units[i].team)
      {
        case 0:
          unitcolor = BLUE;
          break;
        case 1:
          unitcolor = RED;
          break;
      }
        DrawTexture(TEXTURES[TEXTURE_TYPES],UNIT_ICON_RECTANGLES[i].x,UNIT_ICON_RECTANGLES[i].y,unitcolor);
    }
  }
}
void ui_draw_selected_summary()
{
  if(selected)
  {
    int offsetx = 0;
    int offsety = 0;
    char out[25];
    Rectangle sourcerec = {0,0,73,16};

    Vector2 summaryposition;
    summaryposition.x = SCREENWIDTH - SUMMARYWIDTH;
    summaryposition.y = SCREENHEIGHT - (SUMMARYHEADERHEIGHT+(SUMMARYFIGUREHEIGHT*selected->figurecnt));
    DrawTexture(TEXTURES[TEXTURE_SUMMARY],summaryposition.x,summaryposition.y,WHITE);
// unit stats
// name
    offsety = 2;
    offsetx = 2;
    DrawText(selected->name,summaryposition.x+offsetx,summaryposition.y+offsety,9,BLACK);
// command level
    offsety = 14;
    offsetx = 2;
    commandlevel_enum_to_string(out,selected->commandlevel);
    DrawText(out,summaryposition.x+offsetx,summaryposition.y+offsety,9,BLACK);
// figure stats
    int figureid = 0;
    for(int i=0;i<MAXFIGURES;i++)
    {
      if(test_figure_exists(selected->figures[i]))
      {
// name
        offsety = SUMMARYHEADERHEIGHT+(SUMMARYFIGUREHEIGHT*figureid)+2;
        offsetx = 23;
        DrawText(selected->figures[i].name,summaryposition.x+offsetx,summaryposition.y+offsety,9,BLACK);
// action
        offsety = SUMMARYHEADERHEIGHT+(SUMMARYFIGUREHEIGHT*figureid)+14;
        offsetx = 23;
        actions_enum_to_string(out,selected->figures[i].action);
        DrawText(out,summaryposition.x+offsetx,summaryposition.y+offsety,9,BLACK);
// role
        offsety = SUMMARYHEADERHEIGHT+(SUMMARYFIGUREHEIGHT*figureid)+2;
        offsetx = 102;
        flags_to_role(out,selected->figures[i].flags,selected->figures[i].wflags);
        DrawText(out,summaryposition.x+offsetx,summaryposition.y+offsety,9,BLACK);
// status
        offsety = SUMMARYHEADERHEIGHT+(SUMMARYFIGUREHEIGHT*figureid)+2;
        offsetx = 176;
        flags_to_status(out,selected->figures[i].flags);
        DrawText(out,summaryposition.x+offsetx,summaryposition.y+offsety,9,BLACK);
// weapon icon
        sourcerec.y = 16*selected->figures[i].weapon;
        offsety = SUMMARYHEADERHEIGHT+(SUMMARYFIGUREHEIGHT*figureid)+14;
        offsetx = 102;
        Vector2 position = (Vector2){summaryposition.x+offsetx,summaryposition.y+offsety};
        DrawTextureRec(TEXTURES[TEXTURE_WEAPONS], sourcerec, position, WHITE);
// ammo
        offsety = SUMMARYHEADERHEIGHT+(SUMMARYFIGUREHEIGHT*figureid)+14;
        offsetx = 176;
        sprintf(out,"%ld",selected->figures[i].ammo);
        DrawText(out,summaryposition.x+offsetx,summaryposition.y+offsety,9,BLACK);
// next unit
        figureid++;
      }
    }
  }
}

Rectangle ui_clamp_icon_rectangle(Rectangle rectangle)
{
  Rectangle clamped = rectangle;
  if(clamped.x < 0)
    clamped.x = 0;
  if(clamped.x > SCREENWIDTH-TYPEWIDTH)
    clamped.x = SCREENWIDTH-TYPEWIDTH;
  if(clamped.y < 0)
    clamped.y = 0;
  if(clamped.y > SCREENHEIGHT-TYPEHEIGHT)
    clamped.y = SCREENHEIGHT-TYPEHEIGHT;
  return clamped;
}
Rectangle ui_get_icon_rectangle(Unit unit,Camera camera)
{
  Vector2 position2;
  Rectangle iconrectangle;
  Vector3 position3 = (Vector3){0.0f,0.0f,0.0f};
  int figurecnt = 0;
  for(int i=0;i<MAXFIGURES;i++)
  {
    if(test_figure_exists(unit.figures[i]))
    {
      if(!(unit.figures[i].flags & CASUALTY))
      {
        position3 = Vector3Add(position3,unit.figures[i].position);
        figurecnt++;
      }
    }
  }
  if(figurecnt > 0)
  {
    position3.x /= figurecnt;
    position3.y /= figurecnt;
    position3.z /= figurecnt;
  }
  else
    position3 = unit.figures[0].position;
  position2 = GetWorldToScreen(position3,camera);
  iconrectangle.x = position2.x;
  iconrectangle.y = position2.y;
  iconrectangle.width = TYPEWIDTH;
  iconrectangle.height = TYPEHEIGHT;
  iconrectangle = ui_clamp_icon_rectangle(iconrectangle);
  return iconrectangle;
}

Unit *ui_select_unit(Unit *units)
{
  Unit *out = NULL;
  Vector2 mousepos = GetMousePosition();
  for(int i=0;i<NUMUNITS;i++)
  {
    if(units[i].flags & EXISTS)
    {
      if(CheckCollisionPointRec(mousepos,UNIT_ICON_RECTANGLES[i]))
      {
        out = &units[i];
      }
    }
  }
  return out;
}
Vector3 raylib_select_position(Camera camera)
{
  Ray ray;
  RayHitInfo terrainhit;
  Vector3 p1;
  Vector3 p2;
  Vector3 p3;
  ray = GetMouseRay(GetMousePosition(),camera);
  for(int y=0;y<MAPHEIGHT;y++)
  {
    for(int x=0;x<MAPWIDTH;x++)
    {
      p1 = (Vector3){(x*TERRAINRES),0.0f,(y*TERRAINRES)};
      p2 = (Vector3){(x*TERRAINRES),0.0f,(y*TERRAINRES)+TERRAINRES};
      p3 = (Vector3){(x*TERRAINRES)+TERRAINRES,0.0f,(y*TERRAINRES)+TERRAINRES};
      terrainhit = GetCollisionRayTriangle(ray,p1,p2,p3);
      if(terrainhit.hit)
      {
        return terrainhit.position;
      }
      p1 = (Vector3){(x*TERRAINRES),0.0f,(y*TERRAINRES)};
      p2 = (Vector3){(x*TERRAINRES)+TERRAINRES,0.0f,(y*TERRAINRES)};
      p3 = (Vector3){(x*TERRAINRES)+TERRAINRES,0.0f,(y*TERRAINRES)+TERRAINRES};
      terrainhit = GetCollisionRayTriangle(ray,p1,p2,p3);
      if(terrainhit.hit)
      {
        return terrainhit.position;
      }
    }
  }
  return (Vector3){0.0f,0.0f,0.0f};
}

/*
███    ███  █████  ██████
████  ████ ██   ██ ██   ██
██ ████ ██ ███████ ██████
██  ██  ██ ██   ██ ██
██      ██ ██   ██ ██
*/
void map_draw()
{
  for(int y=0;y<MAPHEIGHT;y++)
  {
    for(int x=0;x<MAPWIDTH;x++)
    {
      DrawModel(TERRAINTILES[MAP[x][y].type].model,(Vector3){x*TERRAINRES,0.0f,y*TERRAINRES,},1.0f,WHITE);
    }
  }
}
Vector2 get_tile_xy_at(Vector3 position)
{
  int mx = position.x/10;
  int my = position.z/10;
  int tx = (int)(position.x)%10;
  int ty = (int)(position.z)%10;
  return (Vector2){mx*10+tx,my*10+ty};
}
int get_map_line_value(Vector2 start,Vector2 end,int value)
{
  DebugLog(LOGINFO,"startx:%i,starty:%i\n",(int)start.x,(int)start.y);
  DebugLog(LOGINFO,"endx:%i,endy:%i\n",(int)end.x,(int)end.y);
  int retval = 0;
  int dx = abs(end.x-start.x);
  int sx = start.x<end.x ? 1 : -1;
  int dy = abs(end.y-start.y);
  int sy = start.y<end.y ? 1 : -1;
  int err = (dx>dy ? dx: -dy)/2;
  int err2;
  for(;;)
  {
// value
    int mx = start.x/10;
    int my = start.y/10;
    int tx = (int)(start.x)%10;
    int ty = (int)(start.y)%10;
    switch(value)
    {
      case COVER:
        retval += TERRAINTILES[MAP[mx][my].type].tile[tx][ty].cover;
        break;
      case VISIBLE:
        retval += TERRAINTILES[MAP[mx][my].type].tile[tx][ty].visible;
        break;
      case WALKABLE:
        retval += TERRAINTILES[MAP[mx][my].type].tile[tx][ty].walkable;
        break;
    }

    if(start.x==end.x && start.y==end.y)
      break;
    err2 = err;
    if(err2 > -dx)
    {
      err -= dy;
      start.x += sx;
    }
    if(err2 < dy)
    {
      err += dx;
      start.y += sy;
    }
  }
  return retval;
}

/*
██████  ██    ██ ███    ██
██   ██ ██    ██ ████   ██
██████  ██    ██ ██ ██  ██
██   ██ ██    ██ ██  ██ ██
██   ██  ██████  ██   ████
*/
void run_game(Unit *units)
{
  projectiles_init();
  CURRENTCOMMAND = COMMAND_IDLE;
  int deltatime = 0;
  raylib_init_textures();
  raylib_init_models(units);
  MAP = read_map_file("map.txt","terrain.txt");
  Camera camera = raylib_init_camera();
  ui_init();
  while(!WindowShouldClose())
  {
    deltatime = (int)(GetFrameTime()*1000);
    raylib_input(units,camera);
    units_tick(units,deltatime);
    raylib_draw(units,&camera);
    projectiles_tick(deltatime);
  }
  projectiles_end();
  raylib_free_models(units);
  ui_end();
}

int main()
{
  srand(time(0));

  selected = NULL;
  units = NULL;
//  Unit *units;
  read_weapontable_file("weapons.txt");
  units = read_units_file("units.txt");

  raylib_init();
//test
blastmarker = LoadModel("resources/blastmarker.obj");
//endtest

  run_game(units);
  free(units);
  for(int i=0;i<NUMTEAMS;i++)
  {
    free(TEAMS[i].spots);
    free(TEAMS[i].unitids);
  }
  free(TEAMS);
  raylib_end();

  return 0;
}
