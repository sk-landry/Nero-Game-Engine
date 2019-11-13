////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2019 SANOU A. K. Landry
/////////////////////////////////////////////////////////////
#ifndef EDITORCONSTANT_H
#define EDITORCONSTANT_H
///////////////////////////HEADERS///////////////////////////
#include <string>
#include <SFML/System/Vector2.hpp>
/////////////////////////////////////////////////////////////
namespace nero
{
	struct _EditorConstant
	{
		//ImGUI ID
		const std::string ID_DOCKSPACE					= "editor-dockspace";

		//ImGUI layout file
		const std::string FILE_INTERFACE_LAYOUT			= "imgui";

		//Window
			//toolbar
		const std::string WINDOW_TOOLBAR				= "Toolbar";
			//upper left
		const std::string WINDOW_UTILITY				= "Utility";
		const std::string WINDOW_MUSIC					= "Music";
		const std::string WINDOW_Grid					= "Gid";
			//lower left
		const std::string WINDOW_LEVEL					= "Level";
		const std::string WINDOW_CHUNCK					= "Chunck";
		const std::string WINDOW_SCREEN					= "Screen";
		const std::string WINDOW_LAYER					= "Layer";
			//right
		const std::string WINDOW_EXPLORER				= "Explorer";
		const std::string WINDOW_HELP					= "Engine Help";
		const std::string WINDOW_RESOURCE_BROWSER		= "Resource Browser";
			//bottom
		const std::string WINDOW_RESOURCE				= "Resource";
		const std::string WINDOW_LOGGING				= "Logging";
			//center
		const std::string WINDOW_GAME_SCENE				= "Scene     ";
		const std::string WINDOW_GAME_SETTING			= "Game Settings";
		const std::string WINDOW_GAME_PROJECT			= "Game Project";
		const std::string WINDOW_FACTORY				= "Factory";
		const std::string WINDOW_IMGUI_DEMO				= "Dear ImGui Demo";
			//dockspace
		const std::string WINDOW_EDITOR_DOCKSPACE		= "editor-dockspace";
			//project manager
		const std::string WINDOW_PROJECT_MANAGER		= "Project Manager";
		sf::Vector2f	  WINDOW_PROJECT_MANAGER_SIZE	= sf::Vector2f(800.f, 500.f);

		//Window and View

		const std::string TAB_CREATE_PROJECT				= "Create Project";
		const std::string TAB_OPEN_PROJECT					= "Open Project";
		const std::string TAB_RECENT_PROJECT				= "Recent Project";
		const std::string TAB_WORKSPACE						= "Workspace";
		const std::string ERROR_CREATING_WORKSPACE		= "Error Creating Workspace";
		const std::string ERROR_CREATING_PROJECT		= "Error Creating Project";

		//Texture
		const std::string TEXTURE_PROJECT_BUTTON		= "project_button";

		//Evenrionnment variable
		const std::string NERO_GAME_HOME				= "NERO_GAME_HOME";
		const std::string NERO_GAME_QT_CREATOR			= "NERO_GAME_QT_CREATOR";
		const std::string NERO_GAME_VISUAL_STUDIO		= "NERO_GAME_VISUAL_STUDIO";
	};

	const _EditorConstant EditorConstant;
}


#endif // EDITORCONSTANT_H
