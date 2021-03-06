/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : char_d.c
 * Author : Clode@RevivalWorld
 * Date   : 2000-12-29
 * Note   : 角色精靈
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <gender.h>

void create_char(object user, int gender)
{
	if( previous_object() != find_object(PPL_LOGIN_D) ) return;

	// 初始年齡
	set("age", 15, user);
	
	// 性別
	set("gender", gender, user);
	
	// 五大屬性
	set("attr/str", 10, user);
	set("attr/phy", 10, user);
	set("attr/int", 10, user);
	set("attr/agi", 10, user);
	set("attr/cha", 10, user);
	
	// 各項能力值
	set("abi/stamina/max", 500, user);
	set("abi/stamina/cur", 500, user);
	set("abi/health/max", 100, user);
	set("abi/health/cur", 100, user);
	set("abi/energy/max", 100, user);
	set("abi/energy/cur", 100, user);
	
	// 各項經驗值
	set("exp/social/cur", 0, user);
	set("exp/social/tot", 0, user);
	
	// 狀態值
	set("stat/water/max", 100, user);
	set("stat/water/cur", 0, user);
	set("stat/food/max", 100, user);
	set("stat/food/cur", 0, user);
	
	set("createtime", time(), user);

	/* 設定標準頻道 */
	set("channels", CHANNEL_D->query_default_channel(user->query_id(1)), user);

}

void create_npc(object npc)
{
	if( !npc->is_npc() ) return;
	
	// 初始年齡
	if( !query("age", npc) ) set("age", 15, npc);
	
	// 各項屬性
	if( !query("attr", npc) )
	{
		set("attr/str", 10, npc);
		set("attr/phy", 10, npc);
		set("attr/int", 10, npc);
		set("attr/agi", 10, npc);
		set("attr/cha", 10, npc);
	}

	// 各項能力值
	if( !query("abi", npc) )
	{
		set("abi/stamina/max", 500, npc);
		set("abi/stamina/cur", 500, npc);
		set("abi/health/max", 100, npc);
		set("abi/health/cur", 100, npc);
		set("abi/energy/max", 100, npc);
		set("abi/energy/cur", 100, npc);
	}
	// 各項經驗值
	set("exp/social/cur", 0, npc);
	set("exp/social/tot", 0, npc);

	// 狀態值
	set("stat/water/max", 100, npc);
	set("stat/water/cur", 0, npc);
	set("stat/food/max", 100, npc);
	set("stat/food/cur", 0, npc);
}

int destruct_char(string id)
{
	string path = user_path(id);
	string backup_path;
	
	if( file_size(path) != -2 ) return 0;
	
	// 刪除勞工資訊
	LABOR_D->del_labors(id);

	// 移除密碼資訊
	PASSWORD_D->del_password(id);
	
	// 移除房地產資訊
	ESTATE_D->remove_estate(id);
	
	// 移除金錢資訊
	MONEY_D->clear_money(id);

	// 移除市民資訊
	CITY_D->remove_citizen(id);
	
	// 刪除排行榜紀錄
	TOP_D->delete_top_id(id);
	
	// 註銷企業成員
	ENTERPRISE_D->unregister_member(id);

	backup_path = user_deleted_path(id);
	intact_path(backup_path);
	
	rmtree(user_backup_path(id));
	rmtree(user_deleted_path(id));
	
	return !rename(path, backup_path);
}

void create()
{

}

string query_name()
{
	return "角色系統(CHAR_D)";
}
