#include "ccrip.h"

// bullet stuff

void FireBullet(int ox, int oy, int tx, int ty)
{
	BulletList *looper = bulletlist;
	while(looper)
	{
		looper = looper->next;
	}
	looper->next = malloc(sizeof(Bulletlist));
	looper = looper->next;
	looper->bullet.x = ox;
	looper->bullet.y = oy;
	looper->bullet.dirx = 1;
	looper->bullet.diry = 1;
	looper->bullet.tty = 100;
}

void UpdateBullets(int deltatime)
{
	BulletList *looper = bulletlist;
	BulletList *last = bulletlist;
	while(looper)
	{
		if(looper->bullet.tty == 0)
		{
		// remove bullet
		}
		looper->bullet.x += dirx;
		looper->bullet.y += diry;
		looper->bullet.ttl -= 1;
		last = looper;
		looper = looper->next;
	}
}

void RemoveBullet(Bullet *bullet, BulletList *bulletlist)
{
	
}
