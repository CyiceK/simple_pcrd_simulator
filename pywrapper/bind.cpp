#include "GameController.h"
#include "include/pybind11.h"

using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;

PYBIND11_MODULE(pcrdsim_pylib, m) {
	m.doc() = "Simulator for Princess Connect Re:Dive.";

	py::class_<Unit>(m, "Unit")
		.def_readonly("HP", &Unit::HP)
		.def_readonly("maxHP", &Unit::maxHP)
		.def_readonly("TP", &Unit::TP)
		;

	py::class_<BattleLogger>(m, "BattleLogger")
		.def_readonly("total_dmg", &BattleLogger::total_dmg)
		;

	py::class_<GameController>(m, "GameController")
		.def(py::init<>())
		.def_readonly("logger", &GameController::logger)
		.def("global_controller", &GameController::global_controller)
		.def("single_step_initiator", &GameController::single_step_initiator)
		.def("single_step_has_ub", &GameController::single_step_has_ub)
		.def("single_step_cast_ub", &GameController::single_step_cast_ub)
		.def("single_step_next_frame", &GameController::single_step_next_frame)
		.def("single_step_is_over", &GameController::single_step_is_over)
		.def("get_damage", &GameController::get_damage)
		.def("get_team_tp", &GameController::get_team_tp)
		;

	m.def("game", &game);
}