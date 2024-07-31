#pragma once

#include <string>

namespace Keywords
{
	namespace Syntax
	{
		const std::string Include = "INCLUDE";
	}

	namespace Types
	{
		const std::string Spline = "spline";
		const std::string Function = "func";
		const std::string Atom = "atom";
		const std::string Radical = "radical";
		const std::string Molecule = "mol";
		const std::string Reaction = "react";
		const std::string Labware = "lab";
	}

	namespace Splines
	{
		const std::string Values = "values";
	}

	namespace Atoms
	{
		const std::string Name = "name";
		const std::string Weight = "weight";
		const std::string Valences = "valences";
		const std::string RadicalMatches = "matches";
	}

	namespace Molecules
	{
		const std::string Name = "name";
		const std::string MeltingPoint = "melting_point";
		const std::string BoilingPoint = "boiling_point";
		const std::string SolidDensity = "solid_density";
		const std::string LiquidDensity = "liquid_density";
		const std::string SolidHeatCapacity = "solid_hc";
		const std::string LiquidHeatCapacity = "liquid_hc";
		const std::string FusionLatentHeat = "fusion_lh";
		const std::string VaporizationLatentHeat = "vaporization_lh";
		const std::string SublimationLatentHeat = "sublimation_lh";
		const std::string Hydrophilicity = "hydrophilicity";
		const std::string Lipophilicity = "lipophilicity";
		const std::string InverseSolubility = "has_inverse_solubility";
		const std::string Solubility = "solubility";
		const std::string HenryConstant = "henry_const";
		const std::string Color = "color";
	}

	namespace Reactions
	{
		const std::string Name = "name";
		const std::string Speed = "speed";
		const std::string Energy = "energy";
		const std::string Activation = "activation";
		const std::string Catalysts = "catalysts";
	}

	namespace Labware
	{
		const std::string Flask = "flask";
		const std::string Adaptor = "adaptor";
		const std::string Condenser = "condenser";
		const std::string Heatsource = "heatsource";

		const std::string Name = "name";
		const std::string Ports = "ports";
		const std::string Volume = "volume";
		const std::string Length = "length";
		const std::string Effic = "effic";
		const std::string InnerMask = "inner_mask";
		const std::string CoolantMask = "coolant_mask";
		const std::string Power = "power";
		const std::string Texture = "tx";
		const std::string TextureScale = "tx_scale";
	}
}
