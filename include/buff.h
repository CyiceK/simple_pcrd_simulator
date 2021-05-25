#pragma once

#include <vector>
struct Buff {
	bool active = false;
	int value = 0;
	float value_float = 1.0;
	int max_frame;
	int frame_counter = 0;

	inline void to_next_frame() {
		if (active) {
			frame_counter++;
			if (frame_counter > max_frame)
				active = false;
		}
	}
};

Buff atk_buff(int atk_value, double time);

Buff speed_buff(double speed_value, double time);

Buff def_debuff(int def_value, double time);

Buff critical_buff(int critical_value, double time);

struct Unit;

struct Toxin {
	bool active = false;
	int value = 0;
	int time;
	int time_counter;
	int period_frame = 60;
	Unit* from;

	inline bool to_next_frame() {
		bool ret = false;
		if (active) {
			time_counter++;
			if (time_counter >= time) {
				active = false;
			}
			if (time_counter % period_frame == 1 && time_counter > 1) {
				ret = true;
			}
			else {
				ret = false;
			}			
			return ret;
		}
		else {
			ret = false;
			return ret;
		}
	}
};