#pragma once

#include "System.hpp"

namespace sf
{
    class RenderTarget;
}

class RenderSystem : public System<RenderSystem>
{
    public:
        explicit RenderSystem(sf::RenderTarget& target);
        
        void configure(EventManager& eventManager) override;
        void update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime) override;

        void render(EntityManager& entityManager);

    private:
        sf::RenderTarget& renderTarget;
        
        #ifndef NDEBUG
        bool debugDraw;
        #endif
};