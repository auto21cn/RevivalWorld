/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : search.c
 * Author : Sinji@RevivalWorld
 * Date   : 2003-06-21
 * Note   : 
 * Update :
 *  o 2003-00-00
 *
 -----------------------------------------
 */

#include <feature.h>
#include <message.h>
#include <daemon.h>
inherit COMMAND;

private void command(object me, string arg)
{
	mixed ftp_sockets;
	string msg, stat ="";

	if( !find_object(FTP_D) )
		return tell(me, "FTP_D 尚未啟動。\n");

	ftp_sockets = FTP_D->query_sockets();

	if( !ftp_sockets || !sizeof(ftp_sockets) )
                return tell(me, "FTP_D 目前沒有任何使用者連線。\n");
	msg = sprintf("%-3s %-12s %-17s %-8s %-s\n", "FD", "帳號", "連線位置", "狀態", "最後指令"); 
	foreach(string fd, mixed val in ftp_sockets)
	{
		if( !undefinedp(val["retr"]) ) stat += "<下載中>";
		if( !undefinedp(val["stor"]) ) stat += "<上傳中>";
		msg += sprintf("%3d %-12s %-17s %-8s %s\n",
			fd,
			(val["login"] ? val["user"]:"(未登入)"),
			val["address"],
			stat, 
			(val["lastcmd"] || "(無)"),
		);
	}
	tell(me, msg);
	return;
}
