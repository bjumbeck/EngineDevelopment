#pragma once

#include <cstdint>
#include <SFML/Graphics/RenderWindow.hpp>

class IApplication
{
	public:
		enum class InitResults : uint8_t
		{
			SUCCESS = 0,
			FAILURE = 1
		};

		IApplication() = default;
		virtual ~IApplication() = default;

		// TODO:Brandon Think on whether or not intialization and running the application should be handled
		// in a single method inside of the classes, or if seperate methods and using main.cpp would be more 
		// flexible.
		virtual InitResults InitializeApplication() = 0;
		virtual int RunApplication() = 0;
		virtual void UpdateApplication(const sf::Time& deltaTime) = 0;
		virtual void RenderApplication() = 0;

	private:
		void operator=(const IApplication&) = delete;
};