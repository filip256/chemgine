#include "molecules/data/MoleculeData.hpp"

#include "data/def/DataDumper.hpp"
#include "io/Log.hpp"

MoleculeData::MoleculeData(
    const MoleculeId                                                id,
    const std::string&                                              name,
    MolecularStructure&&                                            structure,
    const Amount<Unit::MOLE_RATIO>                                  hydrophilicity,
    const Amount<Unit::MOLE_RATIO>                                  lipophilicity,
    const Color                                                     color,
    EstimatorRef<Unit::CELSIUS, Unit::TORR>&&                       meltingPointEstimator,
    EstimatorRef<Unit::CELSIUS, Unit::TORR>&&                       boilingPointEstimator,
    EstimatorRef<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>&&        solidDensityEstimator,
    EstimatorRef<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>&&        liquidDensityEstimator,
    EstimatorRef<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>&&        solidHeatCapacityEstimator,
    EstimatorRef<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>&&        liquidHeatCapacityEstimator,
    EstimatorRef<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>&& fusionLatentHeatEstimator,
    EstimatorRef<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>&& vaporizationLatentHeatEstimator,
    EstimatorRef<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>&& sublimationLatentHeatEstimator,
    EstimatorRef<Unit::NONE, Unit::CELSIUS>&&                       relativeSolubilityEstimator,
    EstimatorRef<Unit::TORR_MOLE_RATIO, Unit::CELSIUS>&&            henrysConstantEstimator) noexcept :
    GenericMoleculeData(id, std::move(structure)),
    type(this->structure.isOrganic() ? MoleculeType::ORGANIC : MoleculeType::INORGANIC),
    name(name),
    polarity(hydrophilicity, lipophilicity),
    color(color),
    meltingPointEstimator(std::move(meltingPointEstimator)),
    boilingPointEstimator(std::move(boilingPointEstimator)),
    solidDensityEstimator(std::move(solidDensityEstimator)),
    liquidDensityEstimator(std::move(liquidDensityEstimator)),
    solidHeatCapacityEstimator(std::move(solidHeatCapacityEstimator)),
    liquidHeatCapacityEstimator(std::move(liquidHeatCapacityEstimator)),
    fusionLatentHeatEstimator(std::move(fusionLatentHeatEstimator)),
    vaporizationLatentHeatEstimator(std::move(vaporizationLatentHeatEstimator)),
    sublimationLatentHeatEstimator(std::move(sublimationLatentHeatEstimator)),
    relativeSolubilityEstimator(std::move(relativeSolubilityEstimator)),
    henrysConstantEstimator(std::move(henrysConstantEstimator))
{
    if (this->structure.isGeneric()) {
        Log(this).warn("Generic structure with id {0} defined as molecule.", id);
    }
}

const MolecularStructure& MoleculeData::getStructure() const { return structure; }

void MoleculeData::dumpDefinition(
    std::ostream& out, const bool prettify, std::unordered_set<EstimatorId>& alreadyPrinted) const
{
    static constexpr auto valueOffset = checked_cast<uint8_t>(utils::max(
        def::Molecules::Name.size(),
        def::Molecules::MeltingPoint.size(),
        def::Molecules::BoilingPoint.size(),
        def::Molecules::SolidDensity.size(),
        def::Molecules::LiquidDensity.size(),
        def::Molecules::SolidHeatCapacity.size(),
        def::Molecules::LiquidHeatCapacity.size(),
        def::Molecules::FusionLatentHeat.size(),
        def::Molecules::VaporizationLatentHeat.size(),
        def::Molecules::SublimationLatentHeat.size(),
        def::Molecules::RelativeSolubility.size(),
        def::Molecules::HenryConstant.size(),
        def::Molecules::Hydrophilicity.size(),
        def::Molecules::Lipophilicity.size(),
        def::Molecules::Color.size()));

    def::DataDumper(out, valueOffset, 0, prettify)
        .tryOutlineSubDefinition(meltingPointEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(boilingPointEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(solidDensityEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(liquidDensityEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(solidHeatCapacityEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(liquidHeatCapacityEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(fusionLatentHeatEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(vaporizationLatentHeatEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(sublimationLatentHeatEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(relativeSolubilityEstimator, alreadyPrinted)
        .tryOutlineSubDefinition(henrysConstantEstimator, alreadyPrinted)
        .header(def::Types::Molecule, structure, "")
        .beginProperties()
        .property(def::Molecules::Name, name)
        .subDefinition(def::Molecules::MeltingPoint, meltingPointEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::BoilingPoint, boilingPointEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::SolidDensity, solidDensityEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::LiquidDensity, liquidDensityEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::SolidHeatCapacity, solidHeatCapacityEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::LiquidHeatCapacity, liquidHeatCapacityEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::FusionLatentHeat, fusionLatentHeatEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::VaporizationLatentHeat, vaporizationLatentHeatEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::SublimationLatentHeat, sublimationLatentHeatEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::RelativeSolubility, relativeSolubilityEstimator, alreadyPrinted)
        .subDefinition(def::Molecules::HenryConstant, henrysConstantEstimator, alreadyPrinted)
        .property(def::Molecules::Hydrophilicity, polarity.hydrophilicity)
        .property(def::Molecules::Lipophilicity, polarity.lipophilicity)
        .property(def::Molecules::Color, color)
        .endProperties()
        .endDefinition();
}

void MoleculeData::print(std::ostream& out) const
{
    std::unordered_set<EstimatorId> history;
    dumpDefinition(out, true, history);
}
