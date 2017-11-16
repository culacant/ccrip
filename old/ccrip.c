#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

#include "map.h"
#include "unit.h"

// unit stuff
typedef struct Unit
{
	int posx;
	int posy;
	Model model;
} Unit;

typedef struct UnitList UnitList;

struct UnitList
{
	Unit *unit;
	UnitList *next;
} ;

UnitList *FriendlyList;

Unit *CreateUnit(int x, int y)
{
	Unit *unit = malloc(sizeof(Unit));
	unit->model = LoadModel("resources/infantry1.obj");
	unit->posx = x;
	unit->posy = y;
	
	UnitList *Looper = FriendlyList;
	while(Looper->next!=NULL)
	{
		Looper = Looper->next;
	}
	UnitList *NewUnit = malloc(sizeof(UnitList));
	NewUnit->unit = unit;
	NewUnit->next = NULL;
	Looper->next = NewUnit;

	return unit;
}
void DrawUnits()
{
	UnitList *Looper = FriendlyList;
	while(Looper->next!=NULL)
	{
		Looper = Looper->next;

		DrawModel(Looper->unit->model,(Vector3){Looper->unit->posx,0.0f,Looper->unit->posy},1.0f,BLUE);
	}

}

Unit *SelectUnit(UnitList *unitlist, Camera camera)
{
	Ray mouseray;
	BoundingBox bbox;

	mouseray = GetMouseRay(GetMousePosition(),camera);


	UnitList *Looper = FriendlyList;
	while(Looper->next!=NULL)
	{
		Looper = Looper->next;
		bbox = (BoundingBox){(Vector3){Looper->unit->posx - 0.5,0,Looper->unit->posy-0.5},
				     (Vector3){Looper->unit->posx + 0.5,1,Looper->unit->posy+0.5}};
		DrawBoundingBox(bbox,RED);
		if(CheckCollisionRayBox(mouseray,bbox))
		{
			printf("selected");
			return Looper->unit;
		}
	}

	return NULL;
}

// map stuff
int *LoadMap(int *w, int *h)
{
	FILE*fp;
	fp = fopen("map.txt","r");
	if(!fp)
	{
		printf("error opening map");
		return 0;
	}

	fscanf(fp,"%i %i",w,h);
	fgetc(fp);

	int width = (*w);
	int height = (*h);

	int *map = malloc(width*height*sizeof(int));

	int x = 0;
	int y = 0;
	char i = fgetc(fp);

	while(i!= EOF)
	{
		if(i == '\n')
		{
			y++;
			x = 0;
		}
		else
		{
			map[x+y*width] = (int)i;
			x++;
		}
		i = fgetc(fp);
	}
	fclose(fp);

	return map;
}
int DrawMap(int *map, int w, int h)
{
	int x, y;
	Vector3 pos;
	Color color;
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			pos = (Vector3){x,0,y};
			color;
			switch(map[x+y*w])
			{
				case 49:
					color = GREEN;
					break;
				case 50:
					color = BLUE;
					break;
			}
			DrawPlane(pos,(Vector2){1.0f,1.0f},color);
		}
	}
	return 0;
}

int main()
{
// setup raylib
	int screenWidth 	= 800;
	int screenHeight 	= 450;
	InitWindow(screenWidth, screenHeight, "hello world");
	SetTargetFPS(60);

// map stuff
	int width, height;
	int *map = LoadMap(&width, &height);
	printf("\n\n%i\n\n",map[1+1]);

// unit stuff
	FriendlyList = malloc(sizeof(UnitList));
	FriendlyList->unit = NULL;
	FriendlyList->next = NULL;

	CreateUnit(1,5);
	CreateUnit(5,5);

// camera stuff
	Camera camera;
	camera.position = (Vector3){10.0f,10.0f,10.0f};
	camera.target 	= (Vector3){0.0f,0.0f,0.0f};
	camera.up 	= (Vector3){0.0f,1.0f,0.0f};
	camera.fovy 	= 45.0f;
	SetCameraMode(camera, CAMERA_FIRST_PERSON);


// main loop
	while(!WindowShouldClose())
	{
		UpdateCamera(&camera);
		ShowCursor();

		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText(FormatText("%i",GetFPS()), 10,10,10,BLACK);

		Begin3dMode(camera);

		DrawMap(map, width,height);
		SelectUnit(FriendlyList,camera);


		DrawUnits();

		End3dMode();
		EndDrawing();
	}

// done
	free(map);
	//free(unit);

	CloseWindow();
	return 0;
}
