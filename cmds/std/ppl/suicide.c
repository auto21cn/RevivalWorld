/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : suicide.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-01-31
 * Note   : 
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
自殺指令

這個指令可以讓系統刪除該玩家角色的資料
若您已經厭倦了重生的世界，您可以使用這個指令刪除角色

相關指令:
HELP;

private void redo_passwd(object me);

private void confirm_suicide(object me, string arg)
{
	/* 避免被繼承 */
	if( !is_command() ) return;

	if( lower_case(arg)[0] == 'y' )
	{
		set("password", PASSWORD_D->query_password(me->query_id(1)), me);
		
		me->save();
		if( CHAR_D->destruct_char(me->query_id(1)) )
		{
			CHANNEL_D->channel_broadcast("news", me->query_idname()+"突然想通了，從口袋中拿起兩個 C4 擺在腳下，「轟」，只見一團黑炭衝上雲霄與世長辭，創世至今已有 "+SERIAL_NUMBER_D->query_serial_number("suicide")+" 個自殺案例。");
			
			catch( filter_array(all_inventory(me), (: !query_temp("keep", $1) :))->move(query_temp("location", me) || environment(me)) );

			destruct(me);
		}
		else
			CHANNEL_D->channel_broadcast("news", me->query_idname()+"突然想通了，從口袋中拿起兩個 C4 擺在腳下，「...」，沒爆炸，看來得跟巫師研究研究。");
	}
	else tell(me, "帳號刪除取消。\n", CMDMSG);
}

private void check_passwd(object me, string arg)
{
	string old_passwd = PASSWORD_D->query_password(me->query_id(1));
	
	if( !old_passwd ) return tell(me, "舊密碼之搜尋結果發生錯誤，請通知巫師處理。\n", CMDMSG);
	
	if( !arg || crypt(arg, old_passwd) != old_passwd )
	{
		tell(me, "密碼輸入錯誤。\n", CMDMSG);
		return redo_passwd(me);
	}
	
	tell(me, HIR"警告：自殺後系統將刪除所有"+me->query_idname()+"的資料"NOR"\n是否真的要自殺(Yes/No)？", CMDMSG);
	input_to( (: confirm_suicide, me :), 3 );
}

private void confirm_redo_passwd(object me, string arg)
{
	if( !arg ) return redo_passwd(me);
	arg = lower_case(arg);
	
	if( arg[0] == 'y' )
	{
		tell(me, "請輸入"+pnoun(2, me)+"的密碼：", CMDMSG);
		input_to( (: check_passwd, me :), 3 );
	}
	else tell(me, "帳號刪除取消。\n", CMDMSG);
}
	
private void redo_passwd(object me)
{
	tell(me, "帳號刪除失敗，是否重新輸入(Yes/No)？", CMDMSG);
	input_to( (: confirm_redo_passwd, me :) );
}

private void do_command(object me, string arg)
{
	if( call_stack(1)[<1] != me || !is_command() ) return;

	if( wizardp(me) )
		return tell(me, "巫師不得自殺，巫師自殺前必須先降為玩家。\n", CMDMSG);

	tell(me, "請輸入"+pnoun(2, me)+"的密碼：", CMDMSG);
	input_to( (: check_passwd, me :), 3 );
}