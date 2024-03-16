#pragma once

#include "SFML/Graphics.hpp"
#include "LabwareSystem.hpp"
#include "Flask.hpp"
#include "Logger.hpp"
#include "Adaptor.hpp"

class UIContext
{
private:
	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(1200, 800), "Chemgine");
    std::vector<LabwareSystem> systems;

    class InHand
    {
    public:
        size_t idx = InHand::none;

        bool isSet() const
        {
            return idx != none;
        }

        void set(size_t idx)
        {
            this->idx = idx;
        }
        void unset()
        {
            idx = none;
        }

        constexpr static const size_t none = static_cast<size_t>(-1);
    };

    class DragNDropHelper
    {
        bool inUse = false;
        sf::Vector2f initialPos;

    public:

        void start(const sf::Vector2f& point) 
        { 
            inUse = true;
            initialPos = point; 
        }
        void end()
        { 
            inUse = false;
        }
        void resetOrigin(const sf::Vector2f& point) 
        { 
            initialPos = point;
        }
        bool isInUse() const 
        { 
            return inUse;
        }

        sf::Vector2f getDelta(const sf::Vector2f& point) const { return point - initialPos; }
    };

public:

    void removeEmptySystems()
    {
        for (size_t i = systems.size(); i-- > 0;)
            if(systems[i].size() == 0)
                systems.erase(systems.begin() + i);
    }

	void run()
	{
        Logger::enterContext();

        sf::Font font;
        font.loadFromFile("./Fonts/font.ttf");
        sf::Text text("FPS:", font, 18);
        text.setPosition(sf::Vector2f(5.0f, window.getSize().y - 22.0f));

        bool callRemoveEmptySystems = false;

        InHand inHand;
        DragNDropHelper dndHelper;

        Atmosphere atmosphere(
            1.0_C, 760.0_torr,
            { { Molecule("N#N"), 78.084_mol }, { Molecule("O=O"), 20.946_mol } },
            1000.0_L, DumpContainer::globalDumpContainer);

        auto flask = new Flask(201, atmosphere);
        flask->add(Molecule("CC(=O)OH"), 1.0_mol);
        flask->tick();
        systems.emplace_back(flask);
        systems.emplace_back(new Flask(201, atmosphere));
        systems.emplace_back(new Adaptor(301, atmosphere));
        systems.emplace_back(new Adaptor(302, atmosphere));
        systems.emplace_back(new Flask(201, atmosphere));
        systems.emplace_back(new Adaptor(301, atmosphere));
        systems.emplace_back(new Adaptor(302, atmosphere));

        for(size_t i = 0; i < systems.size(); ++i)
            systems[i].move(sf::Vector2f(100 + 75 * i, 50));

        //window.setFramerateLimit(300);
        window.setVerticalSyncEnabled(true);
        size_t fCount = 0;
        sf::Clock clock;
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                const auto mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                if (event.type == sf::Event::MouseMoved)
                {
                    if (inHand.isSet())
                    {
                        const auto delta = dndHelper.getDelta(mousePos);
                        systems[inHand.idx].move(delta);
                        for (size_t i = 0; i < systems.size(); ++i)
                            if (i != inHand.idx && systems[i].intersects(systems[inHand.idx]))
                            {
                                systems[inHand.idx].move(-delta);
                                sf::Mouse::setPosition(static_cast<sf::Vector2i>(mousePos - delta), window);
                                break;
                            }
                        dndHelper.resetOrigin(mousePos);
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        for (size_t i = 0; i < systems.size(); ++i)
                            if (systems[i].contains(mousePos) != LabwareSystem::npos)
                            {
                                inHand.set(i);
                                dndHelper.start(mousePos);
                                break;
                            }
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        if (inHand.isSet())
                        {
                            for (size_t i = 0; i < systems.size(); ++i)
                            {
                                if (i == inHand.idx)
                                    continue;
                                auto ports = systems[i].findClosestPort(systems[inHand.idx], 1600.0f);
                                if (ports.first.isValid() && LabwareSystem::canConnect(ports.first, ports.second))
                                {
                                    LabwareSystem::connect(ports.first, ports.second);
                                    callRemoveEmptySystems = true;
                                    break;
                                }
                            }

                            inHand.unset();
                            dndHelper.end();
                        }
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        for (size_t i = 0; i < systems.size(); ++i)
                            if (systems[i].size() > 1)
                            {
                                const auto idx = systems[i].contains(mousePos);
                                if (idx == LabwareSystem::npos)
                                    continue;

                                auto newSystems = systems[i].disconnect(idx);
                                for (size_t j = 0; j < newSystems.size(); ++j)
                                    systems.emplace_back(std::move(newSystems[j]));
                                break;
                            }
                    }
                }
                else if (event.type == sf::Event::MouseWheelMoved)
                {
                    if (inHand.isSet())
                        systems[inHand.idx].rotate(event.mouseWheel.delta * 2);
                }

                else if (event.type == sf::Event::Closed)
                    window.close();
            }

            if (callRemoveEmptySystems)
            {
                removeEmptySystems();
                callRemoveEmptySystems = false;
            }

            window.clear();

            for(size_t i = 0; i < systems.size(); ++i)
                window.draw(systems[i]);
            window.draw(text);

            window.display();

            if ((fCount & 3) == 0)
            {
                text.setString("FPS:" + std::to_string(static_cast<int>(
                    4000000.0f / clock.getElapsedTime().asMicroseconds()
                    )));
                clock.restart();
            }

            ++fCount;
        }

        Logger::exitContext();
	}
};