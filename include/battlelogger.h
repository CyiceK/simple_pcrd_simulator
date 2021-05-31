#pragma once

#include <iostream>
#include <map>

struct Unit;

struct BattleLogger {
	int total_dmg = 0;
	int avg_dmg = 0;
	std::map<int, int> dmglist;
	std::map<int, int> avgdmglist;
	virtual void init();
	virtual bool character_filter(Unit* unit);
	virtual void record_damage(Unit* from, Unit* to, int damage, int frame, bool critical);
	virtual void record_atk_buff(Unit* from, Unit* to, int buffvalue, int frame);
	virtual void record_speed_buff(Unit* from, Unit* to, double buffvalue, int frame);
	virtual void record_tp_gain(Unit* from, Unit* to, double value, double current_value, int frame);
	virtual void record_def_buff(Unit* from, Unit* to, int value, int frame);
	virtual void record_ub_cast(Unit* unit, int frame);
	virtual void record_action_start(Unit* unit, int frame, int skill);
	virtual void record_action_end(Unit* unit, int frame, int skill);
	virtual void record_toxin_damage(Unit* from, Unit* to, int damage, int frame);
	virtual void record_toxin_success(Unit* from, Unit* to, int damage, double time, int frame);
	virtual void record_toxin_fail(Unit* from, Unit* to, int frame);
};