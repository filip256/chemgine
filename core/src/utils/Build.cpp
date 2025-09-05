#include "utils/Build.hpp"

const std::string& utils::getBuildTypeName()
{
#ifdef NDEBUG
    static const std::string buildTypeName = "RELEASE";
#else
    static const std::string buildTypeName = "DEBUG";
#endif

    return buildTypeName;
}
