/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : tail.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-07-11
 * Note   : tail 指令。
 * Update :
 *  o 2002-09-17 Clode 增加 -b -c -f -d 等參數
 *
 -----------------------------------------
 */
 
#include <ansi.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
 tail -b 10 filename //讀取 filename 最後 512 * 10 bytes 資料
 tail -c 10 filename //讀取 filename 最後 10 bytes 資料
 tail -f filename    //filename 檔尾增加資料時自動向你送出新增加的資料
 tail -d             //取消 tail -f 的執行狀態
HELP;

#define DEFAULT_BYTES	1080

mapping update_list = allocate_mapping(0);
mapping tailing_users = allocate_mapping(0);

string tail(string fname, int bytes)
{
	int start;
        string str;
        
        start = file_size(fname) - bytes;
        
        if( start < 0 ) start = 0;
        
        str = read_bytes(fname, start, bytes);
        
        if( !str ) return "無任何資料讀取";
        
        // 取整數行 print , 把不足行或可能不足的行捨掉
        if( start && read_bytes(fname, start-1, 1) != "\n") str = str[strsrch(str, "\n")+1..];
        
        return str;
}

/* 用來看檔尾的工具 */
// fname 是欲看檔尾的檔案名稱
int tail_process(object me, string fname, string option, int number) 
{
	int bytes = DEFAULT_BYTES;
	
	if( number < 0 ) number = 0;
	
	switch(option)
	{
		case "-b":
			if( number )
				bytes = number * 512;
			
			return me->more(tail(fname, bytes)+"\n");
		case "-c":
			if( number )
				bytes = number;
				
			return me->more(tail(fname, bytes)+"\n");
		case "-f":
			set_heart_beat(1);
			
			if( !undefinedp(tailing_users[fname]) )
				tailing_users[fname] += ({ me });
			else
				tailing_users[fname] = ({ file_size(fname), me });
			
			me->more(tail(fname, DEFAULT_BYTES)+"\n");
			
			update_list[fname] = stat(fname)[1];
			
			break;
		default:
			me->more(tail(fname, bytes)+"\n");
	}
}

void heart_beat()
{
	foreach(string fname, int last_time_file_touched in update_list)
	{
		tailing_users[fname] -= ({ 0 });
		
		if( sizeof(tailing_users[fname]) == 1 )
		{
			map_delete(update_list, fname);
			map_delete(tailing_users, fname);
			continue;
		}
		
		if( stat(fname)[1] != last_time_file_touched )
		{
			foreach( object users in tailing_users[fname][1..] )
				tell(users, NOR WHT+"\n[ "HIW+ctime(time())[11..18]+" "+fname+" new tail coming ... "+NOR WHT+"]\n"+remove_ansi(tail(fname, (file_size(fname)-tailing_users[fname][0])))+NOR"\n", CMDMSG);
			
			update_list[fname] = stat(fname)[1];
			tailing_users[fname][0] = file_size(fname);
		}
	}
}

private void command(object me, string arg)
{
	int number;
	string *parse;
	string option;
	
	if( !is_command() ) return;
	
	if( !arg ) return tell(me, "請輸入檔案名稱。\n", CMDMSG);
	
	parse = single_quote_parse(arg);
	
	if( sizeof(parse) >= 3 )
	{
		option = parse[0];
		number = to_int(parse[1]);
		arg = implode(parse[2..], " ");
	}
	else if( sizeof(parse) == 2 )
	{
		option = parse[0];
		arg = parse[1];
	}
	else
		arg = parse[0];
	
	if( arg == "-d" )
	{
		int cancel;
		
		foreach(string fname, array data in tailing_users)
		{
			if( member_array(me, data) != -1 )
			{
				tailing_users[fname] -= ({ me });
				
				if( sizeof(tailing_users[fname]) == 1 )
				{
					map_delete(update_list, fname);
					map_delete(tailing_users, fname);
				}
				
				if( !sizeof(tailing_users) )
					set_heart_beat(0);
					
				tell(me, "取消 tail -f "+fname+"。\n", CMDMSG);
				cancel = 1;
			}
		}
		
		if( !cancel )
			return tell(me, pnoun(2, me)+"並沒有執行任何 tail -f 的指令。\n", CMDMSG);
		return;
	}
	else if( arg == "-l" )
		return tell(me, sprintf("%O\n%O\n", update_list, tailing_users), CMDMSG);
	
	arg = resolve_path(me, arg);
	
	if( file_size(arg) < 0 ) return tell(me, "沒有 "+arg+" 這個檔案。\n", CMDMSG);
	
	tail_process(me, arg, option, number);
}