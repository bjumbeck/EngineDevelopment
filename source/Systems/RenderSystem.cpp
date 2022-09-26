#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "RenderSystem.hpp"
#include "Entity/EntityManager.hpp"
#include "Components/Component.hpp"
#include "Components/RenderableComponent.hpp"
#include "Components/TransformableComponent.hpp"
#include "Components/SteeringComponent.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RenderStates.hpp"

RenderSystem::RenderSystem(sf::RenderTarget& target)
    : renderTarget(target)
{
    #ifndef NDEBUG
    debugDraw = true;
    #endif
}

void RenderSystem::configure(EventManager& eventManager)
{}

void RenderSystem::update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime)
{
    // Not needed for now.
}

void RenderSystem::render(EntityManager& entityManager)
{
    ComponentPtr<RenderableComponent> renderComp;
    ComponentPtr<TransformableComponent> transComp;
    for (const Entity& entity : entityManager.getEntitiesWithComponents(renderComp, transComp))
    {
        sf::RenderStates states = sf::RenderStates::Default;

        states.transform *= transComp->getTransform();
        states.texture = &renderComp->texture;
        renderTarget.draw(renderComp->vertexArray, states);
    }

    // Debug information
    #ifndef NDEBUG
    if (debugDraw)
    {
        ComponentPtr<SteeringComponent> steeringComp;
        for (const Entity& entity : entityManager.getEntitiesWithComponents(renderComp, transComp, steeringComp))
        {
            if ((steeringComp->behaviorFlags & BehaviorType::Seek) == BehaviorType::Seek)
            {
                sf::Vertex line[] = { sf::Vertex(transComp->getPosition(), sf::Color::Red), sf::Vertex(steeringComp->seekTarget, sf::Color::Red) };
                renderTarget.draw(line, 2, sf::Lines);
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
                renderTarget.draw(line, 2, sf::Lines);
            }

            if ((steeringComp->behaviorFlags & BehaviorType::Arrive) == BehaviorType::Arrive)
            {
                sf::Vertex line[] = { sf::Vertex(transComp->getPosition(), sf::Color::Red), sf::Vertex(steeringComp->arrivePosition, sf::Color::Red) };
                renderTarget.draw(line, 2, sf::Lines);

                sf::CircleShape circle(4);
                circle.setFillColor(sf::Color::Red);
                circle.setPosition(steeringComp->arrivePosition);
                circle.setOrigin(circle.getRadius() / 2.0f, circle.getRadius() / 2.0f);
                renderTarget.draw(circle);
            }
        }
    }
    #endif
}