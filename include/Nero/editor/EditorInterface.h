////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2020 SANOU A. K. Landry
////////////////////////////////////////////////////////////
#ifndef EDITORINTERFACE_H
#define EDITORINTERFACE_H
///////////////////////////HEADERS//////////////////////////
//IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <Nero/editor/ProjectManager.h>
#include <Nero/editor/SceneManager.h>
#include <functional>
#include <Nero/core/scene/Scene.h>
#include <Nero/core/luascene/LuaScene.h>
#include <Nero/editor/GameProject.h>
#include <json/json.hpp>
#include <Nero/editor/LoggerApplication.h>
#include <Nero/core/utility/StringUtil.h>
#include <Nero/core/utility/LogUtil.h>
#include <Nero/editor/AdvancedScene.h>
#include <Nero/core/resource/ResourceManager.h>
#include <Nero/editor/EditorInterfaceUtility.h>

#include <future>

# include <nodeeditor/imgui_node_editor.h>

////////////////////////////////////////////////////////////

namespace nero
{
    class EditorInterface
    {

        public:
            typedef std::unique_ptr<EditorInterface> Ptr;

                        EditorInterface(sf::RenderWindow& window);
                       ~EditorInterface();

        private:
            void        handleEvent(const sf::Event& event);
            void        update(const sf::Time& timeStep);
            void        render();

            void        updateFrameRate(const float& frameRate, const float& frameTime);

            void        quitEditor();

            //Start Project
            void                    addScene(const std::string& projectName, std::function<Scene::Ptr(Scene::Context)> factory);
            void                    addLuaScene(const std::string& projectName, std::function<LuaScene::Ptr(Scene::Context)> factory);

            void                    loadAllProject();

            //
            void                    setEditorSetting(const nlohmann::json& setting);

            sf::Sprite             flipTexture(const sf::Texture& texture);

			//editor view
                //upper left
			void                    showMusicWindow();
			void                    showUtilityWindow();
			void                    showSceneLevelWindow();
			void                    showSceneChunckWindow();
			void					showSceneLayerWindow();
            void                    showSceneScreenWindow();
                //right
            void                    showCurrentSceneWindow();

            void                    showLogWindow();
            void                    showResourceCategoryWindow();
            void                    showResourceWindow();
                //terminate
            void                    interfaceFirstDraw();

            //utility
            void                    showToggleButton(bool toggle, const std::string& label, std::function<void()> callback);

        private:
			friend class						EngineEditor;
			sf::RenderWindow&					m_RenderWindow;
			TextureHolder						m_TextureHolder;
			//Docksapce
			ImGuiID								m_DockspaceID;
			bool								m_BuildDockspaceLayout;
			bool								m_SetupDockspaceLayout;
			void								createDockSpace();
			void								createEditorMenuBar();

            bool        setup_dock = false;
			ImGuiID actionBarId;
			ImGuiID dock_id_right;
            ImGuiID dock_id_upper_left;
			ImGuiID dock_id_left_bottom;

            bool            show_project_window = false;
            sf::Texture     project_manager_texture;

            Scene::Context      m_Context;

            SceneManager::Ptr       m_SceneManager;
            //
            nlohmann::json m_EditorSetting;

            bool renderImgui = false;

            //project creation

            std::string test_log;

            LoggerApplication m_LoggerApplication;
            bool   m_InterfaceFirstDraw;
            bool open_sprite_browser = false;
            ImGuiIO baseio;
            ImGuiID dockspace_id;
            std::stringstream buffer;
            std::streambuf * old;

			void setCallbackWindowTitle(std::function<void (const std::string&)> fn);

            std::function<void (const std::string&)> m_UpdateWindowTile;

            //
            void                        showGameProjectWindow();
            void                        showGameSettingWindow();
            void                        showSceneWindow();
			void						showBackgroundTaskWindow();



            ////////////////////////Tool Bar////////////////////////
            void                        showToolbarWindow();
            sf::Texture                 m_ProjectButtonTexture;
            sf::Texture                 m_CompileButtonTexture;
            sf::Texture                 m_ReloadButtonTexture;
            sf::Texture                 m_EditButtonTexture;
            sf::Texture                 m_BlankButtonTexture;

            ////////////////////////Project and Workspace////////////////////////
            //General
            ProjectManager::Ptr         m_ProjectManager;
            GameProject::Ptr            m_CurrentProject;
            //Project Workspace
            nlohmann::json              m_WorkspaceTable;               //list of available workspaces
            int                         m_WorksapceStatus;              //0 : no_worksapce, 1 : redirect_user, 2 worksapce_available
            char                        m_InputWorksapceFolder[256];    //read workspace path
            char                        m_InputWorksapceImportFolder[256];    //read workspace path
            char                        m_InputWorkspaceName[100];
            char                        m_InputWorkspaceCompany[100];
            char                        m_InputWorkspaceLead[100];
            char                        m_InputWorkspaceNamespace[10];
            const char*                 m_SelectedWorkpsapce;
			int                         m_SelectedWorkpsapceIdex;
			int                         m_SelectedProjectTypeIdex;
			int                         m_SelectedCodeEditorIdex;

            //Game Project
            char                        m_InputProjectName[100];        //read project name
            char                        m_InputProjectLead[100];        //read project lead
            char                        m_InputProjectCompany[100];     //read project company
            char                        m_InputProjectNamespace[10];
            char                        m_InputProjectDescription[512]; //read project description
			const char*                 m_SelectedProjectType;
			const char*                 m_SelectedCodeEditor;
            std::future<int>            m_CreateProjectFuture;
			std::future<int>            m_CompileProjectFuture;

            int                         m_ProjectCreationStatus;
            std::string                 m_LastCreatedProject;

            //Tabs
			TabBarSwitch				m_ProjectManagerTabBarSwith;
            int                         m_ProjectManagerSelectedTab;    //0 : Create Project, 1 : Open Project, 2 : Recent Project, 3 : Worksapce
            //Banner
            sf::Texture                 m_ProjectManagerBannerTexture;
            //show view
            void                        showProjectManagerWindow();
            void                        showCreateProjectWindow();
            void                        showOpenPorjectWindow();
            void                        showRecentProjectWindow();
            void                        showWorkspaceWindow();
            //function
            int                         createProject(const nlohmann::json& projectJson, int& status);
            void                        createWorkspace(const nlohmann::json& workspaceJson);
            void                        openProject(const std::string& project_name);
            void                        compileProject();
            void                        editProject();
            void                        reloadProject();

            //
            GameProject::Ptr            m_GameProject;
            AdvancedScene::Ptr          m_AdvancedScene;

			ax::NodeEditor::EditorContext*	g_Context;

			ResourceManager		m_ResourceManager;
    };

}

#endif // EDITORINTERFACE_H