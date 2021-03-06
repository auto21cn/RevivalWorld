/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : salary_d.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-02-24
 * Note   : 
 * Update :
 *  o 2005-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <map.h>
#include <location.h>
#include <daemon.h>
#include <message.h>
#include <money.h>
#include <skill.h>

#define UNIT	0
#define MONEY	1

#define LEVEL_SALARY	"50"
#define BASE_SALARY	"20000"

int in_paying_salary = 0;
int current_call_out = 0;
string *ids;

string query_labor_salary(object labor)
{
	int level;

	mapping skills = query("skills", labor);
	
	if( !mapp(skills) )
		return BASE_SALARY;

	foreach( string sk, mapping data in skills )
	{
		if( !SKILL_D->skill_exists(sk) )
			continue;

                level += data["level"]/(SKILL(sk))->exp_multiple();
	}
	
	return count(BASE_SALARY, "+", count(LEVEL_SALARY, "*", to_int(pow(level, 1.1))));
}

// 一個月付一次薪水
private int pay_salary(object boss)
{
	int leadership_exp;
	object labor;
	string bossid, *labor_files, labor_file, msg, moneyunit;
	string salary, salary_paid;
	string total_salary_pay;

	labor_files = query("hirelabors", boss);
	
	if( !labor_files || !sizeof(labor_files) ) return 0;

	bossid = boss->query_id(1);

	moneyunit = MONEY_D->city_to_money_unit(query("city", boss)) || MONEY_D->query_default_money_unit();

	foreach( labor_file in labor_files )
	{
		reset_eval_cost();

		if( !objectp(labor = load_object(labor_file)) ) continue;
	
		salary = query_labor_salary(labor);
		salary_paid = query("salary_paid", boss);

		// 若有薪資預付
		if( count(salary_paid, ">=", salary) )
			set("salary_paid", count(salary_paid, "-", salary), boss);

		// 沒薪資預付且付不起現金
		else if( !(boss->spend_money(moneyunit, salary, BANK_PAYMENT)) )
		{
			// 親密度超過 300 不會離開
			if( query("relationship/"+bossid, labor) > 300 || random(40) )
			{			
				if( userp(boss) )
					tell(boss, labor->query_idname()+"稍來一封信說道：雖然"+pnoun(2, boss)+"付不出我的 "+money(moneyunit, salary)+" 薪水，但我仍然願意留在"+pnoun(2, boss)+"的身邊做事。\n");
			
				// 減少關係
				addn("relationship/" + bossid, -100, labor);
				leadership_exp -= 300+random(300);

			}
			else
			{
				if( userp(boss) )
					tell(boss, pnoun(2, boss)+"無法支付"+labor->query_idname()+"本月份 "+money(moneyunit, salary)+" 的薪水，"+labor->query_idname()+"毅然絕然地離開了。\n");

				leadership_exp -= 3000+random(300);

				// 登錄至 LABOR_D, 讓其他人得以錄用
				LABOR_D->add_labor(labor, 1);

				log_file("command/fire", sprintf("%s 付不起薪水使得 %O 辭職\n", bossid, labor ));
				
				destruct(labor);
				
				continue;
			}
		}

		total_salary_pay = count(total_salary_pay, "+", salary);

		// 增加員工對老闆親密度
		if( addn("relationship/"+bossid, 30, labor) >= 10000 )
			set("relationship/"+bossid, 9999, labor);
	
		// 增加老闆的領導技能
		leadership_exp += 600+random(600);
		labor->save();

	}
	
	msg = HIW"\n又到了月底發放薪水的時候了，本月份總共發出薪資(包括預付金額)：\n"HIY;

	msg += "．"+money(moneyunit, total_salary_pay)+"\n";
	
	tell(boss, msg+NOR"\n");

	boss->add_social_exp(leadership_exp);	

	return 1;
}

private void distributed_pay_salary()
{
	object user;
	
	if( !sizeof(ids) )
	{
		CHANNEL_D->channel_broadcast("news", "勞工薪資處理發放完畢。");
		in_paying_salary = 0;
		return;
	}

	user = load_user(ids[0]);
	
	ids = ids[1..];

	if( objectp(user) )
	{
		catch
		{
			if( pay_salary(user) )
				user->save();
						
			if( !userp(user) )
				destruct(user);
		};
	}

	current_call_out = call_out((: distributed_pay_salary :), 1);
}

void game_month_process()
{
	ids = all_player_ids();

	if( in_paying_salary ) return;

	in_paying_salary = 1;

	CHANNEL_D->channel_broadcast("news", "發放勞工薪資的時候到了，正在計算處理所有薪資資料(共 "+sizeof(ids)+" 筆資料)。");

	current_call_out = call_out((: distributed_pay_salary :), 1);
}

int remove()
{
	remove_call_out(current_call_out);
}
	
void create()
{


}
string query_name()
{
	return "薪水系統(SALARY_D)";
}
