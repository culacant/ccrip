#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int loadmap(int *map, int *width, int *height)
{

	FILE *fp;
	fp = fopen("map.txt","r");

	if(!fp)
	{
		printf("error opening file");
		return 0;
	}

	fscanf(fp,"%i %i",width,height);
	
	int w = (*width);
	int h = (*height);

	fgetc(fp);

	map = malloc(w*h* sizeof(int));

	int x = 0;
	int y = 0;
	char i = fgetc(fp);
	while(i !=EOF)
	{
		if(i == '\n')
		{
			y++;
			x = 0;
		}
		else
		{
			map[x+y*w] = (int)i;
			x++;
		}
		i = fgetc(fp);
	}
	fclose(fp);

	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			printf("%c",map[x+y*w]);
		}
		printf("\n");
	}
	
	return 0;

}

int main()
{
	int *map;
	int width, height;
	loadmap(map,&width,&height);
	return 0;
}
