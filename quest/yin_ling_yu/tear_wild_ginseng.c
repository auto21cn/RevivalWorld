/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : tear wild ginseng.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-12-13
 * Note   : 野山人蔘
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>

inherit STANDARD_OBJECT;

void create()
{
	set_idname("wild ginseng", HIY"野山"NOR YEL"人參"NOR);
	set_temp("status", HIC"淚"NOR CYN"水"NOR);

	if( this_object()->set_shadow_ob() ) return;
	

	set("long","似乎是一種頗稀有的藥材。\n這把不尋常的野山人參隱約泛著"HIB"藍"NOR BLU"光"NOR"，它的背後似乎還有一段故事，\n看來得找個熟識醫藥的人問問。");
	set("unit", "把");
	set("mass", 300);
	set("value", 1000);
}
