//=================================================================================
// Actor.hpp
// Author: Tyler George
// Date  : September 1, 2015
//=================================================================================

#pragma once

#ifndef __included_Actor__
#define __included_Actor__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/FontRenderer.hpp"
#include "Engine/Utilities/XMLParser.h"

#include "GameCode/Entities/Entity.hpp"
#include "GameCode/Inventory.hpp"
#include "GameCode/Pathfinder.hpp"
#include <set>



///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef std::vector< Actor* > Actors;

////===========================================================================================
///===========================================================================================
// Actor Class
///===========================================================================================
////===========================================================================================
class Actor : public Entity
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    Actor( const XMLNode& actorNode );
    Actor( const std::string& name );
    Actor( const Actor& copy, const XMLNode& actorNode );
    ~Actor();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
    void Startup();
    void InitInventory();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    bool TestOneMove( MapDirection direction ) const;
    Actors& GetVisibleActors() { return m_visibleActors; }
    std::vector< Cell* >&  GetVisibleCells() { return m_visibleCells; }

    Entity* GetHostileTarget() { return m_hostileTarget; }
    Entity* GetFriendlyTarget() { return m_friendlyTarget; }
    Inventory* GetInventory() { return m_inventory; }

    virtual void Save( XMLNode& parent, bool createChild );

    bool IsPositionVisible( const MapPosition& position );

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    void MakeOneMove( MapDirection direction );
    void SetVisibleActors( Actors visibleActors );
    void SetVisibleCells( std::vector< Cell* > visibleCells );
    void SetHostileTarget( Entity* hostileTarget ) { m_hostileTarget = hostileTarget; }
    void SetFriendlyTarget( Entity* friendlyTarget ) { m_friendlyTarget = friendlyTarget; }
    void DropInventory();

    virtual InteractionEvent* Think( float& out_actionTime ) = 0;
    virtual void CleanUpPointers( std::map< int, Entity* > hookupMap );
    virtual void HandleEvent( InteractionEvent* event ) = 0;

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
    void ProcessInput( InputSystem* inputSystem, double deltaSeconds );
    void Update( double deltaSeconds, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
    virtual void Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Static Variables
	///---------------------------------------------------------------------------------


protected:
	///---------------------------------------------------------------------------------
	/// Protected Functions
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Protected Member Variables
	///---------------------------------------------------------------------------------
    Inventory* m_inventory;
    Path* m_path;

    Actors m_visibleActors;
    std::vector< Cell* > m_visibleCells;
    int m_visibilityRange;


    Entity* m_hostileTarget;
    Entity* m_friendlyTarget;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif