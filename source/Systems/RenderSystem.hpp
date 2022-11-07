#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "System.hpp"

class RenderSystem : public System<RenderSystem>
{
    public:
        explicit RenderSystem(sf::RenderWindow* target);
        
        void Configure(EventManager& eventManager) override;
        void Update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime) override;

        void Render(EntityManager& entityManager);

    private:
        sf::RenderWindow* renderWindow;
        bool debugDraw;
};
