/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : mapob_d.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-3-18
 * Note   : 地圖系統上的物件定位與移動(任務用 NPC / 戰鬥用 NPC / 其他特殊地圖物品)
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <map.h>
#include <daemon.h>
#include <location.h>

#define LOC_MIN		0
#define LOC_MAX		1
#define MAP_ID		0
#define MAP_NUM		1

array mapob =
({

});

int is_mapob(object ob)
{
	int i, size;
	
	size = sizeof(mapob);

	for(i=0;i<size;i++)
		if( member_array(ob, mapob[i]["objects"]) != -1 )
			return 1;

	return 0;
}

array query_mapob()
{
	return mapob;
}

void heart_beat()
{
	int i, j, size;
	object ob;
	array loc;
	string *valid_direction;
	string direction;
	string filename;
	mapping current_objects_count;
	int amount;

	size = sizeof(mapob);

	for(i=0;i<size;i++)
	{
		// 清除不存在的物件
		mapob[i]["objects"] -= ({ 0 });

		// 找第一個物件來判斷目前座標	
		if( sizeof(mapob[i]["objects"]) )
			loc = query_temp("location", mapob[i]["objects"][0]);
		else
			loc = 0;

		current_objects_count = allocate_mapping(0);
		// 計算現有數量, 找出目前座標
		foreach(ob in mapob[i]["objects"])
			current_objects_count[base_name(ob)]++;

		
		// 檢查整個團隊的物件是否還存在
		foreach(filename, amount in mapob[i]["group"])
		{
			// 實際數量少於設定數量(必須進行重生)
			if( current_objects_count[filename] < amount )
			{
				if( undefinedp(mapob[i]["reborn_filename"]) )
					mapob[i]["reborn_filename"] = allocate_mapping(0);
					
				if( undefinedp(mapob[i]["reborn_filename"][filename]) )
					mapob[i]["reborn_filename"][filename] = allocate(0);
			
				// 若目前排序中的重生資料不足缺少的物件數 -> 新增重生資料
				if( sizeof(mapob[i]["reborn_filename"][filename]) < amount - current_objects_count[filename] )
					mapob[i]["reborn_filename"][filename] += allocate(amount - current_objects_count[filename], mapob[i]["reborn_time"]);
					
				// 掃瞄目前重生資料 -> 倒數完畢進行重生
				for(j=0;j<sizeof(mapob[i]["reborn_filename"][filename]);j++)
				{
					// 倒數完畢, 進行重生
					if( !--mapob[i]["reborn_filename"][filename][j] )
					{
						// 沒有座標, 建立新座標
						if( !arrayp(loc) )
						{
							loc = arrange_location(
								range_random(mapob[i]["x"][LOC_MIN], mapob[i]["x"][LOC_MAX]), 
								range_random(mapob[i]["y"][LOC_MIN], mapob[i]["y"][LOC_MIN]), 
								0, 
								mapob[i]["map"][MAP_ID], 
								mapob[i]["map"][MAP_NUM], 
								0
							);
						}
						
						ob = new(filename);
						ob->move(loc);
						ob->do_command("wahaha 我重生了！！！");
						mapob[i]["objects"] += ({ ob });
					}
				}
				mapob[i]["reborn_filename"][filename] -= ({ 0 });
				
				if( !sizeof(mapob[i]["reborn_filename"][filename]) )
					map_delete(mapob[i]["reborn_filename"], filename);
			}
		}

		// 若通通都還沒重生, 就略過		
		if( !sizeof(mapob[i]["objects"]) ) continue;

		// 進行移動處理
		if( --mapob[i]["move_speed_count"] <= 0 )
		{
			mapob[i]["move_speed_count"] = mapob[i]["move_speed"];
			
			// 有移動範圍的話就進行移動
			if( mapob[i]["x"][LOC_MIN] != mapob[i]["x"][LOC_MAX] || mapob[i]["y"][LOC_MIN] != mapob[i]["y"][LOC_MAX] )
			{				
				valid_direction = allocate(0);
				
				if( loc[X] > mapob[i]["x"][LOC_MIN] ) valid_direction += ({ "west" });
				if( loc[X] < mapob[i]["x"][LOC_MAX] ) valid_direction += ({ "east" });
				if( loc[Y] > mapob[i]["y"][LOC_MIN] ) valid_direction += ({ "north" });
				if( loc[Y] < mapob[i]["y"][LOC_MAX] ) valid_direction += ({ "south" });
				
				direction = valid_direction[random(sizeof(valid_direction))];
				
				foreach(ob in mapob[i]["objects"])
					ob->do_command("go "+direction);
			}
		}
	}	
}

void create()
{
	int i, size;
	mapping data;
	string filename;
	int amount;
	array loc;
	object ob;

	size = sizeof(mapob);

	for(i=0;i<size;i++)
	{
		data = mapob[i];

		loc = arrange_location(
			range_random(data["x"][LOC_MIN], data["x"][LOC_MAX]), 
			range_random(data["y"][LOC_MIN], data["y"][LOC_MIN]), 
			0, 
			data["map"][MAP_ID], 
			data["map"][MAP_NUM], 
			0
		);
		
		mapob[i]["objects"] = allocate(0);

		foreach(filename, amount in data["group"])
		while(amount--)
		{
			ob = new(filename);
			ob->move(loc);
			ob->do_command("wahaha 我重生了！！！");
			mapob[i]["objects"] += ({ ob });
		}
	}
	
	set_heart_beat(10);
}

int remove()
{
	object ob;
	mapping data;

	foreach(data in mapob)
	{
		if( arrayp(data["objects"]) )
			foreach(ob in data["objects"])
				if( objectp(ob) )
					destruct(ob);
	}
	
	mapob = allocate(0);
}

string query_name()
{
	return "地圖物件控制系統(MAPOB_D)";
}
