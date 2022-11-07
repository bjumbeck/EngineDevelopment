#include "MovementSystem.hpp"
#include "Math/VectorMath.hpp"
#include "Components/Component.hpp"
#include "Components/TransformableComponent.hpp"
#include "Components/MovementComponent.hpp"

void MovementSystem::Configure(EventManager& eventManager)
{}

void MovementSystem::Update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime)
{
    ComponentPtr<MovementComponent> movementComp;
    ComponentPtr<TransformableComponent> transComp;
    for (Entity entity : entityManager.getEntitiesWithComponents(transComp, movementComp)) // TODO: Consider changing the value returned by getEntitiesWithComponents to reference
    {
        // First handle entities that have a steering component
        if (entityManager.hasComponent<SteeringComponent>(entity.id()))
        {
            sf::Vector2f steeringForce = calculateSteering(entity, entityManager);
            sf::Vector2f acceleration = steeringForce / movementComp->mass;

            movementComp->velocity += acceleration * deltaTime.asSeconds();
        }

        // Limit all entities to their max velocity
        if (Length(movementComp->velocity) > movementComp->maxSpeed)
        {
            movementComp->velocity = UnitVector(movementComp->velocity) * movementComp->maxSpeed;
        }

        // Update our heading and side vectors. Only support headings that are
        // the same as the velocity at the moment.
        if (SquaredLength(movementComp->velocity) > 0.00000001)
        {
            movementComp->heading = UnitVector(movementComp->velocity);
            movementComp->side = PerpendicularVector(movementComp->heading);
        }

        // Finally apply the movement to the entity position
        transComp->move(movementComp->velocity * deltaTime.asSeconds());
    }
}

sf::Vector2f MovementSystem::calculateSteering(Entity& entity, EntityManager& entityManager)
{
    sf::Vector2f steeringForce;

    ComponentPtr<SteeringComponent> steeringComp = entityManager.getComponent<SteeringComponent>(entity.id());
    ComponentPtr<MovementComponent> movementComp = entityManager.getComponent<MovementComponent>(entity.id());
    ComponentPtr<TransformableComponent> transComp = entityManager.getComponent<TransformableComponent>(entity.id());

    if ((steeringComp->behaviorFlags & BehaviorType::Seek) == BehaviorType::Seek)
    {
        steeringForce += seekBehavior(steeringComp, movementComp, transComp);
    }

    if ((steeringComp->behaviorFlags & BehaviorType::Flee) == BehaviorType::Flee)
    {
        steeringForce += fleeBehavior(steeringComp, movementComp, transComp);
    }

    if ((steeringComp->behaviorFlags & BehaviorType::Arrive) == BehaviorType::Arrive)
    {
        steeringForce += arriveBehavior(steeringComp, movementComp, transComp);
    }

    return steeringForce;
}

sf::Vector2f MovementSystem::seekBehavior(const ComponentPtr<SteeringComponent>& steering,
                                          const ComponentPtr<MovementComponent>& movement,
                                          const ComponentPtr<TransformableComponent>& transform)
{
    sf::Vector2f desiredVelocity = UnitVector(steering->seekTarget - transform->getPosition()) * movement->maxSpeed;

    return desiredVelocity - movement->velocity;
}

sf::Vector2f MovementSystem::fleeBehavior(const ComponentPtr<SteeringComponent>& steering,
                                          const ComponentPtr<MovementComponent>& movement,
                                          const ComponentPtr<TransformableComponent>& transform)
{
    sf::Vector2f desiredVelocity;

    const float fleeDistanceSq = std::pow(steering->fleePanicDistance, 2);
    if (DistanceSquared(transform->getPosition(), steering->fleeTarget) <= fleeDistanceSq)
    {
        desiredVelocity = UnitVector(transform->getPosition() - steering->fleeTarget) * movement->maxSpeed;
    }

    return desiredVelocity - movement->velocity;
}

sf::Vector2f MovementSystem::arriveBehavior(const ComponentPtr<SteeringComponent>& steering,
                                            ComponentPtr<MovementComponent>& movement,
                                            const ComponentPtr<TransformableComponent>& transform)
{
    sf::Vector2f desiredVelocity = sf::Vector2f(0.0f, 0.0f);
    sf::Vector2f toTarget = steering->arrivePosition - transform->getPosition();
    float distance = Length(toTarget);

    // FIXME: This is a hack to make it correctly stop when it is 
    // close to the target position instead of stuttering back and forth.
    // Need to fix the math probably to remove this.
    if (distance > 0.75)
    {
        // Tweak this to play around with the deceleration speeds.
        const float decelerationTweaker = 0.3f;

        float speed = distance / (static_cast<float>(steering->arriveDeceleration) * decelerationTweaker);
        speed = std::min(speed, movement->maxSpeed);

        desiredVelocity = toTarget * speed / distance;
    }
    else
    {
        // Here is the ugly hack
        movement->velocity = sf::Vector2f(0.0f, 0.0f);
    }

    return desiredVelocity - movement->velocity;
}