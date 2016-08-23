//=================================================================================
// Entity.hpp
// Author: Tyler George
// Date  : September 1, 2015
//=================================================================================

#pragma once

#ifndef __included_Entity__
#define __included_Entity__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/FontRenderer.hpp"
#include "Engine/Utilities/XMLParser.h"

#include "GameCode/Map.hpp"
#include "../Faction.hpp"


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------
enum InteractionType
{
    INT_TYPE_DAMAGE,
    INT_TYPE_HEAL
};

///---------------------------------------------------------------------------------
/// Structs
///---------------------------------------------------------------------------------
struct InteractionEvent
{
    InteractionEvent( MapPosition pos, Entity* ins, Entity* oth, InteractionType type, int relVal )
        : m_location( pos ), m_instigator( ins ), m_other( oth ), m_type( type ), m_relevantValue( relVal ) {}
    InteractionEvent() {}
    ~InteractionEvent() {}

    MapPosition m_location;
    Entity* m_instigator;
    Entity* m_other;
    InteractionType m_type;
    int m_relevantValue;

};

////===========================================================================================
///===========================================================================================
// Entity Class
///===========================================================================================
////===========================================================================================
class Entity
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    Entity( const XMLNode& entityNode );
    Entity( const std::string& name );
    Entity( const Entity& copy, const XMLNode& entityNode );
    ~Entity();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------
    void Startup();

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    bool IsReadyToAct() { return true; }
    MapPosition GetMapPosition() { return m_mapPosition; }
    bool IsPlayer() const;
    bool IsActor() const { return m_isActor; }
    bool IsFeature() const { return m_isFeature; }
    std::string GetName() const { return m_entityName; }
    int GetEntityID() const { return m_entityID; }
    int GetHookupID() const { return m_hookupID; }
    int GetMaxHealth() const { return m_maxHealth; }
    int GetHealth() const { return m_health; }
    Map* GetMap() const { return m_owningMap; }
    Faction* GetFaction() const { return m_faction; }
    bool HasDied() const { return m_hasDied; }
    std::string GetSymbol() { return m_symbol; }
    Rgba GetColor() { return m_color; }

    virtual void Save( XMLNode& parent, bool createChild );

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    void SetHealth( const int& health ) { m_health = health; }
    void SetMaxHealth( const int& maxHealth ) { m_maxHealth = maxHealth; }
    void SetDead( bool dead ) { m_hasDied = dead; }
    void SetMap( Map* map ) { m_owningMap = map; }
    void SetMapPosition( const MapPosition& pos ) { m_mapPosition = pos; }
    void SetPrototype( Entity* prototype ) { m_prototype = prototype; }
    bool ApplyDamage( int damage );
    int ApplyHealing( int healing );
    void RemoveFromWorld();
    virtual InteractionEvent* Think( float& out_actionTime );

    virtual void CleanUpPointers( std::map< int, Entity* > hookupMap );

    virtual void HandleEvent( InteractionEvent* event ) = 0;

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
    void ProcessInput( InputSystem* inputSystem, const double& deltaSeconds );
    void Update( const double& deltaSeconds, const bool& debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
    virtual void Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, const bool& debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Static Variables
	///---------------------------------------------------------------------------------
    static int s_nextEntityID;

protected:
	///---------------------------------------------------------------------------------
	/// Protected Functions
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Protected Member Variables
	///---------------------------------------------------------------------------------
    int m_entityID;
    int m_hookupID;
    std::string m_entityName;
    unsigned int m_entityNameID;
    MapPosition m_mapPosition;
    std::string m_symbol;
    int m_health;
    int m_maxHealth;
    Rgba m_color;
    Faction* m_faction;
    Entity* m_prototype;

    bool m_isActor;
    bool m_isFeature;
    bool m_hasDied;

    Map* m_owningMap;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif