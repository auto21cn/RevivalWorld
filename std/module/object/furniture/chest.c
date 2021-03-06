/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : chest.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-05-17
 * Note   : 傢俱 - 置物箱
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <feature.h>
#include <material.h>

inherit OBJECT_ACTION_MOD;

// 放置物品
void do_put(object me, string arg)
{
	
	
}

// 設定物件動作
nosave mapping action_info =
([
	"help"	: "尚未建構說明。",
	"action":
		([
			"put"	: (: do_put :),
		]),
]);


// 設定物件資料
nosave array object_info = ({
	
	// 基本 name
	"置物箱"
	
	// 基本 id
	,"chest"

	// 基本材料需求
	,([ IRON : 2, WOOD : 15 ])
	
	// 製造此產品所需要的科技值
	,1500
	
	// 基本製造速度(每位員工/每遊戲時間一分鐘)
	,40
	
	// 基本價值
	,"600"
	
	// 科技樹資料
	,({ ({"chair"}) })
});
