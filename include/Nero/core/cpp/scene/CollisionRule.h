////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2020 Sanou A. K. Landry
/////////////////////////////////////////////////////////////
#ifndef COLLISIONRULE_H
#define COLLISIONRULE_H
///////////////////////////HEADERS//////////////////////////
//NERO
#include <Nero/core/cpp/object/Object.h>
#include <Nero/core/cpp/object/PhysicObject.h>
//STD
#include <map>
#include <unordered_set>
#include <vector>
/////////////////////////////////////////////////////////////
namespace nero
{
    class CollisionRule
    {
        public:
                                CollisionRule();
            virtual            ~CollisionRule();

            void                canCollide(std::string categoryA, std::string categoryB);
            void                print();
            void                printBit();
            void                printCategoryTab();
            void                apply(Object::Ptr root);

            void                fullCollision(Object::Ptr root);
            void                noCollision(Object::Ptr root);

            PhysicObject::Rule  getCollisionData(std::string category);

        private:
            void                updateCategoryBit();
            void                updateCategoryMaskBit();

        private:
            std::map<std::string, std::unordered_set<std::string>>  m_CollisionMap;
            std::map<std::string, std::pair<int, int>>              m_CollisionRuleMap;
            std::vector<int>                                        m_CatecotryBitTab;
    };
}
#endif // COLLISIONRULE_H
