/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : drop.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-12-07
 * Note   : drop 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <object.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>
#include <map.h>

inherit COMMAND;

string help = @HELP
將物品或金錢丟在地上

drop [數量|all] [物品] [排序]	- 將某數量或排序的某物品丟在地上

HELP;

private void do_command(object me, string arg)
{
	string msg="", amount;
	object ob;
	array loc = query_temp("location", me);
	int is_destruct_env;
	
	if( !arg )
	{
		if( me->is_npc() )
			return me->do_command("say 老闆想要我丟下什麼東西？\n");
		else
			return tell(me, pnoun(2, me)+"想要丟下什麼東西？\n");
	}

	// Delay 中不能下指令
	if( me->is_delaying() )
		return tell(me,	me->query_delay_msg());

	if( MAP_D->query_map_system(loc) && (MAP_D->query_map_system(loc))->query_coor_data(loc, TYPE) == RIVER ) is_destruct_env = 1;
	
	if( sscanf(arg, "$%s %s", arg, amount) == 2 )
	{
		if( me->is_npc() )
			return me->do_command("say 我無法丟棄金錢。\n");

		if( !MONEY_D->money_unit_exist(arg) )
		{
			if( !arg || !arg[0] )
				return tell(me, pnoun(2, me)+"要丟下哪一種貨幣？\n");

			tell(me, "並沒有 "+arg+" 這種貨幣。\n");

			return;
		}
			
		amount = big_number_check(amount);
		if( !amount )
			return tell(me, "請輸入正確的數字。\n");
		
		if( count(amount, "<=", 0) )
			return tell(me, "金額必須要大於零。\n");
		
		if( !me->drop_money(arg, amount) )
			return tell(me, pnoun(2, me)+"身上似乎沒有這麼多 $"+upper_case(arg)+" 錢。\n");
	
		log_file("command/drop", me->query_idname()+"丟下 "+money(arg, amount));
		CHANNEL_D->channel_broadcast("nch", me->query_idname()+"丟下 "+money(arg, amount));

		return msg("$ME丟下了 "HIY+money(arg, amount)+NOR"。\n", me, 0 ,1);
	}
	
	if( sscanf(arg, "%s %s", amount, arg) == 2 )
	{
		if( amount == "all" ) ;
		// 如果 amount 並不是數字單位
		else if( !big_number_check(amount) )
		{
			arg = amount + " " + arg;
			amount = "1";	
		}	
		else if( count(amount, "<", 1) ) amount = "1";
	}
	else amount = "1";
	
	if( objectp(ob = present(arg, me)) )
	{
		if( amount != "all" && count(amount, ">", query_temp("amount", ob)||1) )
		{
			if( me->is_npc() )
				return me->do_command("say 我身上只有 "+(query_temp("amount",ob)||1)+" "+(query("unit",ob)||"個")+ob->query_idname()+"。\n");
			else
				return tell(me, pnoun(2, me)+"身上只有 "+(query_temp("amount",ob)||1)+" "+(query("unit",ob)||"個")+ob->query_idname()+"。\n");
		}
		
		if( ob->is_keeping() )
		{
			if( me->is_npc() )
				return me->do_command("say 必須先解除"+ob->query_idname()+"的保留(keep)狀態。\n");
			else
				return tell(me, pnoun(2, me)+"必須先解除"+ob->query_idname()+"的保留(keep)狀態。\n");
		}
		
		if( query("flag/no_drop", ob) )
			return tell(me, ob->query_idname()+"不允許丟棄。\n");

		if( amount == "all" )
			amount = (query_temp("amount", ob)||"1")+"";
		
		msg = "$ME把"+QUANTITY_D->obj(ob, amount)+"丟在地上。\n";

		if( is_destruct_env )
		{
			msg(msg, me, 0, 1);
			msg(QUANTITY_D->obj(ob, amount, 1)+"被$ME丟棄後消失了。\n", me, ob, 1);
			destruct(ob, amount);
			return;
		}
		
		if( query_temp("endurance", ob) < 0 )
		{
			msg(msg, me, 0, 1);
			msg("損壞的"+QUANTITY_D->obj(ob, amount)+"隨著微風化為塵土...。\n", me, ob, 1);
			destruct(ob, amount);
			return;
		}

		if( !ob->move_to_environment(me, amount) )
			msg = "$ME想把"+QUANTITY_D->obj(ob, amount)+"丟在地上，但地上東西實在太多沒地方放了！\n";
		
		msg(msg, me, 0, 1);

		return;
	}
	
	if( lower_case(arg) == "all" )
	{
		object *all_ob = all_inventory(me);
		
		if( sizeof(all_ob) )
		{
			foreach(ob in all_ob)
			{
				amount = query_temp("amount",ob)||"1";
				
				if( ob->is_keeping() || query("flag/no_drop", ob) ) continue;
			
				msg += HIG"．"NOR+QUANTITY_D->obj(ob, amount, 1)+"\n";

				if( is_destruct_env )
				{
					msg(QUANTITY_D->obj(ob, amount, 1)+"丟棄後消失了。\n", me, ob, 1);
					destruct(ob, amount);
					continue;
				}
				if( !ob->move_to_environment(me, amount) )
				{
					msg("$ME把身上的能丟的東西全丟在地上了。\n"+msg+"\n"NOR, me, 0, 1);
					msg("$ME想把"+QUANTITY_D->obj(ob, amount)+"丟在地上，但地上東西實在太多沒地方放了！\n", me, 0, 1);
					return;
				}
			}
			return msg("$ME把身上的能丟的東西全丟在地上了。\n"+msg+"\n"NOR,me,0,1);
		}
		
		if( me->is_npc() )
			return me->do_command("say 我身上沒有東西可以丟了啦！\n");
		else
			return tell(me, pnoun(2, me)+"身上沒有東西可以丟了啦！\n");
	}
	
	if( me->is_npc() )
		return me->do_command("say 我身上沒有 "+arg+" 這件物品。\n");
	else
		return tell(me, pnoun(2, me)+"身上沒有 "+arg+" 這件物品。\n");
}
