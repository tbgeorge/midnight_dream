//=================================================================================
// Game.cpp
// Author: Tyler George
// Date  : August 20, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "Engine/Utilities/DeveloperConsole.hpp"

#include "GameCode/Game.hpp"
#include "GameCode/Entities/Player.hpp"
#include "GameCode/NPCFactory.hpp"
#include "FieldOfViewCalculator.hpp"
#include "Faction.hpp"
#include "MessageBar.hpp"
#include "ItemFactory.hpp"
#include "Engine/Utilities/FileUtilities.hpp"
#include "FeatureFactory.hpp"
#include "Engine/Utilities/StringTable.hpp"



////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================
Game* Game::s_theGame = nullptr;


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Game::Game( const Vector2& displaySize )
    : m_displaySize( displaySize )
    , m_camera( nullptr )
    , m_isReadyToQuit( false )
    , m_basicShaderID( 0 )
    , m_fontShaderID( 0 )
    , m_renderer( nullptr )
    , m_fontRenderer( nullptr )
    , m_font( nullptr )
    , m_mapFont( nullptr )
    , m_titleFont( nullptr )
    , m_mainMenu( nullptr )
    , m_questMenu( nullptr )
    , m_gameOverMenu( nullptr )
    , m_map( nullptr )
    , m_player( nullptr )
    , m_statusBar( nullptr )
{
    srand( (unsigned int) time(NULL) );

    if (!s_theGame)
        s_theGame = this;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Game::~Game()
{
}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::Startup( OpenGLRenderer* renderer, FontRenderer* fontRenderer, unsigned int basicShaderID )
{
    // Component Initialization
    m_renderer = renderer;
    m_camera = new Camera3D( Vector3( 0.0f, 0.0f, 10.0f ), EulerAngles( -180.0f, 0.0f, 0.0f ) );

    m_fontRenderer = fontRenderer;
    m_titleFont = new Font( "Data/Fonts/MidnightDreamFlavor.fnt" );
    m_font = new Font( "Data/Fonts/Calibri.fnt" );
    m_mapFont = new Font( "Data/Fonts/CourierNew.fnt" );

    // Shader Initialization
    m_basicShaderID = basicShaderID;
    m_fontShaderID = m_renderer->LoadProgram( "Data/Shaders/textured.vert", "Data/Shaders/textured.frag" );
    renderer->GLCheckError();

    Faction::LoadAllFactions();
    ItemFactory::LoadAllItemFactories();
    NPCFactory::LoadAllNPCFactories();
    FeatureFactory::LoadAllFeatureFactories();

    m_mainMenu = new MainMenu( m_displaySize, m_basicShaderID, m_fontShaderID );
    m_mainMenu->Startup( m_titleFont, m_font );

    m_questMenu = new QuestMenu( m_displaySize, m_basicShaderID, m_fontShaderID );
    m_questMenu->Startup( m_titleFont, m_font );

    m_gameOverMenu = new GameOverMenu( m_displaySize, m_basicShaderID, m_fontShaderID );
    m_gameOverMenu->Startup( m_titleFont, m_font );

//     m_map = new Map();

    m_gameState = GS_MAIN_MENU;


    // Testing xml writing functions
//     XMLNode root = CreateRootNode();
//     XMLNode stringsTest = CreateNode( "StringsTest" );
//     
//     Strings strs;
//     strs.push_back( "test" );
//     strs.push_back( "test21" );
//     strs.push_back( "woot" );
// 
//     Strings strs1;
//     strs1.push_back( "test" );
//     strs1.push_back( "test21" );
//     strs1.push_back( "woot" );
// 
//     SetStringsProperty( stringsTest, "testVals", strs, ',', strs1 );
// 
//     AddChild( root, stringsTest );
// 
//     XMLNode charTest = CreateNode( "CharTest" );
//     char x = 'x';
//     char y = 'y';
//     SetCharacterProperty( charTest, "testVals", x, y );
//     AddChild( root, charTest );
// 
//     XMLNode intTest = CreateNode( "IntTest" );
//     int xi = 23;
//     int yi = 85;
//     SetIntProperty( intTest, "testVals", xi, yi );
//     AddChild( root, intTest );
// 
//     XMLNode unsignedIntTest = CreateNode( "UnsignedIntTest" );
//     unsigned int xui = 98;
//     unsigned int yui = 34;
//     SetUnsignedIntProperty( unsignedIntTest, "testVals", xui, yui );
//     AddChild( root, unsignedIntTest );
// 
//     XMLNode floatTest = CreateNode( "FloatTest" );
//     float xf = 24.3f;
//     float yf = 95.4f;
//     SetFloatProperty( floatTest, "testVals", xf, yf );
//     AddChild( root, floatTest );
// 
//     XMLNode doubleTest = CreateNode( "DoubleTest" );
//     double xd = 56.4;
//     double yd = 93.4;
//     SetDoubleProperty( doubleTest, "testVals", xd, yd );
//     AddChild( root, doubleTest );
// 
//     XMLNode boolTest = CreateNode( "BoolTest" );
//     bool xb = true;
//     bool yb = false;
//     SetBooleanProperty( boolTest, "testVals", xb, yb );
//     AddChild( root, boolTest );
// 
//     XMLNode rgbaTest = CreateNode( "RgbaTest" );
//     Rgba xrgba = Rgba::RED;
//     Rgba yrgba = Rgba::BLUE;
//     SetRgbaProperty( rgbaTest, "testVals", xrgba, yrgba );
//     AddChild( root, rgbaTest );
// 
//     XMLNode vec2Test = CreateNode( "Vec2Test" );
//     Vector2 xv2 = Vector2( 4.5f, 43.2f );
//     Vector2 yv2 = Vector2( 3.2f, 3.5f );
//     SetVector2Property( vec2Test, "testVals", xv2, yv2 );
//     AddChild( root, vec2Test );
// 
//     XMLNode vec3Test = CreateNode( "Vec3Test" );
//     Vector3 xv3 = Vector3( 4.5f, 43.2f, 34.0f );
//     Vector3 yv3 = Vector3( 3.2f, 3.5f, 3.0f );
//     SetVector3Property( vec3Test, "testVals", xv3, yv3 );
//     AddChild( root, vec3Test );
// 
//     XMLNode ivec2Test = CreateNode( "IntVec2Test" );
//     IntVector2 xiv2 = IntVector2( 4, 4 );
//     IntVector2 yiv2 = IntVector2( 3, 3 );
//     SetIntVector2Property( ivec2Test, "testVals", xiv2, yiv2 );
//     AddChild( root, ivec2Test );
// 
//     XMLNode ivec3Test = CreateNode( "IntVec3Test" );
//     IntVector3 xiv3 = IntVector3( 4, 4, 34 );
//     IntVector3 yiv3 = IntVector3( 3, 3, 3 );
//     SetIntVector3Property( ivec3Test, "testVals", xiv3, yiv3 );
//     AddChild( root, ivec3Test );
// 
//     XMLNode fltRangeTest = CreateNode( "fltRangeTest" );
//     FltRange xfr = FltRange( 4.5f, 43.2f );
//     FltRange yfr = FltRange( 3.2f, 3.5f );
//     SetFloatIntervalProperty( fltRangeTest, "testVals", xfr, yfr );
//     AddChild( root, fltRangeTest );
// 
//     XMLNode intRangeTest = CreateNode( "IntRangeTest" );
//     IntRange xir = IntRange( 4, 20 );
//     IntRange yir = IntRange( 5, 10 );
//     SetIntIntervalProperty( intRangeTest, "testVals", xir, yir );
//     AddChild( root, intRangeTest );
// 
//     WriteToFile( "Data/Test/test.xml", root );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::Shutdown()
{

}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::LoadGameFromFile()
{
    std::string filePath = "Data/Saves/SaveGame.sav.xml";

    XMLNode saveGameNode = XMLNode::parseFile( filePath.c_str(), "SaveGame" );

    XMLNode mapDataNode = saveGameNode.getChildNode( "MapData" );
    XMLNode entitiesNode = saveGameNode.getChildNode( "Entities" );

    m_map = Map::LoadFromFile( mapDataNode );

    std::map< int, Entity* > hookupMap;

    bool hasAnotherChild = false;
    int entityIndex = 0;
    do 
    {
        XMLNode entityNode = entitiesNode.getChildNode( entityIndex++ );
        if (!entityNode.isEmpty())
        {
            hasAnotherChild = true;

            std::string entityType = GetXMLNodeName( entityNode );

            if (entityType == "NPC")
            {
                // load NPC
                std::string npcName = GetStringProperty( entityNode, "name", "", false );
                NPCFactory* factory = NPCFactory::FindFactoryByName( npcName );
                NPC* npc = factory->SpawnNPC( entityNode );
                m_entities.push_back( npc );
                npc->SetMap( m_map );
                m_map->SetEntityAtPosition( npc, npc->GetMapPosition() );

                hookupMap.insert( std::pair< int, Entity* >( npc->GetHookupID(), npc ) );

            }

            else if (entityType == "Item")
            {
                // load item
                std::string itemName = GetStringProperty( entityNode, "name", "", false );
                ItemFactory* factory = ItemFactory::FindFactoryByName( itemName );
                Item* item = factory->SpawnItem( entityNode );
                m_entities.push_back( item );

                if (item->GetMapPosition() != IntVector2( -1, -1 ))
                {
                    item->SetMap( m_map );
                    m_map->AddItemAtPosition( item, item->GetMapPosition() );

                }

                hookupMap.insert( std::pair< int, Entity* >( item->GetHookupID(), item ) );

            }

            else if (entityType == "Feature")
            {
                // load feature
                std::string featureName = GetStringProperty( entityNode, "name", "", false );
                FeatureFactory* factory = FeatureFactory::FindFactoryByName( featureName );
                Feature* feature = factory->SpawnFeature( entityNode );
                m_entities.push_back( feature );

                feature->SetMap( m_map );
                m_map->SetFeatureAtPosition( feature, feature->GetMapPosition() );
                
                hookupMap.insert( std::pair< int, Entity* >( feature->GetHookupID(), feature ) );
            }

            else if (entityType == "Player")
            {
                // load player
                m_player = new Player( entityNode );
                m_player->SetMap( m_map );
                m_map->SetEntityAtPosition( m_player, m_player->GetMapPosition() );
                m_entities.push_back( m_player );
                m_statusBar = new StatusBar( m_player );
                MessageBar::Startup( m_player, m_font );

                hookupMap.insert( std::pair< int, Entity* >( m_player->GetHookupID(), m_player ) );

            }

            else
            {
                // do stuff
            }

        }

        else
            hasAnotherChild = false;
        
    } while ( hasAnotherChild );


    for each (Entity* entity in m_entities)
        entity->CleanUpPointers( hookupMap );
   
    FieldOfViewCalculator::CalculateFOV_BFR( m_player, m_player->GetVisibilityRange(), m_map, true );

    bool success = DeleteFile_Relative( filePath );

    if (!success)
        DeveloperConsole::WriteLine( "Could not delete save game file: " + filePath, WARNING_TEXT_COLOR );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::ResetGame()
{
    delete m_map;
    m_map = nullptr;
    m_questMenu->Reset();
    m_mainMenu->Reset();
    m_gameOverMenu->Reset();
    MessageBar::SetClearCurrent( true );
    MessageBar::Update( 0.0, false );
    m_player->Reset();
    m_player = nullptr;
    delete m_statusBar;
    m_statusBar = nullptr;

    for (std::vector< Entity* >::iterator entityIter = m_entities.begin(); entityIter != m_entities.end(); ++entityIter)
    {
        Entity* entity = *entityIter;
        delete entity;
    }

    m_entities.clear();
    m_entities.resize( 0 );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::AddEntity( Entity* entity )
{
    s_theGame->m_entities.push_back( entity );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::ProcessInput( InputSystem* inputSystem, const double& deltaSeconds )
{
    ///---------------------------------------------------------------------------------
    /// Mouse/Keyboard controls
    ///---------------------------------------------------------------------------------

    switch (m_gameState)
    {
    case GS_MAIN_MENU:
        m_mainMenu->ProcessInput( inputSystem, deltaSeconds );
        break;

    case GS_QUEST_MENU:
        m_questMenu->ProcessInput( inputSystem, deltaSeconds );
        break;

    case GS_ONE_STEP_GENERATION:
        if (inputSystem->WasKeyJustReleased( VK_SPACE ))
            if (m_questMenu->GetGeneratorToUse()->ProcessOneStep( m_map ))
            {
                m_questMenu->GetGeneratorToUse()->FinalizeMap( m_map );
                for each (Entity* entity in m_map->GetGeneratedEntities() )
                    AddEntity( entity );

                m_entities.push_back( m_player );
                m_player->SetMapPosition( m_map->GetPlayerStart() );
                m_player->SetMap( m_map );
                m_map->GetCellAtPosition( m_player->GetMapPosition() )->SetEntity( m_player );
                FieldOfViewCalculator::CalculateFOV_BFR( (Actor*)m_player, m_player->GetVisibilityRange(), m_map, true );
                SetGameState( GS_IN_GAME );

            } 
        if (inputSystem->WasKeyJustReleased( VK_RETURN ))
            SetGameState( GS_AUTOMATIC_GENERATION );
        ProcessGameInput( inputSystem, deltaSeconds );
        break;

    case GS_AUTOMATIC_GENERATION:
        break;

    case GS_IN_GAME:
        if (MessageBar::HasMoreMessages())
        {
            MessageBar::ProcessInput( inputSystem, deltaSeconds );
        }
        else
        {
            ProcessGameInput( inputSystem, deltaSeconds );
            if ( m_player )
                m_player->ProcessInput( inputSystem, deltaSeconds );
        }
        break;

    case GS_PAUSED:

        break;

    case GS_ACKNOWLEDGE_SAVE:
        if (inputSystem->WasKeyJustReleased( VK_SPACE ))
        {
            ResetGame();
            SetGameState( GS_MAIN_MENU );
        }

    case GS_GAME_OVER:
        m_gameOverMenu->ProcessInput( inputSystem, deltaSeconds );
        break;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::ProcessGameInput( InputSystem* inputSystem, const double& deltaSeconds )
{
    UNUSED( deltaSeconds );

    if (inputSystem->WasKeyJustReleased( VK_ESCAPE ))
    {
        ResetGame();
        SetGameState( GS_QUEST_MENU );
    }

    if ((inputSystem->IsKeyDown( VK_SHIFT ) && inputSystem->WasKeyJustReleased( 'S' )) || inputSystem->WasKeyJustReleased( VK_F6 ))
        SaveGame();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::Update( const double& deltaSeconds, const bool& debugModeEnabled )
{
    UNUSED( deltaSeconds );

    if (debugModeEnabled)
    {

    }


    switch (m_gameState)
    {
    case GS_MAIN_MENU:

        switch (m_mainMenu->Update( deltaSeconds, debugModeEnabled ))
        {
        case MMS_DO_NOTHING:
            break;

        case MMS_IS_READY_TO_QUIT:
            m_isReadyToQuit = true;
            break;

        case MMS_SWITCH_TO_PLAY:
            SetGameState( GS_QUEST_MENU );
            break;

        case MMS_SWITCH_TO_OPTIONS:
            break;

        case MMS_CONTINUE:
            LoadGameFromFile();
            SetGameState( GS_IN_GAME );
            break;
        }
        break;

    case GS_QUEST_MENU:
        switch (m_questMenu->Update( deltaSeconds, debugModeEnabled ))
        {
        case QMS_DO_NOTHING:
            break;

        case QMS_ONE_STEP:
            m_player = new Player();
            m_statusBar = new StatusBar( m_player );
            MessageBar::Startup( m_player, m_font );
            SetGameState( GS_ONE_STEP_GENERATION );
            if( !m_map )
                m_map = m_questMenu->GetGeneratorToUse()->CreateMap( IntVector2( 60, 30 ) );
            break;

        case QMS_AUTOMATIC: 
            m_player = new Player();
            m_statusBar = new StatusBar( m_player );
            MessageBar::Startup( m_player, m_font );
            SetGameState( GS_AUTOMATIC_GENERATION );
            if( !m_map )
                m_map = m_questMenu->GetGeneratorToUse()->CreateMap( IntVector2( 60, 30 ) );
            break;

        case QMS_BACK:
            m_mainMenu->Reset();
            SetGameState( GS_MAIN_MENU );
            m_questMenu->Reset();
            break;
        }
        break;

    case GS_ONE_STEP_GENERATION:
        break;

    case GS_AUTOMATIC_GENERATION:
    {
        while (!m_questMenu->GetGeneratorToUse()->ProcessOneStep( m_map )) {}
        m_questMenu->GetGeneratorToUse()->FinalizeMap( m_map );
        for each (Entity* entity in m_map->GetGeneratedEntities())
            AddEntity( entity );
        m_entities.push_back( m_player );
        m_player->SetMap( m_map );
        m_player->SetMapPosition( m_map->GetPlayerStart() );
        m_map->GetCellAtPosition( m_player->GetMapPosition() )->SetEntity( m_player );
        FieldOfViewCalculator::CalculateFOV_BFR( (Actor*)m_player, m_player->GetVisibilityRange(), m_map, true );

        SetGameState( GS_IN_GAME );
        break;
    }

    case GS_IN_GAME:
        m_map->Update( deltaSeconds, debugModeEnabled );
        UpdateEntities( deltaSeconds, debugModeEnabled );
        if (m_player->HasDied())
        {
            SetGameState( GS_GAME_OVER );
            m_gameOverMenu->SetPlayer( m_player );
            break;
        }
        m_player->Update( deltaSeconds, debugModeEnabled );
        MessageBar::Update( deltaSeconds, debugModeEnabled );
        break;

    case GS_ACKNOWLEDGE_SAVE:
        MessageBar::Update( deltaSeconds, debugModeEnabled );
        break;

    case GS_PAUSED:

        break;

    case GS_GAME_OVER:
        switch (m_gameOverMenu->Update( deltaSeconds, debugModeEnabled ))
        {
        case GOMS_DO_NOTHING:
            break;
        case GOMS_GO_TO_QUEST:
        {
            ResetGame();
            SetGameState( GS_QUEST_MENU );
            break;
        }
        case GOMS_IS_READY_TO_QUIT:
            m_isReadyToQuit = true;
            break;
        }
        break;

    default:
        break;
    }


}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::UpdateEntities( const double& deltaSeconds, const bool& debugModeEnabled )
{
    UNUSED( deltaSeconds );
    UNUSED( debugModeEnabled );

    if (!m_map)
        return;

    static bool playerHasActed = false;


    //remove dead entities from list

    int numEntitiesKilled = 0;
    bool destoryedEntity = false;
    for (std::vector< Entity* >::iterator entityIter = m_entities.begin(); entityIter != m_entities.end();)
    {
        Entity* entity = *entityIter;
        if (entity->HasDied())
        {
            numEntitiesKilled++;
            if (StringTable::GetStringID( entity->GetName() ) == StringTable::GetStringID( "The Dreamer" ))
            {
                SetGameState( GS_GAME_OVER );
                m_gameOverMenu->SetPlayer( m_player );
                return;
            }

            if (entity->IsActor())
                ((Actor*)entity)->DropInventory();

            delete entity;
            entityIter = m_entities.erase( entityIter );
            destoryedEntity = true;
        }
        else
            entityIter++;
    }
    if ( destoryedEntity )
        FieldOfViewCalculator::CalculateFOV_BFR( (Actor*)m_player, m_player->GetVisibilityRange(), m_map, true );

    // spawn a new npc for every npc killed (except the dreamer)
    NPCFactories npcFactories = NPCFactory::GetFactories();
    for (int i = 0; i < numEntitiesKilled; i++)
    {
        
        int factoryIndex = GetRandomIntInRange( 0, npcFactories.size() - 1 );
        NPCFactories::iterator factoryIter = npcFactories.begin();
        std::advance( factoryIter, factoryIndex );

        if (factoryIter->first == "The Dreamer")
        {
            i--;
            continue;
        }

        NPCFactory* factory = factoryIter->second;

        NPC* newNPC = factory->SpawnNPC( XMLNode::emptyNode() );

        MapPosition newNPCPos = m_map->GetRandomOpenPosition();
        
        // ensure the player can't see the spawn
        while ( Map::CalculateManhattanDistance( m_player->GetMapPosition(), newNPCPos ) < ( m_player->GetVisibilityRange() * 2 ) )
            newNPCPos = m_map->GetRandomOpenPosition();

        newNPC->SetMap( m_map );
        m_map->SetEntityAtPosition( newNPC, newNPCPos );

        AddEntity( newNPC );
    }

//     if (!playerHasActed)
//     {
//         float actionTime = 0.0f;
// 
//         bool playerReady = m_player->IsReadyToAct();
//         if (playerReady)
//             actionTime = m_player->Think();
// 
//         if (actionTime > 0.0f)
//             playerHasActed = true;
//     }
// 
//     else if (playerHasActed)
//     {
    for (std::vector< Entity* >::iterator entityIter = m_entities.begin(); entityIter != m_entities.end(); ++entityIter)
    {
        Entity* entity = *entityIter;

        if (entity->IsPlayer())
        {
            float actionTime = 0.0f;

            bool playerReady = m_player->IsReadyToAct();
            if (playerReady)
            {
                InteractionEvent* event = m_player->Think( actionTime );
                if (event)
                    m_events.push_back( event );
            }

            if (actionTime > 0.0f)
                playerHasActed = true;
        }

    }

    if (playerHasActed)
    {
        for (std::vector< Entity* >::iterator entityIter = m_entities.begin(); entityIter != m_entities.end(); ++entityIter)
        {
            Entity* entity = *entityIter;

            if (entity->IsActor() && !entity->HasDied() && !entity->IsPlayer() && playerHasActed)
            {
                float actionTime = 0.0f;
                InteractionEvent* event = entity->Think( actionTime );
                if (event)
                    m_events.push_back( event );
            }

            else if (entity->IsFeature())
            {
                float actionTime = 0.0f;
                InteractionEvent* event = entity->Think( actionTime );
                if (event)
                    m_events.push_back( event );
            }
        }

        playerHasActed = false;
    }
//     }

    for (std::vector< InteractionEvent* >::iterator eventIter = m_events.begin(); eventIter != m_events.end();)
    {
        InteractionEvent* event = *eventIter;

        for (std::vector< Entity* >::iterator entityIter = m_entities.begin(); entityIter != m_entities.end(); ++entityIter)
        {
            Entity* entity = *entityIter;
            entity->HandleEvent( event );
        }

        delete event;
        eventIter = m_events.erase( eventIter );
    }

    m_events.clear();
    m_events.resize( 0 );


}

////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::Render( bool debugModeEnabled )
{

    UNUSED( debugModeEnabled );

    if (!m_renderer)
        return;
    m_renderer->CalculateOrthographicMatrix( m_displaySize.x, m_displaySize.y, 100, 1, true );

    m_renderer->Disable( GL_CULL_FACE );
    m_renderer->Enable( GL_DEPTH_TEST );
    m_renderer->Enable( GL_BLEND );
    m_renderer->Enable( GL_TEXTURE_2D );
    m_renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    switch (m_gameState)
    {
    case GS_MAIN_MENU:
        m_mainMenu->Render( m_renderer, m_fontRenderer, debugModeEnabled );
        break;

    case GS_QUEST_MENU:
        m_questMenu->Render( m_renderer, m_fontRenderer, debugModeEnabled );
        break;

    case GS_ONE_STEP_GENERATION:
    {
        // Title
        int titleSize = 64;
        std::string title = "Generating Map...";
        float titleWidth = m_fontRenderer->CalcTextWidth( titleSize, *m_titleFont, title );
        float xOffsetTitle = (m_displaySize.x - titleWidth) / 2.0f;

        Vector3 titlePos( xOffsetTitle, 800.0f, 2.0f );
        m_fontRenderer->DrawFontText( titleSize, *m_titleFont, title, titlePos, Rgba( 0x660066 ) );

        RenderMap( debugModeEnabled );
        break;
    }

    case GS_AUTOMATIC_GENERATION:
        break;

    case GS_IN_GAME:
        RenderGame( debugModeEnabled );
        break;

    case GS_ACKNOWLEDGE_SAVE:
        RenderGame( debugModeEnabled );
        break;
    

    case GS_PAUSED:

        break;

    case GS_GAME_OVER:
        m_gameOverMenu->Render( m_renderer, m_fontRenderer, debugModeEnabled );
        break;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::RenderGame( bool debugModeEnabled )
{
    // Title
    int titleSize = 64;
    //std::string objective = "Playing";
    std::string title = "Find the Blood Sword and defeat The Dreamer!";
    float titleWidth = m_fontRenderer->CalcTextWidth( titleSize, *m_titleFont, title );
    float xOffsetTitle = (m_displaySize.x - titleWidth) / 2.0f;

//     int objectiveSize = 40;
//     float objectiveWidth = m_fontRenderer->CalcTextWidth( objectiveSize, *m_titleFont, objective );
//     float xOffsetObjective = (m_displaySize.x - objectiveWidth) / 2.0f;

    Vector3 titlePos( xOffsetTitle, 830.0f, 2.0f );
    m_fontRenderer->DrawFontText( titleSize, *m_titleFont, title, titlePos, Rgba( 0x660066 ) );

//     Vector3 objectivePos( xOffsetObjective, 805.0f, 2.0f );
//     m_fontRenderer->DrawFontText( m_fontShaderID, objectiveSize, *m_titleFont, objective, objectivePos, Rgba( 0x660066 ) );

    m_statusBar->Render( m_renderer, m_fontRenderer, *m_font, m_fontShaderID, debugModeEnabled );
    MessageBar::Render( m_renderer, m_fontRenderer, *m_font, m_fontShaderID, debugModeEnabled );

    //         // player pos 
    //         MapPosition playerPos = m_player->GetMapPosition();
    //         MapPosition playerPosFromScreenCoords = m_map->ConvertScreenCoordsToMapCoords( m_map->GetCellAtPosition( playerPos )->GetRenderBounds().m_mins + Vector2( m_map->GetCellSizePixels() / 2.0f ) );
    //        
    //         int coordsSize = 48;
    //         std::string coords = "Pos: " + std::to_string( playerPos.x ) + ", " + std::to_string( playerPos.y ) + " ----------- PosFromScreen: " + std::to_string( playerPosFromScreenCoords.x ) + ", " + std::to_string( playerPosFromScreenCoords.y );
    //         float coordsWidth = m_fontRenderer->CalcTextWidth( coordsSize, *m_font, coords );
    //         float xOffsetCoords = (m_displaySize.x - coordsWidth) / 2.0f;
    // 
    //         Vector3 coordsPos( xOffsetCoords + 30.0f, 800.0f, 2.0f );
    //         m_fontRenderer->DrawFontText( m_fontShaderID, coordsSize, *m_font, coords, coordsPos );


    RenderMap( debugModeEnabled );
    RenderVisibleEntities( debugModeEnabled );

    //m_player->Render( m_renderer, m_fontRenderer, *m_mapFont, m_fontShaderID, debugModeEnabled );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::RenderMap( bool debugModeEnabled )
{
   // m_renderer->DrawColoredQuad( NULL, Vector2( 0.0f, 75.0f ), Vector2( 0.0f + 1280.0f, 75.0f + 720.0f ), 1.0f, Rgba::BLACK );

    m_map->Render( m_renderer, m_fontRenderer, *m_mapFont, m_fontShaderID, debugModeEnabled );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::RenderVisibleEntities( bool debugModeEnabled )
{
    Cells* cells = m_map->GetCells();

    for (Cells::iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter)
    {
        Cell& cell = *cellIter;
        if (cell.IsVisible())
        {
            Entity* cellEntity = cell.GetEntity();
            if (cellEntity && !cellEntity->HasDied())
                cellEntity->Render( m_renderer, m_fontRenderer, *m_mapFont, m_fontShaderID, debugModeEnabled );

            Feature* cellFeature = cell.GetFeature();
            if (cellFeature && !cellFeature->HasDied())
                cellFeature->Render( m_renderer, m_fontRenderer, *m_mapFont, m_fontShaderID, debugModeEnabled );

            Inventory* cellInventory = cell.GetInventory();
            if (cellInventory)
            {
                cellInventory->Render( m_renderer, m_fontRenderer, *m_mapFont, m_fontShaderID, debugModeEnabled );
            }

                
        }
    }
}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::SetGameState( GameState newGameState )
{
    if (m_gameState != newGameState)
    {
        m_gameState = newGameState;
    }
    else
    {
        DeveloperConsole::WriteLine( "Game: Resetting State", WARNING_TEXT_COLOR );
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::SaveGame()
{
    std::string filePath = "Data/Saves/SaveGame.sav.xml";

    XMLNode root = CreateRootNode();

    XMLNode saveGameNode = CreateNode( "SaveGame" );
    AddChild( root, saveGameNode );

    m_map->Save( saveGameNode );

    XMLNode entities = CreateNode( "Entities");
    for each (Entity* entity in m_entities)
        entity->Save( entities, true );
    
    AddChild( saveGameNode, entities );

    CreateDirectory_Relative( "Data/", "Saves" );
    WriteToFile( filePath, root );

    Message msg;
    msg.m_instigator = "SaveGame()";
    msg.m_playerTurn = m_player->GetTurnsTaken();
    msg.m_position = m_player->GetMapPosition();
    msg.m_text = "The game has been successfully saved to: " + filePath + "  Goodbye...   --MORE--";
    MessageBar::AddMessage( msg );
    MessageBar::SetClearCurrent( true );
    SetGameState( GS_ACKNOWLEDGE_SAVE );
}