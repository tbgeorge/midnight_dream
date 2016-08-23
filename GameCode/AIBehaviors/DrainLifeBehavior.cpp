//=================================================================================
// DrainLifeBehavior.cpp
// Author: Tyler George
// Date  : October 8, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AIBehaviors/DrainLifeBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "../CombatSystem.hpp"
#include "../MessageBar.hpp"
#include "Engine/Utilities/StringTable.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration DrainLifeBehavior::s_drainLifeBehaviorRegistration( "DrainLife", &DrainLifeBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
DrainLifeBehavior::DrainLifeBehavior( const std::string& name, const XMLNode& behaviorRoot )
    : BaseAIBehavior( name, behaviorRoot )
    , m_drainAmount( IntRange::ZERO )
    , m_chanceToDrain( 1.0f )
    , m_target( nullptr )
{
    m_drainAmount = GetIntIntervalProperty( behaviorRoot, "drainAmount", m_drainAmount );
    m_chanceToDrain = GetFloatProperty( behaviorRoot, "chanceToDrain", m_chanceToDrain );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
DrainLifeBehavior::DrainLifeBehavior( const DrainLifeBehavior& copy )
    : BaseAIBehavior( copy.m_name )
{
    m_chanceToCalcUtility = copy.m_chanceToCalcUtility;
    m_drainAmount = copy.m_drainAmount;
    m_chanceToDrain = copy.m_chanceToDrain;
    m_target = copy.m_target;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
DrainLifeBehavior::~DrainLifeBehavior()
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
BaseAIBehavior* DrainLifeBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new DrainLifeBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float DrainLifeBehavior::CalcUtility()
{
    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;

    Actors visibleActors = m_actor->GetVisibleActors();

    Actor* mostHostileTarget = nullptr;
    int mostHostileStatus = 0;
    for (Actors::iterator actorIter = visibleActors.begin(); actorIter != visibleActors.end(); ++actorIter)
    {
        Actor* actor = *actorIter;
        int status = m_actor->GetFaction()->GetRelationshipStatus( actor->GetFaction()->GetFactionID(), m_actor->GetEntityID() );

        if (status < mostHostileStatus && Map::IsAdjacent( m_actor->GetMapPosition(), actor->GetMapPosition() ))
        {
            mostHostileStatus = status;
            mostHostileTarget = actor;
        }
    }

    int healthDeficit = m_actor->GetMaxHealth() - m_actor->GetHealth();
    if (mostHostileTarget)
    {
        m_target = mostHostileTarget;
        return 2.0f * healthDeficit;
    }
    else
        return 0.0f;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
InteractionEvent* DrainLifeBehavior::Think( float& out_actionTime )
{


    //     bool DrainLife = GetRandomFloatZeroToOne() <= m_chanceToDrainLife ? true : false;

    //     if (!DrainLife)
    //         return 0.2f;
    AttackData data;
    data.attacker = m_actor;
    data.target = m_target;
    data.chanceToCrit = 0.0f;
    data.chanceToHit = m_chanceToDrain;
    data.damageRange = m_drainAmount;
    data.armorRange = IntRange::ZERO;
    data.primaryWeapon = nullptr;


    data.attackVerb = "touches";

    AttackResult result = CombatSystem::PerformMeleeAttack( data );

    int amountToHeal = result.damageDone;
    m_actor->ApplyHealing( amountToHeal );

    if (result.didHit && result.targetDied)
        m_target->RemoveFromWorld();

    InteractionEvent* event = new InteractionEvent();
    event->m_location = m_actor->GetMapPosition();
    event->m_instigator = m_actor;
    event->m_other = m_target;
    event->m_type = INT_TYPE_DAMAGE;
    event->m_relevantValue = result.damageDone;

    // additional messages

    Message playerDrained;
    playerDrained.m_instigator = m_actor->GetName();
    playerDrained.m_position = m_actor->GetMapPosition();
    playerDrained.m_text = "You feel Drained.";

    Message npcDrained;
    npcDrained.m_instigator = m_actor->GetName();
    npcDrained.m_position = m_actor->GetMapPosition();
    npcDrained.m_text = "The " + data.target->GetName() + " turns paler.";

    Message selfHealed;
    selfHealed.m_instigator = m_actor->GetName();
    selfHealed.m_position = m_actor->GetMapPosition();
    selfHealed.m_text = "The " + m_actor->GetName() + " appears stronger.";

    if (StringTable::GetStringID( data.target->GetName() ) == StringTable::GetStringID( "Player" ))
        MessageBar::AddMessage( playerDrained );
    else
        MessageBar::AddMessage( npcDrained );

    MessageBar::AddMessage( selfHealed );

    out_actionTime = 1.0f;
    return event;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* DrainLifeBehavior::Clone()
{
    BaseAIBehavior* clone = new DrainLifeBehavior( *this );

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

