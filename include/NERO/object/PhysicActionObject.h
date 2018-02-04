////////////////////////////////////////////////////////////
//
// Nero Game Engine
// Author : SANOU A. K. Landry
//
// All rights reserved
//
////////////////////////////////////////////////////////////

#ifndef PHYSICACTIONOBJECT_H
#define PHYSICACTIONOBJECT_H

#include <NERO/object/PhysicObject.h>
#include <NERO/object/Action.h>

namespace nero
{
    class PhysicActionObject : public Object
    {
       public:
            PhysicActionObject();

            template<class T>
            void        registerAction(sf::String name);
            template<class T>
            void        registerAction(sf::String name, const T& action);


            void                dropAction(sf::String name);
            void                callAction(sf::String name);
            void                setObject(Object::Ptr object);
            PhysicObject::Ptr   getObject();

            private:
                virtual void        updateObject(sf::Time time_step);

        protected:
            PhysicObject::Ptr                           m_PhysicObject;
            sf::Time                                    m_TimeStep;
            std::map<sf::String, PhysicAction::Ptr>     m_ActionTable;
    };

    template <typename T>
    void PhysicActionObject::registerAction(sf::String name)
    {
        m_ActionTable[name] = PhysicAction::Ptr(new T());
    }

    template<class T>
    void PhysicActionObject::registerAction(sf::String name, const T& action)
    {
        m_ActionTable[name] = PhysicAction::Ptr(new T(action));
    }
}


#endif // PHYSICACTIONOBJECT_H
