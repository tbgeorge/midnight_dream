//=================================================================================
// Item.cpp
// Author: Tyler George
// Date  : September 22, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Entities/Item.hpp"
#include "Engine/Utilities/XMLHelper.hpp"
#include "Engine/Utilities/StringTable.hpp"


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
Item::Item( const XMLNode& itemNode )
    : Entity( itemNode )
{

    unsigned int itemTypeStrID = StringTable::GetStringID( GetStringProperty( itemNode, "type", "misc", true ) );

    if (itemTypeStrID == StringTable::GetStringID( "weapon" ))
    {
        m_type = IT_WEAPON;
        m_symbol = std::string( 1, '(' );
    }
    if (itemTypeStrID == StringTable::GetStringID( "armor" ))
    {
        m_type = IT_ARMOR;
        m_symbol = std::string( 1, ']' );

    }
    if (itemTypeStrID == StringTable::GetStringID( "potion" ))
    {
        m_type = IT_POTION;
        m_symbol = std::string( 1, '!' );

    }
    if (itemTypeStrID == StringTable::GetStringID( "ring" ))
    {
        m_type = IT_RING;
        m_symbol = std::string( 1, '=' );

    }
    if (itemTypeStrID == StringTable::GetStringID( "food" ))
    {
        m_type = IT_FOOD;
        m_symbol = std::string( 1, '%' );

    }
    if (itemTypeStrID == StringTable::GetStringID( "key" ))
    {
        m_type = IT_KEY;
        m_symbol = std::string( 1, ';' );

    }
    if (itemTypeStrID == StringTable::GetStringID( "misc" ))
    {
        m_type = IT_MISC;
        m_symbol = std::string( 1, '&' );

    }

    unsigned int equipSlotStrID = StringTable::GetStringID( GetStringProperty( itemNode, "equipSlot", "invalid", true ) );

    if (equipSlotStrID == StringTable::GetStringID( "head" ))
        m_equipSlot = ES_HEAD;

    if (equipSlotStrID == StringTable::GetStringID( "chest" ))
        m_equipSlot = ES_CHEST;

    if (equipSlotStrID == StringTable::GetStringID( "legs" ))
        m_equipSlot = ES_LEGS;

    if (equipSlotStrID == StringTable::GetStringID( "feet" ))
        m_equipSlot = ES_FEET;

    if (equipSlotStrID == StringTable::GetStringID( "primary" ))
        m_equipSlot = ES_PRIMARY;

    if (equipSlotStrID == StringTable::GetStringID( "secondary" ))
        m_equipSlot = ES_SECONDARY;

    if (equipSlotStrID == StringTable::GetStringID( "invalid" ))
        m_equipSlot = ES_INVALID;


    m_damage = GetIntIntervalProperty( itemNode, "damage" );
    m_armor = GetIntIntervalProperty( itemNode, "armor" );
    m_adjustHealth = GetIntIntervalProperty( itemNode, "adjustHealth" );

    m_additionalDamage = GetIntIntervalProperty( itemNode, "additionalFactionDamage", IntRange::ZERO );
    m_affectedFaction = GetStringProperty( itemNode, "affectedFaction", "", true );
    
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Item::Item( const Item& copy, const XMLNode& itemNode )
    : Entity( copy, itemNode )
    , m_adjustHealth( copy.m_adjustHealth )
    , m_armor( copy.m_armor )
    , m_type( copy.m_type )
    , m_damage( copy.m_damage )
    , m_equipSlot( copy.m_equipSlot )
    , m_additionalDamage( copy.m_additionalDamage )
    , m_affectedFaction( copy.m_affectedFaction )
{
//     if (!itemNode.isEmpty())
//     {
//         int q = 42;
//     }
    // copy stuff from save data
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Item::~Item()
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
int Item::GetArmorValue()
{
    return GetRandomValueInIntRangeInclusive( m_armor );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
std::string Item::GetTypeAsString( ItemType type )
{
    switch (type)
    {
    case IT_WEAPON:
        return "weapon";
    case IT_ARMOR:
        return "armor";
    case IT_POTION:
        return "potion";
    case IT_RING:
        return "ring";
    case IT_FOOD:
        return "food";
    case IT_KEY:
        return "key";
    case IT_MISC:
        return "misc";
    default:
        return "";
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
std::string Item::GetEquipSlotAsString( EquipmentSlot equipSlot )
{
    switch (equipSlot)
    {
    case ES_HEAD:
        return "head";
    case ES_CHEST:
        return "chest";
    case ES_LEGS:
        return "legs";
    case ES_FEET:
        return "feet";
    case ES_PRIMARY:
        return "primary";
    case ES_SECONDARY:
        return "secondary";
    case ES_INVALID:
        return "invalid";
    default:
        return "";
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Item::Save( XMLNode& parent, bool createChild )
{
    XMLNode nodeToModify = parent;
    if (createChild)
        nodeToModify = CreateNode( "Item" );

    Entity::Save( nodeToModify, false );

    if (m_prototype)
    {
        if (m_damage != ((Item*)m_prototype)->m_damage)
            SetIntIntervalProperty( nodeToModify, "damage", m_damage, ((Item*)m_prototype)->m_damage );
        if (m_armor != ((Item*)m_prototype)->m_armor)
            SetIntIntervalProperty( nodeToModify, "armor", m_armor, ((Item*)m_prototype)->m_armor );
        if (m_adjustHealth != ((Item*)m_prototype)->m_adjustHealth)
            SetIntIntervalProperty( nodeToModify, "adjustHealth", m_adjustHealth, ((Item*)m_prototype)->m_adjustHealth );
        if (m_type != ((Item*)m_prototype)->m_type)
            SetStringProperty( nodeToModify, "type", GetTypeAsString( m_type ), GetTypeAsString( ((Item*)m_prototype)->m_type ) );
        if (m_equipSlot != ((Item*)m_prototype)->m_equipSlot)
            SetStringProperty( nodeToModify, "equipSlot", GetEquipSlotAsString( m_equipSlot ), GetEquipSlotAsString( ((Item*)m_prototype)->m_equipSlot ) );
    }

    if (createChild)
        AddChild( parent, nodeToModify );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Item::HandleEvent( InteractionEvent* event )
{
    UNUSED( event );
    return;
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

