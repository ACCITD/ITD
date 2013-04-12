#include <stdlib.h>
#include <stdio.h>
#include "Map.h"

Color3u initColor(){
	Color3u newColor;
	newColor.red = newColor.green = newColor.blue = 0;
	return newColor;
}

Map initMap(){
	Map newMap;
	newMap.name = NULL;
	newMap.width = 0;
	newMap.height = 0;
	
	newMap.pathColor = initColor();
	newMap.nodeColor = initColor();
	newMap.constructAreaColor = initColor();
	newMap.inAreaColor = initColor();
	newMap.outAreaColor = initColor();
	
	newMap.nodeList.head = NULL;
	newMap.nodeList.bottom = NULL;
	newMap.nodeList.cursor = NULL;
	newMap.nodeList.size = 0;
	newMap.nodeList.position = 0;
	newMap.nodeList.type = NODE;
	
	newMap.pixelArray = NULL;
	
	return newMap;
}
void dumpColor3u(Color3u color){
	printf("(r:%d, g:%d, b:%d)", color.red, color.green, color.blue);
}

void dumpMap(Map map){
	printf("-------- DUMP MAP %s --------\n", map.name == NULL ? "" : map.name);
	printf("Width : %d\n", map.width);
	printf("Height : %d\n", map.height);
	printf("Path color : ");
	dumpColor3u(map.pathColor);
	printf("\nNode color : ");
	dumpColor3u(map.nodeColor);
	printf("\nConstructable area color : ");
	dumpColor3u(map.constructAreaColor);
	printf("\nIn Area color : ");
	dumpColor3u(map.inAreaColor);	
	printf("\nOut Area color : ");
	dumpColor3u(map.outAreaColor);
	
	printf("\nNode list (%d nodes)\n", map.nodeList.size);
	goToHeadList(&map.nodeList);
	
	PathNode* cur = NULL;
	while( (cur = nextData(&map.nodeList)) != NULL){
		printf("Node %d : x=%d y=%d\n", map.nodeList.position, cur->x, cur->y);
	}
}
