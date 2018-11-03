#include "megadrive_header.hpp"
#include "trim.hpp"
#include <algorithm>

static inline std::string trimAndLower(std::string szIn) {
	auto Trimmed = rtrim(szIn);
	std::transform(Trimmed.begin(), Trimmed.end(), Trimmed.begin(), ::tolower);
	return Trimmed;
}

std::string tMegadriveHeader::getDomesticName(void) {
	return trimAndLower(std::string(pNameDomestic, 48));
}

std::string tMegadriveHeader::getOverseasName(void) {
	return trimAndLower(std::string(pNameOverseas, 48));
}
