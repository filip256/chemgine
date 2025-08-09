#pragma once

#include "labware/Lab.hpp"
#include "labware/kinds/Flask.hpp"
#include "io/Input.hpp"
#include "labware/kinds/Adaptor.hpp"
#include "labware/kinds/Heatsource.hpp"
#include "labware/kinds/Condenser.hpp"
#include "graphics/PropertyPane.hpp"
#include "graphics/particles/Vapour.hpp"
#include "data/def/Parsers.hpp"
#include "helpers/DragNDropHelper.hpp"
#include "helpers/CursorHelper.hpp"

#include <SFML/Graphics.hpp>

class UIContext
{
private:
	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(1800, 1000), "Chemgine");
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
        if (not font.loadFromFile("./fonts/font.ttf"))
            Log(this).fatal("Failed to load font.");

        sf::Text textFPS("FPS:", font, 18);
        textFPS.setPosition(sf::Vector2f(5.0f, window.getSize().y - 22.0f));

        sf::Text textTime("T=", font, 18);
        textTime.setPosition(sf::Vector2f(window.getSize().x - 200.0f, window.getSize().y - 22.0f));

        bool isInTimeSetMode = false;
        float_s timeMultiplier = 1.0;
        sf::Text textTimeMult("x" + std::to_string(timeMultiplier).substr(0, 4), font, 18);
        textTimeMult.setPosition(sf::Vector2f(window.getSize().x - 50.0f, window.getSize().y - 22.0f));

        bool callRemoveEmptySystems = false;

        InHand inHand;
        DragNDropHelper dndHelper;

        auto& flask1 = lab.add<Flask>(201);
        lab.add<Adaptor>(301);
        lab.add<Adaptor>(302);
        //auto& flask2 = lab.add<Flask>(201);
        lab.add<Adaptor>(301);
        lab.add<Adaptor>(302);
        lab.add<Heatsource>(401);
        lab.add<Heatsource>(401);
        lab.add<Condenser>(501);
        lab.add<Flask>(201);
        lab.add<Flask>(201);

        CursorHelper cursorHelper(window, sf::Cursor::Type::Cross);

        std::optional<std::pair<Molecule, Amount<Unit::MOLE>>> inputMolecule = std::nullopt;

        flask1.add(Molecule("CC(=O)O"), 4.0_mol);
        //flask2.add(Molecule("O"), 10.0_mol);

        for(size_t i = 0; i < lab.getSystemCount(); ++i)
            lab.getSystem(i).move(sf::Vector2f(100.0f + 85.0f * static_cast<float>(i), 100.0f));

        //Vapour vapour(50, sf::Vector2f(500.0f, 500.0f), sf::Color(255, 255, 255, 10), 0.0_o, 0.7f, 0.5f);

        //window.setFramerateLimit(300);
        window.setVerticalSyncEnabled(true);
        size_t frameCount = 0;
        sf::Clock frameClock, tickClock;
        sf::Time gameTime, lastLabTick, lastEnvTick, lastFrame;
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
                        if (inputMolecule)
                        {
                            if (const auto [sys, comp] = lab.getSystemComponentAt(mousePos); sys != Lab::npos)
                            {
                                auto& component = lab.getSystem(sys).getComponent(comp);
                                if (auto container = component.cast<BaseContainerComponent>())
                                {
                                    container->add(inputMolecule->first, inputMolecule->second);
                                    Log(this).info("Added {0} of {1}.",
                                        inputMolecule->second.toString(), inputMolecule->first.getData().name);
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
                    if (isInTimeSetMode)
                    {
                        timeMultiplier += event.mouseWheel.delta * 0.05f * std::max(timeMultiplier, 1.0f);
                        timeMultiplier = std::max(timeMultiplier, 0.05f);
                        timeMultiplier = std::min(timeMultiplier, 50.0f);

                        textTimeMult.setString("x" + std::to_string(timeMultiplier).substr(0, 4));
                    }
                    else if (inHand.isSet())
                    {
                        lab.getSystem(inHand.idx).rotate(event.mouseWheel.delta * 2.0f);
                    }
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
                    else if (event.key.code == sf::Keyboard::Key::T)
                    {
                        isInTimeSetMode = true;
                    }
                }
                else if (event.type == sf::Event::KeyReleased)
                {
                    if (event.key.code == sf::Keyboard::Key::I)
                    {
                        const auto input = Input::get("Input Molecule   [SMILES]_[moles]");
                        const auto temp = def::parse<std::pair<Molecule, Amount<Unit::MOLE>>>(input, '_');

                        if (not temp)
                        {
                            Log(this).error("Malformed input ignored: {0}.", input);
                            inputMolecule.reset();
                            cursorHelper.setType(sf::Cursor::Type::Cross);
                            continue;
                        }

                        inputMolecule.emplace(*temp);
                        cursorHelper.setType(sf::Cursor::Type::Hand);
                    }
                    else if (event.key.code == sf::Keyboard::Key::P)
                    {
                        inputMolecule.reset();
                        cursorHelper.setType(sf::Cursor::Type::Cross);
                    }
                    else if (event.key.code == sf::Keyboard::Key::LControl)
                    {
                        drawPropertyPane = false;
                    }
                    else if (event.key.code == sf::Keyboard::Key::T)
                    {
                        isInTimeSetMode = false;
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

            auto cTime = tickClock.getElapsedTime();
            if (const auto timespan = (cTime - lastEnvTick).asSeconds(); timespan >= 0.01)
            {
                //vapour.tick(timespan * timeMultiplier);

                textTime.setString("T= " + Amount<Unit::SECOND>(gameTime.asMilliseconds() / 1000.0f).format());

                lastEnvTick = cTime;
            }

            cTime = tickClock.getElapsedTime();
            if (const auto timespan = (cTime - lastLabTick).asSeconds(); timespan >= 0.05)
            {
                lab.tick(timespan * timeMultiplier);
                lastLabTick = cTime;
            }

            cTime = tickClock.getElapsedTime();
            gameTime += sf::microseconds(round_cast<sf::Int64>((cTime - lastFrame).asMicroseconds() * timeMultiplier));
            lastFrame = cTime;

            window.clear();
            window.draw(lab);
            //window.draw(vapour);
            if (drawPropertyPane)
                window.draw(propertyPane);
            window.draw(textFPS);
            window.draw(textTime);
            window.draw(textTimeMult);
            window.display();

            if ((frameCount & 3) == 0)
            {
                textFPS.setString("FPS:" + std::to_string(static_cast<int>(
                    4000000.0f / frameClock.getElapsedTime().asMicroseconds()
                    )));
                frameClock.restart();
            }

            ++frameCount;
        }
	}
};
