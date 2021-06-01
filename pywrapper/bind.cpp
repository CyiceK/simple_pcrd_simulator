#include "GameController.h"
#include "include/pybind11.h"

using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;

PYBIND11_MODULE(pcrdsim_pylib, m) {
	m.doc() = "Simulator for Princess Connect Re:Dive.";

	py::class_<Buff>(m, "Buff")
		.def_readonly("active", &Buff::active)
		.def_readonly("value_float", &Buff::value_float)
		.def_readonly("value", &Buff::value)
		.def_readonly("max_frame", &Buff::max_frame)
		.def_readonly("frame_counter", &Buff::frame_counter)
		;

	py::class_<Unit>(m, "Unit")
		.def_readonly("HP", &Unit::HP)
		.def_readonly("maxHP", &Unit::maxHP)
		.def_readonly("TP", &Unit::TP)
		.def_readonly("status", &Unit::status)
		.def_readonly("action_frame_counter", &Unit::action_frame_counter)
		.def_readonly("wait_frame_counter", &Unit::wait_frame_counter)
		
		.def("get_atk", &Unit::get_atk)
		.def("get_def", &Unit::get_def)
		.def("get_critical", &Unit::get_critical)
		.def("speed_buff", &Unit::speed_buff)
		.def_readonly("atk_buffs", &Unit::atk_buffs)
		.def_readonly("def_buffs", &Unit::def_buffs)
		.def_readonly("critical_buffs", &Unit::critical_buffs)
		;

	py::class_<BattleLogger>(m, "BattleLogger")
		.def_readonly("total_dmg", &BattleLogger::total_dmg)
		.def_readonly("avg_dmg", &BattleLogger::avg_dmg)
		.def_readonly("dmglist", &BattleLogger::dmglist)
		.def_readonly("avgdmglist", &BattleLogger::avgdmglist)
		;

	py::class_<GameController>(m, "GameController")
		.def(py::init<>())
		.def_readonly("logger", &GameController::logger)
		.def("global_controller", &GameController::global_controller)
		.def("single_step_initiator", &GameController::single_step_initiator)
		.def("single_step_has_ub", &GameController::single_step_has_ub)
		.def("get_current_tp", &GameController::get_current_tp)
		.def("get_boss_def", &GameController::get_boss_def)
		.def("single_step_cast_ub", &GameController::single_step_cast_ub)
		.def("single_step_next_frame", &GameController::single_step_next_frame)
		.def("single_step_is_over", &GameController::single_step_is_over)
		.def("get_damage", &GameController::get_damage)
		.def("get_team_tp", &GameController::get_team_tp)
		;

	m.def("game", &game);
}