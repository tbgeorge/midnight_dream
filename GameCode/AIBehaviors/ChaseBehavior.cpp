//=================================================================================
// ChaseBehavior.cpp
// Author: Tyler George
// Date  : September 18, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AIBehaviors/ChaseBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "../Entities/Entity.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration ChaseBehavior::s_ChaseBehaviorRegistration( "Chase", &ChaseBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
ChaseBehavior::ChaseBehavior( const std::string& name, const XMLNode& behaviorRoot )
    : BaseAIBehavior( name, behaviorRoot )
    , m_range( 0 )
    , m_chanceToChase( 1.0f )
    , m_target( nullptr )
{
    m_range = GetIntProperty( behaviorRoot, "range", 5 );
    m_chanceToChase = GetFloatProperty( behaviorRoot, "chanceToChase", 1.0f );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
ChaseBehavior::ChaseBehavior( const ChaseBehavior& copy )
    : BaseAIBehavior( copy.m_name )
{
    m_chanceToCalcUtility = copy.m_chanceToCalcUtility;
    m_range = copy.m_range;
    m_chanceToChase = copy.m_chanceToChase;
    m_target = copy.m_target;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
ChaseBehavior::~ChaseBehavior()
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
BaseAIBehavior* ChaseBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new ChaseBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float ChaseBehavior::CalcUtility()
{
    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;

    Entity* hostileTarget = m_actor->GetHostileTarget();
    Actors visibleActors = m_actor->GetVisibleActors();

    bool targetVisible = false;
    if (hostileTarget)
    {
        for (Actors::iterator actorIter = visibleActors.begin(); actorIter != visibleActors.end(); ++actorIter)
        {
            Actor* actor = *actorIter;
            if (actor->GetEntityID() == hostileTarget->GetEntityID())
            {
                targetVisible = true;
                int distance = Map::CalculateManhattanDistance( m_actor->GetMapPosition(), hostileTarget->GetMapPosition() );
                if (distance <= m_range)
                {
                    m_target = hostileTarget;
                    return (2.0f + (float)(m_range - distance));
                }
            }
        }
    }
    
    // favored target not visible or not set
    Actor* mostHostileTarget = nullptr;
    int mostHostileStatus = 0;
    int distance = 0;
    for (Actors::iterator actorIter = visibleActors.begin(); actorIter != visibleActors.end(); ++actorIter)
    {
        Actor* actor = *actorIter;

		// #KSH: Consider putting into multiple lines--this line of code has 5 dereferences and 5 function calls!
        int status = m_actor->GetFaction()->GetRelationshipStatus( actor->GetFaction()->GetFactionID(), m_actor->GetEntityID() );
        distance = Map::CalculateManhattanDistance( m_actor->GetMapPosition(), actor->GetMapPosition() );

        if (status < mostHostileStatus && distance <= m_range && !Map::IsAdjacent( m_actor->GetMapPosition(), actor->GetMapPosition() ) )
        {
            mostHostileStatus = status;
            mostHostileTarget = actor;
        }
    }

    if (mostHostileTarget)
    {
        m_target = mostHostileTarget;
        return (2.0f + (float)(m_range - distance));
    }
    else 
        return 0.0f;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
InteractionEvent* ChaseBehavior::Think( float& out_actionTime )
{


    bool chase = GetRandomFloatZeroToOne() <= m_chanceToChase ? true : false;

    if (!chase)
    {
        out_actionTime = 0.2f;
        return nullptr;
    }

    if (!m_target)
    {
        out_actionTime = 0.2f;
        return nullptr;
    }

    Path* path = Pathfinder::CalculatePath( m_actor->GetMap(), m_actor, m_actor->GetMapPosition(), m_target->GetMapPosition(), true, true );
    MapPosition nextPosition = path->GetNextStep()->m_position;

    MapDirection dirToMove = Map::GetDirection( m_actor->GetMapPosition(), nextPosition );

    if (dirToMove != MD_INVALID)
    {
        if (m_actor->TestOneMove( dirToMove ))
            m_actor->MakeOneMove( dirToMove );
    }

    out_actionTime = 1.0f;
    return nullptr;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* ChaseBehavior::Clone()
{
    BaseAIBehavior* clone = new ChaseBehavior( *this );

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

