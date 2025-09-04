#pragma once

#include <string_view>

namespace def
{
namespace Syntax
{

constexpr std::string_view Include   = "INCLUDE";
constexpr std::string_view IncludeAs = "AS";

}  // namespace Syntax

namespace Amounts
{

constexpr std::string_view Min = "-INF";
constexpr std::string_view Max = "INF";

}  // namespace Amounts

namespace Types
{

constexpr std::string_view Auto     = "";
constexpr std::string_view Data     = "data";
constexpr std::string_view Atom     = "atom";
constexpr std::string_view Radical  = "radical";
constexpr std::string_view Molecule = "mol";
constexpr std::string_view Reaction = "react";
constexpr std::string_view Labware  = "lab";

}  // namespace Types

namespace Data
{

constexpr std::string_view Constant        = "const";
constexpr std::string_view Values          = "values";
constexpr std::string_view Mode            = "mode";
constexpr std::string_view CompressionLoss = "loss";
constexpr std::string_view Base            = "base";
constexpr std::string_view Input1          = "input_1";
constexpr std::string_view Input2          = "input_2";
constexpr std::string_view VerticalShift   = "v_shift";
constexpr std::string_view HorizontalShift = "h_shift";
constexpr std::string_view Scale           = "scale";
constexpr std::string_view AnchorPoint     = "anchor_point";
constexpr std::string_view RebasePoint     = "rebase_point";
constexpr std::string_view Parameters      = "params";

}  // namespace Data

namespace Atoms
{

constexpr std::string_view Name           = "name";
constexpr std::string_view Weight         = "weight";
constexpr std::string_view Valences       = "valences";
constexpr std::string_view RadicalMatches = "matches";

}  // namespace Atoms

namespace Molecules
{

constexpr std::string_view Name                   = "name";
constexpr std::string_view MeltingPoint           = "melting_point";
constexpr std::string_view BoilingPoint           = "boiling_point";
constexpr std::string_view SolidDensity           = "solid_density";
constexpr std::string_view LiquidDensity          = "liquid_density";
constexpr std::string_view SolidHeatCapacity      = "solid_hc";
constexpr std::string_view LiquidHeatCapacity     = "liquid_hc";
constexpr std::string_view FusionLatentHeat       = "fusion_lh";
constexpr std::string_view VaporizationLatentHeat = "vaporization_lh";
constexpr std::string_view SublimationLatentHeat  = "sublimation_lh";
constexpr std::string_view Hydrophilicity         = "hydrophilicity";
constexpr std::string_view Lipophilicity          = "lipophilicity";
constexpr std::string_view RelativeSolubility     = "rel_solubility";
constexpr std::string_view HenryConstant          = "henry_const";
constexpr std::string_view Color                  = "color";

}  // namespace Molecules

namespace Reactions
{

constexpr std::string_view Id                 = "id";
constexpr std::string_view Name               = "name";
constexpr std::string_view TemperatureSpeed   = "speed_t";
constexpr std::string_view ConcentrationSpeed = "speed_c";
constexpr std::string_view Energy             = "energy";
constexpr std::string_view Activation         = "activation";
constexpr std::string_view Catalysts          = "catalysts";
constexpr std::string_view IsCut              = "is_cut";

}  // namespace Reactions

namespace Labware
{

constexpr std::string_view Id         = "id";
constexpr std::string_view Flask      = "flask";
constexpr std::string_view Adaptor    = "adaptor";
constexpr std::string_view Condenser  = "condenser";
constexpr std::string_view Heatsource = "heatsource";

constexpr std::string_view Name         = "name";
constexpr std::string_view Ports        = "ports";
constexpr std::string_view Volume       = "volume";
constexpr std::string_view Length       = "length";
constexpr std::string_view Efficiency   = "effic";
constexpr std::string_view InnerMask    = "inner_mask";
constexpr std::string_view CoolantMask  = "coolant_mask";
constexpr std::string_view Power        = "power";
constexpr std::string_view Texture      = "tx";
constexpr std::string_view TextureScale = "tx_scale";

}  // namespace Labware

namespace Color
{

constexpr std::string_view R         = "r";
constexpr std::string_view G         = "g";
constexpr std::string_view B         = "b";
constexpr std::string_view Intensity = "intensity";

}  // namespace Color

namespace Port
{

constexpr std::string_view X     = "x";
constexpr std::string_view Y     = "y";
constexpr std::string_view Angle = "angle";

}  // namespace Port
}  // namespace def
