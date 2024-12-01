#include "GuiSystem.hpp"

#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics/RenderWindow.hpp>


GuiSystem::GuiSystem(sf::RenderTarget* target)
    : renderTarget(target)
{

}

void GuiSystem::Configure(EventManager& eventManager)
{
}

void GuiSystem::Update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime)
{
    ImGui::SFML::Update(*dynamic_cast<sf::RenderWindow*>(renderTarget), deltaTime);
}

void GuiSystem::Render(EntityManager& entityManager, EventManager& eventManager)
{
    // This is all just testing code for IMGUI and AI Steering Behaviors
    ImGui::Begin("Testing ImGUI"); // begin window

    const char* steeringOptions[4] = {"None", "Seek", "Flee", "Arrive"};
    static int steeringIndex = 0;
    ImGui::Combo("AI Steering Behavior", &steeringIndex, steeringOptions, IM_ARRAYSIZE(steeringOptions));

    ImGui::ShowStackToolWindow();

    ImGui::End();
    ImGui::SFML::Render(*renderTarget);
}
