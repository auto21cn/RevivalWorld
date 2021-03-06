/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : room_s_afterroom.c
 * Author : tzj@RevivalWorld
 * Date   : 2006-09-09
 * Note   : 蘇菲亞家的置物間
 * Update :
 *  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>

inherit STANDARD_ROOM;

string help = @HELP
在這個平凡無奇的房間內並沒有任何特殊的資訊。
HELP;

void create()
{
	set("short", CYN"置物間"NOR);
	set("long",@TEXT
　　這裡置放了一些盛水器具及幾袋雞群的飼料，也放置了農耕所需要用到的各
種工具。由於經過整理，所以雖然此處東西很多又雜，但看起來卻一點也不亂。
從這裡往屋子深處看過去，可以看到似乎是廚房的地方，食物的香氣不時從該處
傳過來。
TEXT);


	set("exits", ([
		//往東走是門
		"east"    : ({ 82, 66, 0, "lightforest", 0, 0, "82/66/0/lightforest/0/0" }),
		
		//往南走是牆
		//"south" : ,
		
		//往西走
		"west"    : "/quest/suphia_and_benjamin/room_s_kitchen",
		
		//往北走是門
		"north" : ({ 81, 65, 0, "lightforest", 0, 0, "81/65/0/lightforest/0/0" }),
			
		//此處有通往2F的樓梯
		//"up"    : ,
		
	]));

/*	預留放NPC或物品的...
	set("objects", ([
		"/quest/suphia_and_benjamin/npc_suphia" : 1 ,
	]));
*/
	
	::reset_objects();
	replace_program(STANDARD_ROOM);
}
