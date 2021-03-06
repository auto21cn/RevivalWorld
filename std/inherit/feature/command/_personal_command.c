/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _personal_command.c
 * Author : Abcd@RevivalWorld
 * Date   : 2005-1-12
 * Note   : 個人指令繼承檔案
 * Update :
 *
 -----------------------------------------
 */

#include <daemon.h>
#include <origin.h>

private void command(object me, string arg);

private void do_command(object me, string arg)
{
        if( call_stack(1)[<1] == me || call_stack(1)[<2] == me || call_stack(1)[4] == me && call_stack(2)[5] == "force_me" ) 
                command(me, arg);
        else
                printf("權限檢查:\n%O\n%O\n%O\n指令使用權限不足，請恰詢管理者。\n", call_stack(1), call_stack(2), call_stack(3));
}

nomask function query_fp()
{
        // 不使用機車的權限檢查 -.-
        return (: do_command($1, $2) :);
}
