/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : hyperlink.c
 * Author : Cookys@RevivalWorld
 * Date   : 2002-09-15
 * Note   : 建立與修改檔案標頭檔
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */
 
#include <feature.h>
#include <ansi.h>
#include <daemon.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
        標準 hyperlink 指令
HELP;

#define	FROM	0
#define TO	1

#define ENV_OBJ  	0
#define ENV_EXIT	1
#define ENV_PATH	2

/*
** is_mapsys 按下去,把 array 抓起來等按第二次,第二次如果不是 mapsys 要按 exit name
** not_mapsys 按下去要要求 exit name,看第二次是不是 map_sys,不是的話要 exitname
**
**
*/

void generate_2way_link(int flag)
{
	mixed data;
	
	if( !stringp( ( data=query_temp("hyperlink",this_player()))[TO][ENV_OBJ]) && !objectp(data[TO][ENV_OBJ]) )
		return tell(this_player(), "請走到要連結的地點在下一次指令。\n", CMDMSG);
		

	if( data[FROM][ENV_OBJ]==MAP_D)
		MAP_D->set_special_exit(data[FROM][ENV_PATH],data[TO][ENV_PATH]);
	else	set("exits/"+data[FROM][ENV_EXIT],data[TO][ENV_PATH],data[FROM][ENV_OBJ]);
	
	if(flag)
	{
		delete_temp("hyperlink",this_player());
		return tell(this_player(), "完成!\n", CMDMSG);
	}
	
	if(data[TO][ENV_OBJ]==MAP_D)
		MAP_D->set_special_exit(data[TO][ENV_PATH],data[FROM][ENV_PATH]);
	else	set("exits/"+data[TO][ENV_EXIT],data[FROM][ENV_PATH],data[TO][ENV_OBJ]);
	delete_temp("hyperlink",this_player());
	return tell(this_player(), "完成！\n", CMDMSG);
	
}

private void command(object me, string arg)
{
	int flag;
	object env=environment(me);
	mixed data=query_temp("hyperlink",me);

        if( !is_command() ) return;

        if( !arg && !env->is_maproom() )
	        return tell(me, "請輸入要連結的出口名稱或方向。 \n", CMDMSG);
	        
	if( sscanf(arg||"","-s %s",arg) || arg == "-s" )
		flag = 1;
	
	if( undefinedp(data) )
	{
		data = allocate(2);
		data[TO]=allocate(3);
		data[FROM]=allocate(3);
	}
	
	if( env->is_maproom() )
	{
		//mixed tmp=MAP_D->query_special_exit(query_temp("location",me));
		//if( stringp(tmp) || arrayp(tmp) )
		if( !undefinedp(MAP_D->query_special_exit(query_temp("location",me))) )
			return tell(me, "此處已經有連結出口，即時地圖上只能有一處出口。\n請先將其刪除後再連結。\n", CMDMSG);
		
		if( stringp(data[FROM][ENV_OBJ]) || objectp(data[FROM][ENV_OBJ]) )
			data[TO]=({MAP_D,0,query_temp("location",me)});
		else data[FROM]=({MAP_D,0,query_temp("location",me)});
		
		set_temp("hyperlink",data,me);
		
		return generate_2way_link(flag);
	} else {
	
                if( query("exists", environment(me)) && !undefinedp(query("exits",environment(me) )[arg] ) )
			return tell(me, HIC + arg + NOR +" 這個出口已存在。\n", CMDMSG);
		if( stringp(data[FROM][ENV_OBJ]) || objectp(data[FROM][ENV_OBJ]) )
			data[TO]=({env,arg,file_name(env)});
		else data[FROM] =({env,arg,file_name(env)});
		
		set_temp("hyperlink",data,me);
		
		return generate_2way_link(flag);
	}			
		
		
	//set_temp("hyperlink",,me);

}
