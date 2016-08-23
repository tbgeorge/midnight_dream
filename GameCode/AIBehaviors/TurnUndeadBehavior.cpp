//=================================================================================
// TurnUndeadBehavior.cpp
// Author: Tyler George
// Date  : October 8, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AIBehaviors/TurnUndeadBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "../CombatSystem.hpp"
#include "../MessageBar.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration TurnUndeadBehavior::s_turnUndeadBehaviorRegistration( "TurnUndead", &TurnUndeadBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
TurnUndeadBehavior::TurnUndeadBehavior( const std::string& name, const XMLNode& behaviorRoot )
    : BaseAIBehavior( name, behaviorRoot )
    , m_factionName( "undead" )
    , m_damage( IntRange::ZERO )
    , m_radius( 0 )

{
    m_factionName = GetStringProperty( behaviorRoot, "faction", m_factionName, true );
    m_damage = GetIntIntervalProperty( behaviorRoot, "damage", m_damage );
    m_radius = GetIntProperty( behaviorRoot, "radius", m_radius );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
TurnUndeadBehavior::TurnUndeadBehavior( const TurnUndeadBehavior& copy )
    : BaseAIBehavior( copy.m_name )
{
    m_chanceToCalcUtility = copy.m_chanceToCalcUtility;
    m_factionName = copy.m_factionName;
    m_damage = copy.m_damage;
    m_radius = copy.m_radius;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
TurnUndeadBehavior::~TurnUndeadBehavior()
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
BaseAIBehavior* TurnUndeadBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new TurnUndeadBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float TurnUndeadBehavior::CalcUtility()
{
    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;

    Actors visibleActors = m_actor->GetVisibleActors();

    Faction* masterFaction = Faction::GetMasterFactionFromName( m_factionName );

    int numEnemiesInRange = 0;
    for (Actors::iterator actorIter = visibleActors.begin(); actorIter != visibleActors.end(); ++actorIter)
    {
        Actor* actor = *actorIter;
        if (Map::CalculateManhattanDistance( m_actor->GetMapPosition(), actor->GetMapPosition() ) <= m_radius)
        {
            if (actor->GetFaction()->GetFactionID() == masterFaction->GetFactionID())
                numEnemiesInRange++;
        }
    }

    if (numEnemiesInRange >= 3)
        return 10.0f * numEnemiesInRange;

    return 0.0f;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
InteractionEvent* TurnUndeadBehavior::Think( float& out_actionTime )
{

    Actors visibleActors = m_actor->GetVisibleActors();
    Faction* masterFaction = Faction::GetMasterFactionFromName( m_factionName );

    Actors enemiesToDamage;

    Message msg;
    msg.m_instigator = m_actor->GetName();
    msg.m_position = m_actor->GetMapPosition();
    msg.m_text = "The " + m_actor->GetName() + " glows and cries vengeance against the " + m_factionName + "!";
    MessageBar::AddMessage( msg );

    Entity* target = nullptr;

    for (Actors::iterator actorIter = visibleActors.begin(); actorIter != visibleActors.end(); ++actorIter)
    {
        Actor* actor = *actorIter;
        if (Map::CalculateManhattanDistance( m_actor->GetMapPosition(), actor->GetMapPosition() ) <= m_radius)
        {
            if (actor->GetFaction()->GetFactionID() == masterFaction->GetFactionID())
            {
                AttackData data;
                data.attacker = m_actor;
                data.target = actor;
                data.chanceToCrit = 0.0f;
                data.chanceToHit = 1.0f;
                data.damageRange = m_damage;
                data.armorRange = IntRange::ZERO;
                data.attackVerb = "burned";
                data.primaryWeapon = nullptr;


                AttackResult result = CombatSystem::PerformMeleeAttack( data );
                if (result.didHit && result.targetDied)
                    actor->RemoveFromWorld();

                target = actor;
            }
        }
    }

    // need to fix this to return multiple events
    InteractionEvent* event = new InteractionEvent();
    event->m_location = m_actor->GetMapPosition();
    event->m_instigator = m_actor;
    event->m_other = target;
    event->m_type = INT_TYPE_DAMAGE;
    event->m_relevantValue = 3;

    out_actionTime = 1.0f;
    return event;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* TurnUndeadBehavior::Clone()
{
    BaseAIBehavior* clone = new TurnUndeadBehavior( *this );

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

