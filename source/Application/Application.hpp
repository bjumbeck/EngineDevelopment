#pragma once

#include <memory>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class ResourceCache;
class EventManager;
class EntityManager;
class SystemManager;

class Application : private sf::NonCopyable
{
    public:
        ~Application();
        static Application& getInstance() { static Application instance; return instance; }
        Application(const Application&) = delete;
        void operator=(const Application&) = delete;

        void runApplication();
        const sf::RenderWindow& getWindow() const { return window; } 

    private:
        Application();

        void setupSystems();
        
        void processSFMLEvents();
        void updateFrame(const sf::Time& deltaTime);
        void renderFrame();

    private:
        static const sf::Time timePerFrame;

        sf::RenderWindow window;

        std::unique_ptr<ResourceCache> resourceCache;

        // Do not change the ordering of these
        // will mess up the constructor if you do.
        std::unique_ptr<EventManager> eventManager;
        std::unique_ptr<EntityManager> entityManager;
        std::unique_ptr<SystemManager> systemManager;
};