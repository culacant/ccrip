#ifndef CCRIP_H
#define CCRIP_H

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// map stuff

typedef struct Tile
{
	int graphic;
	bool canWalk;
	bool canSee;
} Tile;

Tile *LoadMap(int *w, int *h);
void DrawMap(Tile *map, int w, int h);

// AI stuff
typedef struct Node Node;
struct Node
{
	int x;
	int y;
	float f;
	float g;
	float h;
	Node *prev;
};

typedef struct Path Path;
struct Path 
{
	int x;
	int y;
	Path *next;
};

int FindPath(Tile*map, int w, int h, int ox, int oy, int tx, int ty, Path *path);
void DrawPath(Path *path);

// unit stuff

typedef struct UnitList UnitList;
typedef struct Unit
{
	int posx;
	int posy;
	Model model;
	Path *path;

	int movespeed;
	int movespeedtick;
} Unit;
struct UnitList
{
	Unit *unit;
	UnitList *next;
};

UnitList *FriendlyList;

Unit *CreateUnit(int x, int y);
void DrawUnits();
void UpdateUnits(int deltatime);
Unit *SelectUnit(Camera camera);
#endif
