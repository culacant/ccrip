#include "ccrip.h"
#include <math.h>

// bullet stuff

void FireBullet(int ox, int oy, int tx, int ty)
{
	BulletList *looper = bulletlist;
	if(looper)
	{
		while(looper->next)
		{
			looper = looper->next;
		}
		looper->next = malloc(sizeof(BulletList));
		looper = looper->next;
	}
	else
	{
		bulletlist = malloc(sizeof(BulletList));
		looper = bulletlist;
	}

	looper->next = NULL;

	looper->bullet.x = ox;
	looper->bullet.y = oy;
	looper->bullet.z = 0.05f;

	looper->bullet.velx = 1;
	looper->bullet.velz = 5;
	float angle = atan2(ty - oy, tx - ox);
	looper->bullet.diry = sin(angle);
	looper->bullet.dirx = sin(angle-1.5708)*-1;
}

void UpdateBullets(int deltatime)
{
	BulletList *looper = bulletlist;
	while(looper)
	{
		looper->bullet.velz -= 1;

		looper->bullet.x += looper->bullet.dirx;
		looper->bullet.y += looper->bullet.diry;
		looper->bullet.z += looper->bullet.velz;


		if(looper->bullet.z <= 0)
		{
//			RemoveBullet(&looper->bullet);
		}
		looper = looper->next;
	}
}

void DrawBullets()
{
	BulletList *looper = bulletlist;
	while(looper)
	{
		DrawCube((Vector3){looper->bullet.x,looper->bullet.z,looper->bullet.y},0.5f,0.5f,0.5f,YELLOW);
		looper = looper->next;
	}
}

void RemoveBullet(Bullet *bullet)
{
	BulletList * looper = bulletlist;
	BulletList * last;
	while(looper)
	{
		if(bulletlist->bullet.x == bullet->x &&
			bulletlist->bullet.y == bullet->y &&
			bulletlist->bullet.dirx == bullet->dirx &&
			bulletlist->bullet.diry == bullet->diry)
		{
			if(looper == bulletlist)
				bulletlist = bulletlist->next;
			else
				last->next = looper->next;
			free(looper);
			return;
		}
		last = looper;
		looper = looper->next;
	}
}
