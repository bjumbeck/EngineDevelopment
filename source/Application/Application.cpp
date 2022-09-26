#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <SFML/System/Clock.hpp>

#include "Application.hpp"
#include "Entity/Entity.hpp"

#include "Systems/RenderSystem.hpp"
#include "Systems/MovementSystem.hpp"

#include "Components/TransformableComponent.hpp"
#include "Components/RenderableComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/SteeringComponent.hpp"

#include "EventManagement/EventManager.hpp"
#include "Entity/EntityManager.hpp"
#include "ResourceManagement/ResourceCache.hpp"
#include "ResourceManagement/ResourceContainers.hpp"
#include "ResourceManagement/ResourceHandle.hpp"
#include "Systems/SystemManager.hpp"

const sf::Time Application::timePerFrame = sf::seconds(1.0f / 60.0f);

Application::~Application() = default;

Application::Application()
    : window(sf::VideoMode(1920, 1080), "Testing Grounds", sf::Style::Close)
    , resourceCache(std::make_unique<ResourceCache>(10, new ZipResourceContainer("Assets.zip"))) // Cache will take ownership
    , eventManager(std::make_unique<EventManager>())
    , entityManager(std::make_unique<EntityManager>(*eventManager))
    , systemManager(std::make_unique<SystemManager>(*entityManager, *eventManager))
{
    // Initialize the resource cache
    if (!resourceCache->Initialize())
    {
        // TODO: Move all this initialization outside of the constructor, it's limiting how errors can be handled.
    }

    //=============================================================
    // Testing Code / Feature Demoing
    //=============================================================

    // Entities represent primarily just a unique ID and that is it.
    // All components are housed in their own separate memory pools 
    // (Based on component families) inside of EntityManager.
    Entity aiEntity = entityManager->createEntity();
    Entity playerEntity = entityManager->createEntity();

    // Test 1: Resource Loading - Able to successfully load a png sprite image from our zip file?

    // NOTE: This is very sloppy and is wasting RAM by having to load the asset into memory with the cache and resource handle, then copy that same data over into a
    // sf::Texture which we then carry around in places like RenderableComponent. Though I am planning to ditch using SFML for rendering in the near future and will have to rewrite all
    // rendering code to use pure OpenGL, DirectX or maybe even Vulkan. So will leave it as it is right now.
    auto testSpriteHandle = resourceCache->GetResourceHandle("Textures/TestSprite.png");
    sf::Texture testSpriteTexture;
    testSpriteTexture.loadFromMemory(testSpriteHandle->GetBuffer(), testSpriteHandle->GetSize());

    // Test 2: Rendering - By adding the Transformable and Renderable components to an entity will it be able to be drawn correctly?
    sf::Vector2f origin(testSpriteTexture.getSize().x / 2.0f, testSpriteTexture.getSize().y / 2.0f);
    entityManager->assignComponent<TransformableComponent>(aiEntity.id(), sf::Vector2f(100.0f, 100.0f), 0, sf::Vector2f(1.0f, 1.0f), origin);
    entityManager->assignComponent<RenderableComponent>(aiEntity.id(), testSpriteHandle);

    // Test 3: Movement - By adding the Movement component to an entity will it allow for either Dynamic Movement either through
    // an AI controller or a player controller?
    // Params: Mass, Max Velocity, Max Force, Max Turn Rate
    entityManager->assignComponent<MovementComponent>(aiEntity.id(), 1.0f, 50.0f, 10.0f, 10.0f);

    // Test 3.1: Basic AI (Entity Steering) - Adding the Steering component will use steering behaviors to move an entity around with dynamic movement?
    ComponentPtr<SteeringComponent> steering = entityManager->assignComponent<SteeringComponent>(aiEntity.id());
    steering->behaviorFlags |= BehaviorType::Seek;
    steering->seekTarget = sf::Vector2f(400, 400);


    // Feature TODO: Setup GUI for the engine using ImGui, though since there isn't a supported backend for sfml this will require
    // either 1) Writing a new SFML backend for ImGUI that uses both SFML rendering and windowing 2) Switching rendering from SFML to OpenGL
    // and use the ImGUI OpenGL backend. #1 isn't ideal since it would require constantly keeping our code synced and updated whenever new changes
    // roll out. Whereas if we go with #2 we can piggyback on the ImGUI implementation that they keep up to date and only have to interface it with
    // SFML windowing which shouldn't require maintence since we can hook into the opengl render target directly.

    //=============================================================
    // END Testing Code / Feature Demoing
    //=============================================================

    setupSystems();
}

void Application::runApplication()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (window.isOpen())
    {
        sf::Time deltaTime = clock.restart();
        timeSinceLastUpdate += deltaTime;

        while (timeSinceLastUpdate >= timePerFrame)
        {
            timeSinceLastUpdate -= timePerFrame;

            processSFMLEvents();
            updateFrame(timePerFrame);
        }

        renderFrame();
    }
}

void Application::setupSystems()
{
    systemManager->addSystem<RenderSystem>(window);
    systemManager->addSystem<MovementSystem>();

    systemManager->configure();
}

void Application::processSFMLEvents()
{
    sf::Event sfmlEvent;
    while (window.pollEvent(sfmlEvent))
    {
        switch (sfmlEvent.type)
        {
            case sf::Event::Closed:
            {
                window.close();
                break;
            }

            default:
            {
                break;
            }
        }
    }
}

void Application::updateFrame(const sf::Time& deltaTime)
{
    systemManager->updateAllSystems(deltaTime);
}

void Application::renderFrame()
{
    window.clear(sf::Color::Black);
    systemManager->getSystem<RenderSystem>()->render(*entityManager);
    window.display();
}