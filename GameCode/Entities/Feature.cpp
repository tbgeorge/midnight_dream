//=================================================================================
// Feature.cpp
// Author: Tyler George
// Date  : October 4, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Entities/Feature.hpp"
#include "Engine/Utilities/XMLHelper.hpp"
#include "Engine/Utilities/StringTable.hpp"
#include "../NPCFactory.hpp"
#include "../MessageBar.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Feature::Feature( const XMLNode& featureNode )
    : Entity( featureNode )
    , m_active( false )
    , m_activeBlocksLOS( true )
    , m_inactiveBlocksLOS( true )
    , m_activateVerb( "activated" )
    , m_deactivateVerb( "deactivated" )
{
    m_isFeature = true;
    unsigned int featureTypeStrID = StringTable::GetStringID( GetStringProperty( featureNode, "type", "", true ) );
    std::string initialStatus = GetStringProperty( featureNode, "initialStatus", "", true );

    m_activeColor = GetRgbaProperty( featureNode, "activeColor", m_color );
    m_inactiveColor = GetRgbaProperty( featureNode, "inactiveColor", m_color );


    m_activeBlocksMovement = GetBooleanProperty( featureNode, "activeBlocksMovement", true );
    m_inactiveBlocksMovement = GetBooleanProperty( featureNode, "inactiveBlocksMovement", true );

    if (featureTypeStrID == StringTable::GetStringID( "door" ))
    {
        m_type = FT_DOOR;
        m_symbol = std::string( 1, '+' );

        m_activeSymbol = std::string( 1, '+' );
        m_inactiveSymbol = std::string( 1, '-' );

        if (initialStatus == "open")
            m_active = false;
        else if (initialStatus == "closed")
            m_active = true;
        else
            m_active = true;

        m_activeBlocksMovement = true;
        m_inactiveBlocksMovement = false;

        m_activeBlocksLOS = GetBooleanProperty( featureNode, "activeBlocksLOS", true );
        m_inactiveBlocksLOS = GetBooleanProperty( featureNode, "inactiveBlocksLOS", false );

        m_activateVerb = "closed";
        m_deactivateVerb = "opened";
    }

    else if (featureTypeStrID == StringTable::GetStringID( "spawner" ))
    {
        m_type = FT_SPAWNER;
        m_symbol = std::string( 1, '&' );

        m_activeSymbol = std::string( 1, '&' );
        m_inactiveSymbol = std::string( 1, '&' );

        // defaults to active
        m_active = true;

        m_activeBlocksMovement = false;
        m_inactiveBlocksMovement = false;

        m_activeBlocksLOS = false;
        m_inactiveBlocksLOS = false;

        m_activateVerb = "activated";
        m_deactivateVerb = "disturbed";

        m_countdownMax = GetIntProperty( featureNode, "countdown", 0 );
        m_currentCountdown = m_countdownMax;

        m_spawnNPC = GetStringProperty( featureNode, "spawnNPC", "", false );
        m_spawnRadius = GetIntProperty( featureNode, "spawnRadius", 0 );
        m_phrase = GetStringProperty( featureNode, "phrase", "Spawned NPC", false );
    }

    else
    {
        m_type = FT_INVALID;
        m_symbol = std::string( 1, '?' );
        m_activeSymbol = m_symbol;
        m_inactiveSymbol = m_symbol;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
    Feature::Feature( const Feature& copy, const XMLNode& featureNode )
        : Entity( copy, featureNode )
        , m_type( copy.m_type )
        , m_activeBlocksLOS( copy.m_activeBlocksLOS )
        , m_inactiveBlocksLOS( copy.m_inactiveBlocksLOS )
        , m_activeBlocksMovement( copy.m_activeBlocksMovement )
        , m_inactiveBlocksMovement( copy.m_inactiveBlocksMovement )
        , m_active( copy.m_active )
        , m_activeSymbol( copy.m_activeSymbol )
        , m_inactiveSymbol( copy.m_inactiveSymbol )
        , m_activateVerb( copy.m_activateVerb )
        , m_deactivateVerb( copy.m_deactivateVerb )
        , m_countdownMax( copy.m_countdownMax )
        , m_currentCountdown( copy.m_currentCountdown )
        , m_spawnNPC( copy.m_spawnNPC )
        , m_spawnRadius( copy.m_spawnRadius )
        , m_phrase( copy.m_phrase )
        , m_activeColor( copy.m_activeColor )
        , m_inactiveColor( copy.m_inactiveColor )
{
    m_isFeature = true;

    m_active = GetBooleanProperty( featureNode, "active", m_active );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Feature::~Feature()
{

}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
std::string Feature::ToggleActive()
{
    std::string verb = "";

    if (m_active)
        verb = m_deactivateVerb;
    else
        verb = m_activateVerb;

    m_active = !m_active;
    return verb;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
std::string Feature::GetTypeAsString( FeatureType type )
{
    switch (type)
    {
    case FT_DOOR:
        return "door";
    default:
        return "";
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Feature::Save( XMLNode& parent, bool createChild )
{
    XMLNode nodeToModify = parent;
    if (createChild)
        nodeToModify = CreateNode( "Feature" );

    Entity::Save( nodeToModify, false );

    if (m_prototype)
    {
        if (m_type != ((Feature*)m_prototype)->m_type)
            SetStringProperty( nodeToModify, "type", GetTypeAsString( m_type ), GetTypeAsString( ((Feature*)m_prototype)->m_type ) );
        if (m_active != ((Feature*)m_prototype)->m_active)
            SetBooleanProperty( nodeToModify, "active", m_active, ((Feature*)m_prototype)->m_active );
    }

    if (createChild)
        AddChild( parent, nodeToModify );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Feature::HandleEvent( InteractionEvent* event )
{
    UNUSED( event );
    return;
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
InteractionEvent* Feature::Think( float& out_actionTime )
{
    out_actionTime = 1.0f;
    switch (m_type)
    {
    case FT_DOOR:
        return nullptr;
        break;
    case FT_SPAWNER:
    {
        if (!m_active)
            return nullptr;

        if (m_currentCountdown > 0)
        {
            m_currentCountdown--;
            return nullptr;
        }

        if (m_spawnNPC == "")
            return nullptr;

        NPCFactory* npcFactory = NPCFactory::FindFactoryByName( m_spawnNPC );
        if (!npcFactory)
            return nullptr;

        MapPosition myPos = m_mapPosition;
        AABB2D spawnBounds( myPos.x - m_spawnRadius, myPos.y - m_spawnRadius, myPos.x + m_spawnRadius, myPos.y + m_spawnRadius );

        MapPosition newNPCPos = m_owningMap->GetRandomOpenPositionInBounds( spawnBounds, true );

        if (newNPCPos == MapPosition( -1, -1 ))
            return nullptr;

        NPC* newNPC = npcFactory->SpawnNPC( XMLNode::emptyNode() );
        newNPC->SetMap( m_owningMap );
        m_owningMap->SetEntityAtPosition( newNPC, newNPCPos );

        Message msg;
        msg.m_instigator = m_entityName;
        msg.m_position = m_mapPosition;
        msg.m_text = m_phrase;

        MessageBar::AddMessage( msg );

        m_currentCountdown = m_countdownMax;

        return nullptr;
        break;
    }
    case FT_INVALID:
        return nullptr;
        break;
    }
}

////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Feature::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, const bool& debugModeEnabled )
{
    if (!renderer)
        return;

    UNUSED( debugModeEnabled );

    Cell* myCell = m_owningMap->GetCellAtPosition( m_mapPosition );

    AABB2D renderBounds = myCell->GetRenderBounds();

    Vector3 pos( renderBounds.m_mins.x, renderBounds.m_mins.y, 9.0f );
    float fontWidth = renderBounds.m_maxs.x - renderBounds.m_mins.x;
    float fontHeight = renderBounds.m_maxs.y - renderBounds.m_mins.y;

    if ( m_active )
        fontRenderer->DrawFontTextWH( fontWidth, fontHeight, font, m_activeSymbol, pos, m_activeColor );

    else if (!m_active )
        fontRenderer->DrawFontTextWH( fontWidth, fontHeight, font, m_inactiveSymbol, pos, m_inactiveColor );
}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

