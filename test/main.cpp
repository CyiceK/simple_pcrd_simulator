#include <iostream>
#include <vector>
#include "unit.h"
#include "GameController.h"
#include <chrono>
#include <time.h>
using namespace std;

int main1() {
	clock_t first, second;
	first = clock();
	int avg = 0;

	GameController* gm = game();
	BattleLogger& logger = gm->logger;
	int dmg = logger.total_dmg;
	printf("seed = %7d 总伤害为%d\n", gm->seed, dmg);
	avg += dmg;

	cout << "真琴" << ":" << logger.dmglist[Makoto().unit_id] << endl;
	cout << "病娇" << ":" << logger.dmglist[Eriko().unit_id] << endl;
	cout << "吉他" << ":" << logger.dmglist[Jita().unit_id] << endl;
	cout << "环奈" << ":" << logger.dmglist[Kanna().unit_id] << endl;
	cout << "栞栞" << ":" << logger.dmglist[Shiori().unit_id] << endl;
	delete gm;

	second = clock();
	printf("Time per run is: %f seconds", double(second - first) / CLOCKS_PER_SEC);
	getchar();
	return 0;
}

int main() {
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