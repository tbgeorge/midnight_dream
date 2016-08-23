// //=================================================================================
// // InspectItemBehavior.hpp
// // Author: Tyler George
// // Date  : October 3, 2015
// //=================================================================================
// 
// #pragma once
// 
// #ifndef __included_InspectItemBehavior__
// #define __included_InspectItemBehavior__
// 
// ///---------------------------------------------------------------------------------
// /// Includes
// ///---------------------------------------------------------------------------------
// #include "GameCode/AIBehaviors/BaseAIBehavior.hpp"
// 
// 
// ///---------------------------------------------------------------------------------
// /// Typedefs
// ///---------------------------------------------------------------------------------
// 
// 
// ////===========================================================================================
// ///===========================================================================================
// // InspectItemBehavior Class
// ///===========================================================================================
// ////===========================================================================================
// class InspectItemBehavior : public BaseAIBehavior
// {
// public:
//     ///---------------------------------------------------------------------------------
//     /// Constructors/Destructors
//     ///---------------------------------------------------------------------------------
//     InspectItemBehavior( const std::string& name, const XMLNode& behaviorRoot );
//     InspectItemBehavior( const InspectItemBehavior& copy );
//     ~InspectItemBehavior();
// 
//     ///---------------------------------------------------------------------------------
//     /// Initialization
//     ///---------------------------------------------------------------------------------
// 
//     ///---------------------------------------------------------------------------------
//     /// Accessors/Queries
//     ///---------------------------------------------------------------------------------
//     static BaseAIBehavior* CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot );
//     float CalcUtility();
//     float Think();
//     BaseAIBehavior* Clone();
// 
//     ///---------------------------------------------------------------------------------
//     /// Mutators
//     ///---------------------------------------------------------------------------------
// 
//     ///---------------------------------------------------------------------------------
//     /// Update
//     ///---------------------------------------------------------------------------------
// 
//     ///---------------------------------------------------------------------------------
//     /// Render
//     ///---------------------------------------------------------------------------------
// 
//     ///---------------------------------------------------------------------------------
//     /// Public Member Variables
//     ///---------------------------------------------------------------------------------
// 
//     ///---------------------------------------------------------------------------------
//     /// Static Variables
//     ///---------------------------------------------------------------------------------
//     static AIBehaviorRegistration s_InspectItemBehaviorRegistration;
// 
// private:
//     ///---------------------------------------------------------------------------------
//     /// Private Functions
//     ///---------------------------------------------------------------------------------
// 
//     ///---------------------------------------------------------------------------------
//     /// Private Member Variables
//     ///---------------------------------------------------------------------------------
//     int m_range;
//     float m_chanceToInspectItem;
// 
//     MapPosition m_targetPos;
// };
// 
// ///---------------------------------------------------------------------------------
// /// Inline function implementations
// ///---------------------------------------------------------------------------------
// 
// #endif