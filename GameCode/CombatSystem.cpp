//=================================================================================
// CombatSystem.cpp
// Author: Tyler George
// Date  : September 22, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/CombatSystem.hpp"
#include "GameCode/Entities/Entity.hpp"
#include "MessageBar.hpp"
#include "Engine/Utilities/StringTable.hpp"
#include "GameCode/Entities/Item.hpp"

////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

CombatSystem CombatSystem::s_theCombatSystem;

////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CombatSystem::~CombatSystem()
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
AttackResult CombatSystem::PerformMeleeAttack( const AttackData& data )
{
    AttackResult result;
    result.damageDone = 0;
    result.didCrit = false;
    result.didHit = false;
    result.targetDied = false;

    bool hit = GetRandomFloatZeroToOne() <= data.chanceToHit ? true : false;
    result.didHit = hit;
    if (hit)
    {
        bool crit = GetRandomFloatZeroToOne() <= data.chanceToCrit ? true : false;
        result.didCrit = crit;

        int damageToApply = GetRandomValueInIntRangeInclusive( data.damageRange );
        int armor = GetRandomValueInIntRangeInclusive( data.armorRange );
        int armorMitigation = armor / 3;

        if (data.primaryWeapon)
        {
            Faction* affectedFaction = Faction::GetMasterFactionFromName( data.primaryWeapon->GetAffectedFaction() );
            if (affectedFaction)
            {
                Faction* targetFaction = data.target->GetFaction();
                if (targetFaction)
                {
                    if (targetFaction->GetFactionID() == affectedFaction->GetFactionID() )
                    {
                        int additionalDamage = GetRandomValueInIntRangeInclusive( data.primaryWeapon->GetAdditionalDamage() );
                        damageToApply += additionalDamage;
                    }

                }

            }
        }



        damageToApply -= armorMitigation;
        if (damageToApply <= 0)
            damageToApply = 0;

        if (crit)
        {
            damageToApply *= 2;

			// #KSH: This block of 4 lines keeps getting repeated.  Consider making AddMessage() accept it
			// all to make it easier to post this kind of message...
			// AddMessage( const std::string& instigator, const IntVector2& position, const char* format, ...);
			// Or even more simply, make AddMessage more aware of the game's context:
			// AddMessage( Entity* source, const char* format, ... ); // pulls name, position from source
            Message critMessage;
            critMessage.m_text = ( data.attacker->GetName() == "Player" ? "You" : data.attacker->GetName() ) +" critically " + data.attackVerb + " " + ( data.target->GetName() == "Player" ? "you" : data.target->GetName() ) +" for " + std::to_string( damageToApply ) + " damage. ";
            critMessage.m_instigator = data.attacker->GetName();
            critMessage.m_position = data.attacker->GetMapPosition();
            MessageBar::AddMessage( critMessage );
        }
        else
        {
            Message hitMessage;
            hitMessage.m_text = ( data.attacker->GetName() == "Player" ? "You" : data.attacker->GetName() ) + " " + data.attackVerb + " " + ( data.target->GetName() == "Player" ? "you" : data.target->GetName() ) + " for " + std::to_string( damageToApply ) + " damage. ";
            hitMessage.m_instigator = data.attacker->GetName();
            hitMessage.m_position = data.attacker->GetMapPosition();
            MessageBar::AddMessage( hitMessage );
        }

        result.damageDone = damageToApply;

        bool targetDied = data.target->ApplyDamage( damageToApply );
        result.targetDied = targetDied;

        if (targetDied)
        {
            Message deathMessage;
            deathMessage.m_instigator = data.attacker->GetName();
            deathMessage.m_position = data.attacker->GetMapPosition();

            if ( data.target->IsActor() )
                deathMessage.m_text = ( data.attacker->GetName() == "Player" ? "You" : data.attacker->GetName() ) + " killed " + ( data.target->GetName() == "Player" ? "you" : data.target->GetName() ) + "! ";

            else if ( data.target->IsFeature() )
                deathMessage.m_text = ( data.attacker->GetName() == "Player" ? "You" : data.attacker->GetName() ) + " destroyed the " + ( data.target->GetName() == "Player" ? "you" : data.target->GetName() ) + "! ";

            MessageBar::AddMessage( deathMessage );
        }
    }
    else
    {
        Message missMessage;
        missMessage.m_text = ( data.attacker->GetName() == "Player" ? "You" : data.attacker->GetName() ) + " missed " + ( data.target->GetName() == "Player" ? "you" : data.target->GetName() ) + ". ";
        missMessage.m_instigator = data.attacker->GetName();
        missMessage.m_position = data.attacker->GetMapPosition();
        MessageBar::AddMessage( missMessage );
    }

    return result;
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

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CombatSystem::CombatSystem()
{
}