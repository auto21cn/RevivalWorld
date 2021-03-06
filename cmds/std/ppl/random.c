/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : pplcmd_example.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-00-00
 * Note   : 玩家指令範例
 * Update :
 *  o 2003-00-00  
 *
 -----------------------------------------
 */

#include <feature.h>

inherit COMMAND;

string help = @HELP
	標準玩家指令。
HELP;

private void do_command(object me, string arg)
{
	int i;
	
	if( arg ) sscanf(arg, "%d", i);
	if( i <= 0 ) i = 6;
	msg("$ME從手裡丟出了一顆 1 ~ " + i + " 點的骰子 ...\n", me, 1);
	msg("骰子晃呀晃的停了下來 ... 結果出現的是 " + (random(i) + 1) + " 點。\n", me, 1);
	return;
}