#pragma once

#include "labware/data/AdaptorData.hpp"
#include "labware/kinds/ContainerComponent.hpp"
#include "mixtures/kinds/Atmosphere.hpp"

class Adaptor : public ContainerComponent<Atmosphere>
{
public:
    Adaptor(const LabwareId id, Atmosphere& atmosphere) noexcept;

    const AdaptorData& getData() const override final;

    bool tryConnect(LabwareComponentBase& other) override final;
    void disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other) override final;
};
