/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : create_wizhall.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-02-02
 * Note   : 建立 Wizhall
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <location.h>
#include <map.h>
#include <message.h>

#define WALL_TABLE	0
#define ROOM_TABLE	1

inherit COMMAND;

string help = @HELP
        標準 create_wizhall 指令。
HELP;

private void command(object me, string arg)
{
	int x, y, num, type;
	string city;
	array room_coor, loc = query_temp("location",me);
	mapping bdata;
	
	if( !is_command() ) return;
	
	if( !arrayp(loc) ) return tell(me, pnoun(2, me)+"必須在地圖系統上執行這個指令。\n");
	
	num 	= loc[NUM];
	city 	= loc[CITY];
	
	if( CITY_D->query_coor_data(loc, "owner") != "GOVERNMENT/"+city )
		return tell(me, "這裡並不是市政府的土地。\n");

	if( !mapp(bdata = BUILDING_D->analyze_building_logic(loc)) )
		return tell(me, "此棟建築物的建造結構有問題，無法開張，請將錯誤部份重新建造。\n");


	if( sizeof(bdata["roomtable"]) != 6 )
		return tell(me, "巫師神殿必須是 3 X 2 的建築。\n");
		

	// 牆壁座標資料陣列
	foreach( string code in bdata["walltable"] )
	{
		loc = restore_variable(code);
		
		type = CITY_D->query_coor_data(loc, TYPE);
		
		if( type == WALL )
			CITY_D->set_coor_data(loc, FLAGS, NO_MOVE);
			
		else if( type != DOOR )
			return tell(me, "(Command:open)牆壁資料錯誤，請通知巫師處理。\n");
	}
	
	room_coor = allocate(6);
	
	// 房間座標資料陣列
	foreach( string code in bdata["roomtable"] )
	{	
		loc = restore_variable(code);
		
		// 設定此座標為 ROOM 標記
		CITY_D->set_coor_data(loc, ROOM, "wizhall");
		
		// 設定此座標之地圖圖示為"  "
		CITY_D->set_coor_icon(loc, "  ");
		
		x += loc[X];
		if( !y || y < loc[Y] ) y = loc[Y];
	}	
	x/=6;

	// 設定 Wizhall 出口
	CITY_D->create_wizhall_door(city, num, arrange_city_location(x, y+1, city, num));
	
	// 建立巫師神殿房地產資訊
	ESTATE_D->set_estate("GOVERNMENT/"+city, bdata, "wizhall", city, num);
	
	CITY_D->set_coor_data(arrange_city_location(x, y, city, num), SPECIAL_EXIT, "/wiz/wizhall/room_wizhall_1");
	CITY_D->set_coor_data(arrange_city_location(x+1, y, city, num), SPECIAL_EXIT, "/wiz/wizhall/room_wizhall_2");
	CITY_D->set_coor_data(arrange_city_location(x-1, y, city, num), SPECIAL_EXIT, "/wiz/wizhall/room_wizhall_6");
	CITY_D->set_coor_data(arrange_city_location(x, y-1, city, num), SPECIAL_EXIT, "/wiz/wizhall/room_wizhall_4");
	CITY_D->set_coor_data(arrange_city_location(x+1, y-1, city, num), SPECIAL_EXIT, "/wiz/wizhall/room_wizhall_3");
	CITY_D->set_coor_data(arrange_city_location(x-1, y-1, city, num), SPECIAL_EXIT, "/wiz/wizhall/room_wizhall_5");
	CITY_D->set_coor_icon(arrange_city_location(x-1, y-1, city, num), HIW"巫"NOR);
	CITY_D->set_coor_icon(arrange_city_location(x-1, y, city, num), NOR WHT"神"NOR);
	CITY_D->set_coor_icon(arrange_city_location(x+1, y-1, city, num), NOR HIW"師"NOR);
	CITY_D->set_coor_icon(arrange_city_location(x+1, y, city, num), NOR WHT"殿"NOR);
	CITY_D->set_section_info(city, num, "wizhall", arrange_city_location(x, y+1, city, num));

	CHANNEL_D->channel_broadcast("news", me->query_idname()+"在城市"+CITY_D->query_city_idname(city, num)+"的"+CITY_D->position(x, y)+"附近建造了一棟「"HIW"巫師"NOR WHT"神殿"NOR"」建築。\n");
}
