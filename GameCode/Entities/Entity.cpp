//=================================================================================
// Entity.cpp
// Author: Tyler George
// Date  : September 1, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================
#include "Engine/Utilities/XMLHelper.hpp"

#include "GameCode/Entities/Entity.hpp"
#include "Engine/Utilities/StringTable.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

int Entity::s_nextEntityID = 1;

////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Entity::Entity( const XMLNode& entityNode )
    : m_owningMap( nullptr )
    , m_mapPosition(MapPosition( -1, -1 ) )
    , m_entityID( s_nextEntityID++ )
    , m_hookupID( 0 )
    , m_isActor( false )
    , m_isFeature( false )
    , m_hasDied( false )
    , m_faction( nullptr )
    , m_prototype( nullptr )
    , m_health( 0 )
    , m_maxHealth( 0 )
{
    // do parsing here
    m_entityName = GetStringProperty( entityNode, "name", "entity_" + std::to_string( m_entityID ), false );
    m_entityNameID = StringTable::GetStringID( m_entityName );
    Faction* xmlFaction = Faction::GetMasterFactionFromName( GetStringProperty( entityNode, "faction", "unknown faction", true ) );
    if ( xmlFaction )
        m_faction = new Faction( *xmlFaction );

    m_symbol = GetCharacterProperty( entityNode, "glyph", m_symbol[0] );
    m_color = GetRgbaProperty( entityNode, "color", Rgba() );
    m_mapPosition = GetIntVector2Property( entityNode, "mapPosition", IntVector2( -1, -1 ) );
    m_health = GetIntProperty( entityNode, "health", 0 );
    m_maxHealth = GetIntProperty( entityNode, "maxHealth", 0 );
    m_hookupID = GetIntProperty( entityNode, "hookupID", 0 );
    if (m_maxHealth != 0 && m_health == 0)
        m_health = m_maxHealth;
        
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Entity::Entity( const std::string& name )
    : m_owningMap( nullptr )
    , m_mapPosition(MapPosition( -1, -1 ))
    , m_entityName( name )
    , m_entityNameID( StringTable::GetStringID( name ) )
    , m_entityID( s_nextEntityID++ )
    , m_hookupID( 0 )
    , m_faction( nullptr )
    , m_prototype( nullptr )
    , m_symbol( "?" )
    , m_health( 0 )
    , m_maxHealth( 0 )
    , m_color( Rgba() )
    , m_isActor( false )
    , m_isFeature( false )
    , m_hasDied( false )
{
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Entity::Entity( const Entity& copy, const XMLNode& entityNode )
    : m_owningMap( copy.m_owningMap )
    , m_entityName( copy.m_entityName )
    , m_mapPosition(MapPosition( -1, -1 ))
    , m_entityNameID( copy.m_entityNameID )
    , m_entityID( s_nextEntityID++ )
    , m_symbol( copy.m_symbol )
    , m_prototype( copy.m_prototype )
    , m_health( copy.m_health )
    , m_maxHealth( copy.m_maxHealth )
    , m_color( copy.m_color )
    , m_isActor( copy.m_isActor )
    , m_isFeature( copy.m_isFeature )
    , m_hasDied( copy.m_hasDied )
    , m_faction( nullptr )
    , m_hookupID( 0 )
{
    XMLNode factionNode = entityNode.getChildNode( "Faction" );

    if ( !m_faction && copy.m_faction )
        m_faction = new Faction( *copy.m_faction, factionNode );

    if (!entityNode.isEmpty())
    {
        m_mapPosition = GetIntVector2Property( entityNode, "MapPosition", IntVector2( -1, -1 ) );
        m_maxHealth = GetIntProperty( entityNode, "maxHealth", 0 );
        m_health = GetIntProperty( entityNode, "health", 0 );
        if (m_maxHealth != 0 && m_health == 0)
            m_health = m_maxHealth;

        m_color = GetRgbaProperty( entityNode, "color", m_color );
        m_hookupID = GetIntProperty( entityNode, "hookupID", 0 );

    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Entity::~Entity()
{

}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::Startup()
{

}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Entity::IsPlayer() const
{

    return m_entityName == "Player";
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::Save( XMLNode& parent, bool createChild )
{
    XMLNode nodeToModify = parent;
    if (createChild)
        nodeToModify = CreateNode( "Entity" );

    SetStringProperty( nodeToModify, "name", m_entityName, "" );
    SetIntProperty( nodeToModify, "hookupID", m_entityID, -1 );
    
    if (m_health != m_maxHealth)
        SetIntProperty( nodeToModify, "health", m_health, 0 );
    SetIntProperty( nodeToModify, "maxHealth", m_maxHealth, 0 );

    SetIntVector2Property( nodeToModify, "mapPosition", m_mapPosition, IntVector2( -1, -1 ) );

    if (m_prototype)
    {
        if (!(m_color == m_prototype->m_color))
            SetRgbaProperty( nodeToModify, "color", m_color, m_prototype->m_color );
    }

    if (m_faction)
        m_faction->Save( nodeToModify );

    if (createChild)
        AddChild( parent, nodeToModify );

}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Entity::ApplyDamage( int damage )
{
    m_health -= damage;
    if (m_health <= 0)
    {
        m_health = 0;
        m_hasDied = true;
        return true;
    }
    return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int Entity::ApplyHealing( int healing )
{
    int healingNeeded = m_maxHealth - m_health;
    int healthAdjustment = healing;

    if (healingNeeded <= healing)
    {
        healthAdjustment = healingNeeded;
        m_health = m_maxHealth;
    }
    else
        m_health += healthAdjustment;

    return healthAdjustment;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::RemoveFromWorld()
{
    if (m_isFeature)
        m_owningMap->SetFeatureAtPosition( nullptr, m_mapPosition );
    
    else
        m_owningMap->SetEntityAtPosition( nullptr, m_mapPosition );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
InteractionEvent* Entity::Think( float& out_actionTime )
{
    __debugbreak();
    out_actionTime = 0.0f;
    return nullptr;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::CleanUpPointers( std::map< int, Entity* > hookupMap )
{
    m_faction->CleanUpPointers( hookupMap );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::ProcessInput( InputSystem* inputSystem, const double& deltaSeconds )
{
    UNUSED( inputSystem );
    UNUSED( deltaSeconds );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::Update( const double& deltaSeconds, const bool& debugModeEnabled )
{
    UNUSED( deltaSeconds );
    UNUSED( debugModeEnabled );
}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, const bool& debugModeEnabled )
{
    if (!renderer)
        return;

    UNUSED( debugModeEnabled );
    
    Cell* myCell = m_owningMap->GetCellAtPosition( m_mapPosition );

    AABB2D renderBounds = myCell->GetRenderBounds();

    Vector3 pos( renderBounds.m_mins.x, renderBounds.m_mins.y, 9.0f );
    float fontWidth = renderBounds.m_maxs.x - renderBounds.m_mins.x;
    float fontHeight = renderBounds.m_maxs.y - renderBounds.m_mins.y;
    fontRenderer->DrawFontTextWH( fontWidth, fontHeight, font, m_symbol, pos, m_color );
}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

