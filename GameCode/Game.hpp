//=================================================================================
// Game.hpp
// Author: Tyler George
// Date  : January 29, 2015
//=================================================================================

#pragma once
#ifndef __included_Game__
#define __included_Game__

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Math2D.hpp"
#include "Engine/Utilities/Time.hpp"
#include "Engine/Renderer/FontRenderer.hpp"
#include "GameCode/MainMenu.hpp"
#include "GameCode/Map.hpp"
#include "GameCode/QuestMenu.hpp"
#include "GameCode/Entities/Player.hpp"
#include "StatusBar.hpp"
#include "GameOverMenu.hpp"


///---------------------------------------------------------------------------------
/// Structs/Enums
///---------------------------------------------------------------------------------

enum GameState
{
    GS_MAIN_MENU,
    GS_QUEST_MENU,
    GS_ONE_STEP_GENERATION,
    GS_AUTOMATIC_GENERATION,
    GS_IN_GAME,
    GS_PAUSED,
    GS_ACKNOWLEDGE_SAVE,
    GS_GAME_OVER
};

///---------------------------------------------------------------------------------
/// Constants
///---------------------------------------------------------------------------------


///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class Game
{
public:
	///---------------------------------------------------------------------------------
	/// Constructor/Destructor
	///---------------------------------------------------------------------------------
	Game( const Vector2& displaySize );
	~Game();

	///---------------------------------------------------------------------------------
	/// Initialization Functions
	///---------------------------------------------------------------------------------
	void Startup( OpenGLRenderer* renderer, FontRenderer* fontRenderer, unsigned int basicShaderID );
	void Shutdown();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    bool IsReadyToQuit() const { return m_isReadyToQuit; }

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------
    void LoadGameFromFile();
    void ResetGame();
    static void AddEntity( Entity* entity );

	///---------------------------------------------------------------------------------
	/// Update Functions
	///---------------------------------------------------------------------------------
	void ProcessInput( InputSystem* inputSystem, const double& deltaSeconds );
    void ProcessGameInput( InputSystem* inputSystem, const double& deltaSeconds );
	void Update( const double& deltaSeconds, const bool& debugModeEnabled );
    void UpdateEntities( const double& deltaSeconds, const bool& debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Render Functions
	///---------------------------------------------------------------------------------
	void Render( bool debugModeEnabled );
    void RenderGame( bool debugModeEnabled );
    void RenderMap( bool debugModeEnabled );
    void RenderVisibleEntities( bool debugModeEnabled );


private:
    ///---------------------------------------------------------------------------------
    /// Private Functions
    ///---------------------------------------------------------------------------------
    void SetGameState( GameState newGameState );
    void SaveGame();

	///---------------------------------------------------------------------------------
	/// Private member variables
	///---------------------------------------------------------------------------------
	Vector2		m_displaySize;
	Camera3D*	m_camera;
    
    bool m_isReadyToQuit;

    OpenGLRenderer* m_renderer;
    FontRenderer* m_fontRenderer;

    Font* m_font;
    Font* m_mapFont;
    Font* m_titleFont;

    unsigned int m_basicShaderID;
    unsigned int m_fontShaderID;

    GameState m_gameState;

    MainMenu* m_mainMenu;
    QuestMenu* m_questMenu;
    GameOverMenu* m_gameOverMenu;

    Map* m_map;
    StatusBar* m_statusBar;
    Player* m_player;

    std::vector< Entity* > m_entities;
    std::vector< InteractionEvent* > m_events;

    ///---------------------------------------------------------------------------------
    /// Private Static Variables
    ///---------------------------------------------------------------------------------
    static Game* s_theGame;
};


#endif