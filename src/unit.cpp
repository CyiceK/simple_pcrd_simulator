#include "unit.h"
#include "GameController.h"

using namespace std;
Unit::Unit() {}

void Unit::toxin_damage() {
	bool ret = toxin.to_next_frame();
	if (ret) {
		receive_damage(toxin.value);
		gc->logger.record_toxin_damage(toxin.from, this, toxin.value, gc->frame);
	}
}

int Unit::get_atk() {
	int total_atk = atk + exATK;
	for (auto buff : atk_buffs) {
		if (buff.active)
			total_atk += buff.value;
	}
	if (total_atk <= 0) return 0;
	return total_atk;
}

int Unit::get_def() {
	int total_def = def + exDEF;
	for (auto buff : def_buffs) {
		if (buff.active)
			total_def += buff.value;
	}
	if (total_def <= 0) return 0;
	return total_def;
}

int Unit::get_critical() {
	int total_crt = critical;
	for (auto buff : critical_buffs) {
		if (buff.active)
			total_crt += buff.value;
	}
	if (total_crt <= 0) return 0;
	return total_crt;
}

double Unit::get_critical_rate(Unit * to) {
	return get_critical()*1.0 / 2000.0 * level / to->level;
}

int Unit::get_tp_up() {
	int total_TP = tpup;
	for (auto buff : tp_buffs) {
		if (buff.active)
			total_TP += buff.value;
	}
	if (total_TP <= 0) return 0;
	return tpup;
}

double Unit::get_def_coef() {
	double x = 1.0 / (1.0 * get_def() / 100.0 + 1.0);
	return x;
}

double Unit::get_tp_up_coef() {
	double x = get_tp_up() * 1.0 / 100 + 1.0;
	return x;
}

void Unit::tp_obtain(double newTP) {
	TP += newTP;
	if (TP > 1000) TP = 1000;
	if (TP < 0) TP = 0;
}

double Unit::get_lifesteal_coef(int enemy_level) {
	double lifesteal_coef = 1.0 * lifesteal / (100 + enemy_level + lifesteal);
	return lifesteal_coef;
}

void Unit::hp_obtain(int newHp) {
	// 如果HP低于0，则死亡
	HP += newHp;
	if (HP > maxHP) { HP = maxHP; }
	if (HP <= 0) { dead = true; }
}

void Unit::deal_damage(int damageDeal, int enemy_level) {
	int hpdrain = int(damageDeal * get_lifesteal_coef(enemy_level));
	hp_obtain(hpdrain);
}

void Unit::receive_damage(int damage) {
	// 计算了HP下降和TP上升
	hp_obtain(damage);
	tp_obtain(500.0 * damage / maxHP * get_tp_up_coef());
}

void Unit::new_movement_tp_obtain() {
	tp_obtain(90.0*get_tp_up_coef());
}

void Unit::loop_next() {
	if (onloop) {
		loop_pos++;
		loop_pos %= onloop_actions.size();
		return;
	}
	if (!onloop) {
		if (loop_pos == begin_actions.size() - 1) {
			onloop = true;
			loop_pos = 0;
			return;
		}
		else {
			loop_pos++;
			return;
		}
	}
}

int Unit::get_current_skill() {
	if (!onloop) {
		return begin_actions[loop_pos];
	}
	else {
		return onloop_actions[loop_pos];
	}
}

int Unit::get_next_skill() {
	if (!onloop) {
		if (loop_pos == begin_actions.size() - 1) {
			return onloop_actions[0];
		}
		else
			return begin_actions[(loop_pos + 1) % onloop_actions.size()];
	}
	else {
		return onloop_actions[(loop_pos + 1) % onloop_actions.size()];
	}
}

int Unit::get_next_wait_frame() {
	int next_skill = get_next_skill();
	int raw_wait_frame = action_wait_frame[next_skill];
	if (speed_buff.active) {
		return (int)ceil(raw_wait_frame / speed_buff.value_float);
	}
	else {
		return raw_wait_frame;
	}
}

// 对于每个角色 
// 0 是普攻
// 1 是UB动作1，   2是UB动作2
// 11 是1技能动作1 
// 21 是2技能动作1， 22是2技能动作2
// 依此类推
std::vector<int> Unit::get_action_id(int skillid, int frame) {	
	if (skillid == 0) {
		if (frame == 2) return { 0 };
	}
	if (skillid == 1) {
		if (frame == 2) return { 11 };
	}
	if (skillid == 2) {
		if (frame == 2) return { 21, 22 };
	}
	return {};
}

std::vector<int> Unit::get_ub_detail() {
	return { 1, 2 };
}

ActionDetail Unit::get_action_detail(int action_id) {
	ActionDetail ad;
	ad.type = Damage;
	ad.n_target = 1;
	ad.parameters = { 30.0, 30.0, 5.0 };
	return ad;
}

std::vector<int> Unit::to_next_frame() {
	if (start_frame >= 0) {
		start_frame--;
		return {};
	}
	if (ub_freeze) {
		ub_freeze_frame_counter++;
		if (ub_freeze_frame_counter >= ub_freeze_frame) {
			ub_freeze = false;
		}
		return {};
	}
	if (status == 0) {
		wait_frame_counter++;
		if (wait_frame_counter >= wait_frame) {
			// 字牌出来了
			gc->logger.record_action_start(this, gc->frame, get_current_skill());
			status = 1;
			wait_frame = get_next_wait_frame();
			action_frame_counter = 0;
			int currenct_skill = get_current_skill();
			action_frame = action_full_frame[currenct_skill];
			new_movement_tp_obtain();
		}
		return {};
	}
	if (status == 1) {
		action_frame_counter++;
		auto actions = get_action_id(get_current_skill(), action_frame_counter);
		if (action_frame_counter >= action_frame) {
			gc->logger.record_action_end(this, gc->frame, get_current_skill());
			status = 0;
			wait_frame_counter = 0;
			loop_next();
		}
		return actions;
	}
	throw std::runtime_error("Impossible branch.");
}

void Unit::after_cast_ub() {
	TP = 0;
	ub_freeze = true;
	ub_freeze_frame_counter = 0;
	if (status == 1) {
		status = 0;
		wait_frame_counter = 0;
		loop_next();
	}
}

Makoto::Makoto() {
	side = 0;

	create_unit_from_json(*this, 104301);
}

std::vector<int> Makoto::get_action_id(int skillid, int frame) {
	// 对于每个角色 
	//  0 是普攻
	// 1 是UB动作1，   2是UB动作2
	// 11 是1技能动作1 
	// 21 是2技能动作1， 22是2技能动作2
	// 依此类推
	
	if (skillid == 0) {
		if (frame == 29) return { 0 };
	}
	if (skillid == 1) {
		if (frame == 1) return { 11 };
		if (frame == 33) return { 12 };
	}
	if (skillid == 2) {
		if (frame == 49) return { 21 };
	}
	return {};
}

std::vector<int> Makoto::get_ub_detail() {
	return { 1, 2 };
}

ActionDetail Makoto::get_action_detail(int action_id) {
	ActionDetail ad;
	switch (action_id) {
		// Common Attack
	case 0:
		CommonAttack(ad);
		break;
		// UB
	case 1:
		DamageSkill(ad, 45, 45, 3.6, level);
		break;
	case 2:
		ReduceDef(ad, 0.9, 0.9, 18.0, level);
		break;
		// Skill 1
	case 11:
		SelfATKBuff(ad, 7.5, 7.5, 12, level);
		break;
	case 12:
		DamageSkill(ad, 25, 25, 2, level);
		break;
		// Skill 2
	case 21:
		ReduceDef(ad, 0.8, 0.8, 12.0, level);
		break;
	default:
		throw std::runtime_error("error branch");
	}
	return ad;
}

Eriko::Eriko() {
	side = 0;

	create_unit_from_json(*this, 102701);
}

std::vector<int> Eriko::get_action_id(int skillid, int frame) {
	// 对于每个角色 
	//  0 是普攻
	// 1 是UB动作1，   2是UB动作2
	// 11 是1技能动作1 
	// 21 是2技能动作1， 22是2技能动作2
	// 依此类推
	if (skillid == 0) {
		if (frame == 27) return { 0 };
	}
	if (skillid == 1) {
		if (frame == 36) return { 11,12,13 };
	}
	if (skillid == 2) {
		if (frame == 42) return { 21 };
		if (frame == 43) return { 22 }; //毒斧子的毒
	}
	return {};
}

std::vector<int> Eriko::get_ub_detail() {
	return { 1 };
}

ActionDetail Eriko::get_action_detail(int action_id) {
	ActionDetail ad;
	switch (action_id) {
		// Common Attack
	case 0:
		CommonAttack(ad);
		break;
		// UB
	case 1:
		DamageSkillMultiPart(ad, 60, 60, 4.5, level, 3, 0, { 0.270269, 0.297297, 0.432434 });
		break;
		// Skill 1
	case 11:
		SelfATKBuff(ad, 20, 20, 12, level);
		break;
	case 12:
		SelfATKBuff(ad, 90, 90, 7.5, level);
		break;
	case 13:
		SelfCriticalBuff(ad, 400, 0, 7.5, level);
		break;
		// Skill 2
	case 21:
		DamageSkill(ad, 15, 15, 1.2, level);
		break;
	case 22:
		AddToxin(ad, 2, 2, 8, level);
		break;
	default:
		throw std::runtime_error("error branch");
	}
	return ad;
}

Jita::Jita() {
	side = 0;

	create_unit_from_json(*this, 105701);
}

std::vector<int> Jita::get_action_id(int skillid, int frame) {
	// 对于每个角色 
	//  0 是普攻
	// 1 是UB动作1，   2是UB动作2
	// 11 是1技能动作1 
	// 21 是2技能动作1， 22是2技能动作2
	// 依此类推
	if (skillid == 0) {
		if (frame == 21) return { 0 };
	}
	if (skillid == 1) {
		if (frame == 30) return { 11,12 };
		if (frame == 61) return { 13 };
	}
	if (skillid == 2) {
		if (frame == 67) return { 21 };
	}
	return {};
}

std::vector<int> Jita::get_ub_detail() {
	return { 1 };
}

ActionDetail Jita::get_action_detail(int action_id) {
	ActionDetail ad;
	switch (action_id) {
		// Common Attack
	case 0:
		CommonAttack(ad);
		break;
		// UB
	case 1:
		DamageSkillMultiPart(ad, 60, 60, 4.8, level, 4, 0, { 0.2, 0.2, 0.2, 0.4 });
		break;
		// Skill 1
	case 11:
		SelfTpRestore(ad, 100, 0, level);
		break;
	case 12:
		FriendTpRestore(ad, 100, 0, level);
		break;
	case 13:
		DamageSkill(ad, 28, 28, 2.25, level);
		break;
		// Skill 2
	case 21:
		SelfTpRestore(ad, 150, 1.5, level);
		break;
	default:
		throw std::runtime_error("error branch");
	}
	return ad;
}

Kanna::Kanna() {
	side = 0;

	create_unit_from_json(*this, 170101);
}

std::vector<int> Kanna::get_action_id(int skillid, int frame) {
	// 对于每个角色 
	//  0 是普攻
	// 1 是UB动作1，   2是UB动作2
	// 11 是1技能动作1 
	// 21 是2技能动作1， 22是2技能动作2
	// 依此类推
	if (skillid == 0) {
		if (frame == 21) return { 0 };
	}
	if (skillid == 1) {
		if (frame == 55) return { 11 };
	}
	if (skillid == 2) {
		if (frame == 1) return { 21 };
		if (frame == 56) return { 22 };
	}
	return {};
}

std::vector<int> Kanna::get_ub_detail() {
	return { 1 };
}

ActionDetail Kanna::get_action_detail(int action_id) {
	ActionDetail ad;
	switch (action_id) {
		// Common Attack
	case 0:
		CommonAttack(ad);
		break;
		// UB
	case 1:
		DamageSkillMultiPart(ad, 30, 30, 2.4, level, 4, CRITICAL_4TH, { 0.2, 0.2, 0.2, 0.4 });
		break;
		// Skill 1
	case 11:
		DamageSkillMultiPart(ad, 12.5, 12.5, 1.0, level, 1, CRITICAL_1ST, { 1.0 });
		break;
		// Skill 2
	case 21:
		FriendSpeedBuff(ad, 1.2, 12);
		break;
	case 22:
		FriendATKBuff(ad, 7, 7, 12, level);
		break;
	default:
		throw std::runtime_error("error branch");
	}
	return ad;
}

Shiori::Shiori() {
	side = 0;

	create_unit_from_json(*this, 103801);
}

std::vector<int> Shiori::get_action_id(int skillid, int frame) {
	// 对于每个角色 
	//  0 是普攻
	// 1 是UB动作1，   2是UB动作2
	// 11 是1技能动作1 
	// 21 是2技能动作1， 22是2技能动作2
	// 依此类推
	if (skillid == 0) {
		if (frame == 91) return { 0 };
	}
	if (skillid == 1) {
		if (frame == 79) return { 11 };
		if (frame == 121) return { 12 };
		if (frame == 127) return { 13 };
		if (frame == 133) return { 14 };
	}
	if (skillid == 2) {
		if (frame == 12) return { 21 };
		if (frame == 85) return { 22 };
	}
	return {};
}

std::vector<int> Shiori::get_ub_detail() {
	return { 1, 2 };
}

ActionDetail Shiori::get_action_detail(int action_id) {
	ActionDetail ad;
	switch (action_id) {
		// Common Attack
	case 0:
		CommonAttack(ad);
		break;
		// UB
	case 1:
		DamageSkill(ad, 30, 30, 2.4, level);
		break;
	case 2:
		SelfATKBuff(ad, 22.5, 22.5, 180, level);
		break;
		// Skill 1
	case 11:
		SelfTpRestore(ad, 60, 1.25, level);
		break;
	case 12:
		DamageSkill(ad, 4.5, 4.5, 0.54, level);
		break;
	case 13:
		DamageSkill(ad, 4.5, 4.5, 0.72, level);
		break;
	case 14:
		DamageSkill(ad, 4.5, 4.5, 0.90, level);
		break;
		// Skill 2
	case 21:
		SelfTpRestore(ad, 40, 1.0, level);
		break;
	case 22:
		DamageSkill(ad, 15, 15, 1.2, level);
		break;
	default:
		throw std::runtime_error("error branch");
	}
	return ad;
}

Enemy::Enemy() {
	side = 1;
	auto_ub = true;
	create_unit_from_json(*this, 999999);

	resistance.toxin_resist = 0.3;

	ub_level = 35;
}

// 对于每个角色 
// 0 是普攻
// 1 是UB动作1，   2是UB动作2
// 11 是1技能动作1 
// 21 是2技能动作1， 22是2技能动作2
// 依此类推

std::vector<int> Enemy::get_action_id(int skillid, int frame) {
	if (skillid == 0) {
		if (frame == 33) return { 0 };
	}
	return {};
}
std::vector<int> Enemy::get_ub_detail() {
	return { 1 };
}
ActionDetail Enemy::get_action_detail(int action_id) {
	ActionDetail ad;
	switch (action_id) {
		// Common Attack
	case 0:
		CommonAttack(ad);
		break;
		// UB
	case 1:
		DamageAoeSkill(ad, 13.5, 13.5, 2.6, ub_level, 3);
		break;
	default:
		throw std::runtime_error("error branch");
	}
	return ad;
}
#define GetUnitAttribute(Term) unit.Term = unit_data[#Term]
#define GetUnitListAttribute(Term) {vector<int> lst;json obj=unit_data[#Term];for(auto it=obj.begin();it!=obj.end();++it){lst.push_back(*it);}unit.Term=lst;}

std::string WstringToString(const std::wstring str) {// wstring转string
	size_t len = str.size() * 4;
	setlocale(LC_CTYPE, "");
	char* p = new char[len];
	// size_t tmp[1];
	wcstombs_s(nullptr, p, len, str.c_str(), len);
	std::string str1(p);
	delete[] p;
	return str1;
}

void create_unit_from_json(Unit& unit, int unit_id, string filename) {
	ifstream i(filename);
	json data;
	if (!i) {
		throw runtime_error("File not existed.");
	}
	i >> data;
	wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	char buf[100]; 
	_itoa_s(unit_id, buf, 10);
	string str_id = string(buf);

	json unit_data = data[str_id];

	wstring uname = conv.from_bytes(unit_data["unit_name"]);
	unit.unit_id = unit_id;
	if (unit_id < 200000) { unit.side = 0; }
	else { unit.side = 1; }
	unit.unit_name = WstringToString(uname);
	GetUnitAttribute(level);
	GetUnitAttribute(maxHP);
	unit.HP = unit.maxHP;
	GetUnitAttribute(atk);
	GetUnitAttribute(exATK);
	GetUnitAttribute(def);
	GetUnitAttribute(critical);
	GetUnitAttribute(miss);
	GetUnitAttribute(lifesteal);
	GetUnitAttribute(tpup);
	GetUnitAttribute(tpdown);	
	
	GetUnitListAttribute(begin_actions);
	GetUnitListAttribute(onloop_actions);
	GetUnitAttribute(start_frame);
	GetUnitListAttribute(action_full_frame);
	GetUnitListAttribute(action_wait_frame);
	GetUnitAttribute(ub_freeze_frame);	
}

void CommonAttack(ActionDetail & ad) {
	ad.n_target = 1;
	ad.type = Damage;
	ad.parameters = { 0,0,1. };
	ad.parameters_int = { 0, 0 };
}

void DamageSkill(ActionDetail & ad, double constant, double skill_level_rate, double atk_rate, int skill_level) {
	ad.n_target = 1;
	ad.type = Damage;
	ad.parameters = { constant, skill_level_rate, atk_rate };
	ad.parameters_int = { skill_level, 0 };
}

void DamageSkillMultiPart(ActionDetail & ad, double constant, double skill_level_rate, double atk_rate, int skill_level, int n_part, int always_critical, std::vector<double> damage_portion_each_part) {

	ad.n_target = 1;
	ad.type = Damage;
	ad.parameters = { constant, skill_level_rate, atk_rate };
	for (int i = 0; i < n_part; ++i) {
		ad.parameters.push_back(damage_portion_each_part[i]);
	}
	ad.parameters_int = { skill_level, n_part, always_critical };
}

void DamageAoeSkill(ActionDetail & ad, double constant, double skill_level_rate, double atk_rate, int skill_level, int n_target) {
	ad.n_target = n_target;
	ad.type = Damage;
	ad.parameters = { constant, skill_level_rate, atk_rate };
	ad.parameters_int = { skill_level, 0 };
}

void DamageAoeSkillMultiPart(ActionDetail & ad, double constant, double skill_level_rate, double atk_rate, int skill_level, int n_target, int n_part, int always_critical, std::vector<double> damage_portion_each_part) {

	ad.n_target = n_target;
	ad.type = Damage;
	ad.parameters = { constant, skill_level_rate, atk_rate };
	for (int i = 0; i < n_part; ++i) {
		ad.parameters.push_back(damage_portion_each_part[i]);
	}
	ad.parameters_int = { skill_level, n_part, always_critical };
}

void SelfATKBuff(ActionDetail & ad, double constant, double skill_level_rate, double time, int skill_level) {
	ad.n_target = 0;
	ad.type = Self_ATK_Buff;
	ad.parameters = { constant, skill_level_rate, time };
	ad.parameters_int = { skill_level };
}

void SelfCriticalBuff(ActionDetail & ad, double constant, double skill_level_rate, double time, int skill_level) {
	ad.n_target = 0;
	ad.type = Self_Critical_Buff;
	ad.parameters = { constant, skill_level_rate, time };
	ad.parameters_int = { skill_level };
}

void FriendATKBuff(ActionDetail & ad, double constant, double skill_level_rate, double time, int skill_level) {
	ad.n_target = 0;
	ad.type = ATK_Buff;
	ad.parameters = { constant, skill_level_rate, time };
	ad.parameters_int = { skill_level };
}

void FriendSpeedBuff(ActionDetail & ad, double value, double time) {
	ad.n_target = 0;
	ad.type = Speed_Buff;
	ad.parameters = { value, time };
}

void ReduceDef(ActionDetail & ad, double constant, double skill_level_rate, double time, int skill_level) {
	ad.n_target = 1;
	ad.type = Def_Debuff;
	ad.parameters = { constant, skill_level_rate, time };
	ad.parameters_int = { skill_level };
}

void FriendTpRestore(ActionDetail & ad, double constant, double skill_level_rate, int skill_level) {
	ad.n_target = 0;
	ad.type = TP_Restore;
	ad.parameters = { constant, skill_level_rate };
	ad.parameters_int = { skill_level };
}

void SelfTpRestore(ActionDetail & ad, double constant, double skill_level_rate, int skill_level) {
	ad.n_target = 0;
	ad.type = Self_TP_Restore;
	ad.parameters = { constant, skill_level_rate };
	ad.parameters_int = { skill_level };
}

void AddToxin(ActionDetail & ad, double constant, double skill_level_rate, double time, int skill_level) {
	ad.n_target = 1;
	ad.type = Toxin_Debuff;
	ad.parameters = { constant, skill_level_rate, time };
	ad.parameters_int = { skill_level };
}
