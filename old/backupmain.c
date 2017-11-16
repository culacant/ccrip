#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

int *loadmap(int *w, int *h)
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
int drawmap(int *map, int w, int h)
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
	int *map = loadmap(&width, &height);
	printf("\n\n%i\n\n",map[1+1]);

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

		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText(FormatText("%i",GetFPS()), 10,10,10,BLACK);

		Begin3dMode(camera);
		DrawCube((Vector3){0.0f,0.0f,0.0f},2.0f,2.0f,2.0f,GOLD);

		drawmap(map, width,height);

		End3dMode();
		EndDrawing();
	}

// done
	free(map);

	CloseWindow();
	return 0;
}
