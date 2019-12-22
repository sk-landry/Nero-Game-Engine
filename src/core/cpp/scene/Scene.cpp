////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2020 SANOU A. K. Landry
/////////////////////////////////////////////////////////////
#include <Nero/core/cpp/scene/Scene.h>
#include <Nero/core/cpp/object/GameLevelObject.h>

namespace nero
{
	Scene::Context::Context(std::string sceneName, RenderTexturePtr renderTexture, ResourceManager::Ptr resourceManager, Camera::Ptr camera, Setting::Ptr setting, EngineType engineType, PlatformType platformType):
		 sceneName(sceneName)
		,renderTexture(renderTexture)
		,resourceManager(resourceManager)
		,camera(camera)
		,setting(setting)
		,engineType(engineType)
		,platformType(platformType)

	{

    }

	Scene::Context::Context()
	{

	}

	Scene::Scene(Context context):
		 m_SceneContext(context)
		,m_GameWorld(std::make_shared<Object>())
		,m_PhysicWorld(nullptr)
		,m_ShapeRenderer(context.renderTexture)
		,m_HideWorld(false)
		,m_LevelSetting(nullptr)
		,m_LightEngine(true)
	{
		//setupLightning();
	}

	Scene::~Scene()
    {
		destroy();
    }

	void Scene::destroy()
	{
		//destroy physic world
		m_PhysicWorld = nullptr;
		//delete m_PhysicWorld;
	}

	void Scene::handleEvent(const sf::Event& event)
	{
		switch(event.type)
		{
			//keyboard events
			case sf::Event::KeyPressed:
				handleKeyboardInput(event.key.code, true);
				break;
			case sf::Event::KeyReleased:
				handleKeyboardInput(event.key.code, false);
				break;

			//mouse buttons events
			case sf::Event::MouseButtonPressed:
				handleMouseButtonInput(event.mouseButton, true);
				break;
			case sf::Event::MouseButtonReleased:
				handleMouseButtonInput(event.mouseButton, false);
				break;

			//mouse move event
			case sf::Event::MouseMoved:
				handleMouseMoveInput(event.mouseMove);
				break;

			 //mouse wheel
			case sf::Event::MouseWheelScrolled:
				handleMouseWheelInput(event.mouseWheelScroll);
				break;
		}
	}

	void Scene::update(const sf::Time& timeStep)
	{
		float32 b2TimeStep = m_LevelSetting->getFloat("time_step") > 0.0f ? 1.0f / m_LevelSetting->getFloat("time_step") : float32(0.0f);

		if(b2TimeStep > 0.f)
		{
			b2TimeStep = (b2TimeStep * timeStep.asSeconds())/TIME_PER_FRAME.asSeconds();
		}

		if(!m_HideWorld)
		{
			if(!m_PhysicWorld->IsLocked())
			{
				//m_ObjectManager->removeDeadPhysicObject();
			}

			if(m_LevelSetting->getBool("pause_level") && !m_LevelSetting->getBool("single_step"))
			{
				b2TimeStep = 0.0f;
				m_SceneContext.engineType == EngineType::RENDER_ENGINE ? m_InformationContent = StringPool.BLANK : m_InformationContent = "#-- PAUSED --#";
			}
			else
			{
				m_InformationContent = StringPool.BLANK;
			}

			uint32 flags = 0;
			flags += m_LevelSetting->getUInt("draw_shape") * b2Draw::e_shapeBit;
			flags += m_LevelSetting->getUInt("draw_joint") * b2Draw::e_jointBit;
			flags += m_LevelSetting->getUInt("draw_aabb") * b2Draw::e_aabbBit;
			flags += m_LevelSetting->getUInt("draw_center_of_mass") * b2Draw::e_centerOfMassBit;

			m_ShapeRenderer.SetFlags(flags);

			m_PhysicWorld->SetAllowSleeping(m_LevelSetting->getBool("enable_sleep"));
			m_PhysicWorld->SetWarmStarting(m_LevelSetting->getBool("enable_warm_starting"));
			m_PhysicWorld->SetContinuousPhysics(m_LevelSetting->getBool("enable_sub_stepping"));
			m_PhysicWorld->SetSubStepping(m_LevelSetting->getBool("enable_sub_stepping"));

			m_ContactPointCount = 0;

			m_PhysicWorld->Step(b2TimeStep, m_LevelSetting->getInt("velocity_iteration"), m_LevelSetting->getInt("position_iteration"));


			if(!m_LevelSetting->getBool("pause_level") || m_LevelSetting->getBool("single_step"))
			{
				m_GameWorld->update(sf::seconds(b2TimeStep));
			}

			m_InformationText.setString(m_InformationContent);
		}
	}

	void Scene::render()
	{
		auto gameLevel = m_GameWorld->getAllChild()->front();

		auto chunkTable = gameLevel->getAllChild();

		for(auto it = chunkTable->begin(); it != chunkTable->end(); it++)
		{
			auto childTable = (*it)->getAllChild();

			for(auto it = childTable->begin(); it != childTable->end(); it++)
			{
				LayerObject::Ptr layer_object = LayerObject::Cast(*it);

				if(layer_object->isVisible())
					m_SceneContext.renderTexture->draw(*layer_object);
			}
		}
	}

	void Scene::renderShape()
	{
		m_PhysicWorld->DrawDebugData();
	}

	void Scene::renderFrontScreen()
	{

	}

	void Scene::init()
	{
	   //empty
	}

	void Scene::handleKeyboardInput(const sf::Keyboard::Key& key, const bool& isPressed)
	{
	   //empty
	}

	void Scene::handleMouseButtonInput(const sf::Event::MouseButtonEvent& mouse, const bool& isPressed)
	{
	   //empty
	}

	void Scene::handleMouseMoveInput(const sf::Event::MouseMoveEvent& mouse)
	{
	   //empty
	}

	void Scene::handleMouseWheelInput(const sf::Event::MouseWheelScrollEvent& mouse)
	{
	   //empty
	}

	void Scene::handleCollisionContactBegin(Collision collision)
	{
	   //empty
	}


	void Scene::handleCollisionContactEnd(Collision collision)
	{
	   //empty
	}

	void Scene::handleCollisionPreSolveContact(Collision collision)
	{
	   //empty
	}


	void Scene::handleCollisionPostSolveContact(Collision collision)
	{
	   //empty
	}


	void Scene::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		return;

		const b2Manifold* manifold = contact->GetManifold();

		if (manifold->pointCount == 0)
		{
		  return;
		}

		  b2Fixture* fixtureA = contact->GetFixtureA();
		  b2Fixture* fixtureB = contact->GetFixtureB();

		  b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
		  b2GetPointStates(state1, state2, oldManifold, manifold);

		  b2WorldManifold worldManifold;
		  contact->GetWorldManifold(&worldManifold);

		  for (int32 i = 0; i < manifold->pointCount && m_ContactPointCount < MAX_CONTACT_POINT; ++i)
		  {
			  ContactPoint* cp = m_ContactPointTable + m_ContactPointCount;
			  cp->fixtureA = fixtureA;
			  cp->fixtureB = fixtureB;
			  cp->position = worldManifold.points[i];
			  cp->normal = worldManifold.normal;
			  cp->state = state2[i];
			  cp->normalImpulse = manifold->points[i].normalImpulse;
			  cp->tangentImpulse = manifold->points[i].tangentImpulse;
			  cp->separation = worldManifold.separations[i];
			  ++m_ContactPointCount;
		  }

		  Collision collision(contact, oldManifold, nullptr);

		  int id_A = (int)contact->GetFixtureA()->GetBody()->GetUserData();
		  int id_B = (int)contact->GetFixtureB()->GetBody()->GetUserData();

		  auto objectA = PhysicObject::Cast(m_ObjectManager->findObject(id_A));
		  auto objectB = PhysicObject::Cast(m_ObjectManager->findObject(id_B));

		  if(!objectA || !objectB)
			  return;

		  if(objectA->isDead() || objectB->isDead())
			  return;

		  collision.setObjectA(objectA);
		  collision.setObjectB(objectB);

		  handleCollisionPreSolveContact(collision);
	}


	  void Scene::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	  {
		  return;

		  Collision collision(contact, nullptr, impulse);

		  int id_A = (int)contact->GetFixtureA()->GetBody()->GetUserData();
		  int id_B = (int)contact->GetFixtureB()->GetBody()->GetUserData();

		  auto objectA = PhysicObject::Cast(m_ObjectManager->findObject(id_A));
		  auto objectB = PhysicObject::Cast(m_ObjectManager->findObject(id_B));

		  if(!objectA || !objectB)
			  return;

		  if(objectA->isDead() || objectB->isDead())
			  return;

		  collision.setObjectA(objectA);
		  collision.setObjectB(objectB);

		  handleCollisionPostSolveContact(collision);
	  }


	  void Scene::BeginContact(b2Contact* contact)
	  {
		  return;

		  Collision collision(contact, nullptr, nullptr);

		  int id_A = (int)contact->GetFixtureA()->GetBody()->GetUserData();
		  int id_B = (int)contact->GetFixtureB()->GetBody()->GetUserData();

		  auto objectA = PhysicObject::Cast(m_ObjectManager->findObject(id_A));
		  auto objectB = PhysicObject::Cast(m_ObjectManager->findObject(id_B));

		  if(!objectA || !objectB)
			  return;

		  if(objectA->isDead() || objectB->isDead())
			  return;

		  collision.setObjectA(objectA);
		  collision.setObjectB(objectB);

		  handleCollisionContactBegin(collision);
	  }

	  void Scene::EndContact(b2Contact* contact)
	  {
		  return;

		  Collision collision(contact, nullptr, nullptr);

		  int id_A = (int)contact->GetFixtureA()->GetBody()->GetUserData();
		  int id_B = (int)contact->GetFixtureB()->GetBody()->GetUserData();

		  auto objectA = PhysicObject::Cast(m_ObjectManager->findObject(id_A));
		  auto objectB = PhysicObject::Cast(m_ObjectManager->findObject(id_B));

		  if(!objectA || !objectB)
			  return;

		  if(objectA->isDead() || objectB->isDead())
			  return;

		  collision.setObjectA(objectA);
		  collision.setObjectB(objectB);

		  handleCollisionContactEnd(collision);
	  }

	  void Scene::pauseScene()
	  {

	  }

	  void Scene::resumeScene()
	  {

	  }

	  void Scene::resetScene()
	  {

	  }

	  void Scene::quitScene()
	  {

	  }

	  void Scene::hideGameWorld()
	  {

	  }

	  void Scene::showGameWorld()
	  {

	  }

	  void Scene::loadGameLevel(const std::string& name)
	  {
		  auto gameLevel = std::make_shared<GameLevelObject>();
		  Setting parameter;
		  gameLevel->initialize(parameter);


		  m_PhysicWorld = gameLevel->getPhysicWorld();

		  m_PhysicWorld->SetContactListener(this);
		  m_PhysicWorld->SetDebugDraw(&m_ShapeRenderer);
	  }

	  void Scene::loadWorldChunk(const std::string& name)
	  {

	  }

	  void Scene::unLoadGameLevel(const std::string& name)
	  {

	  }

	  void Scene::unLoadWorldChunk(const std::string& name)
	  {

	  }

	  void Scene::hideGameScreen(const std::string& name)
	  {

	  }

	  void Scene::showGameScreen(const std::string& name)
	  {

	  }

	  void Scene::enableObjectLayer(const std::string& name)
	  {

	  }

	  void Scene::disableObjectLayer(const std::string& name)
	  {

	  }

	  void Scene::enableGameChunk(const std::string& name)
	  {

	  }

	  void Scene::disableGameChunk(const std::string& name)
	  {

	  }

	  Scene::Context& Scene::getSceneContext()
	  {
		  return m_SceneContext;
	  }

	  Scene::RenderTexturePtr Scene::getRenderTexture()
	  {
			return m_SceneContext.renderTexture;
	  }

	  void Scene::setupLightning()
	  {

		  //Create the LightSystem
		  m_LightEngine.create({ -1000.f, -1000.f, 2000.f, 2000.f }, m_SceneContext.renderTexture->getSize());

		  // Load light texture
		  sf::Texture pointLightTexture;
		  pointLightTexture.loadFromFile("resource/starterpack/lightmap/pointLightTexture.png");
		  pointLightTexture.setSmooth(true);
		  m_TextureMap["point_light"] = pointLightTexture;

		  sf::Texture spookyLightTexture;
		  spookyLightTexture.loadFromFile("resource/starterpack/lightmap/spookyLightTexture.png");
		  spookyLightTexture.setSmooth(true);
		  m_TextureMap["spooky_light"] = spookyLightTexture;

		  sf::Texture backgroundTexture;
		  backgroundTexture.loadFromFile("resource/starterpack/normalmap/background.png");
		  m_TextureMap["bacground"] = backgroundTexture;

		  sf::Texture backgroundTextureNormals;
		  backgroundTextureNormals.loadFromFile("resource/starterpack/normalmap/background_NORMALS.png");
		  m_TextureMap["bacground_normal"] = backgroundTexture;

		  sf::Texture headTexture;
		  headTexture.loadFromFile("resource/starterpack/normalmap/head.png");
		  m_TextureMap["head"] = headTexture;

		  sf::Texture headTextureNormals;
		  headTextureNormals.loadFromFile("resource/starterpack/normalmap/head_NORMALS.png");
		  m_TextureMap["head_normal"] = headTexture;

		  // Add a sun light
		  ltbl::LightDirectionEmission* sun = m_LightEngine.createLightDirectionEmission();
		  sun->setColor(sf::Color(255, 255, 255, 50));

		  // Add a light point
		  ltbl::LightPointEmission* mlight = m_LightEngine.createLightPointEmission();
		  mlight->setOrigin(sf::Vector2f(m_TextureMap["point_light"].getSize().x * 0.5f, m_TextureMap["point_light"].getSize().y * 0.5f));
		  mlight->setTexture(m_TextureMap["point_light"]);
		  mlight->setScale(3.f, 3.f);
		  mlight->setColor(sf::Color::White);

		  // Create a shape
		  std::vector<sf::RectangleShape> shapes;
		  sf::RectangleShape blocker;
		  blocker.setSize({ 200.f, 50.f });
		  blocker.setPosition({ 500.f, 300.f });
		  blocker.setFillColor(sf::Color::Red);
		  shapes.push_back(blocker);

		  // Create a light shape with the same shape
		  m_LightEngine.createLightShape(blocker);

		  /*ltbl::Sprite background;
		  background.setTexture(m_TextureMap["bacground"]);
		  background.setNormalsTexture(m_TextureMap["bacground_normal"]);
		  m_LightEngine.addSprite(background);

		  ltbl::Sprite head;
		  head.setTexture(m_TextureMap["head"]);
		  head.setNormalsTexture(m_TextureMap["head_normal"]);
		  head.setPosition(300.f, 200.f);
		  m_LightEngine.addSprite(head);*/
	  }

	  void Scene::renderLightning()
	  {
		  m_LightEngine.render(*m_SceneContext.renderTexture);
	  }
}
