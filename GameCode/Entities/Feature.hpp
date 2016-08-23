//=================================================================================
// Feature.hpp
// Author: Tyler George
// Date  : October 4, 2015
//=================================================================================

#pragma once


#ifndef __included_Feature__
#define __included_Feature__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/Entities/Entity.hpp"
#include "Engine/Math/IntRange.hpp"

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------
enum FeatureType
{
    FT_DOOR, // m_active: true if closed, false if open
    FT_SPAWNER, // m_active: true if can spawn, false if disabled by actor
    FT_INVALID
    
};

////===========================================================================================
///===========================================================================================
// Feature Class
///===========================================================================================
////===========================================================================================
class Feature : public Entity
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    Feature( const XMLNode& featureNode );
    Feature( const Feature& copy, const XMLNode& featureNode );
    ~Feature();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    FeatureType GetType() { return m_type; }
    bool IsActive() { return m_active; }
    std::string ToggleActive();
    void Activate() { m_active = true; }
    void Deactivate() { m_active = false; }
    bool ActiveBlocksLOS() { return m_activeBlocksLOS; }
    bool InactiveBlocksLOS() { return m_inactiveBlocksLOS; }
    bool ActiveBlocksMovement() { return m_activeBlocksMovement; }
    bool InactiveBlocksMovement() { return m_inactiveBlocksMovement; }
    static std::string GetTypeAsString( FeatureType type );

    void Save( XMLNode& parent, bool createChild );
    virtual void CleanUpPointers( std::map< int, Entity* > hookupMap ) { return; }
    virtual void HandleEvent( InteractionEvent* event );

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Update
    ///---------------------------------------------------------------------------------
    InteractionEvent* Think( float& out_actionTime );

    ///---------------------------------------------------------------------------------
    /// Render
    ///---------------------------------------------------------------------------------
    void Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, const bool& debugModeEnabled );

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
    FeatureType m_type;

    bool m_active;

    bool m_activeBlocksLOS;
    bool m_inactiveBlocksLOS;

    bool m_activeBlocksMovement;
    bool m_inactiveBlocksMovement;

    std::string m_activeSymbol;
    std::string m_inactiveSymbol;

    std::string m_activateVerb;
    std::string m_deactivateVerb;

    Rgba m_activeColor;
    Rgba m_inactiveColor;

    // spawner specific
    int m_countdownMax;
    int m_currentCountdown;
    std::string m_spawnNPC;
    int m_spawnRadius;
    std::string m_phrase;

};

typedef std::vector< Feature* > Features;

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif