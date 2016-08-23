//=================================================================================
// MeleeAttackBehavior.hpp
// Author: Tyler George
// Date  : September 22, 2015
//=================================================================================

#pragma once

#ifndef __included_MeleeAttackBehavior__
#define __included_MeleeAttackBehavior__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/AIBehaviors/BaseAIBehavior.hpp"


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------


////===========================================================================================
///===========================================================================================
// MeleeAttackBehavior Class
///===========================================================================================
////===========================================================================================
class MeleeAttackBehavior : public BaseAIBehavior
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    MeleeAttackBehavior( const std::string& name, const XMLNode& behaviorRoot );
    MeleeAttackBehavior( const MeleeAttackBehavior& copy );
    ~MeleeAttackBehavior();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    static BaseAIBehavior* CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot );
    float CalcUtility();
    InteractionEvent* Think( float& out_actionTime );
    BaseAIBehavior* Clone();

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Update
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Render
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Public Member Variables
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Static Variables
    ///---------------------------------------------------------------------------------
    static AIBehaviorRegistration s_MeleeAttackBehaviorRegistration;

private:
    ///---------------------------------------------------------------------------------
    /// Private Functions
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Private Member Variables
    ///---------------------------------------------------------------------------------
    IntRange m_damageRange;
    std::string m_attackVerb;
    float m_chanceToHit;
    float m_chanceToCrit;

    Entity* m_target;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif