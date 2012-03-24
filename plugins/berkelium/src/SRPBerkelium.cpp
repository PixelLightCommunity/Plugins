//[-------------------------------------------------------]
//[ Header                                                ]
//[-------------------------------------------------------]
#include "SRPBerkelium.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLRenderer;
using namespace PLMath;
using namespace PLGraphics;
using namespace PLScene;
using namespace Berkelium;
using namespace PLInput;
using namespace PLEngine;


//[-------------------------------------------------------]
//[ Functions		                                      ]
//[-------------------------------------------------------]
SRPBerkelium::SRPBerkelium(EngineApplication &cEngineApplication, Renderer &cRenderer, int nWidth, int nHeight, const String &sUrl, int nX, int nY, bool bAllowEvents) :
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
	m_pBufferData(nullptr),
	m_bBufferReady(false),
	m_bBerkeliumCreated(false),
	m_pBerkeliumWindow(nullptr),
	m_bNeedsFullUpdate(true),
	m_bIsActive(false),
	m_bLoaded(false),
	m_bLoading(false),
	m_bAllowEvents(bAllowEvents),
	m_pController(nullptr), // cleanup? NOPE.avi
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
			m_pBufferData = m_cImage.GetBuffer()->GetData();
			m_bBufferReady = true;
			CreateController();
			CreateBerkelium();
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
		}
	}
	else
		DebugToConsole("Renderer is not initialized!\n");
}


SRPBerkelium::~SRPBerkelium()
{
	m_bIsActive = false;
	DebugToConsole("Shutting down..\n");
	if (m_bBerkeliumCreated)
	{
		m_pBerkeliumWindow->destroy();
		Berkelium::destroy();
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
	//if (m_pTextureBuffer) // different method
	//	delete m_pTextureBuffer;
	//if (m_pBufferData)  // different method
	//	delete m_pBufferData;
	DebugToConsole("Terminating..\n");
}


void SRPBerkelium::DebugToConsole(const String &sString)
{
	#ifdef _CONSOLE
		System::GetInstance()->GetConsole().Print(String("SRPBerkelium: ") + sString);
	#endif
}


void SRPBerkelium::Draw(Renderer &cRenderer, const SQCull &cCullQuery)
{
	if (m_pProgram)
	{
		cRenderer.SetProgram(m_pProgram);

		if (m_pTextureBuffer && m_bBufferReady && m_bLoaded)
		{
			{ // make global
			const Rectangle &cViewportRect = cRenderer.GetViewport();
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


bool SRPBerkelium::CreateVertexBuffer()
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


bool SRPBerkelium::InitShader()
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


void SRPBerkelium::CreateBerkelium()
{
	if (!m_bBerkeliumCreated)
	{
		if (Berkelium::init(Berkelium::FileString::empty()))
		{
			DebugToConsole("Creating Berkelium..\n");
			Berkelium::Context *pContext = Berkelium::Context::create();
			m_pBerkeliumWindow = Berkelium::Window::create(pContext);
			delete pContext; // not needed anymore
			m_pBerkeliumWindow->resize(m_nFrameWidth, m_nFrameHeight);
			m_pBerkeliumWindow->setTransparent(true);
			m_pBerkeliumWindow->setDelegate(this);
			m_pBerkeliumWindow->navigateTo(m_sUrl.GetASCII(), m_sUrl.GetLength());
			m_pBerkeliumWindow->focus();
			m_bBerkeliumCreated = true;
			m_bIsActive = true;
		}
		else
			DebugToConsole("Could not create Berkelium!\n");
	}
}


void SRPBerkelium::UpdateBerkelium()
{
	if (m_bBerkeliumCreated)
	{
		Berkelium::update();
	}
}


bool SRPBerkelium::IsActive()
{
	return m_bIsActive;
}


#pragma region Berkelium onEvent functions


void SRPBerkelium::onPaint(Window* wini, const unsigned char *bitmap_in, const Rect &bitmap_rect, size_t num_copy_rects, const Rect* copy_rects, int dx, int dy, const Rect& scroll_rect)
{
	if (m_bNeedsFullUpdate)
	{
		// awaiting a full update disregard all partials ones until the full comes in
		if (bitmap_rect.left() == 0 && bitmap_rect.top() == 0 && bitmap_rect.right() == m_nFrameWidth && bitmap_rect.bottom() == m_nFrameHeight)
		{
			MemoryManager::Copy(m_pBufferData, bitmap_in, m_nFrameWidth * m_nFrameHeight * 4);
			m_bNeedsFullUpdate = false;
			DebugToConsole("Expected full update so i rendered it..\n");
		}
	}
	else
	{
		if (bitmap_rect.width() == m_nFrameWidth && bitmap_rect.height() == m_nFrameHeight)
		{
			// did not suspect a full update but got it anyway, it might happen and is ok
			MemoryManager::Copy(m_pBufferData, bitmap_in, m_nFrameWidth * m_nFrameHeight * 4);
			DebugToConsole("Not expected full update\n");
		}
		else
		{
			if (dx != 0 || dy != 0)
			{
				// scrolling
				Berkelium::Rect scrolled_rect = scroll_rect.translate(-dx, -dy);
				Berkelium::Rect scrolled_shared_rect = scroll_rect.intersect(scrolled_rect);
				if (scrolled_shared_rect.width() > 0 && scrolled_shared_rect.height() > 0)
				{
					// Semi-working solution (not good enough)
					// scrolling to the left (big jumps) will cause a buffer overflow to occur
					//////////////////////////////////////////////////////////////////////////
					int wid = scroll_rect.width();
					int hig = scroll_rect.height();
					int top = scroll_rect.top();
					int left = scroll_rect.left();
					int tw = m_nFrameWidth;

					DebugToConsole(String::Format("dx: %i - dy: %i\n", dx, dy));
					
					if (dy < 0) // scroll down
					{
						DebugToConsole("Scroll down\n");
						for (int y = -dy; y < hig; y++)
						{
							unsigned int tb = ((top + y) * tw + left) * 4;
							unsigned int tb2 = tb + dy * tw * 4;
							MemoryManager::Copy(&m_pBufferData[tb2], &m_pBufferData[tb], wid * 4);
						}
					}
					else if (dy > 0) // scroll up
					{
						DebugToConsole("Scroll up\n");
						for (int y = hig - dy; y > -1; y--)
						{
							unsigned int tb = ((top + y) * tw + left) * 4;
							unsigned int tb2 = tb + dy * tw * 4;
							MemoryManager::Copy(&m_pBufferData[tb2], &m_pBufferData[tb], wid * 4);
						}
					}
					if(dx != 0) // scroll??
					{
						DebugToConsole("scroll??\n");
						int subx = dx > 0 ? 0 : -dx;
						for (int y = 0; y < hig; y++)
						{
							unsigned int tb = ((top + y) * tw + left) * 4;
							unsigned int tb2 = tb - dx * 4;
							MemoryManager::Copy(&m_pBufferData[tb], &m_pBufferData[tb2], wid * 4 - 0);
						}
					}
				}
				// new data for scrolling
				for (size_t i = 0; i < num_copy_rects; i++)
				{
					int nCrWidth = copy_rects[i].width();
					int nCrHeight = copy_rects[i].height();
					int nCrTop = copy_rects[i].top() - bitmap_rect.top();
					int nCrLeft = copy_rects[i].left() - bitmap_rect.left();
					int nBrTop = copy_rects[i].top();
					int nBrLeft = copy_rects[i].left();

					for(int nCrHeightIndex = 0; nCrHeightIndex < nCrHeight; nCrHeightIndex++)
					{
						int nStartPosition = (m_nFrameWidth - nBrLeft) * nBrTop + (nBrTop * nBrLeft) + nBrLeft + (m_nFrameWidth * nCrHeightIndex);
						MemoryManager::Copy(
							&m_pBufferData[nStartPosition * 4],
							bitmap_in + (nCrLeft + (nCrHeightIndex + nCrTop) * bitmap_rect.width()) * 4,
							nCrWidth * 4
							);
					}
				}
			}
			else
			{

				// COPY RECTS
				for (size_t i = 0; i < num_copy_rects; i++)
				{
					int nCrWidth = copy_rects[i].width();
					int nCrHeight = copy_rects[i].height();
					int nCrTop = copy_rects[i].top() - bitmap_rect.top();
					int nCrLeft = copy_rects[i].left() - bitmap_rect.left();
					int nBrTop = copy_rects[i].top();
					int nBrLeft = copy_rects[i].left();

					for(int nCrHeightIndex = 0; nCrHeightIndex < nCrHeight; nCrHeightIndex++)
					{
						int nStartPosition = (m_nFrameWidth - nBrLeft) * nBrTop + (nBrTop * nBrLeft) + nBrLeft + (m_nFrameWidth * nCrHeightIndex);
						MemoryManager::Copy(
							&m_pBufferData[nStartPosition * 4],
							bitmap_in + (nCrLeft + (nCrHeightIndex + nCrTop) * bitmap_rect.width()) * 4,
							nCrWidth * 4
							);
					}
				}
				// END COPY RECTS

			}
		}
	}
	// this is where it updates
	if (m_pBufferData && m_bBerkeliumCreated)
	{
		// this is what we sent to the GPU
		m_pTextureBuffer->Upload(0, TextureBuffer::R8G8B8A8, m_pBufferData);
	}
}


void SRPBerkelium::onStartLoading(Window *win, URLString newURL)
{
	// 	Indicates whether the main page has started navigating to a new URL.
	// 	Note:
	// 		See onLoadingStateChanged for deciding to show a loading message.
	DebugToConsole("onStartLoading\n");
	m_bLoading = true;
}


void SRPBerkelium::onNavigationRequested(Window *win, URLString newUrl, URLString referrer, bool isNewWindow, bool &cancelDefaultAction)
{
	// 	A navigation has been requested but not yet started.
	// 	I'm not sure yet if this gets called for frames.
	DebugToConsole("onNavigationRequested to: " + String(newUrl.data()) + String("\n\n"));
}


void SRPBerkelium::onCreatedWindow(Window *win, Window *newWindow, const Rect &initialRect)
{
	// 	A new window has already been created.
	// 	You are now expected to hold onto the instance and be responsible for deleting it.
	DebugToConsole("onCreatedWindow (A new window has already been created)\n");
}


void SRPBerkelium::onLoad(Window *win)
{
	//	A page has loaded without error.
	DebugToConsole("onLoad (A page has loaded without error)\n");
	m_bLoading = false;
	m_bLoaded = true;
}


void SRPBerkelium::onLoadingStateChanged(Window *win, bool isLoading)
{
	// 	Notifies if a page is loading.
	// 	This callback is only useful for deciding whether to show a loading indicator, for example a spinning widget or maybe a capital letter being bombarded by meteors.
	// 	Note: This may happen at different times from onStartLoading.
	DebugToConsole("onLoadingStateChanged (Notifies if a page is loading)\n");
}


void SRPBerkelium::onUnresponsive(Window *win)
{
	// 	A renderer instance is hung.
	// 	You can use this to display the Window in a greyed out state, and offer the user a choice to kill the Window.
	DebugToConsole("onUnresponsive (A renderer instance is hung)\n");
}


void SRPBerkelium::onResponsive(Window *win)
{
	// 	Renderer instance is back to normal.
	// 	Reset the color and close dialogs.
	DebugToConsole("onResponsive (Renderer instance is back to normal)\n");
}


void SRPBerkelium::onJavascriptCallback(Window *win, void* replyMsg, URLString origin, WideString funcName, Script::Variant *args, size_t numArgs)
{
	//	Javascript has called a bound function on this Window.
	//	Parameters:
	//		win			Window instance that fired this event.
	// 		replyMsg	If non-NULL, opaque reply identifier to be passed to synchronousScriptReturn.
	// 		url			Origin of the sending script.
	// 		funcName	name of function to call.
	// 		args		list of variants passed into function.
	// 		numArgs		number of arguments.
	DebugToConsole("onJavascriptCallback NEEDS TO BE INPLEMENTED!\n");
}


void SRPBerkelium::onWidgetPaint(Window *win, Widget *wid, const unsigned char *sourceBuffer, const Rect &sourceBufferRect, size_t numCopyRects, const Rect *copyRects, int dx, int dy, const Rect &scrollRect)
{
	//	A widget overlay has been painted.
	DebugToConsole("onWidgetPaint\n");
}


void SRPBerkelium::onWidgetCreated(Window *win, Widget *newWidget, int zIndex)
{
	//	A widget is a rectangle to display on top of the page, e.g. a context menu or a dropdown.
	DebugToConsole("onWidgetCreated zIndex: " + String(zIndex) + String("\n"));
}


void SRPBerkelium::onTooltipChanged(Window *win, WideString text)
{
	// 	Display a tooltip at the mouse cursor position.
	// 	There's no notification for destroying the tooltip afaik
	DebugToConsole("onTooltipChanged\n");
}


void SRPBerkelium::onConsoleMessage(Window *win, WideString message, WideString sourceId, int line_no)
{
	//	Display a javascript message (Maybe an error?) or console.log.
	DebugToConsole("onConsoleMessage: " + String(message.data()) + "\n");
}


#pragma endregion Berkelium onEvent functions


bool SRPBerkelium::IsLoaded()
{
	return m_bLoaded;
}


void SRPBerkelium::CreateController()
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


void SRPBerkelium::ControllerEvents(Control &cControl)
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
			m_pBerkeliumWindow->refresh();
			System::GetInstance()->GetConsole().ClearScreen();
		}
		else if	(cControl.GetName() == "KeyboardF6")
			m_pBerkeliumWindow->navigateTo(m_sUrl, m_sUrl.GetLength());
		else if	(cControl.GetName() == "KeyboardF8")
			WindowResize(m_nFrameWidth + 10, m_nFrameHeight + 10, 10, 10);
	}
}


void SRPBerkelium::MouseEvents(Control &cControl)
{
	// Mouse moving
	MouseMove(m_cEngineApplication.GetFrontend().GetMousePositionX(), m_cEngineApplication.GetFrontend().GetMousePositionY());
	// Mouse clicking
	if (cControl.GetType() == ControlButton)
	{
		if (cControl.GetName() == "MouseLeft")
		{
			m_pBerkeliumWindow->mouseButton(0, reinterpret_cast<Button&>(cControl).IsPressed());
		}
		if (cControl.GetName() == "MouseRight")
		{
			m_pBerkeliumWindow->mouseButton(2, reinterpret_cast<Button&>(cControl).IsPressed());
		}
	}
	// Mouse scrolling
	else if (cControl.GetType() == ControlAxis)
	{
		if (cControl.GetName() == "MouseWheel")
		{
			m_pBerkeliumWindow->mouseWheel(0, int(static_cast<Axis&>(cControl).GetValue()));
		}
	}
}


void SRPBerkelium::KeyEvents(Control &cControl)
{
	if (cControl.GetType() == ControlButton)
	{
		char nKey = cControl.GetController()->GetChar();
		String sKey = reinterpret_cast<Button&>(cControl).GetName();
		bool bTextKey = true;
		char vk_code = 0;

		if (sKey == "KeyboardBackspace")
		{
			vk_code = 0x08;
			bTextKey = false;
		}

		if (bTextKey)
		{
			wchar_t outchars[2];
			outchars[0] = nKey;
			outchars[1] = 0;
			m_pBerkeliumWindow->textEvent(outchars,1);
		}
		else
		{
			m_pBerkeliumWindow->keyEvent(true, 0, vk_code, 0);
			m_pBerkeliumWindow->keyEvent(false, 0, vk_code, 0);
		}
	}
}


void SRPBerkelium::MouseMove(int nX, int nY)
{
	m_nMouseX = nX;
	m_nMouseY = nY;
	if (m_bBerkeliumCreated)
		m_pBerkeliumWindow->mouseMoved(m_nMouseX - m_nXPos, m_nMouseY - m_nYPos);
}


Window *SRPBerkelium::GetWindow()
{
	return m_bBerkeliumCreated ? m_pBerkeliumWindow : nullptr;
}


void SRPBerkelium::WindowResize(int nWidth, int nHeight, int nX, int nY, bool bAbsolute)
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
		m_pBufferData = m_cImage.GetBuffer()->GetData();
		m_pBerkeliumWindow->resize(m_nFrameWidth, m_nFrameHeight);
		bChanged = true;
		DebugToConsole("Triggered a resize!!!\n");
	}
	if (bChanged) CreateVertexBuffer();
	m_bBufferReady = true;
}


void SRPBerkelium::WindowMove(int nX, int nY, bool bAbsolute)
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


Vector2 SRPBerkelium::GetWindowSize()
{
	return Vector2(float(m_nFrameWidth), float(m_nFrameHeight));
}


Vector2 SRPBerkelium::GetWindowPosition()
{
	return Vector2(float(m_nXPos), float(m_nYPos));
}


void SRPBerkelium::ChangePointerState(bool bDrawPointer)
{
	m_bDrawPointer = bDrawPointer;
}


void SRPBerkelium::SetPointerImagePath(const String &sPointerImagePath, bool bDrawPointer)
{
	m_bDrawPointer = false;
	m_sPointerImagePath = sPointerImagePath;
	m_pPointerTexture = m_cRenderer.GetRendererContext().GetTextureManager().LoadResource(m_sPointerImagePath);
	if (m_pPointerTexture)
	{
		m_bDrawPointer = bDrawPointer;
	}
}


void SRPBerkelium::DrawPointer(const Vector2 &vPos)
{
	if (m_pPointerTexture->GetTextureBuffer())
	{
		m_cRenderer.GetDrawHelpers().Begin2DMode(0.0f, 0.0f, 0.0f, 0.0f);
		m_cRenderer.GetDrawHelpers().DrawImage(*m_pPointerTexture->GetTextureBuffer(), m_cPointerSamplerStates, vPos);
		m_cRenderer.GetDrawHelpers().End2DMode();
	}
}