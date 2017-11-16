#include "ccrip.h"

// unit stuff
Unit *CreateUnit(int x, int y)
{
	UnitList *looper = FriendlyList;
	while(looper)
	{
		looper = looper->next;
	}
	looper = malloc(sizeof(UnitList));

	looper->unit.model = LoadModel("resources/infantry1.obj");
	looper->unit.posx = x;
	looper->unit.posy = y;
	looper->unit.fposx = -1;
	looper->unit.fposy = -1;
	looper->unit.path = NULL;
	looper->unit.movespeed = 1000;
	looper->unit.movespeedtick = looper->unit.movespeed;
	

	return &looper->unit;
}
void DrawUnits()
{
	UnitList *Looper = FriendlyList;
	while(Looper)
	{
		DrawModel(Looper->unit.model,(Vector3){Looper->unit.posx,0.0f,Looper->unit.posy},1.0f,BLUE);
		Looper = Looper->next;
	}

}
void UpdateUnits(int deltatime)
{
	UnitList *looper = FriendlyList;
	while(looper)
	{
// update unit
/*
		if(looper->unit.path != NULL)
		{
			looper->unit.movespeedtick -= deltatime;
			if(looper->unit.movespeedtick <= 0)
			{
				printf("x: %i, y: %i\n",looper->unit.posx,looper->unit.posy);
				looper->unit.movespeedtick = looper->unit.movespeed;

				looper->unit.posx = looper->unit.path->x;
				looper->unit.posy = looper->unit.path->y;

				Path *lastpath = looper->unit.path;
				looper->unit.path = looper->unit.path->next;
				free(lastpath);
				lastpath = NULL;
			}
		}
		*/
		if(looper->unit.fposx!=-1 && looper->unit.fposy!=-1)
		{
		// fire
		}
		looper = looper->next;
	}
}

Unit *SelectUnit(Camera camera)
{
	Ray mouseray;
	BoundingBox bbox;

	mouseray = GetMouseRay(GetMousePosition(),camera);


	UnitList *Looper = FriendlyList;
	while(Looper->next!=NULL)
	{
		Looper = Looper->next;
		bbox = (BoundingBox){(Vector3){Looper->unit.posx - 0.5,0,Looper->unit.posy-0.5},
				     (Vector3){Looper->unit.posx + 0.5,1,Looper->unit.posy+0.5}};
		DrawBoundingBox(bbox,RED);
		if(CheckCollisionRayBox(mouseray,bbox))
		{
			printf("selected");
			return &Looper->unit;
		}
	}

	return NULL;
}
