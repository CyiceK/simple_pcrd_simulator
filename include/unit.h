#pragma once
#include "buff.h"
#include "action.h"
#include <string>
#include <fstream>
#include "json.hpp"
#include "battlelogger.h"
#include <codecvt>

using json = nlohmann::json;
std::string WstringToString(const std::wstring str);

struct GameController;
struct Unit {
	Unit();

	GameController* gc;
	int unit_id;
	std::string unit_name;
	int side; // 0 friend, 1 enemy;
	int level;
	int maxHP;
	int HP;
	bool dead = false;
	double TP = 0;
	int atk;
	int exATK;
	int def;
	int exDEF = 0;
	int critical;
	int miss;
	int lifesteal;
	int tpup;
	int tpdown;

	double collision_radius = 112;

	struct Resistance {
		double toxin_resist = 0;
		double curse_resist = 0;
		double burn_resist  = 0;
	};

	Resistance resistance;

	double get_toxin_resist_rate() {
		return resistance.toxin_resist;
	}

	int skill_1_level;
	int skill_2_level;
	int skill_3_level;
	int skill_4_level;
	int ub_level;

	int onloop = false;
	int loop_pos = 0;

	std::vector<int> begin_actions;
	std::vector<int> onloop_actions;

	int start_frame;			// 不计移动时间的起始帧数
	int wait_frame_counter = 0;
	int wait_frame = 151; // 起手等待时间，后面会更新成技能对应的等待时间
	int status = 0; // 0 wait, 1 movement
	bool ub_freeze = false; // false: no ub; true: on ub freeze;
	int ub_freeze_frame_counter = 0;
	int ub_freeze_frame;

	int action_counter = 0;
	int action_frame;
	std::vector<int> action_full_frame;
	std::vector<int> action_wait_frame;

	std::vector<int> internal_status; // 任何动作中的状态都可以记录于此
	std::vector<double> internal_status_float; // 任何动作中的状态都可以记录于此
	std::vector<Buff> atk_buffs;
	std::vector<Buff> def_buffs;
	std::vector<Buff> critical_buffs;
	std::vector<Buff> tp_buffs;
	Buff speed_buff;
	Toxin toxin;

	bool auto_ub = false;

	void all_buffs_to_next_frame() {
		for (auto& bf : atk_buffs) bf.to_next_frame();
		for (auto& bf : critical_buffs) bf.to_next_frame();
		for (auto& bf : def_buffs) bf.to_next_frame();
		for (auto& bf : tp_buffs) bf.to_next_frame();
		speed_buff.to_next_frame();
	}

	void toxin_damage();
	int get_atk();
	int get_def();
	int get_critical();
	double get_critical_rate(Unit* to);
	double get_critical_multiplier(int action_id) {
		return 1.0;
	}
	int get_tp_up();
	double get_tp_up_coef();
	double get_def_coef();
	void tp_obtain(double newTP);
	double get_lifesteal_coef(int enemy_level);
	void hp_obtain(int newHp);
	void deal_damage(int damageDeal, int enemy_level);
	void receive_damage(int damage);
	void new_movement_tp_obtain();

	void loop_next();
	int get_current_skill();
	int get_next_skill();
	int get_next_wait_frame();
	virtual std::vector<int> get_action_id(int skillid, int frame);
	virtual std::vector<int> get_ub_detail();

	virtual ActionDetail get_action_detail(int action_id);

	std::vector<int> to_next_frame();
	void after_cast_ub();
};

void create_unit_from_json(Unit& unit, int unit_id, std::string filename = "unit_data.json");

void CommonAttack(ActionDetail &ad);

void DamageSkill(ActionDetail &ad, double constant, double skill_level_rate, double atk_rate, int skill_level);

void DamageSkillMultiPart(ActionDetail &ad, double constant, double skill_level_rate, double atk_rate, int skill_level, 
	int n_part, int always_critical, std::vector<double> damage_portion_each_part);

void DamageAoeSkill(ActionDetail &ad, double constant, double skill_level_rate, double atk_rate, int skill_level, int n_target);

void DamageAoeSkillMultiPart(ActionDetail &ad, double constant, double skill_level_rate, double atk_rate, int skill_level, int n_target, 
	int n_part, int always_critical, std::vector<double> damage_portion_each_part);

void SelfATKBuff(ActionDetail &ad, double constant, double skill_level_rate, double time, int skill_level);

void SelfCriticalBuff(ActionDetail &ad, double constant, double skill_level_rate, double time, int skill_level);

void FriendATKBuff(ActionDetail &ad, double constant, double skill_level_rate, double time, int skill_level);

void FriendSpeedBuff(ActionDetail &ad, double value, double time);

void ReduceDef(ActionDetail &ad, double constant, double skill_level_rate, double time, int skill_level);

void FriendTpRestore(ActionDetail &ad, double constant, double skill_level_rate, int skill_level);

void SelfTpRestore(ActionDetail &ad, double constant, double skill_level_rate, int skill_level);

void AddToxin(ActionDetail& ad, double constant, double skill_level_rate, double time, int skill_level);

struct Makoto : public Unit {
	Makoto(); 
	std::vector<int> get_action_id(int skillid, int frame);
	std::vector<int> get_ub_detail();
	ActionDetail get_action_detail(int action_id);
};

struct Eriko : public Unit {
	Eriko();
	std::vector<int> get_action_id(int skillid, int frame);
	std::vector<int> get_ub_detail();
	ActionDetail get_action_detail(int action_id);
};

struct Jita : public Unit {
	Jita();
	std::vector<int> get_action_id(int skillid, int frame);
	std::vector<int> get_ub_detail();
	ActionDetail get_action_detail(int action_id);
};

struct Kanna : public Unit {
	Kanna();
	std::vector<int> get_action_id(int skillid, int frame);
	std::vector<int> get_ub_detail();
	ActionDetail get_action_detail(int action_id);
};

struct Shiori : public Unit {
	Shiori();
	std::vector<int> get_action_id(int skillid, int frame);
	std::vector<int> get_ub_detail();
	ActionDetail get_action_detail(int action_id);
};

struct Enemy : public Unit {
	Enemy();

	std::vector<int> get_action_id(int skillid, int frame);
	std::vector<int> get_ub_detail();
	ActionDetail get_action_detail(int action_id);
};