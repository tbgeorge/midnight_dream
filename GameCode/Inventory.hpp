//=================================================================================
// Inventory.hpp
// Author: Tyler George
// Date  : August 19, 2015
//=================================================================================

#pragma once

#ifndef __included_Inventory__
#define __included_Inventory__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/Entities/Item.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/FontRenderer.hpp"

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef std::multimap< ItemType, Item* > Backpack;
typedef std::map< EquipmentSlot, Item* > Equipment;

////===========================================================================================
///===========================================================================================
// Inventory Class
///===========================================================================================
////===========================================================================================
class Inventory
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    Inventory( Entity* owner );
    Inventory( Map* owningMap, const MapPosition& position );
    Inventory( const Inventory& copy );
    ~Inventory();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
	
	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    int GetArmorScore();
    void AddMessageToMessageBar( Entity* instigator );
    IntRange GetEquippedDamageRange();
    IntRange GetEquippedArmorRange();
    bool IsEmpty();
    Items GetItemList();
    Items GetAllItemsOfType( ItemType type );
    Equipment& GetEquipment() { return m_equipment; }
    bool Save( XMLNode& parent );

    static Inventory* LoadFromXML( const XMLNode& inventoryNode );

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    void AddToBackpack( Item* pickedUpItem, bool addMessage );
    bool Equip( Item* itemToEquip, bool addMessage );
    bool MoveItemsToInventory( Inventory* inventoryToMoveItemsTo );
    bool CheckForBetterEquip( Item* itemToCompare );
    void SetOwner( Entity* owner ) { m_owner = owner; }
    void RemoveItem( Item* itemToRemove );

    void CleanUpPointers( std::map< int, Entity* > hookupMap );

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
    void Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, unsigned int shaderID, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Static Variables
	///---------------------------------------------------------------------------------


private:
	///---------------------------------------------------------------------------------
	/// Private Functions
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Private Member Variables
	///---------------------------------------------------------------------------------
    Backpack m_backpack;
    Equipment m_equipment;
    Entity* m_owner;
    Map* m_owningMap;
    MapPosition m_position;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif