//=================================================================================
// FollowBehavior.cpp
// Author: Tyler George
// Date  : September 18, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AIBehaviors/FollowBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "../Entities/Entity.hpp"
#include "Engine/Utilities/StringTable.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration FollowBehavior::s_followBehaviorRegistration( "Follow", &FollowBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FollowBehavior::FollowBehavior( const std::string& name, const XMLNode& behaviorRoot )
    : BaseAIBehavior( name, behaviorRoot )
    , m_range( 0 )
    , m_chanceToFollow( 1.0f )
    , m_target( nullptr )
{
    m_range = GetIntProperty( behaviorRoot, "range", 10 );
    m_distanceThreshold = GetIntProperty( behaviorRoot, "distanceThreshold", 5 );
    m_chanceToFollow = GetFloatProperty( behaviorRoot, "chanceToFollow", 1.0f );
    m_followName = GetStringProperty( behaviorRoot, "leader", "" );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FollowBehavior::FollowBehavior( const FollowBehavior& copy )
    : BaseAIBehavior( copy.m_name )
{
    m_chanceToCalcUtility = copy.m_chanceToCalcUtility;
    m_range = copy.m_range;
    m_distanceThreshold = copy.m_distanceThreshold;
    m_chanceToFollow = copy.m_chanceToFollow;
    m_target = copy.m_target;
    m_followName = copy.m_followName;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FollowBehavior::~FollowBehavior()
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
BaseAIBehavior* FollowBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new FollowBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float FollowBehavior::CalcUtility()
{
    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;

    Actors visibleActors = m_actor->GetVisibleActors();

    bool targetVisible = false;
    if (m_followName != "")
    {
        for (Actors::iterator actorIter = visibleActors.begin(); actorIter != visibleActors.end(); ++actorIter)
        {
            Actor* actor = *actorIter;
            if (StringTable::GetStringID(actor->GetName()) == StringTable::GetStringID( m_followName ) )
            {
                targetVisible = true;
                int distance = Map::CalculateManhattanDistance( m_actor->GetMapPosition(), actor->GetMapPosition() );
                if (distance <= m_range && distance > m_distanceThreshold)
                {
                    m_target = actor;
                    return (2.0f * distance);
                }
            }
        }
    }

    return 0.0f;

//     // favored target not visible or not set
//     Actor* mostFriendlyTarget = nullptr;
//     int mostFriendlyStatus = 0;
//     int distance = 0;
//     for (Actors::iterator actorIter = visibleActors.begin(); actorIter != visibleActors.end(); ++actorIter)
//     {
//         Actor* actor = *actorIter;
// 
//         // #KSH: Consider putting into multiple lines--this line of code has 5 dereferences and 5 function calls!
//         Faction* myFaction = m_actor->GetFaction();
//         Faction* actorFaction = actor->GetFaction();
//         int status = myFaction->GetRelationshipStatus( actorFaction->GetFactionID(), m_actor->GetEntityID() );
//         distance = Map::CalculateManhattanDistance( m_actor->GetMapPosition(), actor->GetMapPosition() );
// 
//         if (status > mostFriendlyStatus && distance <= m_range && !Map::IsAdjacent( m_actor->GetMapPosition(), actor->GetMapPosition() ))
//         {
//             mostFriendlyStatus = status;
//             mostFriendlyTarget = actor;
//         }
//     }
// 
//     if (mostFriendlyTarget)
//     {
//         m_target = mostFriendlyTarget;
//         return (3.0f + (float)(m_range - distance));
//     }
//     else
//         return 0.0f;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
InteractionEvent* FollowBehavior::Think( float& out_actionTime )
{


    bool Follow = GetRandomFloatZeroToOne() <= m_chanceToFollow ? true : false;

    if (!Follow)
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
BaseAIBehavior* FollowBehavior::Clone()
{
    BaseAIBehavior* clone = new FollowBehavior( *this );

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

