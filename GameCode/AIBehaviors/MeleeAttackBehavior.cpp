//=================================================================================
// MeleeAttackBehavior.cpp
// Author: Tyler George
// Date  : September 18, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AIBehaviors/MeleeAttackBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "../CombatSystem.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration MeleeAttackBehavior::s_MeleeAttackBehaviorRegistration( "Melee", &MeleeAttackBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MeleeAttackBehavior::MeleeAttackBehavior( const std::string& name, const XMLNode& behaviorRoot )
    : BaseAIBehavior( name, behaviorRoot )
    , m_chanceToHit( 1.0f )
    , m_chanceToCrit( 0.2f )
    , m_attackVerb("hits")
    , m_target( nullptr )
{
    m_damageRange = GetIntIntervalProperty( behaviorRoot, "damage" );
    m_chanceToHit = GetFloatProperty( behaviorRoot, "chanceToHit", 1.0f );
    m_chanceToCrit = GetFloatProperty( behaviorRoot, "chanceToCrit", 0.0f );
    m_attackVerb = GetStringProperty( behaviorRoot, "attackVerb", "hits" );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MeleeAttackBehavior::MeleeAttackBehavior( const MeleeAttackBehavior& copy )
    : BaseAIBehavior( copy.m_name )
{
    m_chanceToCalcUtility = copy.m_chanceToCalcUtility;
    m_damageRange = copy.m_damageRange;
    m_chanceToHit = copy.m_chanceToHit;
    m_chanceToCrit = copy.m_chanceToCrit;
    m_attackVerb = copy.m_attackVerb;
    m_target = copy.m_target;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MeleeAttackBehavior::~MeleeAttackBehavior()
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
BaseAIBehavior* MeleeAttackBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new MeleeAttackBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float MeleeAttackBehavior::CalcUtility()
{
    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;

    Entity* hostileTarget = m_actor->GetHostileTarget();
    Actors visibleActors = m_actor->GetVisibleActors();

    bool targetVisible = false;
    if (hostileTarget)
    {
        if (Map::IsAdjacent( m_actor->GetMapPosition(), hostileTarget->GetMapPosition() ))
        {
            targetVisible = true;
            m_target = hostileTarget;
            return 15.0f;
        }
    }

    // favored target not visible or not set
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

    if (mostHostileTarget)
    {
        m_target = mostHostileTarget;
        return 10.0f;
    }
    else
        return 0.0f;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
InteractionEvent* MeleeAttackBehavior::Think( float& out_actionTime )
{


//     bool MeleeAttack = GetRandomFloatZeroToOne() <= m_chanceToMeleeAttack ? true : false;

//     if (!MeleeAttack)
//         return 0.2f;
    AttackData data;
    data.attacker = m_actor;
    data.target = m_target;
    data.chanceToCrit = m_chanceToCrit;
    data.chanceToHit = m_chanceToHit;
    data.damageRange = m_damageRange;
    data.armorRange = IntRange::ZERO;
    data.primaryWeapon = nullptr; 

    if (m_actor->GetInventory())
    {
        data.damageRange += m_actor->GetInventory()->GetEquippedDamageRange();
        data.armorRange = m_actor->GetInventory()->GetEquippedArmorRange();
        data.primaryWeapon = m_actor->GetInventory()->GetEquipment()[ES_PRIMARY];

    }
    data.attackVerb = m_attackVerb;
    
    AttackResult result = CombatSystem::PerformMeleeAttack( data );
    if (result.didHit && result.targetDied)
        m_target->RemoveFromWorld();

    InteractionEvent* event = new InteractionEvent();
    event->m_location = m_actor->GetMapPosition();
    event->m_instigator = m_actor;
    event->m_other = m_target;
    event->m_type = INT_TYPE_DAMAGE;
    event->m_relevantValue = result.damageDone;

    out_actionTime = 1.0f;
    return event;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* MeleeAttackBehavior::Clone()
{
    BaseAIBehavior* clone = new MeleeAttackBehavior( *this );

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

