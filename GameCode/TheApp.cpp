//=================================================================================
// TheApp.cpp
// Author: Tyler George
// Date  : January 13, 2015
//=================================================================================

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Engine/Utilities/Time.hpp"
#include "GameCode/TheApp.hpp"
#include "Engine/Renderer/MeshCreator.hpp"
#include "Engine/Renderer/PuttyMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/MeshRenderer.hpp"
#include "Engine/Utilities/Rgba.hpp"

///---------------------------------------------------------------------------------
/// 
///---------------------------------------------------------------------------------
TheApp::TheApp()
    : m_isRunning( true )
    , m_windowHandle( nullptr )
    , m_renderer( nullptr )
    , m_game( nullptr )
    , m_inputSystem( nullptr )
//     , m_soundSystem( nullptr )
    , m_debugModeEnabled( false )
    , m_developerConsole( nullptr )
    , m_developerConsoleEnabled( false )
    , m_graphicsDebugger( nullptr )
    , m_fontRenderer( nullptr )
    , m_engineLoadingScreen( nullptr )
    , m_fps( "" )
    , m_texturedShaderID( 0 )
    , m_font( nullptr )
{
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
TheApp::~TheApp() 
{
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Startup( void* windowHandle ) 
{
    m_windowHandle = windowHandle;
	m_renderer = new OpenGLRenderer();

    if (m_renderer)
    {
        m_renderer->Startup( (HWND)windowHandle );
        m_displaySize = Vector2( m_renderer->GetDisplayWidth(), m_renderer->GetDisplayHeight() );
    }

    m_engineLoadingScreen = Texture::CreateOrGetTexture( "Data/Images/EngineLoading.png" );

    unsigned int basicShaderID = m_renderer->LoadProgram( "Data/Shaders/basic.vert", "Data/Shaders/basic.frag" );
    m_renderer->GLCheckError();

    m_texturedShaderID = m_renderer->LoadProgram( "Data/Shaders/textured.vert", "Data/Shaders/textured.frag" );
    m_renderer->GLCheckError();

    RenderLoadingScreen();

    //InitializeTimer();

    m_font = new Font( "Data/Fonts/Calibri.fnt" );

    m_inputSystem = new InputSystem();
    if (m_inputSystem)
        m_inputSystem->Startup( windowHandle );

//     m_soundSystem = new SoundSystem();
//     if (m_soundSystem)
//         m_soundSystem->Startup();


//     if (!m_graphicsDebugger)
//         m_graphicsDebugger = GraphicsDebugger::GetInstance( m_renderer, basicShaderID );

    if (!m_fontRenderer)
        m_fontRenderer = new FontRenderer( m_renderer );

    if (!m_developerConsole)
    {
        m_developerConsole = DeveloperConsole::GetInstance( m_renderer, m_graphicsDebugger );
    }

    //m_renderer->SetDevConsole( m_developerConsole );
    m_renderer->CheckCompatability();


	m_game = new Game( m_displaySize );
	m_game->Startup( m_renderer, m_fontRenderer, basicShaderID );	


}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Run()
{
	while ( m_isRunning ) 
	{
        // initialize lastTimeSeconds
		static double lastTimeSeconds = GetCurrentSeconds();
        static double lastDeltaSeconds = 0.0;

        double currentTimeSeconds = GetCurrentSeconds();
		double deltaSeconds = currentTimeSeconds - lastTimeSeconds;

// 		if (deltaSeconds > 0.5)
// 			deltaSeconds = 0.5;

		ProcessInput( deltaSeconds );
		Update( deltaSeconds );

        double averagedDelta = deltaSeconds * 0.9 + lastDeltaSeconds * 0.1;
//         if (averagedDelta > 1.0)
//             int life = 42;

        double fps = 1.0 / averagedDelta;
        m_fps = std::to_string( fps );

		lastTimeSeconds = currentTimeSeconds;
        lastDeltaSeconds = deltaSeconds;

		Render();
	}

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Shutdown() 
{
	if( m_renderer )
		m_renderer->Shutdown();

// 	m_soundSystem->Shutdown();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::ProcessInput( double deltaSeconds )
{
	if( m_inputSystem )
	{
		m_inputSystem->Update();
		if( m_inputSystem->IsReadyToQuit() || m_game->IsReadyToQuit() )
			m_isRunning = false;
		if( m_inputSystem->WasKeyJustReleased( VK_F3 ) )
			m_debugModeEnabled = !m_debugModeEnabled;
        if (m_inputSystem->WasKeyJustReleased( VK_OEM_3 ) || m_developerConsole->IsReadyToDisable() )
        {
            if (m_developerConsole->IsEnabled())
                m_developerConsole->SetDisabled();
            else
                m_developerConsole->SetEnabled();
        }

        if (m_developerConsole->IsEnabled())
        {
            m_developerConsole->ProcessInput( m_inputSystem );
        }

        else
        {
            m_game->ProcessInput( m_inputSystem, deltaSeconds );
        }
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Update( double deltaSeconds )
{
// 	if( m_soundSystem )
// 		m_soundSystem->Update();

	if( m_game )
		m_game->Update( deltaSeconds, m_debugModeEnabled );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Render()
{
	if( !m_renderer )
		return;

    float fieldOfViewDegreesVertical = 50.0f; // 45.0f;
	float aspectRatio = ( m_displaySize.x / m_displaySize.y );
    float nearDepth = 0.1f;// 0.01f;
    float farDepth = 500.0f; // 1000.0f;

	m_renderer->Clear( Rgba::GREY );

    m_renderer->CalculateProjectionPerspectiveMatrix( fieldOfViewDegreesVertical, aspectRatio, nearDepth, farDepth, true );
    m_renderer->CalculateOrthographicMatrix( m_displaySize.x, m_displaySize.y, nearDepth, farDepth, true );

    if (m_debugModeEnabled)
    {
        Vector3 fpsPos( 1300.0f, m_displaySize.y - 58.0f, 1.0f );
        m_fontRenderer->DrawFontText( 48, *m_font, m_fps, fpsPos, Rgba::WHITE );
    }

	RenderGame();
   // m_graphicsDebugger->ExecuteCommands();

    if ( m_developerConsole->IsEnabled() )
        RenderDevConsole();



	m_renderer->SwapBuffers();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::RenderGame() 
{
	m_game->Render( m_debugModeEnabled );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::RenderDevConsole()
{
    m_developerConsole->Render( m_debugModeEnabled );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::RenderLoadingScreen()
{
    m_renderer->Clear( Rgba::GREY );

    m_renderer->CalculateOrthographicMatrix( m_displaySize.x, m_displaySize.y, 0.1f, 500.0f, true );

    PUTBNC_Vertexes quadVerts = GenerateQuadVertexes( Vector3( 0.0f, 0.0f, 1.0f), Vector3( m_displaySize.x, m_displaySize.y, 1.0f ), Rgba() );

    PuttyMesh* quadMesh = new PuttyMesh( m_renderer );
    quadMesh->SetVertexData( quadVerts.data(), DrawInstructions( GL_QUADS, quadVerts.size(), 0, false ), Vertex3D_PUTBNC::GetVertexInfo() );
   

    RenderState rs( false, false, false, false );
    Material* quadMaterial = new Material( m_renderer, m_texturedShaderID, rs );
    quadMaterial->SetTextureUniform( "gTexture", m_engineLoadingScreen );

    MeshRenderer* quadRenderer = new MeshRenderer( m_renderer );
    quadRenderer->SetMesh( quadMesh );
    quadRenderer->SetMaterial( quadMaterial, false );


    quadRenderer->Render( Matrix4f::CreateIdentity(), Matrix4f::CreateIdentity(), m_renderer->GetOrthoMatrix() );

    m_renderer->SwapBuffers();


}