#pragma once

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "ResourceManagement/ResourceHandle.hpp"

// NOTE: Again this whole rendering design will be rewritten in the near future to remove SFML from the rendering pipeline
// and replace it with OpenGL, DirectX, or maybe even Vulkan. After experimenting with using a rendering component, I might have
// to move away from it also since there is some issues with the design that limits it though will see in the future.

struct RenderableComponent
{
    explicit RenderableComponent(const std::shared_ptr<ResourceHandle>& textureHandle);
    RenderableComponent(const std::shared_ptr<ResourceHandle>& textureHandle, const sf::FloatRect& textureRect);

    sf::VertexArray vertexArray;
    sf::Texture texture;
    sf::FloatRect textureRect;
};

inline RenderableComponent::RenderableComponent(const std::shared_ptr<ResourceHandle>& textureHandle)
    : vertexArray(sf::TriangleStrip, 4)
{
    if (!texture.loadFromMemory(textureHandle->GetBuffer(), textureHandle->GetSize()))
    {
        // TODO: Log failure to load the texture from the resource handle.
    }

    textureRect = sf::FloatRect(0.0f, 0.0f,
                                static_cast<float>(texture.getSize().x),
                                static_cast<float>(texture.getSize().y));

    vertexArray[0].position = sf::Vector2f(0.0f, 0.0f);
    vertexArray[1].position = sf::Vector2f(0.0f, textureRect.height);
    vertexArray[2].position = sf::Vector2f(textureRect.width, 0.0f);
    vertexArray[3].position = sf::Vector2f(textureRect.width, textureRect.height);

    const float left = textureRect.left;
    const float right = textureRect.left + textureRect.width;
    const float top = textureRect.top;
    const float bottom = textureRect.top + textureRect.height;

    vertexArray[0].texCoords = sf::Vector2f(left, top);
    vertexArray[1].texCoords = sf::Vector2f(left, bottom);
    vertexArray[2].texCoords = sf::Vector2f(right, top);
    vertexArray[3].texCoords = sf::Vector2f(right, bottom);
}

inline RenderableComponent::RenderableComponent(const std::shared_ptr<ResourceHandle>& textureHandle, const sf::FloatRect& textureRect)
        : vertexArray(sf::TriangleStrip, 4)
        , textureRect(textureRect)
{
    if (!texture.loadFromMemory(textureHandle->GetBuffer(), textureHandle->GetSize()))
    {
        // TODO: Log failure to load the texture from the resource handle.
    }

    vertexArray[0].position = sf::Vector2f(0.0f, 0.0f);
    vertexArray[1].position = sf::Vector2f(0.0f, textureRect.height);
    vertexArray[2].position = sf::Vector2f(textureRect.width, 0.0f);
    vertexArray[3].position = sf::Vector2f(textureRect.width, textureRect.height);

    const float left = textureRect.left;
    const float right = textureRect.left + textureRect.width;
    const float top = textureRect.top;
    const float bottom = textureRect.top + textureRect.height;

    vertexArray[0].texCoords = sf::Vector2f(left, top);
    vertexArray[1].texCoords = sf::Vector2f(left, bottom);
    vertexArray[2].texCoords = sf::Vector2f(right, top);
    vertexArray[3].texCoords = sf::Vector2f(right, bottom);
}
