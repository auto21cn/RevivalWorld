/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : map.c
 * Author : Cookys@RevivalWorld
 * Date   : 2002-3-18
 * Note   : 顯示地圖概況指令。
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <feature.h>
#include <daemon.h>
#include <message.h>
#include <location.h>

inherit COMMAND;

string help = @HELP
    顯示即時地圖
    
map				- 顯示當地的地圖，比例尺為 1:2
map '城市' '編號' -'比例'	- 顯示某城市地圖，例如 map wizard 1 -2(比例範圍 1 - 5)
HELP;

private void do_command(object me, string arg)
{
	array loc;
	string city;
	int num, ratio;
	string map;

	if( !arg )
	{
		if( !arrayp(loc = query_temp("location", me)) )
			return tell(me, pnoun(2, me)+"必須在地圖上使用這個功能。\n");
		
		city = loc[CITY];
		num = loc[NUM];
		ratio = 2;
	}

	else if( sscanf(arg, "%s %d -%d", city, num, ratio) != 3 || ( !CITY_D->city_exist(city, --num) && !AREA_D->area_exist(city, num)) )
		return tell(me, "請輸入正確的城市名稱與都會區編號。(例: map center 1 -2)\n");

	if( ratio > 10 ) ratio = 10;
	if( ratio < 1 ) ratio = 1;
		
	loc = arrange_city_location(1, 1, city, num);
	
	map = MAP_D->query_map_reduction(MAP_D->query_map(loc, 1), ratio);
	
	if( !map )
		tell(me, "無法產生此地圖。\n");
	else
		tell(me, (CITY_D->query_city_idname(city, num)||AREA_D->query_area_idname(city, num))+"縮小 "+ratio+" 倍簡圖：\n"+map+"\n\n");
}