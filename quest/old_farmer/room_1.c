/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : room_1.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-08-19
 * Note   : 標準房間
 * Update :
 *  2006-09-07 遷位 by Tzj@RevivalWorld
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
	set("short", GRN"農舍"NOR);
	set("long",@TEXT
    這間屋子看起來十分老舊，木製的窗戶經過風吹雨打已經不堪使用。屋內的四
周擺放著各式各樣的農耕工具，但看起來似乎已經好一陣子沒使用了。在角落放著
好幾只空米袋，只見一位已經年邁的老農夫站在角落不停地咳嗽，似乎又累又餓的
樣子。
TEXT);

	set("exits", ([
		//原設定出口："east" 	: ({ 50, 50, 0, "lightforest", 0, 0, "50/50/0/lightforest/0/0" }),
		//原設定出口："south"	: "/quest/old_farmer/room_2",
		//該房間往東走後顯示座標是51,51，code設定是50,50。
		
		//以下為`06-09-07，由 tzj 遷位所變更的位置。
		"south" : ({ 74, 41, 0, "lightforest", 0, 0, "74/41/0/lightforest/0/0" }),
		"east"  : "/quest/old_farmer/room_2",
	]));
	
	set("objects", ([
		"/quest/old_farmer/npc_old_farmer" : 1 ,
	]));
	
	::reset_objects();
	replace_program(STANDARD_ROOM);
}
