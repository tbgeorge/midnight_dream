//=================================================================================
// DrainLifeBehavior.hpp
// Author: Tyler George
// Date  : October 8, 2015
//=================================================================================

#pragma once

#ifndef __included_DrainLifeBehavior__
#define __included_DrainLifeBehavior__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/AIBehaviors/BaseAIBehavior.hpp"
#include "Engine/Math/IntRange.hpp"


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------


////===========================================================================================
///===========================================================================================
// DrainLifeBehavior Class
///===========================================================================================
////===========================================================================================
class DrainLifeBehavior : public BaseAIBehavior
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    DrainLifeBehavior( const std::string& name, const XMLNode& behaviorRoot );
    DrainLifeBehavior( const DrainLifeBehavior& copy );
    ~DrainLifeBehavior();

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
    static AIBehaviorRegistration s_drainLifeBehaviorRegistration;

private:
    ///---------------------------------------------------------------------------------
    /// Private Functions
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Private Member Variables
    ///---------------------------------------------------------------------------------
    IntRange m_drainAmount;
    float m_chanceToDrain;
    Entity* m_target;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif