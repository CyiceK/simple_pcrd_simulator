#include <iostream>
#include <vector>
#include "unit.h"
#include "GameController.h"
#include <chrono>
#include <time.h>
using namespace std;

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