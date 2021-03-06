////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2020 Sanou A. K. Landry
/////////////////////////////////////////////////////////////
///////////////////////////HEADERS///////////////////////////
//NERO
#include <Nero/editor/GameProject.h>
#include <Nero/core/cpp/utility/File.h>
#include <Nero/core/cpp/utility/DateTime.h>
#include <boost/dll.hpp>
#include <boost/algorithm/string.hpp>
#include <thread>
/////////////////////////////////////////////////////////////

namespace nero
{
	GameProject::GameProject()
    {

    }

	void GameProject::loadResource(const Parameter& parameter)
	{
		nero_log("loading resource");
		nero_log(m_EngineSetting->getSetting("resource").toString());
		m_ResourceManager = ResourceManager::Ptr(new ResourceManager(m_EngineSetting->getSetting("resource")));

		std::string resource_path = file::getPath({parameter.getString("project_directory"), "Resource"});

		if(parameter.getBool("startup_pack"))
		{
			m_ResourceManager->loadDirectory("resource/starterpack");
		}
		else
		{
			m_ResourceManager->getFontHolder()->loadFile("resource/starterpack/font/Sansation.ttf");
		}

		nero_log("loading project resource");
		nero_log(resource_path);
		m_ResourceManager->loadDirectory(resource_path);
		nero_log("load resource completed");
	}

	void GameProject::init(const Parameter& parameter)
    {
		m_ProjectParameter = parameter;
		m_ProjectParameter.setString("source_directory", file::getPath({m_ProjectParameter.getString("project_directory"), "Source", parameter.getString("project_name")}));
		m_ProjectParameter.setString("build_directory", file::getPath({m_ProjectParameter.getString("project_directory"), "Build"}));
		m_ProjectParameter.setString("cmake_file", file::getPath({m_ProjectParameter.getString("source_directory"), "CMakeLists"}, StringPool.EXT_TEXT));

		std::string library = string::formatString(m_ProjectParameter.getString("project_name"), string::Format::COMPACT_LOWER);

		std::string libraryFile			= file::getPath({m_ProjectParameter.getString("build_directory"), "libnerogame-" + library}, StringPool.EXT_DLL);
		std::string libraryFileCopy		= file::getPath({m_ProjectParameter.getString("build_directory"), "libnerogame-" + library + "-copy"}, StringPool.EXT_DLL);

		m_ProjectParameter.setString("library_file", libraryFile);
		m_ProjectParameter.setString("library_file_copy", libraryFileCopy);

		//nero_log(m_ProjectParameter.toString());

		/*m_AdvancedScene = std::make_shared<AdvancedScene>();
		m_AdvancedScene->setSetting(m_EngineSetting);
		m_AdvancedScene->setRenderTexture(m_RenderTexture);
		m_AdvancedScene->setResourceManager(m_ResourceManager);
		m_AdvancedScene->setRenderContext(m_RenderContext);
		m_AdvancedScene->setCamera(m_Camera);
		m_AdvancedScene->setProjectParameter(m_ProjectParameter);
		m_AdvancedScene->initialize();*/
    }

	ResourceManager::Ptr GameProject::getResourceManager()
	{
		return m_ResourceManager;
	}


    void GameProject::loadProject()
    {
		std::string levelDirectory = file::getPath({m_ProjectParameter.getString("project_directory"), "Scene", "Level"});
		std::experimental::filesystem::path directoryPath(levelDirectory);

		if(std::experimental::filesystem::is_empty(directoryPath))
		{
			return;
		}

		m_AdvancedScene->m_SelectedGameLevel = nullptr;
		m_AdvancedScene->m_SelectedWorldChunk = nullptr;
		m_AdvancedScene->m_SelectedWorldBuilder = nullptr;
		m_AdvancedScene->m_GameLevelTable.clear();

		//Iterate over files in the folder
		std::experimental::filesystem::directory_iterator it{directoryPath};
		while (it != std::experimental::filesystem::directory_iterator{})
		{
			auto gameLevel = loadGameLevel(file::loadJson(it->path().string(), true));

			m_AdvancedScene->m_GameLevelTable.push_back(gameLevel);

			if(gameLevel->selected)
			{
				m_AdvancedScene->m_SelectedGameLevel = gameLevel;

				for(auto chunk : gameLevel->chunkTable)
				{
					if(chunk->selected)
					{
						m_AdvancedScene->m_SelectedWorldChunk	= chunk;
						m_AdvancedScene->m_SelectedWorldBuilder = chunk->sceneBuilder;
						break;
					}
				}
			}

			it++;
		}
    }

	AdvancedScene::GameLevelPtr GameProject::loadGameLevel(const nlohmann::json& level)
	{
		auto gameLevel = std::make_shared<AdvancedScene::GameLevel>();
		gameLevel->name			= level["level_name"].get<std::string>();
		gameLevel->levelId		= level["level_id"];
		gameLevel->chunkCount	= level["chunk_count"];
		gameLevel->selected		= level["selected"];

		for(auto& chunk : level["chunk_table"])
		{
			auto worldChunk = std::make_shared<AdvancedScene::WorldChunk>();
			worldChunk->sceneBuilder->setResourceManager(m_ResourceManager);
			worldChunk->sceneBuilder->setRenderTexture(m_RenderTexture);
			worldChunk->sceneBuilder->setRenderContext(m_RenderContext);

			worldChunk->name			= chunk["chunk_name"].get<std::string>();
			worldChunk->chunkId			= chunk["chunk_id"];
			worldChunk->visible			= chunk["chunk_visible"];
			worldChunk->selected		= chunk["selected"];
			worldChunk->sceneBuilder->loadScene(chunk["world_chunk"]);

			gameLevel->chunkTable.push_back(worldChunk);
		}

		return gameLevel;
	}

	void GameProject::saveProject()
	{

	}

	void GameProject::saveGameLevel()
	{
		if(m_AdvancedScene->m_SelectedGameLevel)
		{
			auto level = m_AdvancedScene->m_SelectedGameLevel;

			nlohmann::json level_save;

			level_save["level_id"]		= level->levelId;
			level_save["level_name"]	= level->name;
			level_save["selected"]		= level->selected;
			level_save["chunk_count"]	= level->chunkCount;
			level_save["chunk_table"]	= nlohmann::json::array();

			for(auto& chunk : level->chunkTable)
			{
				nlohmann::json chunk_save;

				chunk_save["world_chunk"]		= chunk->sceneBuilder->saveScene();
				chunk_save["chunk_id"]			= chunk->chunkId;
				chunk_save["chunk_name"]		= chunk->name;
				chunk_save["chunk_visible"]		= chunk->visible;
				chunk_save["selected"]			= chunk->selected;
				chunk_save["load_with_level"]	= chunk->loadWithLevel;

				level_save["world_chunk_table"].push_back(chunk_save);
			}


			std::string levelFile = file::getPath({m_ProjectParameter.getString("project_directory"), "Scene", "Level", level->name}, StringPool.EXT_JSON);
			file::saveFile(levelFile, level_save.dump(3), true);
		}
	}

	void GameProject::loadGameLevel()
	{

	}

	void GameProject::saveGameScreen()
	{
		if(m_AdvancedScene->m_SelectedGameScreen)
		{
			auto screen = m_AdvancedScene->m_SelectedGameScreen;

			nlohmann::json screen_save;

			screen_save["game_screen"]	= screen->sceneBuilder->saveScene();
			screen_save["screen_id"]	= screen->screenId;
			screen_save["screen_name"]	= screen->name;
			screen_save["selected"]		= screen->selected;

			std::string screenFile = file::getPath({m_ProjectParameter.getString("project_directory"), "Scene", "Screen", screen->name}, StringPool.EXT_JSON);
			file::saveFile(screenFile, screen_save.dump(3), true);
		}
	}

	void GameProject::loadGameScreen()
	{

	}

	void GameProject::loadLibrary()
    {
		loadLibraryDemo();
		return;

		nero_log(m_ProjectParameter.getString("library_file"));

		if(!file::fileExist(m_ProjectParameter.getString("library_file")))
		{
			nero_log("no library to load");
			return;
		}


		if(file::fileExist(m_ProjectParameter.getString("library_file_copy")))
		{
			m_AdvancedScene->m_Scene = nullptr;
			m_AdvancedScene->m_CreateCppScene.clear();
			m_CreateCppSceneFn.clear();

		   file::removeFile(m_ProjectParameter.getString("library_file_copy"));
        }

		file::copyFile(m_ProjectParameter.getString("library_file"), m_ProjectParameter.getString("library_file_copy"));

		boost::dll::fs::path library_path(file::removeFileExtension(m_ProjectParameter.getString("library_file_copy")));

		try
		{
			boost::dll::shared_library lib(m_ProjectParameter.getString("library_file_copy"));

			nero_log_if("project library loaded", lib.is_loaded());

			m_CreateCppSceneFn.clear();

			m_CreateCppSceneFn = boost::dll::import_alias<CreateCppSceneFn>(library_path, "createScene", boost::dll::load_mode::append_decorations);


			if(!m_CreateCppSceneFn.empty())
			{
				m_AdvancedScene->setCppSceneCreator(m_CreateCppSceneFn);
			}

		}
		catch (std::exception e)
		{
			nero_log("loading failed");
		}
    }

	void GameProject::loadLibraryDemo()
	{
		nero_log(m_ProjectParameter.getString("library_file"));

		if(!file::fileExist(m_ProjectParameter.getString("library_file")))
		{
			nero_log("no library to load");
			return;
		}

		if(file::fileExist(m_ProjectParameter.getString("library_file_copy")))
		{
			m_DemoScene = nullptr;
			m_CreateCppSceneFn.clear();

		   file::removeFile(m_ProjectParameter.getString("library_file_copy"));
		}

		file::copyFile(m_ProjectParameter.getString("library_file"), m_ProjectParameter.getString("library_file_copy"));

		boost::dll::fs::path library_path(file::removeFileExtension(m_ProjectParameter.getString("library_file_copy")));

		try
		{
			boost::dll::shared_library lib(m_ProjectParameter.getString("library_file_copy"));

			nero_log_if("project library loaded", lib.is_loaded());

			m_CreateCppSceneFn.clear();

			m_CreateCppSceneFn = boost::dll::import_alias<CreateCppSceneFn>(library_path, "createScene", boost::dll::load_mode::append_decorations);

			if(!m_CreateCppSceneFn.empty())
			{
				m_DemoScene = m_CreateCppSceneFn(Scene::Context(
					 m_ProjectParameter.getString("project_name"),
					 m_RenderTexture,
					 m_ResourceManager,
					 m_Camera,
					 m_EngineSetting,
					 Scene::EngineType::EDITOR_ENGINE,
					 Scene::PlatformType::WINDOWS));

				m_DemoScene->init();
			}
		}
		catch (std::exception e)
		{
			nero_log("loading failed");
		}
	}

    void GameProject::openEditor()
    {
		std::string codeEditor = m_ProjectParameter.getString("code_editor");

		if(codeEditor == "Qt Creator")
		{
			openQtCreator();
		}
		else if(codeEditor == "Visual Studio")
		{
			openVisualStudio();
		}
    }

	void GameProject::openQtCreator(const std::string& file)
	{
		if(cmd::processRunning(m_EditorProcessId))
		{
			std::string cmd = "START \"\" " + file::escapeSpace(file::getWindowsPath(m_EngineSetting->getSetting("environment").getString("qt_creator"))) + " -pid " + m_EditorProcessId;
			system(cmd.c_str());
		}
		else
		{
			std::string cmd =  "START \"\" " + file::escapeSpace(file::getWindowsPath(m_EngineSetting->getSetting("environment").getString("qt_creator")));
			cmd += " " + file::escapeSpace(m_ProjectParameter.getString("cmake_file"));

			system(cmd.c_str());

			m_EditorProcessId = cmd::findProcessId("qtcreator");
		}
	}

	void GameProject::openVisualStudio(const std::string& file)
	{
		if(file == StringPool.BLANK)
		{
			if(cmd::processRunning(m_EditorProcessId))
			{
				cmd::showApplication("devenv", m_EditorProcessId);
			}
			else
			{
				std::string cmd =  "START \"\" " + file::escapeSpace(file::getWindowsPath(m_EngineSetting->getSetting("environment").getString("visual_studio")));
				cmd += " \"" + file::getWindowsPath(m_ProjectParameter.getString("source_directory")) + "\"" + " /Edit";

				nero_log(cmd);

				system(cmd.c_str());

				m_EditorProcessId = cmd::findProcessId("devenv");
			}
		}
		else
		{
			std::string cmd = "START \"\" " + file::escapeSpace(file::getWindowsPath(m_EngineSetting->getSetting("environment").getString("visual_studio"))) + " \"" + file + "\" " + " /Edit";
			system(cmd.c_str());
		}
	}

	void GameProject::compileProject(const BackgroundTask::Ptr backgroundTask)
	{
		std::string sourcePath  = m_ProjectParameter.getString("source_directory");
		std::string buildPath   = m_ProjectParameter.getString("build_directory");

		std::string nero_game_home	= m_EngineSetting->getSetting("environment").getString("nero_game_home");

		std::string mingw32			= nero_game_home + "/Compiler/bin/mingw32-make.exe";
		std::string gxx				= nero_game_home + "/Compiler/bin/g++.exe";
		std::string gcc				= nero_game_home + "/Compiler/bin/gcc.exe";
		std::string cmake			= nero_game_home + "/Tools/Cmake/bin/cmake.exe";

		backgroundTask->setStatus(1);
		backgroundTask->addMessage("Cleaning Project ...");
		cmd::Process cleanProcess	=  cmd::runCommand(mingw32, {"-C", buildPath, "-k", "clean"});
		backgroundTask->setErrorCode(cleanProcess.getExistCode());
		nero_log("clean project exit code = " + toString(cleanProcess.getExistCode()));

		backgroundTask->setStatus(2);
		backgroundTask->addMessage("Configuring Project ...");
		cmd::Process configProcess	=  cmd::runCommand(cmake, {"-G", "MinGW Makefiles", "-S", sourcePath, "-B", buildPath,
															   "-D", "CMAKE_CXX_COMPILER=" + file::getPath(gxx), "-D", "CMAKE_C_COMPILER="	+ file::getPath(gcc),
																"-D", "CMAKE_MAKE_PROGRAM=" + file::getPath(mingw32)});
		backgroundTask->setErrorCode(configProcess.getExistCode());
		nero_log("configure project exit code = " + toString(configProcess.getExistCode()));

		backgroundTask->setStatus(3);
		backgroundTask->addMessage("Building Project ...");
		cmd::Process buildProcess	=  cmd::runCommand(mingw32, {"-C", buildPath});
		backgroundTask->setErrorCode(buildProcess.getExistCode());
		nero_log("build project exit code = " + toString(buildProcess.getExistCode()));

		backgroundTask->setStatus(4);
		backgroundTask->addMessage("Compilation Completed !!");

		std::this_thread::sleep_for(std::chrono::seconds(2));

		backgroundTask->setCompleted(true);
	}

	BackgroundTask& GameProject::createBackgroundTask(const std::string& name, const std::string& category)
	{
		m_BackgroundTaskTable.push_back(BackgroundTask(name, category));

		return m_BackgroundTaskTable.back();
	}

	std::vector<BackgroundTask>& GameProject::getBackgroundTaskTable()
	{
		return m_BackgroundTaskTable;
	}


    AdvancedScene::Ptr GameProject::getAdvancedScene()
    {
        return m_AdvancedScene;
    }

	void GameProject::setRenderTexture(const RenderTexturePtr& renderTexture)
	{
	   m_RenderTexture = renderTexture;
	}

	void GameProject::setCamera(const Camera::Ptr& camera)
	{
	   m_Camera = camera;
	}

	std::string GameProject::getProjectName() const
	{
		return m_ProjectParameter.getString("project_name");
	}

	void GameProject::setRenderContext(const RenderContextPtr& renderContext)
	{
		m_RenderContext = renderContext;
	}

	void GameProject::setSetting(const Setting::Ptr& setting)
	{
		m_EngineSetting = setting;
	}

	void GameProject::close()
	{
		if(cmd::processRunning(m_EditorProcessId))
		{
			std::string kill_command = "taskkill /F /PID " + m_EditorProcessId;
			system(kill_command.c_str());
		}

		nero_log("clearing resources");
		m_ResourceManager->clearResource();

		m_DemoScene = nullptr;
		m_CreateCppSceneFn.clear();

		//delete scene
		//m_AdvancedScene->m_Scene = nullptr;
		//m_AdvancedScene->m_CreateCppScene.clear();
		//m_CreateCppSceneFn.clear();

		//delete level and screen
		//m_AdvancedScene->m_GameLevelTable.clear();
		//m_AdvancedScene->m_GameScreenTable.clear();

		//m_CreateCppSceneFn.clear();
	}

	void GameProject::createScriptObject(const Parameter& parameter)
	{
		nero_log("creating script object");
		std::string header_template	= StringPool.BLANK;
		std::string source_template	= StringPool.BLANK;

		nero_log(parameter.getString("script_type"));

		if(parameter.getString("script_type") == "Game Level Script")
		{
			header_template	= file::loadText("template/cpp-project/CppGameLevel.h");
			source_template	= file::loadText("template/cpp-project/CppGameLevel.cpp");
		}
		else if(parameter.getString("script_type") == "Game Screen Script")
		{
			header_template	= file::loadText("template/cpp-project/CppGameScreen.h");
			source_template	= file::loadText("template/cpp-project/CppGameScreen.cpp");
		}

		std::string class_name		= parameter.getString("class_name") + "ScriptObject";
		std::string class_header	= boost::algorithm::to_upper_copy(class_name) + "_H";

		//file 1 : header
		boost::algorithm::replace_all(header_template, "::Class_Name::",		class_name);
		boost::algorithm::replace_all(header_template, "::Header_Gard::",		class_header);
		boost::algorithm::replace_all(header_template, "::Namespace::",			m_ProjectParameter.getString("project_namespace"));
		boost::algorithm::replace_all(header_template, "::Project_Name::",		m_ProjectParameter.getString("project_name"));
		boost::algorithm::replace_all(header_template, "::Project_Lead::",		m_ProjectParameter.getString("project_lead"));
		boost::algorithm::replace_all(header_template, "::Coypright_Date::",	toString(datetime::getCurrentDateTime().date().year()));

		//file 2 : source
		boost::algorithm::replace_all(source_template, "::Class_Name::",		class_name);
		boost::algorithm::replace_all(source_template, "::Namespace::",			m_ProjectParameter.getString("project_namespace"));
		boost::algorithm::replace_all(source_template, "::Project_Name::",		m_ProjectParameter.getString("project_name"));
		boost::algorithm::replace_all(source_template, "::Project_Lead::",		m_ProjectParameter.getString("project_lead"));
		boost::algorithm::replace_all(source_template, "::Coypright_Date::",	toString(datetime::getCurrentDateTime().date().year()));

		nero_log(m_ProjectParameter.getString("source_directory"));

		std::string header_file = file::getPath({m_ProjectParameter.getString("source_directory"),"cpp", "script", class_name}, StringPool.EXT_CPP);
		nero_log(header_file);

		file::saveFile(header_file, header_template);
		file::saveFile(file::getPath({m_ProjectParameter.getString("source_directory"), "cpp", "script", class_name}, StringPool.EXT_CPP), source_template);
	}

	const std::string GameProject::getResourceFoler() const
	{
		return file::getPath({m_ProjectParameter.getString("project_directory"), "Resource"});
	}

	void GameProject::renderDemo()
	{
		if(m_DemoScene)
		{
			m_DemoScene->render();
			m_DemoScene->renderShape();
		}
	}

	void GameProject::handleEventDemo(const sf::Event& event)
	{
		if(m_DemoScene)
		{
			m_DemoScene->handleEvent(event);
		}
	}

	void GameProject::updateDemo(const sf::Time& timestep)
	{
		if(m_DemoScene)
		{
			m_DemoScene->update(timestep);
		}
	}

	void GameProject::destroyScene()
	{
		m_DemoScene = nullptr;
		m_CreateCppSceneFn.clear();
	}
}

