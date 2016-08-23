//=================================================================================
// Actor.cpp
// Author: Tyler George
// Date  : September 1, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Entities/Actor.hpp"
#include "Engine/Utilities/StringTable.hpp"
#include "Engine/Utilities/XMLHelper.hpp"
#include "Feature.hpp"


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
Actor::Actor( const XMLNode& actorNode )
    : Entity( actorNode )
    , m_path( nullptr )
    , m_hostileTarget( nullptr )
    , m_friendlyTarget( nullptr )
    , m_inventory( nullptr )
    , m_visibilityRange( 6 )
{
    m_isActor = true;
    // do parsing here
    if (!actorNode.isEmpty())
    {
        m_visibilityRange = GetIntProperty( actorNode, "visibilityRange", m_visibilityRange );

        XMLNode inventoryNode = actorNode.getChildNode( "Inventory" );

        if (!inventoryNode.isEmpty())
        {
            m_inventory = Inventory::LoadFromXML( inventoryNode );
            m_inventory->SetOwner( this );
        }
    }
    
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Actor::Actor( const std::string& name )
    : Entity( name )
    , m_path( nullptr )
    , m_hostileTarget( nullptr )
    , m_friendlyTarget( nullptr )
    , m_inventory( nullptr )
    , m_visibilityRange( 6 )
{
    m_isActor = true;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Actor::Actor( const Actor& copy, const XMLNode& actorNode )
    : Entity( copy, actorNode )
    , m_path( copy.m_path )
    , m_hostileTarget( nullptr )
    , m_friendlyTarget( nullptr )
    , m_inventory( nullptr )
    , m_visibilityRange( copy.m_visibilityRange )
{


    if (!actorNode.isEmpty())
    {
        m_visibilityRange = GetIntProperty( actorNode, "visibilityRange", m_visibilityRange );

        XMLNode inventoryNode = actorNode.getChildNode( "Inventory" );

        if (!inventoryNode.isEmpty())
            m_inventory = Inventory::LoadFromXML( inventoryNode );
        else if (copy.m_inventory)
            m_inventory = new Inventory( *copy.m_inventory );
    }

    else if (copy.m_inventory)
        m_inventory = new Inventory( *copy.m_inventory );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Actor::~Actor()
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
void Actor::Startup()
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::InitInventory()
{
    m_inventory = new Inventory( this );
}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Actor::TestOneMove( MapDirection direction ) const
{
    MapPosition cellToMoveToPos;

    switch (direction)
    {
    case MD_EAST:
        cellToMoveToPos = m_mapPosition + MapPosition( 1, 0 );
        break;
    case MD_NORTHEAST:
        cellToMoveToPos = m_mapPosition + MapPosition( 1, 1 );
        break;
    case MD_NORTH:
        cellToMoveToPos = m_mapPosition + MapPosition( 0, 1 );
        break;
    case MD_NORTHWEST:
        cellToMoveToPos = m_mapPosition + MapPosition( -1, 1 );
        break;
    case MD_WEST:
        cellToMoveToPos = m_mapPosition + MapPosition( -1, 0 );
        break;
    case MD_SOUTHWEST:
        cellToMoveToPos = m_mapPosition + MapPosition( -1, -1 );
        break;
    case MD_SOUTH:
        cellToMoveToPos = m_mapPosition + MapPosition( 0, -1 );
        break;
    case MD_SOUTHEAST:
        cellToMoveToPos = m_mapPosition + MapPosition( 1, -1 );
        break;
    }


    // check if cell can be moved to
    Cell* cellToMoveTo = m_owningMap->GetCellAtPosition( cellToMoveToPos );
    if (!cellToMoveTo)
        return false;
    else if ( cellToMoveTo->GetType() != CT_AIR)
        return false;
    else if (cellToMoveTo->IsOccupied())
        return false;
    else if (cellToMoveTo->HasFeature())
    {
        Feature* feature = cellToMoveTo->GetFeature();
        if (feature->IsActive() && feature->ActiveBlocksMovement())
            return false;
        else if (!feature->IsActive() && feature->InactiveBlocksMovement())
            return false;
        else
            return true;
    }
    else
        return true;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::Save( XMLNode& parent, bool createChild )
{
    XMLNode nodeToModify = parent;
    if (createChild)
        nodeToModify = CreateNode( "Actor" );

    Entity::Save( nodeToModify, false );

    if (m_prototype && m_visibilityRange != ((Actor*)m_prototype)->m_visibilityRange)
        SetIntProperty( nodeToModify, "visibilityRange", m_visibilityRange, ((Actor*)m_prototype)->m_visibilityRange );

    if (m_inventory)
    {
        XMLNode inventory = CreateNode( "Inventory" );
        bool hadItems = m_inventory->Save( inventory );
        if (hadItems)
            AddChild( nodeToModify, inventory );
    }

    if (createChild)
        AddChild( parent, nodeToModify );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Actor::IsPositionVisible( const MapPosition& position )
{
    for (std::vector< Cell* >::iterator cellIter = m_visibleCells.begin(); cellIter != m_visibleCells.end(); ++cellIter)
    {
        Cell* cell = *cellIter;
        MapPosition cellPos = cell->GetMapPosition();

        if (cellPos == position)
            return true;
    }
    return false;
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::MakeOneMove( MapDirection direction )
{
    bool canMoveToPosition = TestOneMove( direction );

    if (canMoveToPosition)
    {
        Cell* myCell = m_owningMap->GetCellAtPosition( m_mapPosition );

        MapPosition cellToMoveToPos;

        switch (direction)
        {
        case MD_EAST:
            cellToMoveToPos = m_mapPosition + MapPosition( 1, 0 );
            break;
        case MD_NORTHEAST:
            cellToMoveToPos = m_mapPosition + MapPosition( 1, 1 );
            break;
        case MD_NORTH:
            cellToMoveToPos = m_mapPosition + MapPosition( 0, 1 );
            break;
        case MD_NORTHWEST:
            cellToMoveToPos = m_mapPosition + MapPosition( -1, 1 );
            break;
        case MD_WEST:
            cellToMoveToPos = m_mapPosition + MapPosition( -1, 0 );
            break;
        case MD_SOUTHWEST:
            cellToMoveToPos = m_mapPosition + MapPosition( -1, -1 );
            break;
        case MD_SOUTH:
            cellToMoveToPos = m_mapPosition + MapPosition( 0, -1 );
            break;
        case MD_SOUTHEAST:
            cellToMoveToPos = m_mapPosition + MapPosition( 1, -1 );
            break;
        }

        Cell* cellToMoveTo = m_owningMap->GetCellAtPosition( cellToMoveToPos );

        if (myCell->GetEntity() == this)
        {
            myCell->RemoveEntity();
            cellToMoveTo->SetEntity( this );
            m_mapPosition = cellToMoveTo->GetMapPosition();

            if (m_entityNameID == StringTable::GetStringID( "Player" ))
            {
                Inventory* inventory = cellToMoveTo->GetInventory();
                if (inventory)
                    inventory->AddMessageToMessageBar( this );
            }
        }
        else
            return;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::SetVisibleActors( Actors visibleActors )
{
    m_visibleActors.clear();
    m_visibleActors.resize( 0 );
    m_visibleActors.swap( visibleActors );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::SetVisibleCells( std::vector< Cell* > visibleCells )
{
    m_visibleCells.clear();
    m_visibleCells.resize( 0 );
    m_visibleCells.swap( visibleCells );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::CleanUpPointers( std::map< int, Entity* > hookupMap )
{
    if (m_inventory)
        m_inventory->CleanUpPointers( hookupMap );

    Entity::CleanUpPointers( hookupMap );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::DropInventory()
{
    if (m_inventory)
    {
        Cell* currentCell = m_owningMap->GetCellAtPosition( m_mapPosition );
        Inventory* cellInv = currentCell->GetInventory();
        
        if (!cellInv)
            currentCell->AddItem( nullptr, m_owningMap );

        m_inventory->MoveItemsToInventory( currentCell->GetInventory() );
    }
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::ProcessInput( InputSystem* inputSystem, double deltaSeconds )
{
    UNUSED( inputSystem );
    UNUSED( deltaSeconds );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::Update( double deltaSeconds, bool debugModeEnabled )
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
void Actor::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer,  const Font& font, const unsigned int& shaderID, bool debugModeEnabled )
{
    if (!renderer)
            return;

    UNUSED( fontRenderer );
    UNUSED( font );
    UNUSED( shaderID );
    UNUSED( debugModeEnabled );

}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

