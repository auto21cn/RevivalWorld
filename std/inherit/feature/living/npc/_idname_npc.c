/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _idname_npc.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-01-21
 * Note   : 
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <status.h>

#define ID	0
#define NAME	1

string *id_name;

// Passerby@IT 送給 RW 的 -.-  by Clode
// MudOS 處理 replace_string() 時的效率太差, 因此停用此函式 by Clode (2006/09/12)
/*
nomask string noregexp(string arg)
{
        arg = replace_string(arg, "\\", "\\\\");
        arg = replace_string(arg, "|", "\\|");
        arg = replace_string(arg, "*", "\\*");
        arg = replace_string(arg, "+", "\\+");
        arg = replace_string(arg, "?", "\\?");
        arg = replace_string(arg, "^", "\\^");
        arg = replace_string(arg, ".", "\\.");
        arg = replace_string(arg, "[", "\\[");
        arg = replace_string(arg, "]", "\\]");
        arg = replace_string(arg, "(", "\\(");
        arg = replace_string(arg, ")", "\\)");
        return arg;
}*/

// 多段式縮寫判斷
int id(string arg)
{
	int i, size;
	string *idarray;
	       
	if( !arg || !id_name[ID] ) return 0;

	//arg = "^"+noregexp(lower_case(remove_ansi(arg)));
	
	arg = lower_case(remove_ansi(arg));
	
	idarray = explode(lower_case(remove_ansi(id_name[ID])), " ");
	
	size = sizeof(idarray);
	
	for(i=0;i<size-1;i++)
		idarray |= ({ implode(idarray[i..], " ") });
	
	foreach(string id in idarray)
	{
		if( id[0..strlen(arg)-1] == arg )
			return 1;
	}
	
	return 0;
	
	// ID 簡寫判別
	//return sizeof( regexp(idarray,arg) );
}

// 回傳物件 ID
varargs string query_id(int raw)
{
	return undefinedp(raw) ? id_name[ID] : remove_ansi(id_name[ID]);
}

// 回傳物件 NAME 
varargs string query_name(int raw)
{
	return undefinedp(raw) ? id_name[NAME] : remove_ansi(id_name[NAME]);
}

// 回傳物件完整名稱
varargs string query_idname(int raw)
{
	if( !arrayp(id_name) ) return 0;
	return undefinedp(raw) ? id_name[NAME]+"("+ansi_capitalize(id_name[ID])+")" : remove_ansi(id_name[NAME])+"("+capitalize(remove_ansi(id_name[ID]))+")";
}

varargs string *set_idname(string id, string name)
{
	if( !stringp(id) && !stringp(name) ) return 0;

	if( !arrayp(id_name) )
		id_name = allocate(2);
	
	if( stringp(id) )
	{
		id = kill_repeat_ansi(remove_fringe_blanks(id)+NOR);
 
		id_name[ID] = id;
	}
	
	if( stringp(name) )
	{
		name = kill_repeat_ansi(remove_fringe_blanks(name)+NOR);
		
		id_name[NAME] = name;
	}

	return id_name;
}

varargs string add_status(string status, int sec)
{
	mapping statusmap = query_temp("status");
	
	if( sec < 0 )
		error("不合理的秒數");
	else if( undefinedp(sec) )
		sec = -1;

	if( !mapp(statusmap) )
		statusmap = allocate_mapping(0);
		
	statusmap[status] = sec;
	
	set_temp("status", statusmap);
}

varargs void remove_status(string status)
{
	mapping statusmap = query_temp("status");
	
	if( !mapp(statusmap) ) return;
		
	if( !undefinedp(status) )
		map_delete(statusmap, status);
	else
	{
		delete_temp("status");
		return;
	}
	
	if( !sizeof(statusmap) )
		delete_temp("status");
}

varargs string query_status(int flag)
{
	string status_buff;
	string status = "";
	mapping extra_status;

	if( mapp(extra_status = query_temp("status")) && sizeof(extra_status) )
		foreach(string key, int sec in extra_status)
			status += key+NOR" ";

	if( this_object()->is_module_npc() )
		status += HIM"雇"NOR MAG"用 "NOR;
	
	if( query("faint") ) 
		status += HIR"昏"NOR RED"倒 "NOR;
	
	if( !(flag & STATUS_NO_OBBUFF) )
	foreach(object ob in this_object()->query_equipment_objects())
	{
		if( stringp(status_buff = query("buff/status", ob)) )
			status += status_buff+" ";
	}

	if( status == "" )
		return 0;
	
	return status+NOR;
}

varargs string short(int need_quantity)
{
	string ret = "";
	string status = query_status();
	string position = query("position");
	string nickname = query("nickname");

	if( position )
		ret += position+(nickname?"":" ");

	if( nickname )
		ret += "「"+nickname+"」";

	if( need_quantity && !this_object()->is_living() )
	{
		mixed amount = query_temp("amount") || 1;
		
		if( !query("flag/no_amount") )
			ret += " "+NUMBER_D->number_symbol(amount)+" "+(query("unit")||"個");
	}
	
	ret += query_idname();
	
	if( status  )
		ret += " "+status;

	return kill_repeat_ansi(ret+NOR);
}

varargs string long(int need_quantity)
{
	return DESCRIPTION_D->query_living_description(this_object(), need_quantity);
}
