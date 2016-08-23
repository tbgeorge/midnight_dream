//=================================================================================
// PickupItemBehavior.cpp
// Author: Tyler George
// Date  : October 3, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AIBehaviors/PickupItemBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration PickupItemBehavior::s_PickupItemBehaviorRegistration( "PickupItem", &PickupItemBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
PickupItemBehavior::PickupItemBehavior( const std::string& name, const XMLNode& behaviorRoot )
    : BaseAIBehavior( name, behaviorRoot )
    , m_range( 0 )
    , m_chanceToPickupItem( 1.0f )
    , m_targetPos( MapPosition( -1, -1 ) )
    , m_pickupFromCurrentTile( false )
{
    m_chanceToPickupItem = GetFloatProperty( behaviorRoot, "chanceToPickupItem", 1.0f );
    m_range = GetIntProperty( behaviorRoot, "range", 5 );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
PickupItemBehavior::PickupItemBehavior( const PickupItemBehavior& copy )
    : BaseAIBehavior( copy.m_name )
{
    m_chanceToCalcUtility = copy.m_chanceToCalcUtility;
    m_range = copy.m_range;
    m_chanceToPickupItem = copy.m_chanceToPickupItem;
    m_targetPos = copy.m_targetPos;
    m_pickupFromCurrentTile = copy.m_pickupFromCurrentTile;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
PickupItemBehavior::~PickupItemBehavior()
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
BaseAIBehavior* PickupItemBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new PickupItemBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float PickupItemBehavior::CalcUtility()
{
    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;

    m_pickupFromCurrentTile = false;

    Cell* cell = m_actor->GetMap()->GetCellAtPosition( m_actor->GetMapPosition() );

    Inventory* inv = cell->GetInventory();
    if (inv && !inv->IsEmpty())
    {
        Items itemList = inv->GetItemList();
        Inventory* actorInventory = m_actor->GetInventory();

        for (Items::iterator itemIter = itemList.begin(); itemIter != itemList.end(); ++itemIter)
        {
            Item* item = *itemIter;
            
            if (actorInventory)
            {
                bool itemIsBetter = actorInventory->CheckForBetterEquip( item );

                // item is better than equipped
                if (itemIsBetter)
                {
                    m_pickupFromCurrentTile = true;
                    return 10.0f;
                }
            }
            // not holding any items
            m_pickupFromCurrentTile = true;
            return 15.0f;
        }
    }

    std::vector< Cell* > visibleCells = m_actor->GetVisibleCells();

    MapPosition closestItemPos = MapPosition( -1, -1 );

    for (std::vector< Cell* >::iterator cellIter = visibleCells.begin(); cellIter != visibleCells.end(); ++cellIter)
    {
        Cell* cell = *cellIter;

        if (cell->GetInventory() && !cell->GetInventory()->IsEmpty())
            closestItemPos = cell->GetMapPosition();
    }

    if (closestItemPos == MapPosition( -1, -1 ))
        return 0.0f;

    m_targetPos = closestItemPos;
    int distance = Map::CalculateManhattanDistance( m_actor->GetMapPosition(), m_targetPos );

    m_pickupFromCurrentTile = false;
    return (3.0f + (float)(m_range - distance));

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
InteractionEvent* PickupItemBehavior::Think( float& out_actionTime )
{
    bool pickupItem = GetRandomFloatZeroToOne() <= m_chanceToPickupItem ? true : false;

    if (!pickupItem)
    {
        out_actionTime = 0.2f;
        return nullptr;
    }

    if (m_pickupFromCurrentTile)
    {
        Cell* cell = m_actor->GetMap()->GetCellAtPosition( m_actor->GetMapPosition() );

        if (!m_actor->GetInventory())
            m_actor->InitInventory();

        cell->GetInventory()->MoveItemsToInventory( m_actor->GetInventory() );

        out_actionTime = 1.0f;
        return nullptr;
    }

    else
    {
        Path* path = Pathfinder::CalculatePath( m_actor->GetMap(), m_actor, m_actor->GetMapPosition(), m_targetPos, true, true );
        MapPosition nextPosition = path->GetNextStep()->m_position;

        MapDirection dirToMove = Map::GetDirection( m_actor->GetMapPosition(), nextPosition );

        if (dirToMove != MD_INVALID)
        {
            if (m_actor->TestOneMove( dirToMove ))
            {
                m_actor->MakeOneMove( dirToMove );
                out_actionTime = 1.0f;
                return nullptr;
            }
        }

    }

    out_actionTime = 0.0f;
    return nullptr;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* PickupItemBehavior::Clone()
{
    BaseAIBehavior* clone = new PickupItemBehavior( *this );

    return clone;
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


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

