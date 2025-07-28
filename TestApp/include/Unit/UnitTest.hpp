#pragma once

#include "common/TestSetup.hpp"
#include "MetaUtils.hpp"

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <regex>

class UnitTest
{
private:
	const std::string name;

public:
	UnitTest(std::string&& name) noexcept;
	UnitTest(const UnitTest&) = default;
	UnitTest(UnitTest&&) = default;
	virtual ~UnitTest() = default;

	const std::string& getName() const;
	virtual size_t getTestCount() const;

	virtual bool run() = 0;

	virtual bool isSkipped(const std::regex& filter) const;
};


template<typename SetupT>
class UnitTestSetup final : public UnitTest
{
	static_assert(std::is_base_of_v<TestSetup, SetupT>,
		"UnitTestSetup: SetupT must be a TestSetup derived type.");

private:
	SetupT setup;

public:
	UnitTestSetup(
		std::string&& name,
		SetupT&& setup
	) noexcept;

	using SetupType = SetupT;

	size_t getTestCount() const override final;

	bool run() override final;

	bool isSkipped(const std::regex& filter) const override final;
};

template<typename SetupT>
UnitTestSetup<SetupT>::UnitTestSetup(
	std::string&& name,
	SetupT&& setup
) noexcept :
	UnitTest(std::move(name)),
	setup(std::move(setup))
{}

template<typename SetupT>
size_t UnitTestSetup<SetupT>::getTestCount() const
{
	return 0; // Don't count setups
}

template<typename SetupT>
bool UnitTestSetup<SetupT>::run()
{
	setup.run();
	return true;
}

template<typename SetupT>
bool UnitTestSetup<SetupT>::isSkipped(const std::regex&) const
{
	return false; // Never skip setups
}


class UnitTestGroup : public UnitTest
{
private:
	size_t testCount = 0;
	std::vector<std::unique_ptr<UnitTest>> tests;
	const std::regex& filter;

protected:
	template<typename T, typename... Args>
	void registerTest(std::string&& name, Args&&... args);

public:
	UnitTestGroup(
		std::string&& name,
		const std::regex& filter
	) noexcept;

	size_t getTestCount() const override final;

	bool run() override;

	bool isSkipped(const std::regex& filter) const override final;
};

template<typename T, typename... Args>
void UnitTestGroup::registerTest(std::string&& name, Args&&... args)
{
	static_assert(std::is_base_of_v<UnitTest, T>,
		"UnitTestGroup: T must be a UnitTest derived type.");

	name = getName() + '.' + name; // Append owning group name

	std::unique_ptr<UnitTest> test;
	if constexpr (utils::is_specialization_of_v<T, UnitTestSetup>)
		test = std::make_unique<T>(std::move(name), T::SetupType(std::forward<Args>(args)...));
	else if constexpr (std::is_constructible_v<T, std::string&&, const std::regex&, Args...>)
		test = std::make_unique<T>(std::move(name), filter, std::forward<Args>(args)...);
	else 
		test = std::make_unique<T>(std::move(name), std::forward<Args>(args)...);

	if (test->isSkipped(filter))
		return;

	testCount += test->getTestCount();
	tests.emplace_back(std::move(test));
}
