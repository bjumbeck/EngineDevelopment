#include "RenderSystem.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include "Entity/EntityManager.hpp"
#include "Components/Component.hpp"
#include "Components/RenderableComponent.hpp"
#include "Components/TransformableComponent.hpp"
#include "Components/SteeringComponent.hpp"


RenderSystem::RenderSystem(sf::RenderWindow* target)
    : renderWindow(target)
    , debugDraw(true)
{
}

void RenderSystem::Configure(EventManager& eventManager)
{}

void RenderSystem::Update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime)
{
    // Not needed for now.
}

void RenderSystem::Render(EntityManager& entityManager)
{
    ComponentPtr<RenderableComponent> renderComp;
    ComponentPtr<TransformableComponent> transComp;
    for (const Entity& entity : entityManager.getEntitiesWithComponents(renderComp, transComp))
    {
        sf::RenderStates states = sf::RenderStates::Default;

        states.transform *= transComp->getTransform();
        states.texture = &renderComp->texture;
        renderWindow->draw(renderComp->vertexArray, states);
    }

    // Debug information
    if (debugDraw) // TODO: This variable needs to be set through events
    {
        // Steering component debug drawing
        ComponentPtr<SteeringComponent> steeringComp;
        for (const Entity& entity : entityManager.getEntitiesWithComponents(renderComp, transComp, steeringComp))
        {
            if ((steeringComp->behaviorFlags & BehaviorType::Seek) == BehaviorType::Seek)
            {
                sf::Vertex line[] = { sf::Vertex(transComp->getPosition(), sf::Color::Red), sf::Vertex(steeringComp->seekTarget, sf::Color::Red) };
                renderWindow->draw(line, 2, sf::Lines);
            }

            if ((steeringComp->behaviorFlags & BehaviorType::Flee) == BehaviorType::Flee)
            {
                sf::Color drawColor;
                if ((transComp->getPosition().x - steeringComp->fleeTarget.x) > steeringComp->fleePanicDistance &&
                    ((transComp->getPosition().y - steeringComp->fleeTarget.y) > steeringComp->fleePanicDistance))
                {
                    drawColor = sf::Color::Green;
                }
                else
                {
                    drawColor = sf::Color::Red;
                }

                sf::Vertex line[] = { sf::Vertex(transComp->getPosition(), sf::Color::Red), sf::Vertex(steeringComp->fleeTarget, sf::Color::Red) };
                renderWindow->draw(line, 2, sf::Lines);
            }

            if ((steeringComp->behaviorFlags & BehaviorType::Arrive) == BehaviorType::Arrive)
            {
                sf::Vertex line[] = { sf::Vertex(transComp->getPosition(), sf::Color::Red), sf::Vertex(steeringComp->arrivePosition, sf::Color::Red) };
                renderWindow->draw(line, 2, sf::Lines);

                sf::CircleShape circle(4);
                circle.setFillColor(sf::Color::Red);
                circle.setPosition(steeringComp->arrivePosition);
                circle.setOrigin(circle.getRadius() / 2.0f, circle.getRadius() / 2.0f);
                renderWindow->draw(circle);
            }
        }
    }
}