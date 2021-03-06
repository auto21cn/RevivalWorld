/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _command_ppl.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-4
 * Note   : 玩家指令系統
 * Update :
 *  o 2001-07-08 Clode 整理 action 程式碼
 *
 -----------------------------------------
 */

#include <daemon.h>

/* 啟動互動性質 */
nomask void enable_interactive()
{
	if( !interactive(this_object()) ) destruct(this_object());
	
	CHANNEL_D->register_channel(this_object(), query("channels"));
}
