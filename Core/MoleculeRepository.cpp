#include "MoleculeRepository.hpp"
#include "Parsers.hpp"
#include "EstimatorParsers.hpp"
#include "AffineEstimator.hpp"
#include "SplineEstimator.hpp"
#include "UnitizedEstimator.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

#include <fstream>

MoleculeRepository::MoleculeRepository(EstimatorRepository& estimators) noexcept :
	estimators(estimators)
{}

bool MoleculeRepository::add(const Def::Object& definition)
{
	auto structure = Def::Parser<MolecularStructure>::parse(definition.getSpecifier());
	if (not structure)
	{
		Log(this).error("Invalid SMILES specifier: '{0}', as: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}
	if (findFirstConcrete(*structure) != nullptr)
	{
		Log(this).warn("Already defined molecule: '{0}' skipped.", definition.getSpecifier());
		return false;
	}

	const auto name = definition.getDefaultProperty(Def::Molecules::Name, "?");
	const auto hp = definition.getDefaultProperty(Def::Molecules::Hydrophilicity, 1.0f,
		Def::parse<float_s>);
	const auto lp = definition.getDefaultProperty(Def::Molecules::Lipophilicity, 0.0f,
		Def::parse<float_s>);
	const auto col = definition.getDefaultProperty(Def::Molecules::Color, Color(0, 255, 255, 100),
		Def::parse<Color>);
	auto mp = definition.getDefinition(Def::Molecules::MeltingPoint,
		Def::Parser<UnitizedEstimator<AbsCelsius, Torr>>::parse, estimators);
	auto bp = definition.getDefinition(Def::Molecules::BoilingPoint,
		Def::Parser<UnitizedEstimator<AbsCelsius, Torr>>::parse, estimators);
	auto sd = definition.getDefinition(Def::Molecules::SolidDensity,
		Def::Parser<UnitizedEstimator<GramPerMilliLiter, Celsius>>::parse, estimators);
	auto ld = definition.getDefinition(Def::Molecules::LiquidDensity,
		Def::Parser<UnitizedEstimator<GramPerMilliLiter, Celsius>>::parse, estimators);
	auto shc = definition.getDefinition(Def::Molecules::SolidHeatCapacity,
		Def::Parser<UnitizedEstimator<JoulePerMoleCelsius, Torr>>::parse, estimators);
	auto lhc = definition.getDefinition(Def::Molecules::LiquidHeatCapacity,
		Def::Parser<UnitizedEstimator<JoulePerMoleCelsius, Torr>>::parse, estimators);
	auto flh = definition.getDefinition(Def::Molecules::FusionLatentHeat,
		Def::Parser<UnitizedEstimator<JoulePerMole, Celsius, Torr>>::parse, estimators);
	auto vlh = definition.getDefinition(Def::Molecules::VaporizationLatentHeat,
		Def::Parser<UnitizedEstimator<JoulePerMole, Celsius, Torr>>::parse, estimators);
	auto slh = definition.getDefinition(Def::Molecules::SublimationLatentHeat,
		Def::Parser<UnitizedEstimator<JoulePerMole, Celsius, Torr>>::parse, estimators);
	auto sol = definition.getDefinition(Def::Molecules::RelativeSolubility,
		Def::Parser<UnitizedEstimator<Dimless, Celsius>>::parse, estimators);
	auto hen = definition.getDefinition(Def::Molecules::HenryConstant,
		Def::Parser<UnitizedEstimator<Torr, Celsius>>::parse, estimators);

	const auto id = getFreeId();
	concreteMolecules.emplace(id,
		std::make_unique<MoleculeData>(
			id, name, std::move(*structure),
			hp, lp, col,
			std::move(*mp), std::move(*bp),
			std::move(*sd), std::move(*ld),
			std::move(*shc), std::move(*lhc), std::move(*flh),
			std::move(*vlh), std::move(*slh), std::move(*sol), std::move(*hen)));

	definition.logUnusedWarnings();
	return true;
}

bool MoleculeRepository::contains(const MoleculeId id) const
{
	return concreteMolecules.contains(id);
}

const MoleculeData& MoleculeRepository::at(const MoleculeId id) const
{
	return *concreteMolecules.at(id);
}

size_t MoleculeRepository::totalDefinitionCount() const
{
	return concreteMolecules.size() + genericMolecules.size();
}

const MoleculeData* MoleculeRepository::findFirstConcrete(const MolecularStructure& structure) const
{
	for (const auto& m : concreteMolecules)
		if (m.second->getStructure() == structure)
			return m.second.get();

	return nullptr;
}

const GenericMoleculeData* MoleculeRepository::findFirstGeneric(const MolecularStructure& structure) const
{
	for (const auto& m : genericMolecules)
		if (m.second->getStructure() == structure)
			return m.second.get();

	return nullptr;
}

const MoleculeData& MoleculeRepository::findOrAddConcrete(MolecularStructure&& structure)
{
	if (structure.isEmpty())
		Log(this).fatal("Tried to create a concrete molecule from an empty structure.");
	if (structure.isGeneric())
		Log(this).fatal("Tried to create a concrete molecule from a generic structure.");

	const auto existing = findFirstConcrete(structure);
	if (existing != nullptr)
		return *existing;

	Log(this).debug("New structure discovered: \n{0}", structure.print());

	const auto hydro = 1.0f;
	const auto lipo = 0.0f;
	const auto color = Color(0, 255, 255, 100);
	auto mp = estimators.add<ConstantEstimator<AbsCelsius, Torr>>(0.0f * _AbsCelsius);
	auto bp = estimators.add<ConstantEstimator<AbsCelsius, Torr>>(100.0f * _AbsCelsius);
	auto sd = estimators.add<ConstantEstimator<GramPerMilliLiter, Celsius>>(1.0f * _GramPerMilliLiter);
	auto ld = estimators.add<ConstantEstimator<GramPerMilliLiter, Celsius>>(1.0f * _GramPerMilliLiter);
	auto shc = estimators.add<ConstantEstimator<JoulePerMoleCelsius, Torr>>(36.0f * _JoulePerMoleCelsius);
	auto lhc = estimators.add<ConstantEstimator<JoulePerMoleCelsius, Torr>>(75.4840232f * _JoulePerMoleCelsius);
	auto flh = estimators.add<ConstantEstimator<JoulePerMole, Celsius, Torr>>(6020.0f * _JoulePerMole);
	auto vlh = estimators.add<ConstantEstimator<JoulePerMole, Celsius, Torr>>(40700.0f * _JoulePerMole);
	auto slh = estimators.add<ConstantEstimator<JoulePerMole, Celsius, Torr>>(std::numeric_limits<float_s>::max() * _JoulePerMole);
	auto sol = estimators.add<ConstantEstimator<Dimless, Celsius>>(1.0f * _Dimless);
	auto hen = estimators.add<ConstantEstimator<Torr, Celsius>>(1000.0f * _Torr);

	const auto id = getFreeId();
	const auto it = concreteMolecules.emplace(id, std::make_unique<MoleculeData>(
		id, structure.toSMILES(), std::move(structure),
		hydro, lipo, color,
		std::move(mp), std::move(bp),
		std::move(sd), std::move(ld),
		std::move(shc), std::move(lhc),
		std::move(flh), std::move(vlh), std::move(slh),
		std::move(sol), std::move(hen)));

	return *it.first->second;
}

const GenericMoleculeData& MoleculeRepository::findOrAdd(MolecularStructure&& structure)
{
	if (structure.isEmpty())
		Log(this).fatal("Tried to create a concrete molecule from an empty structure.");
	if (structure.isConcrete())
		return findOrAddConcrete(std::move(structure));

	const auto existing = findFirstGeneric(structure);
	if (existing != nullptr)
		return *existing;

	const auto id = getFreeId();
	const auto it = genericMolecules.emplace(id, std::make_unique<GenericMoleculeData>(id, std::move(structure)));
	return *it.first->second;
}

MoleculeRepository::Iterator MoleculeRepository::begin() const
{
	return concreteMolecules.begin();
}

MoleculeRepository::Iterator MoleculeRepository::end() const
{
	return concreteMolecules.end();
}

size_t MoleculeRepository::size() const
{
	return concreteMolecules.size();
}

void MoleculeRepository::clear()
{
	concreteMolecules.clear();
	genericMolecules.clear();
}

MoleculeId MoleculeRepository::getFreeId() const
{
	static MoleculeId id = 0;
	while (concreteMolecules.contains(id) || genericMolecules.contains(id))
	{
		if (id == std::numeric_limits<MoleculeId>::max())
			Log(this).fatal("Molecule id limit reached: {0}.", id);
		++id;
	}
	return id;
}
