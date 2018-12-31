/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : room.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-3-21
 * Note   : 標準房間
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <inherit.h>

inherit STANDARD_ROOM;

string help = @HELP
在這個平凡無奇的房間內並沒有任何特殊的資訊。
HELP;

void create()
{
	set("short","空房間");
	set("long",@TEXT
    這是一個什麼都沒有的房間，只見房間四周除了破舊的牆壁外, 實在看不出這個房間有什麼特別之處。

TEXT);
}