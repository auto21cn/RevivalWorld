/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : expedition.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-02-24
 * Note   : 探險基地
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <map.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>
#include <location.h>
#include <citydata.h>
#include <object.h>
#include <npc.h>

inherit ROOM_ACTION_MOD;

#define BASE_COST		20000
#define COST_PER_NPC		1000

void confirm_explore(object me, object env, mapping data, string arg)
{
	object ob;
	string basename = base_name(env);
	mapping adventuredata = allocate_mapping(0);
	string npctype;

	if( arg != "y" && arg != "yes" )
	{
		tell(me, "取消探險活動。\n");
		return me->finish_input();
	}
	
	if( !me->spend_money(data["money_unit"], data["money"]) )
		return tell(me, pnoun(2, me)+"身上的錢不足 "HIY+money(data["money_unit"], data["money"])+NOR"。\n");

	adventuredata["from"] = basename;
	adventuredata["area"] = data["area"];
	adventuredata["num"] = data["num"];

	foreach(npctype in ({ "leader", "prospector", "explorer", "guard", "scout"}))
	{
		if( data[npctype] )
		{
			ob = find_object(data[npctype]);
			set("adventure", adventuredata, ob);
			ob->move(VOID_OB);
		}
	}
	
	ADVENTURE_D->start_adventure(env, data);
}

void do_plan(object me, string arg)
{
	object env = environment(me);
	string option;
	string value;
	string myid = me->query_id(1);

	if( query("owner", env) != myid )
		return tell(me, pnoun(2, me)+"不是這個探險基地的擁有者。\n");
		
	if( !arg )
		return tell(me, "請輸入正確的指令格式(help plan)。\n");
	
	// 開始探險	
	if( arg == "start" )
	{
		mapping data = allocate_mapping(0);

		string msg;

		object leader_ob;
		object prospector_ob;
		object explorer_ob;
		object guard_ob;
		object scout_ob;
		
		data["leader"] = query("plan/team/leader", env);
		data["prospector"] = query("plan/team/prospector", env);
		data["explorer"] = query("plan/team/explorer", env);
		data["guard"] = query("plan/team/guard", env);
		data["scout"] = query("plan/team/scout", env);
		data["area"] = query("plan/area/area", env);
		data["num"] = query("plan/area/num", env);
		data["time"] = query("plan/time" , env);
		data["money"] = BASE_COST;
		data["money_unit"] = env->query_money_unit();
		data["boss"] = myid;

		if( ADVENTURE_D->is_doing_adventure(env) )
			return tell(me, "這個計畫室的探險活動正在進行中。\n");

		if( !data["area"] || !AREA_D->area_exist(data["area"], data["num"]) )
			return tell(me, "未設定探險區域。\n");

		if( !data["time"] )
			return tell(me, "未設定探險時間。\n");
		
		if( !stringp(data["leader"]) || !stringp(data["prospector"]) )
			return tell(me, "一個探險隊的成員中至少要有一個隊長和一個探勘者。\n");
		
		if( !query("warehouse", env) )
			return tell(me, "尚未設定物資集散地。\n");
			
		if( stringp(data["leader"]) )
		{
			if( !file_exists(data["leader"]) )
				return tell(me, "隊長的員工資料已經消失。\n");

			leader_ob = load_object(data["leader"]);
			
			if( !same_environment(me, leader_ob) )
				return tell(me, leader_ob->query_idname()+"不在這裡。\n");

			if( query("boss", leader_ob) != myid )
				return tell(me, leader_ob->query_idname()+"的雇主不是"+pnoun(2, me)+"。\n");
			
			if( query("job/cur", leader_ob) != ADVENTURER )
				return tell(me, "必須先命令"+leader_ob->query_idname()+"從事探險的工作。\n");

			data["money"] += COST_PER_NPC * data["time"];
		}
		
		if( stringp(data["prospector"]) )
		{
			if( !file_exists(data["prospector"]) )
				return tell(me, "探勘者的員工資料已經消失。\n");

			prospector_ob = load_object(data["prospector"]);
			
			if( !same_environment(me, prospector_ob) )
				return tell(me, prospector_ob->query_idname()+"不在這裡。\n");

			if( query("boss", prospector_ob) != myid )
				return tell(me, prospector_ob->query_idname()+"的雇主不是"+pnoun(2, me)+"。\n");

			if( query("job/cur", prospector_ob) != ADVENTURER )
				return tell(me, "必須先命令"+prospector_ob->query_idname()+"從事探險的工作。\n");

			data["money"] += COST_PER_NPC * data["time"];
		}
	
		if( stringp(data["explorer"]) )
		{
			if( !file_exists(data["explorer"]) )
				return tell(me, "探險家的員工資料已經消失。\n");

			explorer_ob = load_object(data["explorer"]);
		
			if( !same_environment(me, explorer_ob) )
				return tell(me, explorer_ob->query_idname()+"不在這裡。\n");
		
			if( query("boss", explorer_ob) != myid )
				return tell(me, explorer_ob->query_idname()+"的雇主不是"+pnoun(2, me)+"。\n");
				
			if( query("job/cur", explorer_ob) != ADVENTURER )
				return tell(me, "必須先命令"+explorer_ob->query_idname()+"從事探險的工作。\n");

			data["money"] += COST_PER_NPC * data["time"];
		}
		
		if( stringp(data["guard"]) )
		{
			if( !file_exists(data["guard"]) )
				return tell(me, "護衛的員工資料已經消失。\n");

			guard_ob = load_object(data["guard"]);
			
			if( !same_environment(me, guard_ob) )
				return tell(me, guard_ob->query_idname()+"不在這裡。\n");

			if( query("boss", guard_ob) != myid )
				return tell(me, guard_ob->query_idname()+"的雇主不是"+pnoun(2, me)+"。\n");

			if( query("job/cur", guard_ob) != ADVENTURER )
				return tell(me, "必須先命令"+guard_ob->query_idname()+"從事探險的工作。\n");

			data["money"] += COST_PER_NPC * data["time"];
		}
			
		if( stringp(data["scout"]) )
		{
			if( !file_exists(data["scout"]) )
				return tell(me, "偵查者的員工資料已經消失。\n");

			scout_ob = load_object(data["scout"]);

			if( !same_environment(me, scout_ob) )
				return tell(me, scout_ob->query_idname()+"不在這裡。\n");
				
			if( query("boss", scout_ob) != myid )
				return tell(me, scout_ob->query_idname()+"的雇主不是"+pnoun(2, me)+"。\n");

			if( query("job/cur", scout_ob) != ADVENTURER )
				return tell(me, "必須先命令"+scout_ob->query_idname()+"從事探險的工作。\n");

			data["money"] += COST_PER_NPC * data["time"];
		}
		
		msg =  "這次探險的資料與需要的金錢、物資\n";
		msg += WHT"────────────────────────────\n"NOR;
		msg += "區域    ："+AREA_D->query_area_idname(data["area"], data["num"])+"\n";
		msg += HIW"隊"NOR WHT"長"NOR"    ："+leader_ob->query_idname()+"\n";
		msg += HIY"探"NOR YEL"勘者"NOR"  ："+prospector_ob->query_idname()+"\n";
		msg += HIG"探"NOR GRN"險家"NOR"  ："+(explorer_ob ? explorer_ob->query_idname() : "未指定")+"\n";
		msg += HIR"護"NOR RED"衛"NOR"    ："+(guard_ob ? guard_ob->query_idname() : "未指定")+"\n";
		msg += HIM"偵"NOR MAG"查者"NOR"  ："+(scout_ob ? scout_ob->query_idname() : "未指定")+"\n";
		msg += "需要金錢："HIY+money(data["money_unit"], data["money"])+NOR"\n";
		msg += "需要物資：無\n";
		msg += WHT"────────────────────────────\n"NOR;

		tell(me, msg + "是否確定開始探險(y/n)？");
		
		input_to( (: confirm_explore, me, env, data :) );

		return;
	}	
	
	if( sscanf(arg, "%s %s", option, value) != 2 )
		return tell(me, "請輸入正確的指令格式(help plan)。\n");
		
	switch(option)
	{
		case "area":
		{
			string area;
			int num;
			
			if( sscanf(value, "%s %d", area, num) != 2 )
				return tell(me, "請輸入正確的指令格式(help plan)。\n");
		
			num--;

			if( !AREA_D->area_exist(area, num) )
				return tell(me, "沒有這個區域。\n");
				
			set("plan/area/area", area, env);
			set("plan/area/num", num, env);
			
			env->save();
			
			msg("$ME將探險區域設定在「"+AREA_D->query_area_idname(area, num)+"」。\n", me, 0, 1);
		
			break;
		}
		case "team":
		{
			string id;
			string type;
			object ob;
			
			if( value == "-d" )
			{
				delete("plan/team", env);
				
				env->save();

				return tell(me, "清除所有探險隊隊員設定。\n");	
			}
			
			if( sscanf(value, "%s as %s", id, type) != 2 )
				return tell(me, "請輸入正確的指令格式(help plan)。\n");
				
			if( !objectp(ob = present(id)) )
				return tell(me, "這裡沒有這個東西。\n");
		
			if( !ob->is_living() )
				return tell(me, ob->query_idname()+"不是生物。\n");
				
			if( query("boss", ob) != myid )
				return tell(me, ob->query_idname()+"的雇主不是"+pnoun(2, me)+"。\n");
				
			switch(type)
			{
				case "leader":
				{
					set("plan/team/"+type, base_name(ob), env);
					msg("$ME將$YOU設定為探險隊的"HIW"隊"NOR WHT"長"NOR"。\n", me, ob, 1);
					env->save();
					break;
				}
				case "prospector":
				{
					set("plan/team/"+type, base_name(ob), env);
					msg("$ME將$YOU設定為探險隊的"HIY"探"NOR YEL"勘者"NOR"。\n", me, ob, 1);
					env->save();
					break;
				}
				case "explorer":
				{
					set("plan/team/"+type, base_name(ob), env);
					msg("$ME將$YOU設定為探險隊的"HIG"探"NOR GRN"險家"NOR"。\n", me, ob, 1);
					env->save();
					break;
				}
				case "guard":
				{
					set("plan/team/"+type, base_name(ob), env);
					msg("$ME將$YOU設定為探險隊的"HIR"護"NOR RED"衛"NOR"。\n", me, ob, 1);
					env->save();
					break;
				}
				case "scout":
				{
					set("plan/team/"+type, base_name(ob), env);
					msg("$ME將$YOU設定為探險隊的"HIM"偵"NOR MAG"查者"NOR"。\n", me, ob, 1);
					env->save();
					break;
				}
				default:
				{
					return tell(me, "沒有 "+type+" 的隊員種類。\n");
					break;
				}
			}
			
			break;
		}
		case "time":
		{
			int time = to_int(value);
			
			if( time < 10 || time > 1440 )
				return tell(me, "探險時間最短 10 分鐘，最長 1440 分鐘。\n");
				
			set("plan/time", time, env);
			env->save();
			msg("$ME將預計的探險時間訂為 "+time+" 分鐘。\n", me, 0, 1);
			break;
		}
		default:
		{
			return tell(me, "請輸入正確的指令格式(help plan)。\n");
			break;
		}
	}
}

void do_setup(object me, string arg)
{
	string value;
	int x, y, num;
	object warehouse;
	string file;
	string city;
	object env = environment(me);
	object master = env->query_master();
	array loc = env->query_location();

	city = loc[CITY];

	if( !arg )
		return tell(me, pnoun(2, me)+"想要設定什麼項目？(help setup)\n");
	
	if( query("owner", master) != me->query_id(1) )
		return tell(me, pnoun(2, me)+"不是這個探險基地的擁有者。\n");

	sscanf(arg, "%s %s", arg, value);

	switch(arg)
	{
		case "warehouse":
		{
			if( value == "-d" )
			{
				if( !query("warehouse", master) )
					return tell(me, "這裡原本便沒有設定物資集散地。\n");
					
				delete("delivery", master);
				master->save();
				return tell(me, "取消物資集散地完畢。\n");
			}

			if( !value || sscanf(value, "%d %d,%d", num, x, y) != 3 )
				return tell(me, "請輸入正確的倉庫座標(例 1 21,33)。\n");
			
			// 交貨至倉庫
			file = CITY_ROOM_MODULE(city, (num-1), (x-1), (y-1), "warehouse");

			if( !objectp(warehouse = load_module(file)) || query("function", warehouse) != "warehouse" )
				return tell(me, "座標"+loc_short(city, num-1, x-1, y-1)+"並沒有倉庫。\n");
			
			if( warehouse != warehouse->query_master() )
			{
				file = base_name(warehouse->query_master());
				warehouse = warehouse->query_master();
			}

			if( query("owner", warehouse) != me->query_id(1) )
				return tell(me, warehouse->query_room_name()+"並不屬於"+pnoun(2, me)+"。\n");
			
			set("warehouse", file, master);
			
			master->save();
			
			msg("$ME設定"+warehouse->query_room_name()+"為此探險基地的物資集散地。\n", me, 0, 1);
			
			break;
		}

		default:
			return tell(me, "請輸入正確的選項(help setup)。\n");
	}
}

// 設定建築物內房間型態種類
nosave mapping action_info =
([
	"plan"	:
	([
		"short"	: HIG"計劃室"NOR,
		"job"	: ADVENTURER,
		"help"	:
			([
"topics":
@HELP
    探險活動的計劃室。
HELP,

"plan":
@HELP
計畫探險內容，用法如下：
plan area '區域 ID' '分區編號'		- 設定計劃探險的區域，例：plan area memoryland 3
plan team '員工 ID' as leader		- 設定某個員工作為這次探險的隊長(講求魅力)
plan team '員工 ID' as prospector	- 設定某個員工作為這次探險的探勘者(講求力量)
plan team '員工 ID' as explorer		- 設定某個員工作為這次探險的探險家(講求智力)
plan team '員工 ID' as guard		- 設定某個員工作為這次探險的護衛(講求體格)
plan team '員工 ID' as scout		- 設定某個員工作為這次探險的偵查者(講求敏捷)
plan team -d				- 清除所有探險隊員設定
plan time '分鐘'			- 設定預計的探險時間
plan start				- 開始探險！
HELP,

"setup":
@HELP
設定生產線資料的指令，用法如下：[管理指令]
  setup warehouse 1 75,90	- 設定物資集散地座標在第一衛星都市的 75,90
  setup warehouse -d	 	- 取消物資集散地的設定
HELP,

			]),
		"action":
			([
				"plan"		:	(: do_plan($1, $2) :),
				"setup"		:	(: do_setup($1, $2) :),
			]),
	]),
]);


// 設定建築物資料
nosave array building_info = ({
	
	// 建築物之中文名稱
	HIW"探險"NOR WHT"基地"NOR

	// 開張此建築物之最少房間限制
	,1

	// 城市中最大相同建築數量限制(0 代表不限制)
	,0

	// 建築物建築種類, GOVERNMENT(政府), ENTERPRISE(企業集團), INDIVIDUAL(私人)
	,INDIVIDUAL

	// 建築物可建築區域, AGRICULTURE_REGION(農業), INDUSTRY_REGION(工業), COMMERCE_REGION(商業)
	,AGRICULTURE_REGION

	// 開張儀式費用
	,"200000"
	
	// 建築物關閉測試標記
	,1

	// 繁榮貢獻度
	,0
	
	// 最高可加蓋樓層
	,1
	
	// 最大相同建築物數量(0 代表不限制)
	,0
	
	// 建築物時代
	,1
});

