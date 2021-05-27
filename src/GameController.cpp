#include "GameController.h"
#include <limits>
#include "pcrmath.h"
using namespace std;

default_random_engine GameController::seed_reng((int)time(nullptr));
uniform_int_distribution<int16_t> GameController::seed_dist(numeric_limits<int16_t>::min(), numeric_limits<int16_t>::max());

GameController::GameController() {
	Unit *u0 = new Makoto();
	Unit *u1 = new Eriko();
	Unit *u2 = new Jita();
	Unit *u3 = new Kanna();
	Unit *u4 = new Shiori();
	Unit *enemy = new Enemy();

	position_map[0] = u0;
	position_map[1] = u1;
	position_map[2] = u2;
	position_map[3] = u3;
	position_map[4] = u4;

	position_map[-1] = enemy;

	units_view = { u0, u1, u2, u3, u4, enemy };
	seed = seed_dist(seed_reng);
	reng = new std::default_random_engine(seed);
	for (auto unit : units_view) {
		unit->gc = this;
	}
}

GameController::~GameController() {
	for (auto unit : units_view) {
		delete unit;
	}
	delete reng;
}

double GameController::rng() {
	return ud(*reng);
}

bool GameController::get_critical(double critical_rate, int from_level, int to_level)
{
	bool is_critical = false;
	if (critical_rate >= 1) {
		is_critical = true;
	}
	else {
		double r = rng();
		if (r < critical_rate) {
			is_critical = true;
		}
	}
	return is_critical;
}

bool GameController::get_cast_ub(int unit_id, int frame) {
	if (find(ublist.begin(), ublist.end(), pair<int, int>(unit_id, frame)) != ublist.end()) {
		return true;
	}
	return false;
}

vector<Unit*> GameController::get_friends() {
	vector<Unit*> ret;
	for (int i = 0; i <= 4; ++i) {
		if (!units_view[i]->dead) ret.push_back(units_view[i]);
	}
	return ret;
}

vector<Unit*> GameController::get_front_enemy(Unit *unit, int num_target) {
	if (unit->side == 0) {
		return { units_view.back() };
	}
	if (unit->side == 1) {
		if (num_target == 1) {
			for (int i = 0; i <= 4; ++i) {
				if (!units_view[i]->dead) return { units_view[i] };
			}
			return {};
		}
		if (num_target == 3) {
			vector<Unit*> ret;
			for (int i = 0; i <= 4; ++i) {
				if (!units_view[i]->dead) ret.push_back(units_view[i]);
				if (ret.size() == 3) return ret;
			}
			return ret;
		}
	}
	throw runtime_error("Impossible branch.");
}

vector<Unit*> GameController::get_all_friend(Unit * unit) {
	if (unit->side == 0) {
		return get_friends();
	}
	else {
		return { units_view.back() };
	}
}

void GameController::multipart_damage(Unit *from, Unit *to, int raw_damage, int part, std::vector<double> portion, int always_critical) {
	int reduced_damage = int(raw_damage * to->get_def_coef()); 
	double critical_rate = from->get_critical_rate(to);
	for (int i = 0; i < part; ++i) {
		int dmg = int(reduced_damage * portion[i]);
		if ((always_critical >> i) == 1) {
			dmg *= 2;
			to->receive_damage(dmg);
			from->deal_damage(dmg, to->level);
			logger.record_damage(from, to, dmg, frame, true);
		}
		else {
			bool is_critical = get_critical(critical_rate, from->level, to->level);
			if (is_critical) dmg *= 2;
			to->receive_damage(dmg);
			from->deal_damage(dmg, to->level);
			logger.record_damage(from, to, dmg, frame, is_critical);
		}
	}
}

void GameController::simple_damage(Unit* from, Unit* to, int raw_damage) {
	double critical_rate = from->get_critical_rate(to);
	bool is_critical = get_critical(critical_rate, from->level, to->level);
	int reduced_damage = int(raw_damage * to->get_def_coef());
	if (is_critical) reduced_damage *= 2;

	to->receive_damage(reduced_damage);
	from->deal_damage(reduced_damage, to->level);
	logger.record_damage(from, to, reduced_damage, frame, is_critical);
}

void GameController::damage_dealer(Unit *from, Unit *to, ActionDetail &ad) {
	// 未考虑skill level

	int atk = from->get_atk();
	int skill_level = from->level;
	int raw_damage = int(ad.parameters[0] + skill_level * ad.parameters[1] + ad.parameters[2] * atk);
	double critical_rate = from->get_critical() * 1.0 / 2000.0 * from->level / to->level;

	int part = ad.parameters_int[1];

	if (part == 0) {		
		simple_damage(from, to, raw_damage);
	}
	else {
		vector<double> portion;
		for (int i = 0; i < part; ++i) {
			portion.push_back(ad.parameters[3 + i]);
		}
		int always_critical = ad.parameters_int[2];
		multipart_damage(from, to, raw_damage, part, portion, always_critical);
	}
}

void GameController::toxin(Unit *from, Unit *to, int raw_damage, double time)
{
	double resist_rate = to->get_toxin_resist_rate();
	double r = rng();
	if (r >= resist_rate) {
		to->toxin.active = true;
		to->toxin.from = from;
		to->toxin.value = raw_damage;
		to->toxin.period_frame = 60;
		to->toxin.time = int(time * 60);
		to->toxin.time_counter = 0;
		logger.record_toxin_success(from, to, raw_damage, time, frame);
	}
	else {
		logger.record_toxin_fail(from, to, frame);
	}
}

void GameController::action_handler(Unit *unit, int action) {
	if (unit->dead) { return; }
	// 死了就不结算了
	ActionDetail ad = unit->get_action_detail(action);
	vector<Unit*> enemies;
	vector<Unit*> friends;
	int value;
	double value_float;
	double duration;
	Buff buff; 
	double actualTPgain;
	switch (ad.type) {
	case Damage:
		enemies = get_front_enemy(unit, ad.n_target);
		for (auto enemy : enemies) {
			damage_dealer(unit, enemy, ad);
		}
		break;

	case ATK_Buff:
		friends = get_friends();
		value = ceil_int(ad.parameters[0] + ad.parameters[1] * ad.parameters_int[0]);
		duration = ad.parameters[2];
		buff = atk_buff(value, duration);
		for (auto friendunit : friends) {
			friendunit->atk_buffs.push_back(buff);
			logger.record_atk_buff(unit, friendunit, value, frame);
		}
		break;

	case Speed_Buff:
		friends = get_friends();
		value_float = ad.parameters[0];
		duration = ad.parameters[1];
		buff = speed_buff(value_float, duration);
		for (auto friendunit : friends) {
			friendunit->speed_buff = buff; 
			logger.record_speed_buff(unit, friendunit, value_float, frame);
		}		
		break;

	case Self_ATK_Buff:
		value = ceil_int(ad.parameters[0] + ad.parameters[1] * ad.parameters_int[0]);
		duration = ad.parameters[2];
		buff = atk_buff(value, duration);
		unit->atk_buffs.push_back(buff);
		logger.record_atk_buff(unit, unit, value, frame);
		break;

	case TP_Restore:
		friends = get_friends();
		value_float = ceill(ad.parameters[0] + ad.parameters[1] * ad.parameters_int[0]);
		for (auto friendunit : friends) {
			actualTPgain = value_float * friendunit->get_tp_up_coef();
			friendunit->tp_obtain(actualTPgain);
			logger.record_tp_gain(unit, friendunit, actualTPgain, friendunit->TP, frame);
		}
		break;

	case Self_TP_Restore:
		value_float = ceill(ad.parameters[0] + ad.parameters[1] * ad.parameters_int[0]);
		actualTPgain = value_float * unit->get_tp_up_coef(); 
		unit->tp_obtain(actualTPgain); 
		logger.record_tp_gain(unit, unit, actualTPgain, unit->TP, frame);
		break;

	case Def_Debuff:
		enemies = get_front_enemy(unit, ad.n_target);
		for (auto enemy : enemies) {
			value = ceil_int(ad.parameters[0] + ad.parameters[1] * ad.parameters_int[0]);
			duration = ad.parameters[2];
			buff = def_debuff(value, duration);
			enemy->def_buffs.push_back(buff);
			logger.record_def_buff(unit, enemy, -value, frame);
		}
		break;

	case Self_Critical_Buff:
		value = ceil_int(ad.parameters[0] + ad.parameters[1] * ad.parameters_int[0]);
		duration = ad.parameters[2];
		buff = critical_buff(value, duration);
		unit->critical_buffs.push_back(buff);
		break;

	case Toxin_Debuff:
		enemies = get_front_enemy(unit, ad.n_target);
		for (auto enemy : enemies) {
			value = ceil_int(ad.parameters[0] + ad.parameters[1] * ad.parameters_int[0]);
			duration = ad.parameters[2];
			toxin(unit, enemy, value, duration);
		}

	default:
		;
		// throw runtime_error("Impossible branch");
	}
}

std::vector<std::pair<Unit*, int>> GameController::obtain_actionlist() {
	std::vector<std::pair<Unit*, int>> action_list;
	for (auto unit : units_view) {
		std::vector<int> actions = unit->to_next_frame();
		if (actions.size() != 0) {
			for (auto action : actions) {
				action_list.push_back({ unit, action });
			}
		}
	}
	return action_list;
}

void GameController::calculate_action() {
	auto action_list = obtain_actionlist();
	for (auto &unitaction : action_list) {
		action_handler(unitaction.first, unitaction.second);
	}
}

void GameController::calculate_toxin_buff() {
	for (auto unit : units_view) {
		unit->toxin_damage();
	}
	for (auto unit : units_view) {
		unit->all_buffs_to_next_frame();
	}
}

void GameController::auto_cast_ub() {
	for (auto unit : units_view) {
		if (unit->auto_ub && unit->TP >= 1000.0 && unit->status == 0) {
			vector<int> actions = unit->get_ub_detail();
			for (auto action : actions) {
				action_handler(unit, action);
			}
			unit->after_cast_ub();
			logger.record_ub_cast(unit, frame);
		}
	}

}

void GameController::cast_ub_from_ublist() {
	for (auto unit : units_view) {
		if (get_cast_ub(unit->unit_id, frame)) {
			if (unit->TP < 1000.0) {
				printf("%4d帧 %s想要施放UB，但是TP只有%lf",
					frame, unit->unit_name.c_str(), unit->TP);
				throw runtime_error("Cannot cast ub.");
			}
			vector<int> actions = unit->get_ub_detail();
			for (auto action : actions) {
				action_handler(unit, action);
			}
			unit->after_cast_ub();
			logger.record_ub_cast(unit, frame);
		}
	}
}

void GameController::global_controller() {
	for (frame = 0; frame < 5400; ++frame) {
		calculate_toxin_buff();
		calculate_action();
		auto_cast_ub();
		cast_ub_from_ublist();
	}
}

void GameController::single_step_initiator() {
	frame = 0;
}

void GameController::single_step_next_frame() {
	calculate_toxin_buff();
	calculate_action();
	auto_cast_ub();
	frame++;
}

bool GameController::single_step_is_over()
{
	return frame >= 5400;
}

bool GameController::single_step_has_ub(int position) {
	if (position >= 0 && position <= 4) {
		Unit* unit = position_map[position];
		return unit->TP >= 1000.0;			
	}
	else {
		throw runtime_error("Bad position input.");
	}
}

int GameController::get_current_tp(int position) {
	if (position >= 0 && position <= 4) {
		Unit* unit = position_map[position];
		return unit->TP;
	}
	else {
		throw runtime_error("Bad position input.");
	}
}

int GameController::get_boss_def() {
	Unit* unit = position_map[-1];
	return unit->get_def();
}


bool GameController::single_step_cast_ub(int position) {
	
	if (position >= 0 && position <= 4) {
		Unit* unit = position_map[position];
		if (unit->TP < 1000.0) {
			/*printf("%4d帧 %s想要施放UB，但是TP只有%lf",
				frame, unit->unit_name.c_str(), unit->TP);*/

			return false;
		}
		vector<int> actions = unit->get_ub_detail();
		for (auto action : actions) {
			action_handler(unit, action);
		}
		unit->after_cast_ub();
		logger.record_ub_cast(unit, frame);

		return true;
	}
	else {
		throw runtime_error("Bad position input.");
	}
}

#define UB_Time(Name, Time) {Name().unit_id, Time}
GameController* game() {
	GameController *gc = new GameController();
	gc->logger.init();
	gc->ublist = {
			UB_Time(Shiori, 683),
			UB_Time(Jita, 904),
			UB_Time(Makoto, 1237),
			UB_Time(Shiori, 1426),
			UB_Time(Eriko, 1665),
			UB_Time(Jita, 1671),
			UB_Time(Kanna, 1672),
			UB_Time(Shiori, 2257),
			UB_Time(Makoto, 2303),
			UB_Time(Jita, 2597),
			UB_Time(Eriko, 2812),
			UB_Time(Kanna, 3063),
			UB_Time(Shiori, 3282),
			UB_Time(Makoto, 3357),
			UB_Time(Jita, 3548),
			UB_Time(Eriko, 4014),
			UB_Time(Kanna, 4291),
			UB_Time(Shiori, 4292),
			UB_Time(Makoto, 4404),
			UB_Time(Jita, 4475),
			UB_Time(Eriko, 5017),
			UB_Time(Shiori, 5327),
			UB_Time(Jita, 5382)
	};
	gc->global_controller();
	return gc;
}

