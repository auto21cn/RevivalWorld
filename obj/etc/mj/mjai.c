/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : mjai_clode.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-12-31
 * Note   : 臺灣十六張麻將桌 AI 物件
 * Update :
 *  o 2000-00-00  

 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include "mj.h"

inherit STANDARD_NPC;

#define MJOB	(query_temp("mjobject"))

varargs void create()
{
	set_idname("AI MJer",HIC"人工智慧"NOR CYN"麻將俠"NOR);
	set("unit", "位");
	startup_living();
}

// 每打一張牌後呼叫 AI_pg() 設定自動碰槓資料
void AI_pg()
{
	string *group;
	mapping Pdata = copy(fetch_variable("Pdata", MJOB));
	mapping data = Pdata[this_object()];

	// 若已經聽牌則取消碰槓檢查
	if( sizeof(data[LISTEN]) ) return;

	foreach( group in MJOB->tile_logic_check(data[TILE]) )
	{
		if( sizeof(group) == 2 && group[0] == group[1] && member_array(group[0], data[AUTOPON]) == -1 )
			data[AUTOPON] += ({ group[0] });

		if( sizeof(group) == 3 && group[0] == group[1] && group[1] == group[2] && member_array(group[0], data[AUTOGON]) == -1 )
			data[AUTOGON] += ({ group[0] });
	}
}

// 上家打牌後呼叫 AI_eat 判斷是否吃牌
// 回傳要吃的兩張牌 ex:({ "2w", "3w" })
string *AI_eat()
{
	int i;
	string *group;
	array choice = allocate(2, allocate(0));
	mapping MJdata = copy(fetch_variable("MJdata", MJOB));
	mapping Pdata = copy(fetch_variable("Pdata", MJOB));
	mapping data = Pdata[this_object()];

	if( sizeof(data[LISTEN]) || !MJdata[DUMPING] ) return 0;

	foreach( group in MJOB->tile_logic_check(data[TILE]) )
	switch(sizeof(group))
	{
	case 1: choice[0] += group;
		break;
	case 2: if( group[0] != group[1] )
			choice[1] += ({ group });
		break;
	}

	// check 雙支
	foreach( group in choice[1] )
	{
		// 若有雙隻又在等對的時候
		if( group[0][0] == group[1][0] - 1
		    && MJdata[DUMPING][1] == group[0][1]
		    && (MJdata[DUMPING][0] == group[0][0] - 1 || MJdata[DUMPING][0] == group[1][0] + 1 ))
			return group;
	}

	// check 單支是否有等中洞
	for(i=0;i<sizeof(choice[0])-1;i++)
	{
		if( choice[0][i][1] == choice[0][i+1][1]
		    && choice[0][i][0] == choice[0][i+1][0] - 2
		    && MJdata[DUMPING][1] == choice[0][i][1]
		    && MJdata[DUMPING][0] == choice[0][i][0] + 1)
			return ({ choice[0][i], choice[0][i+1] });
	}

	return 0;
}

// 輪到 AI 打牌時呼叫 AI_da()
string AI_da()
{
	string *group;
	array choice = allocate(2, allocate(0));
	string *result = allocate(0);
	mapping MJdata = copy(fetch_variable("MJdata", MJOB));
	mapping Pdata = copy(fetch_variable("Pdata", MJOB));
	mapping data = Pdata[this_object()];

	foreach( group in MJOB->tile_logic_check(data[TILE]) )
	switch(sizeof(group))
	{
		case 1: choice[0] += group;
			break;
		case 2: if( group[0] != group[1] )
				choice[1] += ({ group });
			break;
	}

	// 依照字牌 -> 邊張 -> 其他, 將單隻排序
	choice[0] = sort_array(choice[0],
	    (: member_array($1[0]-48, ({ ({1,9,2,8,3,7,4,6,5}), ({9,1,8,2,7,3,6,4,5}) })[random(2)])
		> member_array($2[0]-48, ({ ({1,9,2,8,3,7,4,6,5}), ({9,1,8,2,7,3,6,4,5}) })[random(2)]) ? 1 : -1 :));

	// 有人聽牌時開始小心打牌
	if( MJdata[LISTENING] || sizeof(MJdata[DUMPED]) > 35 )
	{
		if( sizeof(choice[0]) )
		{
			// 只選被打過的牌
			result = choice[0] & (string *)MJdata[DUMPED];
			
			// 有打過的牌則排順序
			if( sizeof(result) )
			{
				// 排序剛被打過的牌, 最近的牌排最前頭
				result = sort_array(result, (: member_array($1, $(MJdata)[DUMPED]) < member_array($2, $(MJdata)[DUMPED]) ? 1 : -1 :));
			}
			// 若都是沒被打過的牌則打字牌
			else
			{
				result = choice[0] & ({ "ea", "so", "we", "no", "jo", "fa", "ba" });
			}
		}

		if( !sizeof(result) && sizeof(choice[1]) )
		{
			// 選被打過的打
			foreach( group in choice[1] )
			{
				if( member_array(group[0], MJdata[DUMPED]) != -1 )
					result += ({ group[0] });
				if( member_array(group[1], MJdata[DUMPED]) != -1 )
					result += ({ group[1] });
			}

			if( sizeof(result) )
			{
				result = sort_array(result, (: member_array($1, $(MJdata)[DUMPED]) < member_array($2, $(MJdata)[DUMPED]) ? 1 : -1 :));
				
			}
		}
	}
	else
	{
		// 若有單支則打單支
		if( sizeof(choice[0]) )
		{
			// 判斷是否在等中洞, 有的話則不打那張牌
			foreach( string t in choice[0] )
			{
				if( member_array(t[1], ({ 'w', 's', 't' })) == -1 ||
				(member_array(sprintf("%c%c", t[0]+2, t[1]), choice[0]) == -1 && member_array(sprintf("%c%c", t[0]-2, t[1]), choice[0]) == -1) )
					result += ({ t });
			}
		}
		
		// 盡量選單支打
		if( !sizeof(result) )
			result = choice[0];

		// 若無單支, 則選雙支打
		if( !sizeof(result) && sizeof(choice[1]) )
		{
			// 選被打過的打
			foreach( group in choice[1] )
			{
				if( member_array(group[0], MJdata[DUMPED]) != -1 )
					result += ({ group[0] });
				if( member_array(group[1], MJdata[DUMPED]) != -1 )
					result += ({ group[1] });
			}
		}
	}

	// 再沒有就亂拆牌專打安全牌
	if( !sizeof(result) )
		result = sort_array(data[TILE], (: member_array($1, $(MJdata)[DUMPED]) < member_array($2, $(MJdata)[DUMPED]) ? 1 : -1 :));

	return result[0];
}

// 每次打完牌後呼叫 AI_tin() 判斷是否聽牌或 au hu
array AI_tin()
{
	int i, j, listencount;
	object ppl;
	string tile, seatile;
	mapping listening = allocate_mapping(0);
	mapping MJdata = copy(fetch_variable("MJdata", MJOB));
	mapping Pdata = copy(fetch_variable("Pdata", MJOB));
	mapping data = Pdata[this_object()];
	string *mytiles = data[TILE];
		
	// 自動分析有哪些牌可以聽
	foreach( tile in Tsort )
	{
		if( MJOB->valid_hu_check(MJOB->tile_sort(mytiles+({ tile })), 0, 0) )
		{
			// 檢查自己拿了幾張
			i = member_array(tile, mytiles);
			if(i!=-1)
			{
				j++;
				while( i++<sizeof(mytiles)-1 && mytiles[i-1] == mytiles[i] )
					j++;
			}

			// 檢查海底有幾張
			foreach( seatile in MJdata[DUMPED] )
			if( seatile == tile ) j++;

			// 檢查所有人門前有幾張
			foreach( ppl in fetch_variable("Psort", MJOB) )
			{
				i = member_array( tile, fetch_variable("Pdata", MJOB)[ppl][OTILE] );
				if(i!=-1)
				{
					j++;
					while( i++<sizeof(Pdata[ppl][OTILE])-1 && Pdata[ppl][OTILE][i-1] == Pdata[ppl][OTILE][i] )
						j++;
				}
			}

			listening[tile] = 4-j;
		}
	}


	foreach( i in values(listening) )
	listencount += i;

	// 都不聽
	if( !sizeof(listening) || !listencount )
		return 0;

	// 若只聽一牌, 則不聽, 但設定自動胡
	if( sizeof(listening) == 1 )
		return keys(listening); // 作 au hu 動作
	else if( listencount < 3 )
		return keys(listening);
	else
		return ({1});// 聽牌
}

void emotion_begin()
{
	set_this_player(this_object());
	switch(random(10))
	{
		case 0:this_object()->do_command("say 讓我想一下該哪張牌先打.....");break;
                case 1:this_object()->do_command("pace 我是被機八人寫出來的");break;
		case 2:this_object()->do_command("pace 147 258 369");break;
		case 3:this_object()->do_command("pace 13579 2468");break;
	}
}
void emotion_eat()
{
	set_this_player(this_object());
	switch(random(10))
	{
		case 1:this_object()->do_command("mars");	break;
		case 2:this_object()->do_command("wire 吃");	break;
		case 3:this_object()->do_command("fascinate");	break;
	}
}

void emotion_pon()
{
	set_this_player(this_object());
	switch(random(10))
	{
		case 1: this_object()->do_command("pace 碰"); break;
		case 2: this_object()->do_command("sing"); break;
		case 3: this_object()->do_command("excellent 碰"); break;
		case 4: this_object()->do_command("fascinate 碰"); break;
	}
}

void emotion_mo()
{
	mapping MJdata = copy(fetch_variable("MJdata", MJOB));

	set_this_player(this_object());
	switch(random(20))
	{
		case 1:this_object()->do_command("wahaha");break;
                case 2:this_object()->do_command("pace 我是被ㄐㄅ人寫出來ㄉ");break;
		case 3..4:
			if( MJdata[LISTENING] )
			{
				this_object()->do_command("imho "+MJOB->full_chinese(Tsort[random(sizeof(Tsort))])+"放槍的機率應該有 "+sprintf("%.2f%%", random(1000)/10.)); break;
			}
			break;
		case 5..8:
			if( MJdata[LISTENING] )
			{
				this_object()->do_command("imho 應該有人在聽 "+MJOB->full_chinese(Tsort[random(sizeof(Tsort))])); break;
			}
			break;
		case 9:
			if( MJdata[LISTENING] )
				this_object()->do_command("imho 應該有人在聽 "HIY"MP"NOR YEL"3"NOR); break;
	}
}
