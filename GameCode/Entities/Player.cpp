
//=================================================================================
// Player.cpp
// Author: Tyler George
// Date  : September 1, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Entities/Player.hpp"
#include "GameCode/FieldOfViewCalculator.hpp"
#include "Engine/Utilities/Time.hpp"
#include "Engine/Utilities/DeveloperConsole.hpp"
#include "../CombatSystem.hpp"
#include "../MessageBar.hpp"
#include "Engine/Utilities/XMLHelper.hpp"
#include "Feature.hpp"
#include "Engine/Utilities/StringTable.hpp"
#include "../ItemFactory.hpp"
#include "../Game.hpp"


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
Player::Player( const XMLNode& playerNode )
    : Actor( playerNode )
{
    m_numTurnsTaken = GetIntProperty( playerNode, "numTurnsTaken", 0 );
    m_actorsKilled = GetIntProperty( playerNode, "numEnemiesKilled", 0 );
    m_symbol = "@";
    m_currentAction = PA_UNSPECIFIED;
    m_damageRange = GetIntIntervalProperty( playerNode, "damageRange", IntRange( 2, 4 ) );
    m_visibilityRange = GetIntProperty( playerNode, "visibilityRange", 10 );
    m_faction = new Faction( *Faction::GetMasterFactionFromName( "player" ) );

    if (!m_inventory)
        m_inventory = new Inventory( this );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Player::Player()
    : Actor( "Player" )
    , m_numTurnsTaken( 0 )
    , m_actorsKilled( 0 )
    , m_target( nullptr )
{
    m_symbol = "@";
    m_maxHealth = 100;
    m_health = m_maxHealth;
    m_currentAction = PA_UNSPECIFIED;
    m_damageRange = IntRange( 2, 4 );
    m_faction = new Faction( *Faction::GetMasterFactionFromName( "player" ) );
    
    m_visibilityRange = 6;


    if ( !m_inventory )
        m_inventory = new Inventory( this );

    ItemFactory* swordFactory = ItemFactory::FindFactoryByName( "Rusty Sword" );
    Item* newSword = swordFactory->SpawnItem( XMLNode::emptyNode() );

    Game::AddEntity( newSword );
    m_inventory->AddToBackpack( newSword, false );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Player::~Player()
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
void Player::Startup()
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
void Player::Save( XMLNode& parent, bool createChild )
{
    XMLNode nodeToModify = parent;
    if (createChild)
        nodeToModify = CreateNode( "Player" );

    Actor::Save( nodeToModify, false );

    SetIntProperty( nodeToModify, "numTurnsTaken", m_numTurnsTaken, 0 );
    SetIntProperty( nodeToModify, "numEnemiesKilled", m_actorsKilled, 0 );


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
InteractionEvent* Player::Think( float& out_actionTime )
{
    FieldOfViewCalculator::CalculateFOV_BFR( (Actor*) this, m_visibilityRange, m_owningMap, true );

    out_actionTime = 0.0f;
    InteractionEvent* event = nullptr;


    if (m_currentAction == PA_DETERMINE)
    {
        if (m_actionDirection == MD_INVALID)
            m_currentAction = PA_UNSPECIFIED;

        else
        {
            bool movePossible = TestOneMove( m_actionDirection );
            if (movePossible)
                m_currentAction = PA_MOVE;
            else
            {
                Cell* cell = m_owningMap->GetCellInDirection( m_mapPosition, m_actionDirection );
                if (cell->IsOccupied())
                {
                    m_currentAction = PA_ATTACK;
                    m_target = cell->GetEntity();
                }
                else if (cell->HasFeature())
                {
                    Feature* feature = cell->GetFeature();
                    if (feature->GetMaxHealth() != 0)
                    {
                        m_currentAction = PA_ATTACK;
                        m_target = feature;
                    }
                    else
                    {
                        Message msg;
                        msg.m_instigator = m_entityName;
                        msg.m_position = m_mapPosition;
                        msg.m_text = "You discovered a " + feature->GetName() + ". ";
                        MessageBar::AddMessage( msg );
                    }
                }
            }
        }
    }

    switch (m_currentAction)
    {
    case PA_UNSPECIFIED:
        out_actionTime = 0.0f;
        break;
    case PA_MOVE:
        MakeOneMove( m_actionDirection );  
        out_actionTime = 1.0f;
        break;
    case PA_ATTACK:
    {
        AttackData data;
        data.attacker = this;
        data.target = m_target;
        data.attackVerb = "hit";
        data.damageRange = m_damageRange;
        data.armorRange = IntRange::ZERO;
        data.primaryWeapon = nullptr;

        if (m_inventory)
        {
            data.damageRange += m_inventory->GetEquippedDamageRange();
            data.primaryWeapon = GetInventory()->GetEquipment()[ES_PRIMARY];
        }
        if (((Actor*)m_target)->GetInventory())
            data.armorRange = ((Actor*)m_target)->GetInventory()->GetEquippedArmorRange();

        data.chanceToHit = 0.75f;
        data.chanceToCrit = 0.05f;

        out_actionTime = 1.0f;
        AttackResult result = CombatSystem::PerformMeleeAttack( data );

        if (m_target->IsActor())
        {
            // create event
            event = new InteractionEvent();
            event->m_location = m_mapPosition;
            event->m_instigator = this;
            event->m_other = m_target;
            event->m_type = INT_TYPE_DAMAGE;
            event->m_relevantValue = result.damageDone;
        }



        if (result.targetDied)
        {
            if (m_target->IsActor())
                m_actorsKilled++;

            m_target->RemoveFromWorld();
            for (std::vector< Actor* >::iterator actorIter = m_visibleActors.begin(); actorIter != m_visibleActors.end();)
            {
                Entity* entity = *actorIter;
                if (entity->HasDied())
                {
                    actorIter = m_visibleActors.erase(actorIter);
                }
                else
                    actorIter++;
            }
        }

        // check if wielding blood sword
        Equipment currentEquipment = m_inventory->GetEquipment();
        Item* currentPrimary = currentEquipment[ES_PRIMARY];
        if ( currentPrimary && StringTable::GetStringID( currentPrimary->GetName() ) == StringTable::GetStringID( "Blood Sword" ) )
        {
            int healingRecieved = result.damageDone / 2;
            ApplyHealing( healingRecieved );

            Message msg;
            msg.m_instigator = m_entityName;
            msg.m_position = m_mapPosition;
            msg.m_text = "The Blood Sword healed you for " + std::to_string( healingRecieved ) + " health points. ";
            MessageBar::AddMessage( msg );

        }

        break;
    }

    case PA_PICKUPITEM:
    {
        Cell* cell = m_owningMap->GetCellAtPosition( m_mapPosition );
        if (cell)
        {
            Inventory* cellInventory = cell->GetInventory();
            if (cellInventory)
            {
                if (!m_inventory)
                    m_inventory = new Inventory( this );

                bool movedItems = cellInventory->MoveItemsToInventory( m_inventory );
                if ( movedItems )
                    out_actionTime = 1.0f;
            }
        }
        break;
    }

    case PA_QUAFF:
        if (m_inventory)
        {
            Items heldPotions = m_inventory->GetAllItemsOfType( IT_POTION );
            if (heldPotions.size() != 0)
            {
                int randomPotionIndex = GetRandomIntLessThan( heldPotions.size() );
                Item* potionToUse = heldPotions[randomPotionIndex];
                IntRange healthAdjustmentRange = potionToUse->GetAdjustHealth();
                int healingDone = ApplyHealing( GetRandomValueInIntRangeInclusive( healthAdjustmentRange ) );

                Message msg;
                msg.m_instigator = m_entityName;
                msg.m_position = m_mapPosition;
                msg.m_text = potionToUse->GetName() + " healed you for " + std::to_string( healingDone ) + " health points. ";
                MessageBar::AddMessage( msg );
                m_inventory->RemoveItem( potionToUse );
                potionToUse->SetDead( true );
                out_actionTime = 1.0f;
            }
        }
        break;

    case PA_INTERACT_WITH_FEATURE:
        std::vector< Feature* > adjacentFeatures = m_owningMap->GetAdjacentFeatures( m_mapPosition );
        if (adjacentFeatures.size() != 0)
        {
            int randomFeatureToToggle = GetRandomIntLessThan( adjacentFeatures.size() );
            Feature* featureToToggle = adjacentFeatures[randomFeatureToToggle];
            std::string verb = featureToToggle->ToggleActive();

            Message msg;
            msg.m_instigator = m_entityName;
            msg.m_position = m_mapPosition;
            
            msg.m_text = "You " + verb + " the " + featureToToggle->GetName() + ". ";
            MessageBar::AddMessage( msg );
            out_actionTime = 1.0f;
            FieldOfViewCalculator::CalculateFOV_BFR( (Actor*) this, m_visibilityRange, m_owningMap, true );
        }

        break;
    }


    m_currentAction = PA_UNSPECIFIED;
    FieldOfViewCalculator::CalculateFOV_BFR( (Actor*) this, m_visibilityRange, m_owningMap, true );

    if ( out_actionTime != 0.0f )
        m_numTurnsTaken++;

    MessageBar::SetClearCurrent( true );
    return event;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Player::HandleEvent( InteractionEvent* event )
{
    UNUSED( event );
    return;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Player::Reset()
{
    delete m_path;
    m_path = nullptr;
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Player::ProcessInput( InputSystem* inputSystem, double deltaSeconds )
{
    if (inputSystem->WasKeyJustReleased( 'H' ) || inputSystem->WasKeyJustReleased( 'A' ))
    {
        m_currentAction = PA_DETERMINE;
        m_actionDirection = MD_WEST;
    }

    else if (inputSystem->WasKeyJustReleased( 'J' ) || inputSystem->WasKeyJustReleased( 'S' ))
    {
        m_currentAction = PA_DETERMINE;
        m_actionDirection = MD_SOUTH;
    }

    else if (inputSystem->WasKeyJustReleased( 'K' ) || inputSystem->WasKeyJustReleased( 'W' ))
    {
        m_currentAction = PA_DETERMINE;
        m_actionDirection = MD_NORTH;
    }

    else if (inputSystem->WasKeyJustReleased( 'L' ) || inputSystem->WasKeyJustReleased( 'D' ))
    {
        m_currentAction = PA_DETERMINE;
        m_actionDirection = MD_EAST;
    }

    else if (inputSystem->WasKeyJustReleased( 'B' ) || inputSystem->WasKeyJustReleased( 'Z' ))
    {
        m_currentAction = PA_DETERMINE;
        m_actionDirection = MD_SOUTHWEST;
    }

    else if (inputSystem->WasKeyJustReleased( 'N' ) || inputSystem->WasKeyJustReleased( 'C' ))
    {
        m_currentAction = PA_DETERMINE;
        m_actionDirection = MD_SOUTHEAST;
    }

    else if (inputSystem->WasKeyJustReleased( 'Y' ) || inputSystem->WasKeyJustReleased( 'Q' ))
    {
        m_currentAction = PA_DETERMINE;
        m_actionDirection = MD_NORTHWEST;
    }

    else if (inputSystem->WasKeyJustReleased( 'U' ) || inputSystem->WasKeyJustReleased( 'E' ))
    {
        m_currentAction = PA_DETERMINE;
        m_actionDirection = MD_NORTHEAST;
    }

    else if (inputSystem->WasKeyJustReleased( VK_OEM_COMMA ) || inputSystem->WasKeyJustReleased( 'F' ))
    {
        m_currentAction = PA_PICKUPITEM;
        m_actionDirection = MD_INVALID;
    }

    else if (inputSystem->WasKeyJustReleased( 'R' ))
    {
        m_currentAction = PA_QUAFF;
        m_actionDirection = MD_INVALID;
    }

    else if (inputSystem->WasKeyJustReleased( 'V' ))
    {
        m_currentAction = PA_INTERACT_WITH_FEATURE;
        m_actionDirection = MD_INVALID;
    }

    else
    {
        m_currentAction = PA_UNSPECIFIED;
        m_actionDirection = MD_INVALID;
    }

    if (inputSystem->WasKeyJustReleased( 'P' ))
    {
        if (m_path == nullptr)
        {
            MapPosition goal = m_owningMap->GetRandomOpenPosition();
            m_path = Pathfinder::CalculatePath( m_owningMap, this, m_mapPosition, goal, false, true );
        }
        else if (m_path->m_isFinished)
        {
            delete m_path;
            m_path = nullptr;
        }
        else
            m_path->ProcessOneStep();

    }

    if (inputSystem->IsKeyDown( VK_SHIFT ))
    {
        if (inputSystem->WasKeyJustReleased( 'P' ))
        {
            double startTime = GetCurrentSeconds();
            for (int i = 0; i < 10000; ++i)
            {
                MapPosition pathGoal = m_owningMap->GetRandomOpenPosition();
                Path* path = Pathfinder::CalculatePath( m_owningMap, this, GetMapPosition(), pathGoal, true, true );
                delete(path);

                if (i % 1000 == 0)
                {
                    ConsolePrintf( "Path: %d \n", i );
                }
            }
            double endTime = GetCurrentSeconds();

            double elapsedTime = endTime - startTime;

            double timePerPath = elapsedTime / 10000.0;
            double pathsPerSecond = 1.0 / timePerPath;

            DeveloperConsole::WriteLine( "10,000 A* paths completed in " + std::to_string( elapsedTime ) + "s.", INFO_TEXT_COLOR );
            DeveloperConsole::WriteLine( "    - " + std::to_string( timePerPath ) + "s per path.", INFO_TEXT_COLOR );
            DeveloperConsole::WriteLine( "    - " + std::to_string( pathsPerSecond ) + " paths per second.", INFO_TEXT_COLOR );
        }
    }

    if (inputSystem->WasKeyJustReleased( 'M' ))
    {
        Cells* cells = m_owningMap->GetCells();
        for (Cells::iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter)
        {
            Cell& cell = *cellIter;

            cell.SetVisible( true );
        }
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Player::Update( double deltaSeconds, bool debugModeEnabled )
{
}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Player::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, bool debugModeEnabled )
{
    if (!renderer)
        return;

    Cell* myCell = m_owningMap->GetCellAtPosition( m_mapPosition );

    AABB2D renderBounds = myCell->GetRenderBounds();

    Vector3 pos( renderBounds.m_mins.x, renderBounds.m_mins.y, 9.0f );
    float fontWidth = renderBounds.m_maxs.x - renderBounds.m_mins.x;
    float fontHeight = renderBounds.m_maxs.y - renderBounds.m_mins.y;
    fontRenderer->DrawFontTextWH( fontWidth, fontHeight, font, m_symbol, pos );

    if (debugModeEnabled)
    {
        if (m_path)
        {
            Cell* goalCell = m_owningMap->GetCellAtPosition( m_path->m_goal );
            AABB2D renderBounds = goalCell->GetRenderBounds();

            renderer->DrawColoredQuad( NULL, renderBounds.m_mins, renderBounds.m_maxs, 8.0f, Rgba::GREEN );

            for (PathNodeMap::const_iterator nodeIter = m_path->m_openList.begin(); nodeIter != m_path->m_openList.end(); ++nodeIter)
            {
                PathNode* node = nodeIter->second;
                Cell* openCell = m_owningMap->GetCellAtPosition( node->m_position );
                AABB2D openCellRenderBounds = openCell->GetRenderBounds();
                renderer->DrawColoredQuad( NULL, openCellRenderBounds.m_mins, openCellRenderBounds.m_maxs, 7.0f, Rgba( 0.0f, 0.0f, 1.0f, 0.5f ) );

            }

            for (PathNodeMap::const_iterator nodeIter = m_path->m_closedList.begin(); nodeIter != m_path->m_closedList.end(); ++nodeIter)
            {
                PathNode* node = nodeIter->second;
                Cell* closedCell = m_owningMap->GetCellAtPosition( node->m_position );
                AABB2D closedCellRenderBounds = closedCell->GetRenderBounds();
                renderer->DrawColoredQuad( NULL, closedCellRenderBounds.m_mins, closedCellRenderBounds.m_maxs, 7.0f, Rgba( 1.0f, 0.0f, 0.0f, 0.5f ) );
            }

            bool finished = false;
            PathNode* node = m_path->m_active;
            while (!finished && node)
            {
                Cell* cell = m_owningMap->GetCellAtPosition( node->m_position );
                AABB2D cellRenderBounds = cell->GetRenderBounds();
                renderer->DrawColoredQuad( NULL, cellRenderBounds.m_mins, cellRenderBounds.m_maxs, 8.0f, Rgba( 1.0f, 0.0f, 1.0f, 0.5f ) );

                if (node->m_parent)
                {
                    node = node->m_parent;

                }
                else
                {
                    finished = true;
                }                
            }
        }
    }


}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

