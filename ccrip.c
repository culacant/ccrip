#include "ccrip.h"

int main()
{
// setup raylib
	int screenWidth 	= 800;
	int screenHeight 	= 450;
	InitWindow(screenWidth, screenHeight, "hello world");
	SetTargetFPS(60);

// map stuff
	int width, height;
	Tile *map = LoadMap(&width, &height);

	FriendlyList = NULL;

// unit stuff
	CreateUnit(1,5);
	CreateUnit(5,5);

	Unit *selected = NULL;

// camera stuff
	Camera camera;
	camera.position = (Vector3){10.0f,10.0f,10.0f};
	camera.target 	= (Vector3){0.0f,0.0f,0.0f};
	camera.up 	= (Vector3){0.0f,1.0f,0.0f};
	camera.fovy 	= 45.0f;
	SetCameraMode(camera, CAMERA_FIRST_PERSON);


	Path *path = NULL;

	printf("w:%i,h:%i",width,height);

// main loop
	while(!WindowShouldClose())
	{
		UpdateUnits(10);
		UpdateBullets(10);
		UpdateCamera(&camera);
		ShowCursor();

		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText(FormatText("%i",GetFPS()), 10,10,10,BLACK);

		Begin3dMode(camera);

		DrawMap(map, width,height);
// input
		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			selected = SelectUnit(camera);
		}
/*
		if(IsKeyDown(KEY_M))
		{
			if(selected)
			{
				Ray ray;
				ray = GetMouseRay(GetMousePosition(),camera);
				RayHitInfo groundhit = GetCollisionRayGround(ray,0.0f);
				if(groundhit.hit)
				{
					int x,y;
					x = (int)groundhit.position.x;
					y = (int)groundhit.position.z;
					printf("hit on: %i,%i\n",x,y);
					if(selected->path)
						free(selected->path);

					path = malloc(sizeof(Path));
					FindPath(map,width,height,selected->posx,selected->posy,x,y,path);
					selected->path = path;
					path = NULL;

					DrawCube(groundhit.position,0.3,0.3,0.3,RED);
				}
			}
		}
*/
		if(IsKeyDown(KEY_F))
		{
			if(selected)
			{
				FireBullet(selected->posx, selected->posy, 5,5);
				printf("bullet fired");
			}
		}
// selected
		if(selected)
		{
			if(selected->path)
				DrawPath(selected->path);
		}

		DrawUnits();
		DrawBullets();

		End3dMode();
		EndDrawing();
	}

// done
	free(map);

	CloseWindow();
	return 0;
}
