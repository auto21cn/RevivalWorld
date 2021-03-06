/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : beep.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-06-10
 * Note   : beep 指令
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <daemon.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
呼叫指令

這個指令可以用來呼叫其他使用者，若該使用者的 Client 支援則會播放提示聲

指令格式:
beep <使用者代號>

相關指令: waitback
HELP;

private void do_command(object me, string arg)
{
	object ob;
    	
    	if( !arg )
    		return tell(me, pnoun(2, me)+"想呼叫哪位線上玩家？\n", CMDMSG);
    	if( !(ob = find_player(arg)) ) 
    		return tell(me, "沒有這玩家。\n", CMDMSG);
    	
        msg("$ME拿著牌尺不斷的敲打著$YOU的大頭，似乎是想提醒$YOU打快一點。\n", me, ob, 1, ENVMSG);
    	tell(ob,  "\a", 0);
}
