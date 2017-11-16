#include "ccrip.h"

// bullet stuff

void FireBullet(int ox, int oy, int tx, int ty)
{
	BulletList *looper = bulletlist;
	while(looper)
	{
		looper = looper->next;
	}
	looper->next = malloc(sizeof(bulletlist));
	looper = looper->next;
	looper->bullet.x = ox;
	looper->bullet.y = oy;
	looper->bullet.dirx = 1;
	looper->bullet.diry = 1;
	looper->bullet.ttl = 100;
}

void UpdateBullets(int deltatime)
{
	BulletList *looper = bulletlist;
	BulletList *last = bulletlist;
	while(looper)
	{
		if(looper->bullet.ttl == 0)
		{
		// remove bullet
		}
		looper->bullet.x += looper->bullet.dirx;
		looper->bullet.y += looper->bullet.diry;
		looper->bullet.ttl -= 1;
		last = looper;
		looper = looper->next;
	}
}

void RemoveBullet(Bullet *bullet, BulletList *bulletlist)
{
	
}
