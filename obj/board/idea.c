/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : idea.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-09-15
 * Note   : 構想建言板
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>

inherit BULLETIN_BOARD;

void create()
{
	set_idname(HIM"idea"NOR MAG" board"NOR, HIM"構想"NOR MAG"建言"NOR);
	
	// 利用 shadow_ob 技術來做分散式公佈欄, 即時資料皆儲存於 shadow_ob 上
	if( clonep() ) 
		set("shadow_ob", find_object(base_name(this_object())));
	else
	{
		set("boardid", "idea");
		set("capacity", 2000);
		
		initialize_board();
	}
	startup_action();
}
