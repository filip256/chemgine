#pragma once

#include <string>

namespace def
{
	namespace Syntax
	{
		const std::string Include = "INCLUDE";
		const std::string IncludeAs = "AS";
	}

	namespace Amounts
	{
		const std::string Min = "-INF";
		const std::string Max = "INF";
	}

	namespace Types
	{
		const std::string Auto = "";
		const std::string Data = "data";
		const std::string Atom = "atom";
		const std::string Radical = "radical";
		const std::string Molecule = "mol";
		const std::string Reaction = "react";
		const std::string Labware = "lab";
	}

	namespace Data
	{
		const std::string Constant = "const";
		const std::string Values = "values";
		const std::string Mode = "mode";
		const std::string CompressionLoss = "loss";
		const std::string Base = "base";
		const std::string Input1 = "input_1";
		const std::string Input2 = "input_2";
		const std::string VerticalShift = "v_shift";
		const std::string HorizontalShift = "h_shift";
		const std::string Scale = "scale";
		const std::string AnchorPoint = "anchor_point";
		const std::string RebasePoint = "rebase_point";
		const std::string Parameters = "params";
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
		const std::string RelativeSolubility = "rel_solubility";
		const std::string HenryConstant = "henry_const";
		const std::string Color = "color";
	}

	namespace Reactions
	{
		const std::string Id = "id";
		const std::string Name = "name";
		const std::string TemperatureSpeed = "speed_t";
		const std::string ConcentrationSpeed = "speed_c";
		const std::string Energy = "energy";
		const std::string Activation = "activation";
		const std::string Catalysts = "catalysts";
		const std::string IsCut = "is_cut";
	}

	namespace Labware
	{
		const std::string Id = "id";
		const std::string Flask = "flask";
		const std::string Adaptor = "adaptor";
		const std::string Condenser = "condenser";
		const std::string Heatsource = "heatsource";

		const std::string Name = "name";
		const std::string Ports = "ports";
		const std::string Volume = "volume";
		const std::string Length = "length";
		const std::string Efficiency = "effic";
		const std::string InnerMask = "inner_mask";
		const std::string CoolantMask = "coolant_mask";
		const std::string Power = "power";
		const std::string Texture = "tx";
		const std::string TextureScale = "tx_scale";
	}

	namespace Color
	{
		const std::string R = "r";
		const std::string G = "g";
		const std::string B = "b";
		const std::string Intensity = "intensity";
	}

	namespace Port
	{
		const std::string X = "x";
		const std::string Y = "y";
		const std::string Angle = "angle";
	}
}
