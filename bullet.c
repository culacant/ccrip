#include "ccrip.h"

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
	looper->bullet.dirx = 1;
	looper->bullet.diry = 1;
	looper->bullet.ttl = 100;
}

void UpdateBullets(int deltatime)
{
	BulletList *looper = bulletlist;
	while(looper)
	{
		if(looper->bullet.ttl == 0)
		{
			RemoveBullet(&looper->bullet);
		}
		else
		{
			looper->bullet.x += looper->bullet.dirx;
			looper->bullet.y += looper->bullet.diry;
			looper->bullet.ttl -= 1;
			looper = looper->next;
		}
	}
}

void DrawBullets()
{
	BulletList *looper = bulletlist;
	while(looper)
	{
		DrawCube((Vector3){looper->bullet.x,0.5f,looper->bullet.y},0.5f,0.5f,0.5f,YELLOW);
		looper = looper->next;
	}
}

void RemoveBullet(Bullet *bullet)
{
	BulletList * looper = bulletlist;
	BulletList * last = NULL;
	while(looper)
	{
		if(looper->bullet.x == bullet->x && looper->bullet.y == bullet->y && looper->bullet.dirx == bullet->dirx && looper->bullet.diry == bullet->diry)
		{
			if(last)
			{
				last->next = looper->next;
				free(looper);
			}
			else
			{
				bulletlist = looper->next;
				free(looper);
			}
			return;
		}
		last = looper;
		looper = looper->next;
	}
	
}
