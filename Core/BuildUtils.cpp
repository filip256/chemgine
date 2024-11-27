#include "BuildUtils.hpp"

const std::string& Utils::getBuildTypeName()
{
#ifdef NDEBUG
	static const std::string buildTypeName = "RELEASE";
#else
	static const std::string buildTypeName = "DEBUG";
#endif

	return buildTypeName;
}
