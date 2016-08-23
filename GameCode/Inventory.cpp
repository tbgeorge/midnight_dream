//=================================================================================
// Inventory.cpp
// Author: Tyler George
// Date  : September 24, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Inventory.hpp"
#include "MessageBar.hpp"
#include "Engine/Utilities/StringTable.hpp"
#include "Engine/Utilities/XMLHelper.hpp"
#include "ItemFactory.hpp"
#include "Game.hpp"


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
Inventory::Inventory( Entity* owner )
    : m_owner( owner )
    , m_owningMap( nullptr )
    , m_position(MapPosition( -1, -1 ) )
{
    m_equipment[ES_HEAD] = nullptr;
    m_equipment[ES_CHEST] = nullptr;
    m_equipment[ES_LEGS] = nullptr;
    m_equipment[ES_FEET] = nullptr;
    m_equipment[ES_PRIMARY] = nullptr;
    m_equipment[ES_SECONDARY] = nullptr;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Inventory::Inventory( Map* owningMap, const MapPosition& position )
    : m_owner( nullptr )
    , m_owningMap( owningMap )
    , m_position(position )
{
    m_equipment[ES_HEAD] = nullptr;
    m_equipment[ES_CHEST] = nullptr;
    m_equipment[ES_LEGS] = nullptr;
    m_equipment[ES_FEET] = nullptr;
    m_equipment[ES_PRIMARY] = nullptr;
    m_equipment[ES_SECONDARY] = nullptr;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Inventory::Inventory( const Inventory& copy )
    : m_owner( copy.m_owner )
    , m_owningMap( copy.m_owningMap )
    , m_position( copy.m_position )
{ 
    for (Backpack::const_iterator backpackIter = copy.m_backpack.begin(); backpackIter != copy.m_backpack.end(); ++backpackIter)
    {
        ItemType type = backpackIter->first;
        Item* copyItem = backpackIter->second;

        Item* newItem = new Item( *copyItem, XMLNode::emptyNode() );
        m_backpack.insert( std::pair< ItemType, Item*>( type, newItem ) );

        Game::AddEntity( newItem );
    }

    for (Equipment::const_iterator equipmentIter = copy.m_equipment.begin(); equipmentIter != copy.m_equipment.end(); ++equipmentIter)
    {
        EquipmentSlot slot = equipmentIter->first;
        Item* copyItem = equipmentIter->second;

        if (copyItem)
        {
            Item* newItem = new Item( *copyItem, XMLNode::emptyNode() );
            m_equipment[slot] = newItem;
            Game::AddEntity( newItem );

        }
        else
            m_equipment[slot] = nullptr;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Inventory::~Inventory()
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
int Inventory::GetArmorScore()
{
    int totalArmor = 0;
    for (Equipment::const_iterator equipmentIter = m_equipment.begin(); equipmentIter != m_equipment.end(); ++equipmentIter)
    {
        Item* equippedItem = equipmentIter->second;
        if (equippedItem)
        {
            totalArmor += equippedItem->GetArmorValue();
        }
    }

    return totalArmor;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Inventory::AddMessageToMessageBar( Entity* instigator )
{
    for (Backpack::const_iterator backpackIter = m_backpack.begin(); backpackIter != m_backpack.end(); ++backpackIter)
    {
        Item* item = backpackIter->second;

        Message msg;
        msg.m_text = "You discovered a " + item->GetName() + ". ";
        msg.m_position = instigator->GetMapPosition();
        msg.m_instigator = instigator->GetName();
        MessageBar::AddMessage( msg );
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
IntRange Inventory::GetEquippedDamageRange()
{
    Item* primary = m_equipment[ES_PRIMARY];
    Item* secondary = m_equipment[ES_SECONDARY];

    IntRange equippedDamage = IntRange::ZERO;
    if (primary)
        equippedDamage += primary->GetDamageRange();
    if (secondary)
        equippedDamage += secondary->GetDamageRange();

    return equippedDamage;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
IntRange Inventory::GetEquippedArmorRange()
{
    Item* head = m_equipment[ES_HEAD];
    Item* chest = m_equipment[ES_CHEST];
    Item* legs = m_equipment[ES_LEGS];
    Item* feet = m_equipment[ES_FEET];
    Item* secondary = m_equipment[ES_SECONDARY];

    IntRange equippedArmor = IntRange::ZERO;
    if (head)
        equippedArmor += head->GetArmorRange();
    if (chest)
        equippedArmor += chest->GetArmorRange();
    if (legs)
        equippedArmor += legs->GetArmorRange();
    if (feet)
        equippedArmor += feet->GetArmorRange();
    if (secondary)
        equippedArmor += secondary->GetArmorRange();

    return equippedArmor;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Inventory::Save( XMLNode& parent )
{
    XMLNode backpack = CreateNode( "Backpack" );
    XMLNode equipment = CreateNode( "Equipment" );

    bool hasBackpack = false;
    if (m_backpack.size() != 0)
    {
        hasBackpack = true;
        AddChild( parent, backpack );

        for (Backpack::const_iterator backpackIter = m_backpack.begin(); backpackIter != m_backpack.end(); ++backpackIter)
        {
            Item* item = backpackIter->second;

            XMLNode itemNode = CreateNode( "Item" );
            SetStringProperty( itemNode, "type", Item::GetTypeAsString( item->GetType() ), "misc" );
            SetIntProperty( itemNode, "hookupID", item->GetEntityID(), -1 );

            AddChild( backpack, itemNode );
        }
    }

    bool hasEquipment = false;
    if (m_equipment[ES_HEAD])
    {
        hasEquipment = true;
        XMLNode headNode = CreateNode( "Head" );
        SetIntProperty( headNode, "hookupID", m_equipment[ES_HEAD]->GetEntityID(), -1 );
        AddChild( equipment, headNode );
    }
    if (m_equipment[ES_CHEST])
    {
        hasEquipment = true;
        XMLNode chestNode = CreateNode( "Chest" );
        SetIntProperty( chestNode, "hookupID", m_equipment[ES_CHEST]->GetEntityID(), -1 );
        AddChild( equipment, chestNode );
    }
    if (m_equipment[ES_LEGS])
    {
        hasEquipment = true;
        XMLNode legsNode = CreateNode( "Legs" );
        SetIntProperty( legsNode, "hookupID", m_equipment[ES_LEGS]->GetEntityID(), -1 );
        AddChild( equipment, legsNode );
    }
    if (m_equipment[ES_FEET])
    {
        hasEquipment = true;
        XMLNode feetNode = CreateNode( "Feet" );
        SetIntProperty( feetNode, "hookupID", m_equipment[ES_FEET]->GetEntityID(), -1 );
        AddChild( equipment, feetNode );
    }
    if (m_equipment[ES_PRIMARY])
    {
        hasEquipment = true;
        XMLNode primaryNode = CreateNode( "Primary" );
        SetIntProperty( primaryNode, "hookupID", m_equipment[ES_PRIMARY]->GetEntityID(), -1 );
        AddChild( equipment, primaryNode );
    }
    if (m_equipment[ES_SECONDARY])
    {
        hasEquipment = true;
        XMLNode secondaryNode = CreateNode( "Secondary" );
        SetIntProperty( secondaryNode, "hookupID", m_equipment[ES_SECONDARY]->GetEntityID(), -1 );
        AddChild( equipment, secondaryNode );
    }

    if ( hasEquipment )
        AddChild( parent, equipment );

    if (!hasBackpack && !hasEquipment)
        return false;

    return true;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Inventory* Inventory::LoadFromXML( const XMLNode& inventoryNode )
{
    XMLNode backpackNode = inventoryNode.getChildNode( "Backpack" );
    XMLNode equipmentNode = inventoryNode.getChildNode( "Equipment" );

    Inventory* inventory = new Inventory( nullptr );
    if (!backpackNode.isEmpty())
    {
        // backpack exists
        bool hasAnotherItem = false;
        int itemCounter = 0;
        do 
        {
            XMLNode itemNode = backpackNode.getChildNode( itemCounter++ );
            if (!itemNode.isEmpty())
            {
                hasAnotherItem = true;

                unsigned int itemTypeStrID = StringTable::GetStringID( GetStringProperty( itemNode, "type", "misc", true ) );
                ItemType itemType;

                if (itemTypeStrID == StringTable::GetStringID( "weapon" ))
                    itemType = IT_WEAPON;
                if (itemTypeStrID == StringTable::GetStringID( "armor" ))
                    itemType = IT_ARMOR;
                if (itemTypeStrID == StringTable::GetStringID( "potion" ))
                    itemType = IT_POTION;
                if (itemTypeStrID == StringTable::GetStringID( "ring" ))
                    itemType = IT_RING;
                if (itemTypeStrID == StringTable::GetStringID( "food" ))
                    itemType = IT_FOOD;
                if (itemTypeStrID == StringTable::GetStringID( "key" ))
                    itemType = IT_KEY;
                if (itemTypeStrID == StringTable::GetStringID( "misc" ))
                    itemType = IT_MISC;


                int hookupID = GetIntProperty( itemNode, "hookupID", 0 );
                Item* hookupItem = (Item*)hookupID;

                inventory->m_backpack.insert( std::pair< ItemType, Item* >( itemType, hookupItem ) );

            }
            else
                hasAnotherItem = false;

        } while ( hasAnotherItem );
    }

    if (!equipmentNode.isEmpty())
    {
        // equipment exists
        int hookupID = 0;
        Item* hookupItem = nullptr;

        // head
        XMLNode headNode = equipmentNode.getChildNode( "Head" );
        if (!headNode.isEmpty())
        {
            hookupID = GetIntProperty( headNode, "hookupID", 0 );
            hookupItem = (Item*)hookupID;

            if (hookupID != 0)
                inventory->m_equipment[ES_HEAD] = hookupItem;
            else
            {
                std::string itemName = GetStringProperty( headNode, "name", "", false );
                ItemFactory* factory = ItemFactory::FindFactoryByName( itemName );
                Item* newItem = factory->SpawnItem( XMLNode::emptyNode() );
                inventory->m_equipment[ES_HEAD] = newItem;

            }
        }

        // chest
        XMLNode chestNode = equipmentNode.getChildNode( "Chest" );
        if (!chestNode.isEmpty())
        {
            hookupID = GetIntProperty( chestNode, "hookupID", 0 );
            hookupItem = (Item*)hookupID;

            if (hookupID != 0)
                inventory->m_equipment[ES_CHEST] = hookupItem;
            else
            {
                std::string itemName = GetStringProperty( chestNode, "name", "", false );
                ItemFactory* factory = ItemFactory::FindFactoryByName( itemName );
                Item* newItem = factory->SpawnItem( XMLNode::emptyNode() );
                inventory->m_equipment[ES_CHEST] = newItem;

            }
        }

        // legs
        XMLNode legsNode = equipmentNode.getChildNode( "Legs" );
        if (!legsNode.isEmpty())
        {
            hookupID = GetIntProperty( legsNode, "hookupID", 0 );
            hookupItem = (Item*)hookupID;

            if (hookupID != 0)
                inventory->m_equipment[ES_LEGS] = hookupItem;
            else
            {
                std::string itemName = GetStringProperty( legsNode, "name", "", false );
                ItemFactory* factory = ItemFactory::FindFactoryByName( itemName );
                Item* newItem = factory->SpawnItem( XMLNode::emptyNode() );
                inventory->m_equipment[ES_LEGS] = newItem;

            }
        }

        // feet
        XMLNode feetNode = equipmentNode.getChildNode( "Feet" );
        if (!feetNode.isEmpty())
        {
            hookupID = GetIntProperty( feetNode, "hookupID", 0 );
            hookupItem = (Item*)hookupID;

            if (hookupID != 0)
                inventory->m_equipment[ES_FEET] = hookupItem;
            else
            {
                std::string itemName = GetStringProperty( feetNode, "name", "", false );
                ItemFactory* factory = ItemFactory::FindFactoryByName( itemName );
                Item* newItem = factory->SpawnItem( XMLNode::emptyNode() );
                inventory->m_equipment[ES_FEET] = newItem;

            }
        }

        // primary
        XMLNode primaryNode = equipmentNode.getChildNode( "Primary" );
        if (!primaryNode.isEmpty())
        {
            hookupID = GetIntProperty( primaryNode, "hookupID", 0 );
            hookupItem = (Item*)hookupID;

            if ( hookupID != 0 )
                inventory->m_equipment[ES_PRIMARY] = hookupItem;
            else
            {
                std::string itemName = GetStringProperty( primaryNode, "name", "", false );
                ItemFactory* factory = ItemFactory::FindFactoryByName( itemName );
                Item* newItem = factory->SpawnItem( XMLNode::emptyNode() );
                inventory->m_equipment[ES_PRIMARY] = newItem;

            }
        }

        // secondary
        XMLNode secondaryNode = equipmentNode.getChildNode( "Secondary" );
        if (!secondaryNode.isEmpty())
        {
            hookupID = GetIntProperty( secondaryNode, "hookupID", 0 );
            hookupItem = (Item*)hookupID;

            if (hookupID != 0)
                inventory->m_equipment[ES_SECONDARY] = hookupItem;
            else
            {
                std::string itemName = GetStringProperty( secondaryNode, "name", "", false );
                ItemFactory* factory = ItemFactory::FindFactoryByName( itemName );
                Item* newItem = factory->SpawnItem( XMLNode::emptyNode() );
                inventory->m_equipment[ES_SECONDARY] = newItem;

            }
        }

    }

    return inventory;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Inventory::IsEmpty()
{
    return (m_backpack.size() == 0 ? true : false);
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Items Inventory::GetItemList()
{
    Items itemList;
    for (Backpack::iterator backpackIter = m_backpack.begin(); backpackIter != m_backpack.end(); ++backpackIter)
    {
        Item* item = backpackIter->second;
        itemList.push_back( item );
    }
    return itemList;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Items Inventory::GetAllItemsOfType( ItemType type )
{
    std::pair< Backpack::iterator, Backpack::iterator > typeEqualRangePair;
    typeEqualRangePair = m_backpack.equal_range( type );

    Items itemsOfType;
    for (Backpack::iterator itemTypeIter = typeEqualRangePair.first; itemTypeIter != typeEqualRangePair.second; ++itemTypeIter)
    {
        Item* item = itemTypeIter->second;
        itemsOfType.push_back( item );
    }
    return itemsOfType;
}


////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Inventory::AddToBackpack( Item* pickedUpItem, bool addMessage)
{
    if (pickedUpItem)
    {
        if (m_owner)
        {
            pickedUpItem->SetMapPosition( IntVector2( -1, -1 ) );

            if (addMessage)
            {
                if (StringTable::GetStringID( m_owner->GetName() ) == StringTable::GetStringID( "Player" ))
                {
                    Message msg;
                    msg.m_text = pickedUpItem->GetName() + " was picked up. ";
                    msg.m_position = m_owner->GetMapPosition();
                    msg.m_instigator = m_owner->GetName();
                    MessageBar::AddMessage( msg );
                }
            }
        }

        bool betterThanEquipped = CheckForBetterEquip( pickedUpItem );
        if (betterThanEquipped)
            Equip( pickedUpItem, addMessage );

        else
            m_backpack.insert( std::pair< ItemType, Item* >( pickedUpItem->GetType(), pickedUpItem ) );
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Inventory::Equip( Item* itemToEquip, bool addMessage )
{
    EquipmentSlot slotToEquip = itemToEquip->GetEquipSlot();
    if (slotToEquip != ES_INVALID)
    {
        if (m_equipment[slotToEquip] != nullptr)
            AddToBackpack( m_equipment[slotToEquip], false );
        m_equipment[slotToEquip] = itemToEquip;
    
        if (m_owner && addMessage )
        {
            if (StringTable::GetStringID( m_owner->GetName() ) == StringTable::GetStringID( "Player" ))
            {
                Message msg;
                msg.m_text = itemToEquip->GetName() + " was equipped. ";
                msg.m_position = m_owner->GetMapPosition();
                msg.m_instigator = m_owner->GetName();
                MessageBar::AddMessage( msg );
            }
        }
        return true;
    }
    return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Inventory::MoveItemsToInventory( Inventory* inventoryToMoveItemsTo )
{
    bool movedItems = false;
    for (Backpack::iterator backpackIter = m_backpack.begin(); backpackIter != m_backpack.end(); ++backpackIter)
    {
        Item* item = backpackIter->second;
        inventoryToMoveItemsTo->AddToBackpack( item, true );
        movedItems = true;
    }

    m_backpack.clear();

    for (Equipment::iterator equipmentIter = m_equipment.begin(); equipmentIter != m_equipment.end(); ++equipmentIter)
    {
        Item* item = equipmentIter->second;
        if (item)
        {
            inventoryToMoveItemsTo->AddToBackpack( item, true );
            movedItems = true;
        }
    }

    m_equipment[ES_HEAD] = nullptr;
    m_equipment[ES_CHEST] = nullptr;
    m_equipment[ES_LEGS] = nullptr;
    m_equipment[ES_FEET] = nullptr;
    m_equipment[ES_PRIMARY] = nullptr;
    m_equipment[ES_SECONDARY] = nullptr;

    return movedItems;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Inventory::CheckForBetterEquip( Item* itemToCompare )
{
    if (!m_owner)
        return false;

    EquipmentSlot slot = itemToCompare->GetEquipSlot();
    
    if (slot == ES_INVALID)
        return false;

    Item* equippedItem = m_equipment[slot];
    if (equippedItem)
    {
        if (slot == ES_HEAD || slot == ES_CHEST || slot == ES_FEET || slot == ES_LEGS)
        {
            if (itemToCompare->GetMaxArmor() > equippedItem->GetMaxArmor())
                return true;
            return false;
        }
        else if (slot == ES_PRIMARY )
        {
            if (itemToCompare->GetMaxDamage() > equippedItem->GetMaxDamage())
                return true;
            return false;
        }
        else if (slot == ES_SECONDARY)
        {
            int itemToCompareMaxStats = itemToCompare->GetMaxDamage() + itemToCompare->GetMaxArmor();
            int equippedMaxStats = equippedItem->GetMaxDamage() + equippedItem->GetMaxArmor();

            if (itemToCompareMaxStats > equippedMaxStats)
                return true;
            return false;
        }
    }
    else
        return true;

    return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Inventory::CleanUpPointers( std::map< int, Entity* > hookupMap )
{
    for (Backpack::iterator backpackIter = m_backpack.begin(); backpackIter != m_backpack.end(); ++backpackIter)
    {
        Item*& item = backpackIter->second;

        int hookupID = (int)item;

        item = (Item*)hookupMap[hookupID];
    }

    for (Equipment::iterator equipmentIter = m_equipment.begin(); equipmentIter != m_equipment.end(); ++equipmentIter)
    {
        Item*& item = equipmentIter->second;

        int hookupID = (int)item;

        if ( hookupID != 0 )
            item = (Item*)hookupMap[hookupID];
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Inventory::RemoveItem( Item* itemToRemove )
{
    for (Backpack::iterator backpackIter = m_backpack.begin(); backpackIter != m_backpack.end(); ++backpackIter)
    {
        Item* item = backpackIter->second;
        if (item == itemToRemove)
        {
            m_backpack.erase( backpackIter );
            return;
        }
    }
}

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

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Inventory::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, unsigned int shaderID, bool debugModeEnabled )
{

    if (!renderer)
        return;

    UNUSED( debugModeEnabled );


    if ( !m_owner && m_owningMap)
    {
        Cell* myCell = m_owningMap->GetCellAtPosition( m_position );
        AABB2D renderBounds = myCell->GetRenderBounds();

        Vector3 pos( renderBounds.m_mins.x, renderBounds.m_mins.y, 9.0f );
        float fontWidth = renderBounds.m_maxs.x - renderBounds.m_mins.x;
        float fontHeight = renderBounds.m_maxs.y - renderBounds.m_mins.y;

        if (m_backpack.size() > 1)
            fontRenderer->DrawFontTextWH( fontWidth, fontHeight, font, "*", pos, Rgba::WHITE );

        else
        {
            for (Backpack::const_iterator backpackIter = m_backpack.begin(); backpackIter != m_backpack.end(); ++backpackIter)
            {
                Item* item = backpackIter->second;
                fontRenderer->DrawFontTextWH( fontWidth, fontHeight, font, item->GetSymbol(), pos, item->GetColor() );

            }
        }
        


    }


}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

