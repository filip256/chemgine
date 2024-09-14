#pragma once

namespace Utils
{
	template<typename FisrtT, typename... RestT>
	constexpr FisrtT getFirstArg();
}

template<typename FisrtT, typename... RestT>
constexpr FisrtT Utils::getFirstArg()
{
	return FisrtT
}
