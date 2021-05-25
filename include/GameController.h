#pragma once
#include "unit.h"
#include "battlelogger.h"
#include <random>

struct GameController {
	std::vector<Unit*> units_view;
	std::map<int, Unit*> position_map;
	std::vector<std::pair<int, int>> ublist;
	std::map<int, std::vector<int>> unit_ub_list;

	int seed;
	std::default_random_engine *reng;
	std::uniform_real_distribution<double> ud;
	static std::default_random_engine seed_reng;
	static std::uniform_int_distribution<int16_t> seed_dist;
	int frame;
	BattleLogger logger;
	GameController();
	~GameController();

	double rng();
	bool get_critical(double critical_rate, int from_level, int to_level);
	bool get_cast_ub(int unit_id, int frame);

	std::vector<Unit*> __get_friends();
	std::vector<Unit*> get_front_enemy(Unit* unit, int num_target);
	std::vector<Unit*> get_all_friend(Unit* unit);

	void multipart_damage(Unit* from, Unit* to, int raw_damage, int part, std::vector<double> portion, int always_critical);
	void simple_damage(Unit* from, Unit* to, int raw_damage);
	void damage_dealer(Unit* from, Unit* to, ActionDetail &ad);
	void buff_gain(Unit* unit, Unit buff);
	void toxin(Unit* from, Unit* to, int raw_damage, double time);
	void action_handler(Unit* unit, int action);

	std::vector<std::pair<Unit*, int>> obtain_actionlist();

	void calculate_action();

	void calculate_toxin_buff();
	void try_cast_ub();
	void auto_cast_ub();
	void cast_ub_from_ublist();
	void global_controller();
	
	void single_step_initiator();

	// 返回true则成功施放
	// 返回false则施放失败
	bool single_step_cast_ub(int position);
	void single_step_next_frame();

	bool single_step_is_over();

};
