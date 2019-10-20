////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2019 SANOU A. K. Landry
////////////////////////////////////////////////////////////
#ifndef ADVANCEDCAMERA_H
#define ADVANCEDCAMERA_H
///////////////////////////HEADERS//////////////////////////
//NERO
#include <Nero/core/camera/Camera.h>
//SFML
#include <SFML/Graphics/RectangleShape.hpp>
//SFGUI
////////////////////////////////////////////////////////////
namespace nero
{
    class AdvancedCamera : public Camera
    {
        public:
            typedef std::shared_ptr<AdvancedCamera>    Ptr;

        public:
									AdvancedCamera(const sf::Vector2f& viewSize, const sf::Vector2f& defaultPos = sf::Vector2f(0.f, 0.f), const float& panSpeed = 300.f, const float& rotSpeed = 0.5f, const float& zRatio = 0.99f);
            virtual                ~AdvancedCamera();

            void                    update(const sf::Time& deltaTime);
            void                    render();
            void                    handleKeyboardInput(const sf::Keyboard::Key& key, const bool& isPressed);
            void                    handleMouseWheelInput(const sf::Event::MouseWheelScrollEvent& mouse);
            void                    cancelAction();

        private:
            bool                    m_IsPanningUp;
            bool                    m_IsPanningDown;
            bool                    m_IsPanningLeft;
            bool                    m_IsPanningRight;
            bool                    m_IsRotatingLeft;
            bool                    m_IsRotatingRight;
            bool                    m_IsZoomingIn;
            bool                    m_IsZoomingOut;
            sf::RectangleShape      m_xAxis;
            sf::RectangleShape      m_yAxis;
    };
}
#endif // ADVANCEDCAMERA_H
