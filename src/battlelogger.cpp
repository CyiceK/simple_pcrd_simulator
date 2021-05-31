#include "battlelogger.h"
#include "unit.h"

using namespace std;

void BattleLogger::init() {
	total_dmg = 0;
}

bool BattleLogger::character_filter(Unit *unit) {
	return false;
	if (unit->unit_id == Eriko().unit_id) {
		return true;
	}
	return false;
}

void BattleLogger::record_damage(Unit *from, Unit *to, int damage, int frame, bool critical, double critical_rate) {

	int avg_this_damage;
	if (critical_rate >= 1.0) critical_rate = 1.0;

	if (to->side == 1) {
		total_dmg += damage;
		avg_this_damage = damage * (1.0 + critical_rate) / (critical ? 2.0 : 1.0);
		avg_dmg += avg_this_damage;

		if (dmglist.find(from->unit_id) == dmglist.end()) {
			dmglist[from->unit_id] = damage;
			avgdmglist[from->unit_id] = avg_this_damage;
		}
		else {
			dmglist[from->unit_id] += damage;
			avgdmglist[from->unit_id] += avg_this_damage;
		}
	}
	else {
		avg_this_damage = damage;
	}
	
	if (!character_filter(from) 
		&& !character_filter(to)
	) return;

	printf("%4d帧 %s对%s造成了%d伤害(蓝字%d) %s \n",
		frame, from->unit_name.c_str(), to->unit_name.c_str(), damage, avg_this_damage, critical ? "(暴击)" : "");
}

void BattleLogger::record_atk_buff(Unit *from, Unit *to, int buffvalue, int frame)
{
	if (!character_filter(from)) return;
	printf("%4d帧 %s对%s增加了%d ATK buff\n",
		frame, from->unit_name.c_str(), to->unit_name.c_str(), buffvalue);
}

void BattleLogger::record_speed_buff(Unit *from, Unit *to, double buffvalue, int frame)
{
	if (!character_filter(from)) return;
	printf("%4d帧 %s对%s增加了%lf速度buff\n",
		frame, from->unit_name.c_str(), to->unit_name.c_str(), buffvalue);
}

void BattleLogger::record_tp_gain(Unit * from, Unit * to, double value, double current_value, int frame)
{
	if (!character_filter(to)) return;
	printf("%4d帧 %s对%s增加了%lf TP（现值：%lf）\n",
		frame, from->unit_name.c_str(), to->unit_name.c_str(), value, current_value);
}

void BattleLogger::record_def_buff(Unit * from, Unit * to, int value, int frame)
{
	if (!character_filter(from)) return;
	printf("%4d帧 %s对%s增加了%d 防御\n",
		frame, from->unit_name.c_str(), to->unit_name.c_str(), value);
}

void BattleLogger::record_ub_cast(Unit * unit, int frame)
{
	if (!character_filter(unit)) return;
	printf("%4d帧 %s施放了UB\n", frame, unit->unit_name.c_str());
}

void BattleLogger::record_action_start(Unit * unit, int frame, int skill)
{
	if (!character_filter(unit)) return;
	if (skill > 0)
		printf("%4d帧 %s开始施放%d技能\n", frame, unit->unit_name.c_str(), skill);
	else if (skill == 0)
		printf("%4d帧 %s开始施放普攻\n", frame, unit->unit_name.c_str());
	else throw runtime_error("error branch.");
}

void BattleLogger::record_action_end(Unit * unit, int frame, int skill)
{
	if (!character_filter(unit)) return;
	if (skill > 0)
		printf("%4d帧 %s结束%d技能\n", frame, unit->unit_name.c_str(), skill);
	else if (skill == 0)
		printf("%4d帧 %s结束普攻\n", frame, unit->unit_name.c_str());
	else throw runtime_error("error branch.");
}

void BattleLogger::record_toxin_damage(Unit * from, Unit *to, int damage, int frame)
{
	if (to->side == 1) {
		total_dmg += damage;
		avg_dmg += damage;
		if (dmglist.find(from->unit_id) == dmglist.end()) {
			dmglist[from->unit_id] = damage;
			avgdmglist[from->unit_id] = damage;
		}
		else {
			dmglist[from->unit_id] += damage;
			avgdmglist[from->unit_id] += damage;
		}
	}

	if (!character_filter(from)
		&& !character_filter(to)
		) return;

	printf("%4d帧 %s对%s造成了%d伤害\n",
		frame, from->unit_name.c_str(), to->unit_name.c_str(), damage);

}

void BattleLogger::record_toxin_success(Unit * from, Unit * to, int damage, double time, int frame)
{
	if (!character_filter(from)) return;
	printf("%4d帧 %s对%s成功施放毒状态(%d伤害, %lf秒)\n", 
		frame, from->unit_name.c_str(), to->unit_name.c_str(), damage, time);
}

void BattleLogger::record_toxin_fail(Unit * from, Unit * to, int frame)
{
	if (!character_filter(from)) return;
	printf("%4d帧 %s对%s施放的毒状态被抵抗\n",
		frame, from->unit_name.c_str(), to->unit_name.c_str());
}
