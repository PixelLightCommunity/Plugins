//[-------------------------------------------------------]
//[ Header                                                ]
//[-------------------------------------------------------]
#include "SRPAwesomium.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLRenderer;
using namespace PLMath;
using namespace PLGraphics;
using namespace PLScene;
using namespace PLInput;
using namespace PLEngine;
using namespace AWESOMIUM;


void AWESOMIUM::js_console_callback(awe_webview *caller, const awe_string *message, int line_number, const awe_string *source)
{
	System::GetInstance()->GetConsole().Print(String(line_number) + ": " + String(awe_string_get_utf16(message)) + "\n");
}


void AWESOMIUM::js_callback(awe_webview *caller, const awe_string *object_name, const awe_string *callback_name, const awe_jsarray *arguments)
{
	System::GetInstance()->GetConsole().Print("object_name: " + String(awe_string_get_utf16(object_name)) + "\n");
	System::GetInstance()->GetConsole().Print("callback_name: " + String(awe_string_get_utf16(callback_name)) + "\n");
	System::GetInstance()->GetConsole().Print("parameters: " + String(awe_jsarray_get_size(arguments)) + "\n");
	for (size_t i = 0; i < awe_jsarray_get_size(arguments); i++)
	{
		awe_string *argument = awe_jsvalue_to_string(awe_jsarray_get_element(arguments, i));
		System::GetInstance()->GetConsole().Print("value: " + String(awe_string_get_utf16(argument)) + "\n");
		awe_string_destroy(argument); // not needed anymore
	}
}


//[-------------------------------------------------------]
//[ Functions		                                      ]
//[-------------------------------------------------------]
SRPAwesomium::SRPAwesomium(EngineApplication &cEngineApplication, Renderer &cRenderer, int nWidth, int nHeight, const String &sUrl, int nX, int nY, bool bAllowEvents) :
	m_cEngineApplication(cEngineApplication),
	m_cRenderer(cRenderer),
	m_pVertexBuffer(nullptr),
	m_vPosition(float(nX), float(nY)),
	m_vImageSize(float(nWidth), float(nHeight)),
	m_fZValue2D(0.0f),
	m_vTextureCoordinate(Vector2::Zero),
	m_vTextureCoordinateSize(Vector2::One),
	m_fTextureCoordinateScaleX(1.0f),
	m_fTextureCoordinateScaleY(1.0f),
	m_pVertexShader(nullptr),
	m_pFragmentShader(nullptr),
	m_pProgram(nullptr),
	m_nFrameWidth(nWidth),
	m_nFrameHeight(nHeight),
	m_sUrl(sUrl),
	m_nXPos(nX),
	m_nYPos(nY),
	m_cImage(Image::CreateImage(DataByte, ColorRGBA, Vector3i(nWidth, nHeight, 1))),
	m_pTextureBuffer(nullptr),
	m_bBufferReady(false),
	m_bAwesomiumCreated(false),
	m_bNeedsFullUpdate(true),
	m_bIsActive(false),
	m_bLoaded(false),
	m_bLoading(false),
	m_bAllowEvents(bAllowEvents),
	m_pController(nullptr),
	SlotControllerEvents(this),
	m_nMouseX(0),
	m_nMouseY(0),
	m_bDrawPointer(true),
	m_sPointerImagePath("Pointer.png"),
	m_pPointerTexture(cRenderer.GetRendererContext().GetTextureManager().LoadResource(m_sPointerImagePath))
{
	if (cEngineApplication.IsRunning() && cRenderer.IsInitialized())
	{
		DebugToConsole("Initializing..\n");
		DebugToConsole("CreateVertexBuffer(): " + String(CreateVertexBuffer() ? "True\n" : "False\n")); // creates the vertex buffer
		DebugToConsole("InitShader(): " + String(InitShader() ? "True\n" : "False\n")); // initialized the shader
		if (m_cImage.CheckConsistency() == 2) // no buffer data!
		{
			m_pTextureBuffer = reinterpret_cast<TextureBuffer*>(cRenderer.CreateTextureBuffer2D(m_cImage, TextureBuffer::Unknown, 0));
			m_bBufferReady = true;
			CreateAwesomium();
			CreateController();
			//////////////////////////////////////////////////////////////////////////
			// JavaScript callbacks proof of concept
			//////////////////////////////////////////////////////////////////////////
			awe_webview_set_callback_js_console_message(m_pWebView, js_console_callback); // sets javascript console messages callback function
			awe_string *str_object = awe_string_create_from_ascii(String("Client").GetASCII(), String("Client").GetLength());
			awe_webview_create_object(m_pWebView, str_object); // creates a object so javascript knows about it
			awe_string *str_call = awe_string_create_from_ascii(String("setToolBrush").GetASCII(), String("setToolBrush").GetLength());
			awe_webview_set_object_callback(m_pWebView, str_object, str_call); // set callback for function
			awe_string_destroy(str_call); // not needed anymore
			awe_webview_set_callback_js_callback(m_pWebView, js_callback); // get callbacks
			awe_string_destroy(str_object); // not needed anymore
			//////////////////////////////////////////////////////////////////////////
		}
	}
	else
		DebugToConsole("Renderer is not initialized!\n");
}


SRPAwesomium::~SRPAwesomium()
{
	m_bIsActive = false;
	DebugToConsole("Shutting down..\n");
	if (m_bAwesomiumCreated)
	{
		awe_webview_destroy(m_pWebView);
		awe_webcore_shutdown();
	}
	if (m_pProgram)
		delete m_pProgram;
	if (m_pFragmentShader)
		delete m_pFragmentShader;
	if (m_pVertexShader)
		delete m_pVertexShader;
	if (m_pVertexBuffer)
		delete m_pVertexBuffer;
	if (m_pPointerTexture)
		delete m_pPointerTexture;
	DebugToConsole("Terminating..\n");
}


void SRPAwesomium::DebugToConsole(const String &sString)
{
	#ifdef _CONSOLE
		System::GetInstance()->GetConsole().Print(String("SRPAwesomium: ") + sString);
	#endif
}


void SRPAwesomium::Draw(Renderer &cRenderer, const SQCull &cCullQuery)
{
	if (m_pProgram)
	{
		cRenderer.SetProgram(m_pProgram);

		if (m_pTextureBuffer && m_bBufferReady)
		{
			{ // make global
				const PLMath::Rectangle &cViewportRect = cRenderer.GetViewport();
				float fX1 = cViewportRect.vMin.x;
				float fY1 = cViewportRect.vMin.y;
				float fX2 = cViewportRect.vMax.x;
				float fY2 = cViewportRect.vMax.y;

				Matrix4x4 m_mObjectSpaceToClipSpace;
				m_mObjectSpaceToClipSpace.OrthoOffCenter(fX1, fX2, fY1, fY2, -1.0f, 1.0f);

				ProgramUniform *pProgramUniform = m_pProgram->GetUniform("ObjectSpaceToClipSpaceMatrix");
				if (pProgramUniform)
					pProgramUniform->Set(m_mObjectSpaceToClipSpace);
			}

			cRenderer.SetRenderState(RenderState::BlendEnable, true);

			const int nTextureUnit = m_pProgram->Set("TextureMap", m_pTextureBuffer);
			if (nTextureUnit >= 0)
			{
				cRenderer.SetSamplerState(nTextureUnit, Sampler::AddressU, TextureAddressing::Clamp);
				cRenderer.SetSamplerState(nTextureUnit, Sampler::AddressV, TextureAddressing::Clamp);
				cRenderer.SetSamplerState(nTextureUnit, Sampler::MagFilter, TextureFiltering::None);
				cRenderer.SetSamplerState(nTextureUnit, Sampler::MinFilter, TextureFiltering::None);
				cRenderer.SetSamplerState(nTextureUnit, Sampler::MipFilter, TextureFiltering::None);
			}
			m_pProgram->Set("VertexPosition", m_pVertexBuffer, VertexBuffer::Position);
			m_pProgram->Set("VertexTexCoord", m_pVertexBuffer, VertexBuffer::TexCoord);

			cRenderer.DrawPrimitives(Primitive::TriangleStrip, 0, 4);
			if (m_bDrawPointer && m_nMouseX > 0 && m_nMouseY > 0) DrawPointer(Vector2(float(m_nMouseX - 4), float(m_nMouseY + 2)));
		}
	}
}


bool SRPAwesomium::CreateVertexBuffer()
{
	// Create vertex buffer
	if (!m_pVertexBuffer || !m_bBufferReady)
	{
		m_pVertexBuffer = m_cRenderer.CreateVertexBuffer();
		if (m_pVertexBuffer)
		{
			m_pVertexBuffer->AddVertexAttribute(VertexBuffer::Position, 0, VertexBuffer::Float3);
			m_pVertexBuffer->AddVertexAttribute(VertexBuffer::Color,    0, VertexBuffer::RGBA);
			m_pVertexBuffer->AddVertexAttribute(VertexBuffer::TexCoord, 0, VertexBuffer::Float2);
			m_pVertexBuffer->Allocate(4, Usage::WriteOnly);

			// Setup the vertex buffer
			if (m_pVertexBuffer->Lock(Lock::WriteOnly))
			{
				// Vertex 0
				float *pfVertex = static_cast<float*>(m_pVertexBuffer->GetData(0, VertexBuffer::Position));
				pfVertex[0] = m_vPosition.x;
				pfVertex[1] = m_vPosition.y + m_vImageSize.y;
				pfVertex[2] = m_fZValue2D;
				pfVertex	= static_cast<float*>(m_pVertexBuffer->GetData(0, VertexBuffer::TexCoord));
				pfVertex[0] = m_vTextureCoordinate.x*m_fTextureCoordinateScaleX;
				pfVertex[1] = (m_vTextureCoordinate.y + m_vTextureCoordinateSize.y)*m_fTextureCoordinateScaleY;

				// Vertex 1
				pfVertex	= static_cast<float*>(m_pVertexBuffer->GetData(1, VertexBuffer::Position));
				pfVertex[0] = m_vPosition.x + m_vImageSize.x;
				pfVertex[1] = m_vPosition.y + m_vImageSize.y;
				pfVertex[2] = m_fZValue2D;
				pfVertex	= static_cast<float*>(m_pVertexBuffer->GetData(1, VertexBuffer::TexCoord));
				pfVertex[0] = (m_vTextureCoordinate.x + m_vTextureCoordinateSize.x)*m_fTextureCoordinateScaleX;
				pfVertex[1] = (m_vTextureCoordinate.y + m_vTextureCoordinateSize.y)*m_fTextureCoordinateScaleY;

				// Vertex 2
				pfVertex	= static_cast<float*>(m_pVertexBuffer->GetData(2, VertexBuffer::Position));
				pfVertex[0] = m_vPosition.x;
				pfVertex[1] = m_vPosition.y;
				pfVertex[2] = m_fZValue2D;
				pfVertex	= static_cast<float*>(m_pVertexBuffer->GetData(2, VertexBuffer::TexCoord));
				pfVertex[0] = m_vTextureCoordinate.x*m_fTextureCoordinateScaleX;
				pfVertex[1] = m_vTextureCoordinate.y*m_fTextureCoordinateScaleY;

				// Vertex 3
				pfVertex	= static_cast<float*>(m_pVertexBuffer->GetData(3, VertexBuffer::Position));
				pfVertex[0] = m_vPosition.x + m_vImageSize.x;
				pfVertex[1] = m_vPosition.y;
				pfVertex[2] = m_fZValue2D;
				pfVertex	= static_cast<float*>(m_pVertexBuffer->GetData(3, VertexBuffer::TexCoord));
				pfVertex[0] = (m_vTextureCoordinate.x + m_vTextureCoordinateSize.x)*m_fTextureCoordinateScaleX;
				pfVertex[1] = m_vTextureCoordinate.y*m_fTextureCoordinateScaleY;

				// Unlock the vertex buffer
				m_pVertexBuffer->Unlock();
			}

		}
		else
			return false; // Error!
	}

	// Done
	return true;
}


bool SRPAwesomium::InitShader()
{
	String sVertexShaderSourceCode;
	String sFragmentShaderSourceCode;

	if (m_cRenderer.GetAPI() == "OpenGL ES 2.0")
	{
		sVertexShaderSourceCode   = "#version 100\n" + sVertexShaderSourceCodeGLSL;
		sFragmentShaderSourceCode = "#version 100\n" + sFragmentShaderSourceCodeGLSL;
	}
	else
	{
		sVertexShaderSourceCode   = "#version 110\n" + Shader::RemovePrecisionQualifiersFromGLSL(sVertexShaderSourceCodeGLSL);
		sFragmentShaderSourceCode = "#version 110\n" + Shader::RemovePrecisionQualifiersFromGLSL(sFragmentShaderSourceCodeGLSL);
	}

	m_pVertexShader = m_cRenderer.GetShaderLanguage(m_cRenderer.GetDefaultShaderLanguage())->CreateVertexShader(sVertexShaderSourceCode, "arbvp1");
	m_pFragmentShader = m_cRenderer.GetShaderLanguage(m_cRenderer.GetDefaultShaderLanguage())->CreateFragmentShader(sFragmentShaderSourceCode, "arbfp1");
	m_pProgram = static_cast<ProgramWrapper*>(m_cRenderer.GetShaderLanguage(m_cRenderer.GetDefaultShaderLanguage())->CreateProgram(m_pVertexShader, m_pFragmentShader));
	if (m_pProgram)
		return true;
	else
		return false;
}


void SRPAwesomium::CreateAwesomium()
{
	if (!m_bAwesomiumCreated)
	{
		DebugToConsole("Creating Awesomium..\n");
		awe_webcore_initialize(
			true,				// enable_plugins
			true,				// enable_javascript
			false,				// enable_databases
			awe_string_empty(), // package_path
			awe_string_empty(), // locale_path
			awe_string_empty(), // user_data_path
			awe_string_empty(), // plugin_path
			awe_string_empty(), // log_path
			AWE_LL_VERBOSE,		// log_level
			false,				// forceSingleProcess
			awe_string_empty(), // childProcessPath
			true,				// enable_auto_detect_encoding
			awe_string_empty(), // accept_language_override
			awe_string_empty(), // default_charset_override
			awe_string_empty(), // user_agent_override
			awe_string_empty(), // proxy_server
			awe_string_empty(), // proxy_config_script
			awe_string_empty(), // auth_server_whitelist
			true,				// save_cache_and_cookies
			0,					// max_cache_size
			false,				// disable_same_origin_policy
			false,				// disable_win_message_pump
			awe_string_empty()	// custom_css
		);
		m_pWebView = awe_webcore_create_webview(m_nFrameWidth, m_nFrameHeight, false);
		awe_string *url_str = awe_string_create_from_ascii(m_sUrl.GetASCII(), m_sUrl.GetLength());
		awe_webview_load_url(m_pWebView, url_str, awe_string_empty(), awe_string_empty(), awe_string_empty());
		awe_webview_set_transparent(m_pWebView, true);
		awe_webview_focus(m_pWebView);
		awe_string_destroy(url_str); // not needed anymore
		m_pAwesomiumRenderBuffer = awe_webview_render(m_pWebView);
		if (m_pWebView && m_pAwesomiumRenderBuffer)
		{
			m_bAwesomiumCreated = true;
			m_bIsActive = true;
			DebugToConsole("awe_webcore_are_plugins_enabled: " + String(awe_webcore_are_plugins_enabled() ? "True\n" : "False\n"));
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
		}
		else
			DebugToConsole("Could not create Awesomium!\n");
	}
}


void SRPAwesomium::UpdateAwesomium()
{
	if (m_bAwesomiumCreated)
	{
		{
			awe_webcore_update();
			m_pTextureBuffer->Upload(0, TextureBuffer::R8G8B8A8, awe_renderbuffer_get_buffer(m_pAwesomiumRenderBuffer));
		}
	}
}


bool SRPAwesomium::IsActive()
{
	return m_bIsActive;
}


bool SRPAwesomium::IsLoaded()
{
	return m_bLoaded;
}


void SRPAwesomium::CreateController()
{
	if (m_bAllowEvents)
	{
		m_pController = reinterpret_cast<Controller*>(m_cEngineApplication.GetInputController());
		if (m_pController)
		{
			m_pController->SignalOnControl.Connect(SlotControllerEvents);
			DebugToConsole("SlotControllerEvents connected..\n");
		}
	}
}


void SRPAwesomium::ControllerEvents(Control &cControl)
{
	KeyEvents(cControl);
	MouseEvents(cControl);

	// base events
	if (cControl.GetType() == ControlButton && reinterpret_cast<Button&>(cControl).IsHit())
	{
		if (cControl.GetName() == "KeyboardEscape")
			m_cEngineApplication.Exit(0);
		else if	(cControl.GetName() == "KeyboardM")
		{
			m_cEngineApplication.GetFrontend().SetMouseVisible(!m_cEngineApplication.GetFrontend().IsMouseVisible());
			m_cEngineApplication.GetFrontend().SetTrapMouse(!m_cEngineApplication.GetFrontend().IsMouseVisible());
		}
		else if	(cControl.GetName() == "KeyboardF5")
		{
			awe_webview_reload(m_pWebView);
			System::GetInstance()->GetConsole().ClearScreen();
		}
		else if	(cControl.GetName() == "KeyboardF6")
		{
			awe_string* url_str = awe_string_create_from_ascii(m_sUrl.GetASCII(), m_sUrl.GetLength());
			awe_webview_load_url(m_pWebView, url_str, awe_string_empty(), awe_string_empty(), awe_string_empty());
			awe_string_destroy(url_str); // not needed anymore
			System::GetInstance()->GetConsole().ClearScreen();
		}
	}
}


void SRPAwesomium::MouseEvents(Control &cControl)
{
	// Mouse moving
	MouseMove(m_cEngineApplication.GetFrontend().GetMousePositionX(), m_cEngineApplication.GetFrontend().GetMousePositionY());
	// Mouse clicking
	if (cControl.GetType() == ControlButton)
	{
		if (cControl.GetName() == "MouseLeft")
		{
			if (reinterpret_cast<Button&>(cControl).IsPressed())
				awe_webview_inject_mouse_down(m_pWebView, AWE_MB_LEFT);
			else
				awe_webview_inject_mouse_up(m_pWebView, AWE_MB_LEFT);
		}
		if (cControl.GetName() == "MouseRight")
		{
			if (reinterpret_cast<Button&>(cControl).IsPressed())
				awe_webview_inject_mouse_down(m_pWebView, AWE_MB_RIGHT);
			else
				awe_webview_inject_mouse_up(m_pWebView, AWE_MB_RIGHT);
		}
	}
	// Mouse scrolling
	else if (cControl.GetType() == ControlAxis)
	{
		if (cControl.GetName() == "MouseWheel")
		{
			awe_webview_inject_mouse_wheel(m_pWebView, int(static_cast<Axis&>(cControl).GetValue()), 0);
		}
	}
}


void SRPAwesomium::KeyEvents(Control &cControl)
{
// 	if (cControl.GetType() == ControlButton)
// 	{
// 		char nKey = cControl.GetController()->GetChar();
// 		String sKey = reinterpret_cast<Button&>(cControl).GetName();
// 		bool bTextKey = true;
// 		char vk_code = 0;
// 
// 		if (sKey == "KeyboardBackspace")
// 		{
// 			vk_code = 0x08;
// 			bTextKey = false;
// 		}
// 
// 		if (bTextKey)
// 		{
// 			wchar_t outchars[2];
// 			outchars[0] = nKey;
// 			outchars[1] = 0;
// 			//m_pBerkeliumWindow->textEvent(outchars,1);
// 
// 			awe_webview_inject_keyboard_event(m_pWebView, '');
// 		}
// 		else
// 		{
// 			m_pBerkeliumWindow->keyEvent(true, 0, vk_code, 0);
// 			m_pBerkeliumWindow->keyEvent(false, 0, vk_code, 0);
// 		}
// 	}
}


void SRPAwesomium::MouseMove(int nX, int nY)
{
	m_nMouseX = nX;
	m_nMouseY = nY;
	if (m_bAwesomiumCreated)
	{
		awe_webview_inject_mouse_move(m_pWebView, m_nMouseX - m_nXPos, m_nMouseY - m_nYPos);
	}
}


void SRPAwesomium::ChangePointerState(bool bDrawPointer)
{
	m_bDrawPointer = bDrawPointer;
}


void SRPAwesomium::SetPointerImagePath(const String &sPointerImagePath, bool bDrawPointer)
{
	m_bDrawPointer = false;
	m_sPointerImagePath = sPointerImagePath;
	m_pPointerTexture = m_cRenderer.GetRendererContext().GetTextureManager().LoadResource(m_sPointerImagePath);
	if (m_pPointerTexture)
	{
		m_bDrawPointer = bDrawPointer;
	}
}


void SRPAwesomium::DrawPointer(const Vector2 &vPos)
{
	if (m_pPointerTexture->GetTextureBuffer())
	{
		m_cRenderer.GetDrawHelpers().Begin2DMode(0.0f, 0.0f, 0.0f, 0.0f);
		m_cRenderer.GetDrawHelpers().DrawImage(*m_pPointerTexture->GetTextureBuffer(), m_cPointerSamplerStates, vPos);
		m_cRenderer.GetDrawHelpers().End2DMode();
	}
}


void SRPAwesomium::NavigateTo(const String &sUrl)
{
	if (m_bIsActive)
	{
		awe_string* url_str = awe_string_create_from_ascii(sUrl.GetASCII(), sUrl.GetLength());
		awe_webview_load_url(m_pWebView, url_str, awe_string_empty(), awe_string_empty(), awe_string_empty());
		awe_string_destroy(url_str); // not needed anymore
	}
}


void SRPAwesomium::WindowResize(int nWidth, int nHeight, int nX, int nY, bool bAbsolute)
{
	bool bChanged = false;
	m_bBufferReady = false;
	if (nX != 0 || nY != 0 || bAbsolute)
	{
		if (bAbsolute)
		{
			m_nXPos = nX;
			m_nYPos = nY;
		}
		else
		{
			m_nXPos += nX;
			m_nYPos += nY;
		}
		m_vPosition = Vector2(float(m_nXPos), float(m_nYPos));
		bChanged = true;
		DebugToConsole("Triggered a move!!!\n");
	}
	if (nWidth != m_nFrameWidth && nHeight != m_nFrameHeight)
	{
		m_nFrameWidth = nWidth;
		m_nFrameHeight = nHeight;
		m_vImageSize = Vector2(float(m_nFrameWidth), float(m_nFrameHeight));
		m_cImage = Image::CreateImage(DataByte, ColorRGBA, Vector3i(m_nFrameWidth, m_nFrameHeight, 1));
		m_pTextureBuffer = reinterpret_cast<TextureBuffer*>(m_cRenderer.CreateTextureBuffer2D(m_cImage, TextureBuffer::Unknown, 0));
		awe_webview_resize(m_pWebView, m_nFrameWidth, m_nFrameHeight, true, 100);
		bChanged = true;
		DebugToConsole("Triggered a resize!!!\n");
	}
	if (bChanged) CreateVertexBuffer();
	m_bBufferReady = true;
}


void SRPAwesomium::WindowMove(int nX, int nY, bool bAbsolute)
{
	m_bBufferReady = false;
	if (bAbsolute)
	{
		m_nXPos = nX;
		m_nYPos = nY;
	}
	else
	{
		m_nXPos += nX;
		m_nYPos += nY;
	}
	m_vPosition = Vector2(float(m_nXPos), float(m_nYPos));
	CreateVertexBuffer();
	m_bBufferReady = true;
}


Vector2 SRPAwesomium::GetWindowSize()
{
	return Vector2(float(m_nFrameWidth), float(m_nFrameHeight));
}


Vector2 SRPAwesomium::GetWindowPosition()
{
	return Vector2(float(m_nXPos), float(m_nYPos));
}