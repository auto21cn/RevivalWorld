/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : mem.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-5
 * Note   : mem 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
	標準 mem 指令。
HELP;

private varargs string memory_expression(int m, int flag);

private void command(object me, string arg)
{
        object obj;
        int mem, total=0;
        
       	if( !is_command() ) return;
       	
        if (!arg)
                return tell(me, sprintf( "%s目前總共使用 %s 記憶體。\n", MUD_NAME, memory_expression(memory_info(),1 ) ), CMDMSG);

        if( arg == "-m" )
        {
        	string msg = "";
        	mixed tmp, tmp2;
		foreach(string line in explode(malloc_status(), "\n"))
		{
			if( sscanf(line, "Using %s malloc.", tmp) )
				msg += "目前使用 " + tmp + " 記憶體配置。\n";
			if( line[0..27] == "Memory allocation statistics" )
				msg += "記憶體配置統計資料 " + line[29..] + "\n";
			if( line[0..3] == "free" )
				msg += "未使用：" + line[7..] + "\n";
			if( line[0..3] == "used" )
				msg += "已使用：" + line[7..] + "\n";
			if( sscanf(line, "%*sTotal in use: %d, total free: %d", tmp, tmp2) == 3 )
			{
				tmp = memory_expression(tmp, 1);
				tmp2 = memory_expression(tmp2, 1);
				msg += "總共已使用：" + tmp + "，總共未使用：" + tmp2 + "\n";
			}
		}
                return tell(me, msg, CMDMSG);
	}
        if( arg[0..1] == "-a" )
        {
        	int i;
                object *list = sort_array(objects(),(:memory_info($2) - memory_info($1):));
                string *str = allocate(sizeof(list)+3);
                
                str[i++] = "□所有物件記憶體用量排序：";
                foreach(obj in list)
                {
                        str[i++] = sprintf("%-50s%s",file_name(obj),memory_expression( mem = memory_info(obj) ));
                        total += mem;
                }
                
                str[i++] = sprintf( "物件記憶體使用合計共 %s 記憶體。", memory_expression(total, 1) );
                str[i++] = sprintf( "%s目前總共使用 %s 記憶體。\n", MUD_NAME, memory_expression(memory_info(), 1) );

                me->more(implode(str, "\n"));
                return;
        }

        obj = find_object(arg) || present(arg, me) || present(arg, environment(me)) || find_object( resolve_path(me, arg) );
        
        if (!obj)
        	return tell(me, "沒有 "+arg+" 這個物件。\n", CMDMSG);


        tell(me, sprintf( "%O 共使用 %s 記憶體。\n", obj, memory_expression(memory_info(obj))), CMDMSG);
}

private varargs string memory_expression(int m, int flag)
{ 
        float mem = to_float(m);
        if( mem<1024 )
        {
                if(flag) return sprintf("%.2f  Bytes", mem);
                else return sprintf("%7.2f  Bytes", mem);
        }
        if( mem<1024*1024 )
        {
                if(flag) return sprintf("%.2f KBytes", (float)mem / 1024);
                return sprintf("%7.2f KBytes", (float)mem / 1024);
        }
        if(flag) return sprintf("%.3f MBytes", (float)mem / (1024*1024));
        return sprintf("%7.3f MBytes", (float)mem / (1024*1024));
}  