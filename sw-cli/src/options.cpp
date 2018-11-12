#include <options.hpp>

const std::map<std::string, tOp> g_mOpFromString = {
	{"-r", tOp::READ},
	{"-w", tOp::WRITE},
	{"-dd", tOp::DEV_DUMP},
	{"-db", tOp::DEV_BURN},
	{"-dc", tOp::DEV_CLEAR},
};

const std::map<tOp, std::string> g_mOpDescription = {
	{tOp::READ, "Read data fragment from chip"},
	{tOp::WRITE, "Write data fragment to chip"},
	{tOp::DEV_DUMP, "Read data from whole device"},
	{tOp::DEV_BURN, "Burn data into device"},
	{tOp::DEV_CLEAR, "Clear device"}
};
