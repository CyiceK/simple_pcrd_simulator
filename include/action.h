#pragma once
#include <vector>

enum ActionType {
	Damage,
	ATK_Buff,
	Speed_Buff,
	Self_ATK_Buff,
	TP_Restore,
	Self_TP_Restore,
	Def_Debuff,
	Self_Critical_Buff,
	Toxin_Debuff
};

struct ActionDetail {
	int n_target;
	double range;

	std::vector<double> parameters;
	std::vector<int> parameters_int;
	ActionType type;
	
	enum TargetType {
		target_enemy,
		target_friend,
	};
	enum SearchType {
		search_nearest,
		search_range
	};

	int TargetType;
	
};

#define CRITICAL_1ST 1
#define CRITICAL_2ND 2
#define CRITICAL_3RD 4
#define CRITICAL_4TH 8
#define CRITICAL_5TH 16
#define CRITICAL_6TH 32
#define CRITICAL_7TH 64
#define CRITICAL_8TH 128
#define CRITICAL_9TH 256
#define CRITICAL_10TH 1024

/****************

0 前方伤害 
	n_target为搜寻目标数
	parameters依次为常数项，等级倍率，ATK倍率，从parameters[3]开始为每一段伤害比例
	parameters_int[0]为技能等级
	parameters_int[1]为段数
	parameters_int[2]为是否必爆（根据段数的bool位置）
1 全体友方攻击力BUFF
	parameters依次为常数项，等级倍率，持续时间
	parameters_int[0]为技能等级
2 全体友方速度BUFF
	parameter为加速量，持续时间
3 自身攻击力BUFF
	parameters依次为常数项，等级倍率，持续时间
	parameters_int[0]为技能等级
4 全体TP回复
	parameters为常数项，等级倍率
	parameters_int[0]为技能等级
5 自身TP回复
	parameters为常数项，等级倍率
6 前方破甲
	n_target为搜寻目标数
	parameters依次为常数项，等级倍率，持续时间
	parameters_int[0]为技能等级
7 自身暴击率BUFF
	parameters依次为常数项，等级倍率，持续时间
	parameters_int[0]为技能等级
8 毒
	parameters依次为常数项，等级倍率，持续时间
	parameters_int[0]为技能等级


*****************/