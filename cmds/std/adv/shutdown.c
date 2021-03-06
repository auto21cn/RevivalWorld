/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : shutdown.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-5
 * Note   : shutdown 指令
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <daemon.h>
#include <feature.h>
#include <ansi.h>
#include <mudlib.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
	shutdown [-r|-h|-k] [time|now|cancel|force]
HELP;

#define SYNOPSIS		"shutdown [-r|-h|-k] [time|now|cancel|force]\n"

//#define BEEP			"\a"
#define MAX_SHUTDOWN_WAITING	1800

private nosave int shutdowntime;
private nosave string shutdowntype;

private void regular_remove_process()
{
	string systeminfo = SYSTEM_D->query_total_system_info();

	// 儲存玩家資料
	foreach( object user in users() )
	{
		reset_eval_cost();
		
		if( !objectp(user) || !environment(user) ) continue;
	
		tell(user, "\n"+systeminfo+"\n");

		flush_messages(user);

		if( wizardp(user) )
			LOGOUT_D->save_user_info(user);
		else
			LOGOUT_D->quit(user);
	}

	// 先紀錄主要資料
	MONEY_D->save();
	MAP_D->save();
	ESTATE_D->save();
	CITY_D->save_all();
	
	// 使所有物件正常關閉
	foreach(object ob in objects())
	{
		reset_eval_cost();
		ob->remove();
	}
}

void heart_beat()
{
	if( !(shutdowntime % 60) || (shutdowntime <= 20 && !(shutdowntime % 2) ) )
	{
		switch(shutdowntype)
		{
		case "r":
			shout(HIY"\n\n再過 "+shutdowntime+" 秒，"MUD_FULL_NAME HIY"將開始執行重新啟動程序。\n\n"NOR);
			break;
		case "h":
			shout(HIY"\n\n再過 "+shutdowntime+" 秒，"MUD_FULL_NAME HIY"將開始執行關閉程序。\n\n"NOR);
			break;
		case "k":
			shout(HIY"\n\n再過 "+shutdowntime+" 秒，"MUD_FULL_NAME HIY"將開始執行踢除玩家程序。\n\n"NOR);
			break;
		}
	}

	if( !shutdowntime-- )
	{
		set_heart_beat(0);
		
		switch(shutdowntype)
		{
			case "r":
				shout(BEEP"\n\n"MUD_FULL_NAME"開始執行重新啟動程序...\n\n"HIR"請稍候幾秒後等待系統重新啟動完畢後再行登入。\n\n"NOR);
				regular_remove_process();
				shutdown();
				break;
			case "h":
				shout(BEEP"\n\n"MUD_FULL_NAME"開始執行關閉程序...\n\n"HIR"請等待系統啟動後再行登入。\n\n"NOR);
				regular_remove_process();
				shutdown(-1);
				break;
			case "k":
				shout(BEEP"\n\n"MUD_FULL_NAME"開始執行踢除玩家程序...\n\n"HIR"請稍後幾秒後再行登入。\n\n"NOR);
				foreach( object user in users() )
					if( !wizardp(user) ) LOGOUT_D->quit(user);
				break;
			default:
				break;
		}
		
		shutdowntime = 0;
		shutdowntype = 0;
	}
}

void system_auto_reboot()
{
	if( call_stack(1)[1]!=load_object(TIME_D) )
	{
		log_file("command/deny_auto_shutdown", "deny shutdown - "+sprintf("%O\n",call_stack(1)));
		return;
	}
	//tell(find_player("cookys"),sprintf("%O\n",call_stack(1)));
	
	log_file("command/shutdown", "system auto reboot.");
	
	if( query_heart_beat() ) return;
	
	shutdowntime = 300;
	shutdowntype = "r";
	set_heart_beat(10);
	
	shout(BEEP"\n\n"+HIY+MUD_FULL_NAME HIY"記憶體使用過量，自行啟動了重新啟動程序。\n\n"NOR);
}


private void command(object me, string arg)
{
	int itime;
	string type, time;
	
	if( !is_command() ) return;
	
	if( !arg )
		return tell(me, SYNOPSIS);
	
	arg = remove_fringe_blanks(lower_case(arg));
	
	sscanf( arg, "-%s %s", type, time );
	
	if( !type || !time ) 
		return tell(me, SYNOPSIS);
	
	log_file("command/shutdown", me->query_idname()+" shutdown -"+type+" "+time);
	
	switch(time)
	{
		case "force":
			switch(type)
			{
			case "r":
				shout(BEEP"\n\n"+HIR+me->query_idname()+HIR"強制重啟系統，請稍後幾秒後等待系統重新啟動完畢後再行登入。\n\n"NOR);
				shutdown();
				break;
			case "h":
				shout(BEEP"\n\n"+HIR+me->query_idname()+HIR"強制關閉系統，請等待系統啟動後再行登入。\n\n"NOR);
				shutdown(-1);
				break;
			case "k":
				shout(BEEP"\n\n"+HIR+me->query_idname()+HIR"強制踢除所有玩家，請稍後幾秒後再行登入。\n\n"NOR);
				foreach( object user in users() )
					if( !wizardp(user) ) destruct(user);
				break;
			default:
				return tell(me, SYNOPSIS);
			}
			break;
			
		case "now":
			switch(type)
			{
			case "r":
				shout(BEEP"\n\n"+HIY+me->query_idname()+HIY"執行了"MUD_FULL_NAME HIY"的立即重新啟動程序...\n\n"HIR"請稍後幾秒後等待系統重新啟動完畢後再行登入。\n\n"NOR);
				regular_remove_process();
				shutdown();
				break;
			case "h":
				shout(BEEP"\n\n"+HIY+me->query_idname()+HIY"執行了"MUD_FULL_NAME HIY"的立即關閉程序...\n\n"HIR"請等待系統啟動後再行登入。\n\n"NOR);
				regular_remove_process();
				shutdown(-1);
				break;
			case "k":
				shout(BEEP"\n\n"+HIY+me->query_idname()+HIY"執行了"MUD_FULL_NAME HIY"的立即踢除玩家程序...\n\n"HIR"請稍後幾秒後再行登入。\n\n"NOR);
				regular_remove_process();
				foreach( object user in users() )
					if( !wizardp(user) ) destruct(user);
				break;
			default:
				return tell(me, SYNOPSIS);
			}
			break;
		case "cancel":
			if( query_heart_beat() )
			{
				set_heart_beat(0);
				
				switch(shutdowntype)
				{
					case "r":
						shout(BEEP"\n\n"+HIY+me->query_idname()+HIY"取消了"MUD_FULL_NAME HIY"的預約重新啟動程序。\n\n"NOR);
						break;
					case "h":
						shout(BEEP"\n\n"+HIY+me->query_idname()+HIY"取消了"MUD_FULL_NAME HIY"的預約關閉程序。\n\n"NOR);
						break;
					case "k":
						shout(BEEP"\n\n"+HIY+me->query_idname()+HIY"取消了"MUD_FULL_NAME HIY"的預約踢除玩家程序。\n\n"NOR);
						break;
				}
				
				shutdowntime = 0;
				shutdowntype = 0;
				return;
			}
			else
				return tell(me, "目前並沒有任何預約 shutdown 程序。\n");
			break;
		default:
			if( sscanf(time, "%d", itime) )
			{
				if( itime > MAX_SHUTDOWN_WAITING )
					return tell(me, "最多只能設定 "+MAX_SHUTDOWN_WAITING+" 秒的預約時間。\n");
				
				if( type != "r" && type != "h" && type != "k" )
					return tell(me, SYNOPSIS);
					
				switch(type)
				{
					case "r":
						shout(BEEP"\n\n"+HIY+me->query_idname()+HIY"執行了"MUD_FULL_NAME HIY"的 "+itime+" 秒預約重新啟動程序。\n\n"NOR);
						break;
					case "h":
						shout(BEEP"\n\n"+HIY+me->query_idname()+HIY"執行了"MUD_FULL_NAME HIY"的 "+itime+" 秒預約關閉程序。\n\n"NOR);
						break;
					case "k":
						shout(BEEP"\n\n"+HIY+me->query_idname()+HIY"執行了"MUD_FULL_NAME HIY"的 "+itime+" 秒預約踢除玩家程序。\n\n"NOR);
						break;
					default:
						return tell(me, SYNOPSIS);
				}
				
				shutdowntime = itime;
				shutdowntype = type;
				
				set_heart_beat(10);
			}
			else
				return tell(me, SYNOPSIS);
	}
}