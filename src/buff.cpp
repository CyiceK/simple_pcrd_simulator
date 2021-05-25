#include "buff.h"

Buff atk_buff(int atk_value, double time) {
	Buff buff;
	buff.active = true;
	buff.value = atk_value;
	buff.max_frame = (int)(time * 60.0);
	return buff;
}

Buff speed_buff(double speed_value, double time) {
	Buff buff;
	buff.active = true;
	buff.value_float = speed_value;
	buff.max_frame = (int)(time * 60.0);
	return buff;
}

Buff def_debuff(int def_value, double time) {
	Buff buff;
	buff.active = true;
	buff.value = -def_value;
	buff.max_frame = (int)(time * 60.0);
	return buff;
}

Buff critical_buff(int critical_value, double time) {
	Buff buff;
	buff.active = true;
	buff.value = critical_value;
	buff.max_frame = (int)(time * 60.0);
	return buff;
}
