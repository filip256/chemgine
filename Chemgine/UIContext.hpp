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

        InHand inHand;
        DragNDropHelper dndHelper;

        systems.emplace_back(Flask(201));
        systems.emplace_back(Flask(201));
        systems.emplace_back(Adaptor(301));
        systems.emplace_back(Adaptor(301));
        systems[0].move(sf::Vector2f(100, 100));
        systems[1].move(sf::Vector2f(200, 200));
        systems[2].move(sf::Vector2f(300, 300));

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
                    for (size_t i = 0; i < systems.size(); ++i)
                        if (systems[i].contains(mousePos) != LabwareSystem::npos)
                        {
                            inHand.set(i);
                            dndHelper.start(mousePos);
                        }
                }
                else if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (inHand.isSet())
                    {
                        for (size_t i = 0; i < systems.size(); ++i)
                        {
                            if (i == inHand.idx)
                                continue;

                            auto ports = systems[i].findClosestPort(systems[inHand.idx], 900.0f);
                            if (ports.first.isValid())
                            {
                                LabwareSystem::connect(ports.first, ports.second);
                                removeEmptySystems();
                                break;
                            }
                        }

                        inHand.unset();
                        dndHelper.end();
                    }
                }
                else if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();

            for(size_t i = 0; i < systems.size(); ++i)
                systems[i].draw(window);
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