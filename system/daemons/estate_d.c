/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : estate_d.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-3-17
 * Note   : 不動產精靈, 處理玩家所有不動產資料
 * Update :
 *  o 2002-09-16 Clode 修改為分散式資料儲存與處理, 加大資料量處理能力 
 *
 -----------------------------------------
 */

#define WALL_TABLE		0
#define ROOM_TABLE		1

#include <ansi.h>
#include <estate.h>
#include <location.h>
#include <daemon.h>
#include <citydata.h>
#include <map.h>
#include <roommodule.h>

private mapping estates;

/* 資料結構

	estate =
		([ 
		   city_1:
			([
			   num_1:
			   ([
				   id_1 : 
					({ 
						//第一個建築之資料組
						([ 
							"type": 建築物種類,
							"regtime": 登記時間,
							"roomtable": ({ 房間座標1, 房間座標2, ... }),
							"walltable": ({ 牆壁座標1, 牆壁座標2, ... }),
						]),

						//第二個建築之資料組
						...
					}),

				   id_2 : 
				   ...
			    ]),

			    num_2:
			    ...
			])

		   city_2:
		   ...
		])
*/

// 建立城市房地產資訊
int create_city_estate(string city, int num)
{
	//if( previous_object() != load_object(CITY_D) )
	//	return 0;

	if( !mapp(estates[city]) )
		estates[city] = allocate_mapping(0);

	if( !mapp(estates[city][num]) )
		estates[city][num] = allocate_mapping(0);

	return 1;
}

// 設定空地房地產資訊
int set_land_estate(string id, array loc)
{
	string sloc = save_variable(loc);
	string city = loc[CITY];
	int i, num = loc[NUM];
	mapping ori_estdata, new_estdata = allocate_mapping(0);
	
	if( undefinedp(estates[city]) || undefinedp(estates[city][num]) ) return 0;

	if( !arrayp(estates[city][num][id]) )
		estates[city][num][id] = allocate(0);

	// 找自己的每一棟建築
	foreach( ori_estdata in estates[city][num][id] )
	{
		// 若有重覆建物,以新的為主..
		if( member_array(sloc, ori_estdata["walltable"] + ori_estdata["roomtable"]) != -1 )
			estates[city][num][id][i] = 0;

		i++;
	}
	
	estates[city][num][id] -= ({ 0 });

	new_estdata["type"] = "land";
	new_estdata["regtime"] = time();
	new_estdata["walltable"] = ({ sloc });
	new_estdata["roomtable"] = allocate(0);

	estates[city][num][id] += ({ new_estdata });
}


// 設定房地產資訊
int set_estate(string id, mapping table, string btype, string city, int num)
{
	int i;
	string sloc;
	mapping ori_estdata, new_estdata = allocate_mapping(0);
	
	if( undefinedp(estates[city]) || undefinedp(estates[city][num]) ) return 0;

	if( !arrayp(estates[city][num][id]) )
		estates[city][num][id] = allocate(0);

	// 檢查城市的每一棟建築, 若有重覆建物, 更新成新地產資訊
	foreach( string ids, array all_estdata in estates[city][num] )
	{
		i = 0;
		foreach( ori_estdata in all_estdata )
		{
			foreach( sloc in table["walltable"] + table["roomtable"] )
				if( member_array(sloc, ori_estdata["walltable"] + ori_estdata["roomtable"]) != -1 )
					estates[city][num][ids][i] = 0;
					
			i++;
		}
		
		estates[city][num][ids] -= ({ 0 });
	}
	
	new_estdata["type"] = btype;
	new_estdata["regtime"] = time();
	new_estdata["walltable"] = table["walltable"];
	new_estdata["roomtable"] = table["roomtable"];

	estates[city][num][id] += ({ new_estdata });
}


// 回傳座標位置之房地產擁有者 id
string whose_estate(array loc)
{
	string sloc = save_variable(loc);
	string city = loc[CITY];
	int num = loc[NUM];
	mapping estdata;

	if( undefinedp(estates[city]) || undefinedp(estates[city][num]) )
		return 0;

	foreach(string id, array myestates in estates[city][num])
		foreach(estdata in myestates)
			if( member_array( sloc, estdata["walltable"] + estdata["roomtable"] ) != -1 )
				return id;

	return 0;
}


// 回傳某座標之完整房地產資訊
mapping query_loc_estate(array loc)
{
	string sloc = save_variable(loc);
	string city = loc[CITY];
	int num = loc[NUM];
	string id;
	mapping estdata;
	array myestates;

	if( !estates || !estates[city] || !estates[city][num] )
		return 0;

	foreach(id, myestates in estates[city][num])
		foreach(estdata in myestates)
			if( member_array( sloc, estdata["walltable"] + estdata["roomtable"] ) != -1 )
				return copy(estdata);

	return 0;
}

// 回傳城市所有房地產資料
varargs mapping query_city_estate(string city, int num)
{
	if( undefinedp(estates[city]) )
		return 0;

	if( undefinedp(num) )	
		return copy(estates[city]);

	return copy(estates[city][num]);
}


// 回傳某位玩家在某個城市中或所有的房地產資料
varargs array query_player_estate(string id, string city, int num)
{
	mapping citydata, numdata;
	array all_myestates;

	if( !id ) return 0;

	all_myestates = allocate(0);

	if( undefinedp(city) )
	{
		// 回傳玩家所有房地產資料
		foreach( city, citydata in estates )
		foreach( num, numdata in citydata )
		{
			if( !mapp(numdata) || undefinedp(numdata[id]) ) continue;

			all_myestates += numdata[id];
		}
	}
	else
	{
		if( undefinedp(estates[city]) )
			return 0;

		// 回傳玩家在城市 city 中的所有房地產資料
		if( undefinedp(num) )
		{
			foreach( num, numdata in estates[city] )
			{
				if( !numdata || undefinedp(numdata[id]) ) continue;

				all_myestates += numdata[id];
			}
		}
		// 回傳玩家在城市 city num 中的房地產資料
		else 
		{
			if( arrayp(estates[city][num][id]) )
				all_myestates += estates[city][num][id];
			else
				return 0;
		}
	}

	return all_myestates;
}



// 自動回收沒有利用的空地
varargs int handle_freeland(string city, int num, int outdate)
{
	int i;
	array myestates;
	string id;
	object owner;
	array loc;
	mapping estdata;
	array removing_loc = allocate(0);

	if( !outdate )
		outdate = 3*24*60*60;

	if( !estates || !estates[city] || !estates[city][num] ) return 0;

	foreach(id, myestates in estates[city][num])
	{
		if( belong_to_government(id) ) continue;

		i = -1;
		foreach(estdata in myestates)
		{
			i++;
			
			if( estdata["type"] != "land" ) continue;
			
			if( !estdata["regtime"] ) 
			{
				estates[city][num][id][i]["regtime"] = time();
				continue;
			}
			
			// 過期徵收
			if( time() - estdata["regtime"] > outdate )
			{
				reset_eval_cost();

				loc = copy(restore_variable(estdata["walltable"][0]));

				removing_loc += ({ copy(loc) });

				CHANNEL_D->channel_broadcast("nch", "房地產：強制回收 "+id+" 房地產 "+save_variable(estdata)+"\n"+save_variable(loc));
				
				if( objectp(owner = find_player(id)) )
					tell(owner, pnoun(2, owner)+"的荒地"+loc_short(loc)+"由於多日未使用因此被政府強制徵收。\n");
			}	
		}
	}
	
	foreach(loc in removing_loc)
		ESTATE_D->remove_estate(loc);

	return 1;
}

int remove_unknown_estate(array loc)
{
	string file;
	object room;

	if( !arrayp(loc) ) return 0;

	CITY_D->delete_coor_data(loc);
	CITY_D->delete_coor_icon(loc);
	TOP_D->delete_top("maintain", save_variable(loc));

	// 刪除所有房間檔案
	foreach( file in city_roomfiles(loc) )
	{
		if( objectp(room = find_object(file[0..<3])) )
		{
			TOP_D->delete_top("building", base_name(room));
				
			room->remove_master();
			room->remove_slave();
			destruct(room);
		}
		rm(file);
		CHANNEL_D->channel_broadcast("nch", "刪除房間檔案："+file);
	}
	
	return 1;
}

// 若輸入為 id, 則刪除所有 id 之不動產資訊
// 若輸入為座標, 則刪除與座標相關之整棟建築資訊
array remove_estate(mixed arg)
{
	// 若輸入為字串(id)
	if( stringp(arg) )
	{
		int num;
		string city, sloc, file;
		mapping estdata, citydata, numdata;
		object room;
		array loc, removed_estates = allocate(0);

		foreach( city, citydata in estates )
		foreach( num, numdata in citydata )
		{
			if( !numdata || !numdata[arg] ) continue;

			removed_estates += numdata[arg];

			foreach( estdata in numdata[arg] )
			{
				if( !mapp(estdata) ) continue;

				foreach( sloc in estdata["walltable"] + estdata["roomtable"] )
				{
					loc = restore_variable(sloc);

					CITY_D->delete_coor_data(loc);
					CITY_D->delete_coor_icon(loc);
					TOP_D->delete_top("maintain", save_variable(loc));

					// 刪除所有房間檔案
					foreach( file in city_roomfiles(loc) )
					{
						if( objectp(room = find_object(file[0..<3])) )
						{
							TOP_D->delete_top("building", base_name(room));
							
							room->remove_master();
							room->remove_slave();
							destruct(room);
						}
						rm(file);
						CHANNEL_D->channel_broadcast("nch", "刪除房間檔案："+file);
					}
				}
			}

			map_delete(estates[city][num], arg);
		}

		if( sizeof(removed_estates) )
			CHANNEL_D->channel_broadcast("nch", "房地產：刪除所有 "+arg+" 房地產\n"+save_variable(removed_estates));

		return removed_estates;
	}
	// 若輸入為 array loc
	else if( arrayp(arg) )
	{
		string id, sloc, file, tempsloc;
		string city = arg[CITY];
		int i, num = arg[NUM];
		object room;
		array loc, myestates, removed_estates = allocate(0);
		mapping estdata;

		if( undefinedp(estates[city]) || undefinedp(estates[city][num]) ) return 0;

		sloc = save_variable(arg);

		foreach( id, myestates in estates[city][num] )
		{
			i = 0;
			foreach( estdata in myestates )
			{
				if( member_array( sloc, estdata["walltable"] + estdata["roomtable"] ) != -1 )
				{
					foreach( tempsloc in estdata["walltable"] + estdata["roomtable"] )
					{
						loc = restore_variable(tempsloc);

						CITY_D->delete_coor_data(loc);
						CITY_D->delete_coor_icon(loc);
						TOP_D->delete_top("maintain", save_variable(loc));

						// 刪除所有房間檔案
						foreach( file in city_roomfiles(loc) )
						{
							if( objectp(room = find_object(file[0..<3])) )
							{
								TOP_D->delete_top("building", base_name(room));
								
								room->remove_master();
								room->remove_slave();
								destruct(room);
							}
							rm(file);
							CHANNEL_D->channel_broadcast("nch", "刪除房間檔案："+file);
						}
					}

					removed_estates += ({ copy(estdata) });
					
					estates[city][num][id][i] = 0;

					if( !sizeof(myestates) )
						map_delete(estates[city][num], id);

					CHANNEL_D->channel_broadcast("nch", "房地產：刪除 "+id+" 房地產\n"+save_variable(removed_estates));
				}
				i++;
			}
			estates[city][num][id] -= ({ 0 });
		}
		return removed_estates;
	}

	return 0;
}


// 解除房地產連結
int unlink_estate(array loc)
{
	string id, file;
	string sloc = save_variable(loc);
	string city = loc[CITY];
	int i, num = loc[NUM];
	object room;
	array myestates;
	mapping estdata;

	foreach( id, myestates in estates[city][num] )
	{
		i = 0;
		foreach( estdata in copy(myestates) )
		{
			if( member_array( sloc, estdata["walltable"] + estdata["roomtable"] ) != -1 )
			{
				foreach( sloc in estdata["walltable"] + estdata["roomtable"] )
				{
					loc = restore_variable(sloc);
					
					if( sizeof(estdata["roomtable"]) )
					{
						CITY_D->delete_coor_data(loc, ROOM);
						CITY_D->delete_coor_data(loc, FLAGS);
						CITY_D->delete_coor_data(loc, "lock");
					}
					else
					{
						CITY_D->delete_coor_data(loc, TYPE);
						CITY_D->delete_coor_data(loc, "capacity");
						CITY_D->delete_coor_data(loc, "maintain_time");
						CITY_D->delete_coor_data(loc, "growth_level");
						CITY_D->delete_coor_data(loc, "status");
						CITY_D->delete_coor_data(loc, "growth");
						CITY_D->delete_coor_data(loc, FLAGS);
						TOP_D->delete_top("maintain", save_variable(loc));
						CITY_D->set_coor_icon(loc, BWHT"□"NOR);
					}
					if( member_array(sloc, estdata["roomtable"]) != -1 )
						CITY_D->set_coor_icon(loc, BWHT"□"NOR);
					
					// 刪除所有房間檔案
					foreach( file in city_roomfiles(loc) )
					{
						if( objectp(room = find_object(file[0..<3])) )
						{
							TOP_D->delete_top("building", base_name(room));
		
							room->remove_master();
							room->remove_slave();
							destruct(room);
						}
						rm(file);
						CHANNEL_D->channel_broadcast("nch", "刪除房間檔案："+file);
					}
				}

				estates[city][num][id][i] = 0;
				estates[city][num][id] -= ({ 0 });
				
				foreach( sloc in copy(estdata["walltable"] + estdata["roomtable"]) )
					set_land_estate(id, restore_variable(sloc));				

				return 1;
			}
			i++;
		}
	}
	
	return 0;
}



// 刪除整座城市之不動產資料
int remove_city_estate(string city, int num)
{
	object room;
	string sloc, file;
	array loc;
	mapping estdata;

	if( undefinedp(estates[city]) || undefinedp(estates[city][num]) ) return 0;

	if( mapp(estates[city][num]) )
	foreach( string id, array myestates in estates[city][num] )
	foreach( estdata in myestates )
	foreach( sloc in estdata["walltable"] + estdata["roomtable"] )
	{
		loc = restore_variable(sloc);

		if( !CITY_D->valid_coordinate(loc) )
			continue;
			
		CITY_D->delete_coor_data(loc);
		CITY_D->delete_coor_icon(loc);
		TOP_D->delete_top("maintain", save_variable(loc));

		// 刪除所有房間檔案
		foreach( file in city_roomfiles(loc) )
		{
			if( objectp(room = find_object(file[0..<3])) )
			{
				TOP_D->delete_top("building", base_name(room));

				room->remove_master();
				room->remove_slave();
				
				destruct(room);
			}
			rm(file);
			CHANNEL_D->channel_broadcast("sys", "刪除房間檔案："+file);
		}
	}

	map_delete(estates[city], num);

	if( !sizeof(estates[city]) )
		map_delete(estates, city);
}


void scan_overlap_estate(string city, int num)
{
	int overlaps;
	string *allcoor=allocate(0), sloc;
	string *overlapcoor=allocate(0);
	mapping estdata;

	if( !mapp(estates[city]) || !mapp(estates[city][num]) )
		return 0;

	foreach( string id, array myestates in estates[city][num] )
	if( arrayp(myestates) )
	foreach( estdata in myestates )
	if( mapp(estdata["walltable"]) && mapp(estdata["roomtable"]) )
	foreach( sloc in estdata["walltable"] + estdata["roomtable"] )
	{
		if( member_array(sloc, allcoor) != -1 )
		{
			overlaps++;
			overlapcoor += ({ id+":"+sloc });
		}
		else
			allcoor+= ({ sloc });
	}

	CHANNEL_D->channel_broadcast("sys", "掃描 "+city+"_"+num+" 之地產資料重疊數為 "+overlaps+"\n"+sprintf("%O", overlapcoor));
}



// 轉移地產
int transfer_estate(string from, string to, array loc)
{
	object room, master, inv;
	mapping temp_estdata, estdata;
	string sloc = save_variable(loc);
	string file;
	string city = loc[CITY];
	int i, num = loc[NUM];

	city = loc[CITY];
	num = loc[NUM];
	
	if( undefinedp(estates[city]) || undefinedp(estates[city][num]) || undefinedp(estates[city][num][from]) ) return 0;

	foreach( estdata in estates[city][num][from] )
	{
		if( member_array(sloc, estdata["walltable"] + estdata["roomtable"] ) != -1 )
		{
			temp_estdata = copy(estdata);
			estates[city][num][from][i] = 0;
			estates[city][num][from] -= ({0});
			break;
		}
		i++;
	}

	if( !temp_estdata ) return 0;

	foreach( sloc in temp_estdata["walltable"] + temp_estdata["roomtable"] )
	{
		loc = restore_variable(sloc);
	
		CITY_D->set_coor_data(loc, "owner", to);

		TOP_D->update_top("maintain", save_variable(loc), CITY_D->query_coor_data(loc, "growth_level"), capitalize(to), CITY_D->query_coor_data(loc, TYPE));
		
		// 修改所有房間檔案
		foreach( file in city_roomfiles(loc, temp_estdata["type"]) )
		{
			if( objectp(room = load_object(file[0..<3])) )
			{
				master = room->query_master();
				
				if( master == room )
					room->remove_slave();
				else
					room->remove_master();

				foreach(inv in all_inventory(room))
				{
					if( inv->is_npc() )
						delete("job", inv);
				}

				set("owner", to, room);
				
				if( query("floor", room) && !SECURE_D->is_wizard(to) )
					TOP_D->update_top("building", base_name(room), query("floor", room)+1, to, room->query_room_name(), city);

				room->save();
			}
		}
	}
	
	if( undefinedp(estates[city][num][to]) )
		estates[city][num][to] = allocate(0);
		
	estates[city][num][to] += ({ temp_estdata });
	
	return 1;
}

// 回傳城市裡某種類建築物的總數
int query_city_amount(string city, int num, string estate_type)
{
	int amount;
	string id;
	array hisestates;
	mapping estdata;
	
	if( undefinedp(estates[city]) || undefinedp(estates[city][num]) ) return 0;
	
	foreach(id, hisestates in estates[city][num])
	foreach(estdata in hisestates)
		if( estdata["type"] == estate_type )
			++amount;
	
	return amount;
}

// 回傳擁有者擁有某建築物的總數
int query_owner_amount(string owner, string estate_type)
{
	int amount;
	string city;
	mapping citydata;
	int num;
	mapping numdata;
	string id;
	array hisestates;
	mapping estdata;
	
	foreach(city, citydata in estates)
	foreach(num, numdata in citydata)
	{
		if( !sizeof(numdata) )
			continue;

		foreach(id, hisestates in numdata)
		{
			if( owner == id && sizeof(hisestates) )
			foreach(estdata in hisestates)
				if( estdata["type"] == estate_type )
					++amount;
		}
	}
	return amount;
}

// 回傳某建築物在全世界的總數
int query_world_amount(string estate_type)
{
	int amount;
	string city;
	mapping citydata;
	int num;
	mapping numdata;
	string id;
	array hisestates;
	mapping estdata;
	
	foreach(city, citydata in estates)
	foreach(num, numdata in citydata)
	{
		if( !sizeof(numdata) )
			continue;

		foreach(id, hisestates in numdata)
		{
			if( sizeof(hisestates) )
			foreach(estdata in hisestates)
				if( estdata["type"] == estate_type )
				{
					CHANNEL_D->channel_broadcast("sys", sprintf("%O\n", estdata));
					++amount;
				}
		}
	}
	return amount;
}


// 回傳所有房地產資訊
mapping query_estate()
{
	return copy(estates);
}

// 用於手動修復資料庫的錯誤
void fix_estate_database()
{	
	int i;
	foreach(string city, mapping citydata in estates)
	{
		foreach( int num, mapping numdata in citydata )
		{
			foreach( string id, array myestates in numdata )
			{
				i = 0;
				foreach( mapping estdata in myestates )
				{

					i++;
				}
			}
		}
	}
}

int query_estate_flourish(mapping estdata)
{
	string sloc;
	string roomfile;
	object room;
	string type;
	object master;
	object *checked_master = allocate(0);
	int flourish, floor;
	int typeflourish;
	mapping buildingtable = BUILDING_D->query_building_table();

	typeflourish = to_int(buildingtable[estdata["type"]][ROOMMODULE_FLOURISH]);
	
	flourish = 0;
			
	foreach(sloc in estdata["roomtable"])
	{
		floor = 0;
		type = estdata["type"];

		foreach( roomfile in city_roomfiles(restore_variable(sloc), type) )
		{
			room = find_object(roomfile[0..<3]);
			
			if( !objectp(room) ) continue;
		
			switch(query("function", room))
			{
				case 0:
					continue;
					break;
				case "front":
					master = room->query_master();
					
					if( member_array(master, checked_master) == -1 )
					{
						// 連鎖數量大於管理能力
						// 沒有產品或者沒有員工
						if( 
							sizeof(master->query_slave()) + 1 > query("shopping/management", master) ||
							!sizeof(query("products", master)) || 
							!sizeof(filter_array(all_inventory(master), (: $1->is_module_npc() :)) ) 
						)
							return 0;
					
						checked_master += ({ master });
					}
					
					break;
			}

			++floor;
		}
		
		if( floor <= 100 )
			flourish += to_int(typeflourish*pow(floor, 1.2 + (0.02*floor/20.)));
		else
			flourish += to_int(typeflourish*pow(floor, 1.2 + (0.022*floor/20.))) + 150*(floor-100);
	}
	
	if( flourish > 50000 )
		flourish = 50000;
		
	return flourish;
}


varargs string query_estate_value(array loc, int noland)
{
	string value;
	string sloc = save_variable(loc);
	string city = loc[CITY];
	int num = loc[NUM];
	mapping estdata;
	mapping floor_total_value = BUILDING_D->query_floor_total_value();
	object room;
	
	if(!estates[city] || !estates[city][num]) return "-1";

	foreach( string id, array myestates in estates[city][num] )
	foreach( estdata in myestates )
	if( member_array( sloc, estdata["walltable"] + estdata["roomtable"] ) != -1 )
	{
		value = count(sizeof(estdata["roomtable"]), "*", BUILDING_D->query_building_table()[estdata["type"]][ROOMMODULE_OPENCOST]);
		
		foreach( sloc in estdata["walltable"] + estdata["roomtable"] )
		{
			loc = restore_variable(sloc);
			
			if( !noland ) 
			switch( CITY_D->query_coor_data(loc, "region") )
			{
				case AGRICULTURE_REGION:
					value = count(value, "+", 100000); break;
				case INDUSTRY_REGION:
					value = count(value, "+", 200000); break;
				case COMMERCE_REGION:
					value = count(value, "+", 300000); break;
				default: break;
			}
		
			room = find_object(CITY_ROOM_MODULE(loc[CITY], loc[NUM], loc[X], loc[Y], estdata["type"]));
			
			if( objectp(room) )
				value = count(value, "+", floor_total_value[query("floor", room)]);
		}
		
		
		break;
	}
	
	return value;
}

varargs string query_all_estate_value(string id, int default_unit)
{
	int num;
	string defaultmoneyunit = MONEY_D->query_default_money_unit();
	string city, sloc, totalvalue, value;
	array loc;
	mapping estdata, citydata, numdata;
	mapping buildingtable = BUILDING_D->query_building_table();
	mapping floor_total_value = BUILDING_D->query_floor_total_value();
	object room;

	foreach( city, citydata in estates)
	foreach( num, numdata in citydata )
        if( numdata && arrayp(numdata[id]) )
	foreach( estdata in numdata[id] )
	{
		value = count(sizeof(estdata["roomtable"]), "*", buildingtable[estdata["type"]][ROOMMODULE_OPENCOST]);
		
		if( !default_unit )
			totalvalue = count(totalvalue, "+", EXCHANGE_D->convert(value, MONEY_D->city_to_money_unit(city), defaultmoneyunit));
		else
			totalvalue = count(totalvalue, "+", value);

		foreach( sloc in estdata["walltable"] + estdata["roomtable"] )
		{
			loc = restore_variable(sloc);
					
			switch( CITY_D->query_coor_data(loc, "region") )
			{
				case AGRICULTURE_REGION:
					value = "100000"; break;
				case INDUSTRY_REGION:
					value = "200000"; break;
				case COMMERCE_REGION:
					value = "300000"; break;
				default: 
					value = 0; break;
			}
			
			room = find_object(CITY_ROOM_MODULE(loc[CITY], loc[NUM], loc[X], loc[Y], estdata["type"]));
			
			if( objectp(room) )
				value = count(value, "+", floor_total_value[query("floor", room)]);

			if( default_unit )
				totalvalue = count(totalvalue, "+", EXCHANGE_D->convert(value, MONEY_D->city_to_money_unit(city), defaultmoneyunit));
			else
				totalvalue = count(totalvalue, "+", value);
		}
	}

	return totalvalue;
}

void restore_all_data()
{
	mixed num;
	string city;

	estates = allocate_mapping(0);

	// 讀取所有城市房地產資料檔
	foreach(city in CITY_D->query_cities())
	{
		reset_eval_cost();
		estates[city] = allocate_mapping(0);

		foreach( num in get_dir(CITY_PATH(city)) )
			if( sscanf(num ,"%d", num) ) {
				if(!read_file(CITY_NUM_ESTATE(city, num))) {
					CHANNEL_D->channel_broadcast("sys", sprintf("%s 經讀取後字串值為 0 ... 略過。", CITY_NUM_ESTATE(city, num)));
					continue;
				}
				estates[city][num] = restore_variable(read_file(CITY_NUM_ESTATE(city, num)));
				if(!estates[city][num]) {
					CHANNEL_D->channel_broadcast("sys", sprintf("%s 所存資料以 restore_variable 讀出結果為 0 ... 略過。", CITY_NUM_ESTATE(city, num)));
				}
			}
	}
}

void error_check()
{
	string city;
	int num;
	mapping citydata, numdata;

	foreach( city, citydata in estates)
	foreach( num, numdata in citydata )
	{

	}	
}

void create()
{
	if( clonep() ) destruct(this_object());

	//floorvalue = allocate(101);

	//for(int i=0;i<=100;i++)
		//floorvalue[i] = count(floorvalue[i-1<0?0:i-1], "+", count(100000*i, "*", (i-4)<1 ? 1 : i-4));
	
	restore_all_data();
}

int remove()
{
	// 由 CITY_D 作統一 remove() 資料儲存處理
	return CITY_D->remove();
}

mapping wahaha() { return estates; }

string query_name()
{
	return "房地產系統(ESTATE_D)";
}
