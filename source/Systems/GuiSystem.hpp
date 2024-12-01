#pragma once

#include "System.hpp"

namespace sf
{
    class RenderTarget;
}

class GuiSystem : public System<GuiSystem>
{
    public:
        explicit GuiSystem(sf::RenderTarget* target);

        void Configure(EventManager& eventManager) override;
        void Update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime) override;
        void Render(EntityManager& entityManager, EventManager& eventManager);

        void SetRenderTarget(sf::RenderTarget* newRenderTarget) { renderTarget = newRenderTarget; }

    private:
        sf::RenderTarget* renderTarget;
};
