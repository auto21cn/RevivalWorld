/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : lily_seed.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-09-09
 * Note   : 百合種子
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <nature.h>
#include <map.h>

inherit STANDARD_OBJECT;

void create()
{
        set_idname("lily seed","百合種子");
        
        if( this_object()->set_shadow_ob() ) return;
        
        set("long", "百合花花語：純潔•純潔的心•百年好合•偉大的愛。\n");
        set("unit", "粒");
        set("mass", 2);
        set("value", 200);
        set("badsell", 1);

	// 種子資訊
	set("growth",
	([
		//種類
		"type":		FARM,

		//名稱
		"idname":	this_object()->query_idname(),
		
		//插秧播種時的訊息
		"start_msg":"百合種子整齊地佈滿整片田地，一片欣欣向榮的感覺。\n" ,
			
	       	//每單位種子或秧苗需要的原料數量
	       	"material":
	       	([
	       		"/obj/materials/water":15,
	       	]),
			
	       	//收割產品的數量設定與產品路徑
	       	"harvest":
	       	([
	       		"/obj/farming/product/lily":55,
	       	]),

		//成熟收割時的訊息
		"harvest_msg":"$ME歡喜地收割這片成熟的百合花田。\n",

		//種植此類產品所需要的技能與其等級
		"skill":
		([
			"farming":80,
		]),

		//成長時的訊息
		"growing":
		([
			5:	"百合種子萌出一根根嫩芽漸漸生長。\n",
			50:	"百合幼苖慢慢生長出有刺藤蔓，上面的綠葉一一舒展開來。\n",
			150:	"百合田冒出了許多百合花苞，再過不久就會開花了。\n",
			350:	"綻開的白色百合花在綠葉的映襯之下聖潔無比，再等一下子就可以採收了。\n",
			600:	"百合花田上的百合已全部綻放，已經可以採收了。\n",
		]),
	
		// 天氣影響敘述
		"weather":
		([
			SNOWY:	"大雪連連嚴重使得百合陸續地枯萎了。\n",
			SHOWER:	"連日來的雨量使得一些百合泡水腐爛了。\n",
			TYPHOON:"颱風來襲使得部份百合被強風攔腰折斷。\n",
			FOEHN:	"焚風的吹襲讓部份的百合快速的枯黃。\n",
		]),
	]));
}
