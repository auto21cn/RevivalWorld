/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : true_fly_horse.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-09-09
 * Note   : �u�D����
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
	set_idname("true fly horse", HIW"�u��"NOR WHT"��"NOR);
	set_temp("status", HIG"��"NOR GRN"�_");

	if( this_object()->set_shadow_ob() ) return;
	
	set("long", "�@�ǥi�H�ΨӸ������~���ʪ��C\n");
	set("unit", "��");
	set("mass", 0);
	set("value", 6000000);
	set("flag/no_amount", 1);
	set(BUFF_STR, 36);
	set(BUFF_STAMINA_REGEN, 1);
	set("buff/status", HIW"�u��"NOR WHT"��"NOR);
	
	::setup_equipment(EQ_MOUNT, HIG"�y"NOR GRN"�M"NOR);
}