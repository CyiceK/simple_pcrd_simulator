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

0 ǰ���˺� 
	n_targetΪ��ѰĿ����
	parameters����Ϊ������ȼ����ʣ�ATK���ʣ���parameters[3]��ʼΪÿһ���˺�����
	parameters_int[0]Ϊ���ܵȼ�
	parameters_int[1]Ϊ����
	parameters_int[2]Ϊ�Ƿ�ر������ݶ�����boolλ�ã�
1 ȫ���ѷ�������BUFF
	parameters����Ϊ������ȼ����ʣ�����ʱ��
	parameters_int[0]Ϊ���ܵȼ�
2 ȫ���ѷ��ٶ�BUFF
	parameterΪ������������ʱ��
3 ��������BUFF
	parameters����Ϊ������ȼ����ʣ�����ʱ��
	parameters_int[0]Ϊ���ܵȼ�
4 ȫ��TP�ظ�
	parametersΪ������ȼ�����
	parameters_int[0]Ϊ���ܵȼ�
5 ����TP�ظ�
	parametersΪ������ȼ�����
6 ǰ���Ƽ�
	n_targetΪ��ѰĿ����
	parameters����Ϊ������ȼ����ʣ�����ʱ��
	parameters_int[0]Ϊ���ܵȼ�
7 ��������BUFF
	parameters����Ϊ������ȼ����ʣ�����ʱ��
	parameters_int[0]Ϊ���ܵȼ�
8 ��
	parameters����Ϊ������ȼ����ʣ�����ʱ��
	parameters_int[0]Ϊ���ܵȼ�


*****************/