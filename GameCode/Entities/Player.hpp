//=================================================================================
// Player.hpp
// Author: Tyler George
// Date  : September 1, 2015
//=================================================================================

#pragma once

#ifndef __included_Player__
#define __included_Player__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/FontRenderer.hpp"

#include "GameCode/Entities/Actor.hpp"
#include "Engine/Math/IntRange.hpp"


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

enum PlayerAction
{
    PA_UNSPECIFIED,
    PA_DETERMINE,
    PA_MOVE,
    PA_ATTACK,
    PA_PICKUPITEM,
    PA_QUAFF,
    PA_INTERACT_WITH_FEATURE
};

////===========================================================================================
///===========================================================================================
// Player Class
///===========================================================================================
////===========================================================================================
class Player : public Actor
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    Player( const XMLNode& playerNode );
    Player();
    ~Player();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
    void Startup();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    bool IsReadyToAct() { return m_currentAction == PA_UNSPECIFIED ? false : true; }
    int GetTurnsTaken() { return m_numTurnsTaken; }
    int GetActorsKilled() { return m_actorsKilled; }
    int GetVisibilityRange() { return m_visibilityRange; }

    void Save( XMLNode& parent, bool createChild );

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    InteractionEvent* Think( float& out_actionTime );
    virtual void HandleEvent( InteractionEvent* event );
    void Reset();

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
    void ProcessInput( InputSystem* inputSystem, double deltaSeconds );
    void Update( double deltaSeconds, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
    void Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Static Variables
	///---------------------------------------------------------------------------------


private:
	///---------------------------------------------------------------------------------
	/// Private Functions
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Private Member Variables
	///---------------------------------------------------------------------------------
    PlayerAction m_currentAction;
    MapDirection m_actionDirection;
    Entity* m_target;
    IntRange m_damageRange;

    int m_numTurnsTaken;
    int m_actorsKilled;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif