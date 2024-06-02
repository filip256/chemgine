#pragma once

#include "SFML/Graphics.hpp"
#include "Lab.hpp"
#include "Flask.hpp"
#include "Logger.hpp"
#include "Adaptor.hpp"
#include "Heatsource.hpp"
#include "Condenser.hpp"
#include "PropertyPane.hpp"
#include "Vapour.hpp"
#include "DataHelpers.hpp"
#include "DragNDropHelper.hpp"
#include "CursorHelper.hpp"

class UIContext
{
private:
	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(1200, 800), "Chemgine");
    Lab lab;

    sf::Font font;

    bool drawPropertyPane = false;
    MixturePropertyPane propertyPane = MixturePropertyPane(font);

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

public:

	void run()
	{
        Logger::enterContext();

        font.loadFromFile("./Fonts/font.ttf");

        sf::Text text("FPS:", font, 18);
        text.setPosition(sf::Vector2f(5.0f, window.getSize().y - 22.0f));

        bool callRemoveEmptySystems = false;

        InHand inHand;
        DragNDropHelper dndHelper;

        auto& flask1 = lab.add<Flask>(201);
        lab.add<Adaptor>(301);
        lab.add<Adaptor>(302);
        auto& flask2 = lab.add<Flask>(201);
        lab.add<Adaptor>(301);
        lab.add<Adaptor>(302);
        lab.add<Heatsource>(401);
        lab.add<Heatsource>(401);
        lab.add<Condenser>(501);

        CursorHelper cursorHelper(window, sf::Cursor::Type::Cross);

        std::optional<std::pair<Molecule, Amount<Unit::MOLE>>> inputMolecule = std::nullopt;

        flask1.add(Molecule("CC(=O)O"), 4.0_mol);
        flask2.add(Molecule("O"), 10.0_mol);

        for(size_t i = 0; i < lab.getSystemCount(); ++i)
            lab.getSystem(i).move(sf::Vector2f(100 + 75 * i, 50));

        Vapour vapour(50, sf::Vector2f(500.0f, 500.0f), sf::Color(255, 255, 255, 10), 0.0_o, 0.7f, 0.5f);

        //window.setFramerateLimit(300);
        window.setVerticalSyncEnabled(true);
        size_t frameCount = 0;
        sf::Clock frameClock, tickClock;
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
                        auto& inHandSys = lab.getSystem(inHand.idx);
                        inHandSys.move(delta);
                        if (lab.anyIntersects(inHand.idx) != Lab::npos)
                        {
                            inHandSys.move(-delta);
                            sf::Mouse::setPosition(static_cast<sf::Vector2i>(mousePos - delta), window);
                        }
                        dndHelper.resetOrigin(mousePos);
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        if (const auto sys = lab.getSystemAt(mousePos); sys != Lab::npos)
                        {
                            inHand.set(sys);
                            dndHelper.start(mousePos);
                        }
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        if (inputMolecule.has_value())
                        {
                            if (const auto [sys, comp] = lab.getSystemComponentAt(mousePos); sys != Lab::npos)
                            {
                                auto& component = lab.getSystem(sys).getComponent(comp);
                                if (auto container = component.cast<BaseContainerComponent>())
                                {
                                    container->add(inputMolecule->first, inputMolecule->second);
                                    Logger::log("Added " + inputMolecule->second.toString() + " of " + inputMolecule->first.data().name, LogType::INFO);
                                }
                            }
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        if (inHand.isSet())
                        {
                            callRemoveEmptySystems |= lab.tryConnect(inHand.idx, 1600.f);

                            inHand.unset();
                            dndHelper.end();
                        }
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        lab.tryDissconnect(mousePos);
                    }
                }
                else if (event.type == sf::Event::MouseWheelMoved)
                {
                    if (inHand.isSet())
                        lab.getSystem(inHand.idx).rotate(event.mouseWheel.delta * 2);
                }
                else if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Key::LControl)
                    {
                        if (const auto sys = lab.getSystemComponentAt(mousePos); sys.first != Lab::npos)
                        {
                            const auto& comp = lab.getSystem(sys.first).getComponent(sys.second);
                            propertyPane.setSubject(comp);
                            drawPropertyPane = true;
                        }
                    }
                }
                else if (event.type == sf::Event::KeyReleased)
                {
                    if (event.key.code == sf::Keyboard::Key::I)
                    {
                        const auto input = Logger::input("Input Molecule   [SMILES]@[moles]");
                        const auto temp = DataHelpers::parsePair<Molecule, Unit::MOLE>(input);

                        if (temp.has_value() == false)
                        {
                            Logger::log("Malformed input ignored: " + input, LogType::BAD);
                            inputMolecule.reset();
                            cursorHelper.setType(sf::Cursor::Type::Cross);
                            continue;
                        }

                        inputMolecule.emplace(*temp);
                        cursorHelper.setType(sf::Cursor::Type::Hand);
                    }
                    else if (event.key.code == sf::Keyboard::Key::LControl)
                    {
                        drawPropertyPane = false;
                    }
                }

                else if (event.type == sf::Event::Closed)
                    window.close();
            }

            if (callRemoveEmptySystems)
            {
                lab.removeEmptySystems();
                callRemoveEmptySystems = false;
            }

            if (const auto timespan = tickClock.getElapsedTime().asSeconds(); timespan >= 0.05)
            {
                //flask2.add(100.0_J);
                lab.tick(timespan);
                vapour.tick(timespan);
                tickClock.restart();
            }

            window.clear();
            window.draw(lab);
            window.draw(vapour);
            if (drawPropertyPane)
                window.draw(propertyPane);
            window.draw(text);
            window.display();

            if ((frameCount & 3) == 0)
            {
                text.setString("FPS:" + std::to_string(static_cast<int>(
                    4000000.0f / frameClock.getElapsedTime().asMicroseconds()
                    )));
                frameClock.restart();
            }

            ++frameCount;
        }

        Logger::exitContext();
	}
};
