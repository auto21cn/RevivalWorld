/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : userid.c
 * Author : Sinji@RevivalWorld
 * Date   : 2002-8-23
 * Note   : sinji 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <socket.h>
#include <message.h>

#define AUTH_PORT		113
#define IDENT_CALL_HANDLE       0
#define IDENT_COMMANDER         1
#define IDENT_USER              2
#define IDENT_PORT		3
#define IDENT_RPORT		4

inherit COMMAND;

string help = @HELP
指令格式：ident <使用者>

這個指令可以取得使用者在其主機上所使用的
帳號及主機資訊。
HELP;

private nosave object receiver;
private nosave mapping idents = allocate_mapping(0);
private void time_out(int fd);

private void command(object me, string arg)
{
	object ob;
	int fd, res, address, port, rport, call_handle;
	string socket;

	if( !is_command() ) return;

	if( !arg ) ob = me;
	if( !ob ) ob = find_player(arg);
	if( !ob )
		return tell(me, "沒有 " + arg + " 這個使用者。\n", CMDMSG);
	if( (socket = socket_address(ob)) == "" )
		return tell(me, "無法取得遠端連線位址。\n", CMDMSG);
	if( sscanf(socket, "%s %d", address, rport) != 2 )
		return tell(me, "位址格式錯誤。\n", CMDMSG);
	if( !(port = query_ip_port(ob)) )
	 	return tell(me, "無法取得連線埠號。\n", CMDMSG);
	
	fd = socket_create(STREAM, "create_read_callback", "create_close_callback");

	if( fd < 0 )
		return tell(me, "無法建立連線：" + socket_error(fd), CMDMSG);

	res = socket_connect(fd, address + " " + AUTH_PORT, "read_callback", "write_callback");

	if( res < 0 )
	{
		tell(me, "無法連線：" + socket_error(fd), CMDMSG);
		socket_close(fd);
		return;
	}
	tell(me, "[FD: " + fd + "] 與使用者主機連線中...\n", CMDMSG);
	call_handle = call_out((: time_out :), 30, fd);
	idents[fd] = ({ call_handle, me, ob, port, rport });
	return;

}
void create_read_callback(int fd, mixed message) {}
void create_close_callback(int fd)
{
	int call_handle;
	object ob;

	call_handle = idents[fd][IDENT_CALL_HANDLE];

	if( ob = idents[fd][IDENT_COMMANDER] )
		tell(ob, "\n[FD: " + fd + "] 使用者主機拒絕連線。\n", CMDMSG);
	if( find_call_out(call_handle) != -1 ) remove_call_out(call_handle);
	socket_close(fd);
	return;
}
void write_callback(int fd)
{
	int res;
	object ob, user;

	if( !(ob = idents[fd][IDENT_COMMANDER]) )
	{
		socket_close(fd);
		return;
	}
	if( !(user = idents[fd][IDENT_USER]) )
	{
		tell(ob, "\n[FD: " + fd + "] 使用者已離線。\n", CMDMSG);
		socket_close(fd);
		return;
	}
	tell(ob, "\n[FD: " + fd + "] 取得使用者資訊 ...\n", CMDMSG);
	res = socket_write(fd, idents[fd][IDENT_RPORT] + "," + idents[fd][IDENT_PORT] + "\n");
	if( res < 0 )
	{
		tell(ob, "[FD: " + fd + "] 訊息傳送錯誤：" + socket_error(res), CMDMSG);
		socket_close(fd);
	}
}
/*
 * receive ident response
 *
 * format of response (string):
 *     their_port, our_port : USERID : operating_system : user_id
 *     their_port, our_port : USERID : OTHER : user_id_octet_string
 *     their_port, our_port : ERROR : INVALID-PORT
 *     their_port, our_port : ERROR : NO-USER
 *     their_port, our_port : ERROR : HIDDEN-USER
 *     their_port, our_port : ERROR : UNKNOWN-ERROR
 *
 */
void read_callback(int fd, mixed info)
{
	int rport, port;
	string type, arg, os, uid;
	object ob, user;

	remove_call_out(idents[fd][IDENT_CALL_HANDLE]);

	if( !(ob = idents[fd][IDENT_COMMANDER]) )
	{
		socket_close(fd);
		return;
	}
	if( !(user = idents[fd][IDENT_USER]) )
	{
		tell(ob, "\n[FD: " + fd + "] 使用者已離線。\n", CMDMSG);
		socket_close(fd);
		return;
	}
	if( !info || sscanf(info, "%d,%d : %s : %s", rport, port, type, arg) != 4 )
	{
		tell(ob, "\n[FD: " + fd + "] 傳回訊息格式錯誤。\n", CMDMSG);
		socket_close(fd);
		return;
	}
	if( type == "USERID" && (sscanf(arg, "%s :%s", os, uid) == 2) )
	{
		if( uid[<1] == '\n' ) uid = uid[0..<2];
		if( uid[<1] == '\r' ) uid = uid[0..<2];
		tell(ob, "\n[FD: " + fd + "]\n", CMDMSG);
		tell(ob, user->query_idname() + "的主機資訊如下：\n", CMDMSG);
		tell(ob, "作業系統：" + os + "\n", CMDMSG);
		tell(ob, "使用者帳號：" + uid + "\n", CMDMSG);
	}
	else if( type == "ERROR" )
	{
		tell(ob, "\n[FD: " + fd + "]\n", CMDMSG);
		tell(ob, user->query_idname() + " 的主機資訊如下：\n", CMDMSG);
		tell(ob, "錯誤：" + arg + "\n", CMDMSG);
	}
	socket_close(fd);
	return;
}
private void time_out(int fd)
{
	object ob;

	if( !(ob = idents[fd][IDENT_COMMANDER]) )
	{
		socket_close(fd);
		return;
	}
	tell(ob, "\n[FD: " + fd + "] 連線愈時。\n", CMDMSG);
	socket_close(fd);
	return;
}
