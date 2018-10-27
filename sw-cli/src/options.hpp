#include <cstdint>
#include <map>
#include <string>

enum class tOp: uint8_t {
	INVALID,
	READ,
	WRITE,
	DEV_DUMP,
	DEV_BURN,
	DEV_CLEAR
};

extern const std::map<std::string, tOp> g_mOpFromString;
extern const std::map<tOp, std::string> g_mOpDescription;
