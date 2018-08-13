/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : pack_animal.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-09-09
 * Note   : ���~
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <feature.h>
#include <equipment.h>
#include <buff.h>

inherit STANDARD_OBJECT;
inherit EQUIPMENT;

void create()
{
	set_idname("pack animal", HIY"��"NOR YEL"�~"NOR);
	set_temp("status", HIC"�}"NOR CYN"��");

	if( this_object()->set_shadow_ob() ) return;
	
	set("long", "�@�ǥi�H�ΨӸ������~���ʪ��C\n");
	set("unit", "��");
	set("mass", 0);
	set("value", 3000000);
	set("flag/no_amount", 1);
	set(BUFF_STR, 20);
	set("buff/status", HIY"��"NOR YEL"�~"NOR);

	::setup_equipment(EQ_MOUNT, HIG"�y"NOR GRN"�M"NOR);
}