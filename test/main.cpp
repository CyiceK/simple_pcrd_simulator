#include <iostream>
#include <vector>
#include "unit.h"
#include "GameController.h"
#include <chrono>
#include <time.h>
using namespace std;

#define ShowDamage(CharacterName) \
	printf("%s\t:%d \t(蓝字%d)\n", \
	CharacterName().unit_name.c_str(), \
	logger.dmglist[CharacterName().unit_id], \
	logger.avgdmglist[CharacterName().unit_id])

int main() {
	clock_t first, second;
	first = clock();
	int avg = 0;

	GameController* gm = game();
	BattleLogger& logger = gm->logger;
	int dmg = logger.total_dmg;
	int avg_dmg = logger.avg_dmg;
	printf("seed = %7d\n总伤害为%d  (蓝字%d)\n", gm->seed, dmg, avg_dmg);
	avg += dmg;

	ShowDamage(Makoto);
	ShowDamage(Eriko);
	ShowDamage(Jita);
	ShowDamage(Kanna);
	ShowDamage(Shiori);

	delete gm;

	second = clock();
	printf("Time per run is: %f seconds", double(second - first) / CLOCKS_PER_SEC);
	getchar();
	return 0;
}

int main1() {
	clock_t first, second;
	first = clock();
	int repeat = 30;
	int avg = 0;

	for (int i = 0; i < repeat; ++i) {
		GameController* gm = game();
		BattleLogger& logger = gm->logger;
		int dmg = logger.total_dmg;
		printf("seed = %7d 第%3d刀 伤害为%d\n", gm->seed, i + 1, dmg);
		avg += dmg;
		delete gm;
	}
	printf("平均%d伤害\n", avg / repeat);
	second = clock();
	printf("Time per run is: %f seconds", double(second - first) / CLOCKS_PER_SEC / repeat);
	getchar();
	return 0;
}