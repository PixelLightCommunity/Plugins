//[-------------------------------------------------------]
//[ Header                                                ]
//[-------------------------------------------------------]
#include "PLAwesomium/SRPWindows.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLRenderer;
using namespace PLScene;
using namespace PLMath;
using namespace PLGraphics;

namespace PLAwesomium {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(SRPWindows)


//[-------------------------------------------------------]
//[ Functions		                                      ]
//[-------------------------------------------------------]
SRPWindows::SRPWindows(const String &sName) :
	m_pCurrentAwesomiumWebCore(nullptr), /*set by constructor?*/
	m_pWindow(nullptr),
	m_sWindowName(sName),
	m_pCurrentSceneRenderer(nullptr),
	m_pCurrentRenderer(nullptr),
	m_pVertexBuffer(nullptr),
	m_pProgramWrapper(nullptr),
	m_pTextureBuffer(nullptr),
	m_cImage(),
	m_pImageBuffer(nullptr),
	m_psWindowsData(new sWindowsData),
	m_bInitialized(false),
	m_bReadyToDraw(false),
	m_pDefaultCallBacks(new HashMap<String, sCallBack*>),
	m_pCallBackFunctions(new HashMap<PLCore::String, PLCore::DynFuncPtr>),
	m_bIgnoreBufferUpdate(false)
{
}


SRPWindows::~SRPWindows()
{
}


void SRPWindows::DebugToConsole(const String &sString)
{
	System::GetInstance()->GetConsole().Print("PLAwesomium::SRPWindows - " + sString);
}


void SRPWindows::Draw(Renderer &cRenderer, const SQCull &cCullQuery)
{
	if (m_bReadyToDraw)
	{
		// draw the window if we are ready
		DrawWindow();
	}
}


VertexBuffer *SRPWindows::CreateVertexBuffer(const Vector2 &vPosition, const Vector2 &vImageSize)
{
	// lets create a vertex buffer
	VertexBuffer *pVertexBuffer = m_pCurrentRenderer->CreateVertexBuffer();
	if (pVertexBuffer)
	{
		pVertexBuffer->AddVertexAttribute(VertexBuffer::Position, 0, VertexBuffer::Float3);
		pVertexBuffer->AddVertexAttribute(VertexBuffer::Color,    0, VertexBuffer::RGBA);
		pVertexBuffer->AddVertexAttribute(VertexBuffer::TexCoord, 0, VertexBuffer::Float2);
		pVertexBuffer->Allocate(4, Usage::WriteOnly);

		// Setup the vertex buffer
		if (pVertexBuffer->Lock(Lock::WriteOnly))
		{
			float fZValue2D(0.0f);
			Vector2 vTextureCoordinate(Vector2::Zero);
			Vector2 vTextureCoordinateSize(Vector2::One);
			float fTextureCoordinateScaleX(1.0f);
			float fTextureCoordinateScaleY(1.0f);

			// Vertex 0
			float *pfVertex = static_cast<float*>(pVertexBuffer->GetData(0, VertexBuffer::Position));
			pfVertex[0] = vPosition.x;
			pfVertex[1] = vPosition.y + vImageSize.y;
			pfVertex[2] = fZValue2D;
			pfVertex	= static_cast<float*>(pVertexBuffer->GetData(0, VertexBuffer::TexCoord));
			pfVertex[0] = vTextureCoordinate.x*fTextureCoordinateScaleX;
			pfVertex[1] = (vTextureCoordinate.y + vTextureCoordinateSize.y)*fTextureCoordinateScaleY;

			// Vertex 1
			pfVertex	= static_cast<float*>(pVertexBuffer->GetData(1, VertexBuffer::Position));
			pfVertex[0] = vPosition.x + vImageSize.x;
			pfVertex[1] = vPosition.y + vImageSize.y;
			pfVertex[2] = fZValue2D;
			pfVertex	= static_cast<float*>(pVertexBuffer->GetData(1, VertexBuffer::TexCoord));
			pfVertex[0] = (vTextureCoordinate.x + vTextureCoordinateSize.x)*fTextureCoordinateScaleX;
			pfVertex[1] = (vTextureCoordinate.y + vTextureCoordinateSize.y)*fTextureCoordinateScaleY;

			// Vertex 2
			pfVertex	= static_cast<float*>(pVertexBuffer->GetData(2, VertexBuffer::Position));
			pfVertex[0] = vPosition.x;
			pfVertex[1] = vPosition.y;
			pfVertex[2] = fZValue2D;
			pfVertex	= static_cast<float*>(pVertexBuffer->GetData(2, VertexBuffer::TexCoord));
			pfVertex[0] = vTextureCoordinate.x*fTextureCoordinateScaleX;
			pfVertex[1] = vTextureCoordinate.y*fTextureCoordinateScaleY;

			// Vertex 3
			pfVertex	= static_cast<float*>(pVertexBuffer->GetData(3, VertexBuffer::Position));
			pfVertex[0] = vPosition.x + vImageSize.x;
			pfVertex[1] = vPosition.y;
			pfVertex[2] = fZValue2D;
			pfVertex	= static_cast<float*>(pVertexBuffer->GetData(3, VertexBuffer::TexCoord));
			pfVertex[0] = (vTextureCoordinate.x + vTextureCoordinateSize.x)*fTextureCoordinateScaleX;
			pfVertex[1] = vTextureCoordinate.y*fTextureCoordinateScaleY;

			// Unlock the vertex buffer
			pVertexBuffer->Unlock();
		}

		// return the created vertex buffer
		return pVertexBuffer;
	}
	else
	{
		// something when wrong so we return nothing
		return nullptr;
	}
}


void SRPWindows::SetRenderer(Renderer *pRenderer)
{
	m_pCurrentRenderer = pRenderer;
}


ProgramWrapper *SRPWindows::CreateProgramWrapper()
{
	// declare vertex and fragment shader
	String sVertexShaderSourceCode;
	String sFragmentShaderSourceCode;

	// account for OpenGL version
	if (m_pCurrentRenderer->GetAPI() == "OpenGL ES 2.0")
	{
		sVertexShaderSourceCode   = "#version 100\n" + sAwesomiumVertexShaderSourceCodeGLSL;
		sFragmentShaderSourceCode = "#version 100\n" + sAwesomiumFragmentShaderSourceCodeGLSL;
	}
	else
	{
		sVertexShaderSourceCode   = "#version 110\n" + Shader::RemovePrecisionQualifiersFromGLSL(sAwesomiumVertexShaderSourceCodeGLSL);
		sFragmentShaderSourceCode = "#version 110\n" + Shader::RemovePrecisionQualifiersFromGLSL(sAwesomiumFragmentShaderSourceCodeGLSL);
	}

	// create the vertex and fragment shader
	VertexShader *pVertexShader = m_pCurrentRenderer->GetShaderLanguage(m_pCurrentRenderer->GetDefaultShaderLanguage())->CreateVertexShader(sVertexShaderSourceCode, "arbvp1");
	FragmentShader *pFragmentShader = m_pCurrentRenderer->GetShaderLanguage(m_pCurrentRenderer->GetDefaultShaderLanguage())->CreateFragmentShader(sFragmentShaderSourceCode, "arbfp1");

	// create the program wrapper
	ProgramWrapper *pProgram = static_cast<ProgramWrapper*>(m_pCurrentRenderer->GetShaderLanguage(m_pCurrentRenderer->GetDefaultShaderLanguage())->CreateProgram(pVertexShader, pFragmentShader));

	if (pProgram)
	{
		// return the created program wrapper
		return pProgram;
	}
	else
	{
		// return nothing because the program wrapper could not be created
		return nullptr;
	}
}


bool SRPWindows::UpdateVertexBuffer(VertexBuffer *pVertexBuffer, const Vector2 &vPosition, const Vector2 &vImageSize)
{
	/*i am not sure yet if this method is right for this use case*/

	if (!pVertexBuffer)
	{
		DebugToConsole("VertexBuffer not valid!\n");
		return false;
	}
	else
	{
		if (!pVertexBuffer->Clear())
		{
			DebugToConsole("Could not clear VertexBuffer!\n");
			return false;
		}
		if (pVertexBuffer->IsAllocated())
		{
			DebugToConsole("VertexBuffer already allocated!\n");
			return false;
		}
		else
		{
			if (!pVertexBuffer->ClearVertexAttributes())
			{
				DebugToConsole("Could not clear attributes from VertexBuffer!\n");
				return false;
			}
			else
			{
				pVertexBuffer->AddVertexAttribute(VertexBuffer::Position, 0, VertexBuffer::Float3);
				pVertexBuffer->AddVertexAttribute(VertexBuffer::Color,    0, VertexBuffer::RGBA);
				pVertexBuffer->AddVertexAttribute(VertexBuffer::TexCoord, 0, VertexBuffer::Float2);
				pVertexBuffer->Allocate(4, Usage::WriteOnly);

				// Setup the vertex buffer
				if (pVertexBuffer->Lock(Lock::WriteOnly))
				{
					float fZValue2D(0.0f);
					Vector2 vTextureCoordinate(Vector2::Zero);
					Vector2 vTextureCoordinateSize(Vector2::One);
					float fTextureCoordinateScaleX(1.0f);
					float fTextureCoordinateScaleY(1.0f);

					// Vertex 0
					float *pfVertex = static_cast<float*>(pVertexBuffer->GetData(0, VertexBuffer::Position));
					pfVertex[0] = vPosition.x;
					pfVertex[1] = vPosition.y + vImageSize.y;
					pfVertex[2] = fZValue2D;
					pfVertex	= static_cast<float*>(pVertexBuffer->GetData(0, VertexBuffer::TexCoord));
					pfVertex[0] = vTextureCoordinate.x*fTextureCoordinateScaleX;
					pfVertex[1] = (vTextureCoordinate.y + vTextureCoordinateSize.y)*fTextureCoordinateScaleY;

					// Vertex 1
					pfVertex	= static_cast<float*>(pVertexBuffer->GetData(1, VertexBuffer::Position));
					pfVertex[0] = vPosition.x + vImageSize.x;
					pfVertex[1] = vPosition.y + vImageSize.y;
					pfVertex[2] = fZValue2D;
					pfVertex	= static_cast<float*>(pVertexBuffer->GetData(1, VertexBuffer::TexCoord));
					pfVertex[0] = (vTextureCoordinate.x + vTextureCoordinateSize.x)*fTextureCoordinateScaleX;
					pfVertex[1] = (vTextureCoordinate.y + vTextureCoordinateSize.y)*fTextureCoordinateScaleY;

					// Vertex 2
					pfVertex	= static_cast<float*>(pVertexBuffer->GetData(2, VertexBuffer::Position));
					pfVertex[0] = vPosition.x;
					pfVertex[1] = vPosition.y;
					pfVertex[2] = fZValue2D;
					pfVertex	= static_cast<float*>(pVertexBuffer->GetData(2, VertexBuffer::TexCoord));
					pfVertex[0] = vTextureCoordinate.x*fTextureCoordinateScaleX;
					pfVertex[1] = vTextureCoordinate.y*fTextureCoordinateScaleY;

					// Vertex 3
					pfVertex	= static_cast<float*>(pVertexBuffer->GetData(3, VertexBuffer::Position));
					pfVertex[0] = vPosition.x + vImageSize.x;
					pfVertex[1] = vPosition.y;
					pfVertex[2] = fZValue2D;
					pfVertex	= static_cast<float*>(pVertexBuffer->GetData(3, VertexBuffer::TexCoord));
					pfVertex[0] = (vTextureCoordinate.x + vTextureCoordinateSize.x)*fTextureCoordinateScaleX;
					pfVertex[1] = vTextureCoordinate.y*fTextureCoordinateScaleY;

					// Unlock the vertex buffer
					pVertexBuffer->Unlock();
				}

				return true;
			}
		}
	}
}


bool SRPWindows::Initialize(Renderer *pRenderer, const Vector2 &vPosition, const Vector2 &vImageSize)
{
	// set the renderer
	SetRenderer(pRenderer);

	// set the vertex buffer
	m_pVertexBuffer = CreateVertexBuffer(vPosition, vImageSize);

	// set the program wrapper
	m_pProgramWrapper = CreateProgramWrapper();

	if (m_pVertexBuffer && m_pProgramWrapper)
	{
		// create the image
		m_cImage = Image::CreateImage(DataByte, ColorRGBA, Vector3i(m_psWindowsData->nFrameWidth, m_psWindowsData->nFrameHeight, 1));
		// create the texture buffer
		m_pTextureBuffer = reinterpret_cast<TextureBuffer*>(pRenderer->CreateTextureBuffer2D(m_cImage, TextureBuffer::Unknown, 0));

		if (m_pTextureBuffer)
		{
			// create the image buffer
			m_pImageBuffer = m_cImage.GetBuffer()->GetData();

			if (m_pImageBuffer)
			{
				// create a awesomium window
				CreateAwesomiumWindow();
				// set the default window settings
				SetWindowSettings();
				// set the default callback functions
				SetDefaultCallBackFunctions();
				m_bInitialized = true;
				return true;
			}
			// image buffer could not be created
			return false;
		}
		// texture buffer could not be created
		return false;
	}
	else
	{
		// vertex buffer or program wrapper could not be created
		return false;
	}
}


void SRPWindows::DestroyInstance() const
{
	// cleanup
	delete this;
}


void SRPWindows::DrawWindow()
{
	if (m_bInitialized && m_psWindowsData->bIsVisable) // should suffice
	{
		// set the program
		m_pCurrentRenderer->SetProgram(m_pProgramWrapper);
		// set the render state to allow for transparency
		m_pCurrentRenderer->SetRenderState(RenderState::BlendEnable, true);

		/*
		possible change
		- let (re)sizing be handled by the program uniform, see http://dev.pixellight.org/forum/viewtopic.php?f=6&t=503
		*/
		{
			const PLMath::Rectangle &cViewportRect = m_pCurrentRenderer->GetViewport();
			float fX1 = cViewportRect.vMin.x;
			float fY1 = cViewportRect.vMin.y;
			float fX2 = cViewportRect.vMax.x;
			float fY2 = cViewportRect.vMax.y;

			Matrix4x4 m_mObjectSpaceToClipSpace;
			m_mObjectSpaceToClipSpace.OrthoOffCenter(fX1, fX2, fY1, fY2, -1.0f, 1.0f);

			// create program uniform
			ProgramUniform *pProgramUniform = m_pProgramWrapper->GetUniform("ObjectSpaceToClipSpaceMatrix");
			if (pProgramUniform)
				pProgramUniform->Set(m_mObjectSpaceToClipSpace);

			const int nTextureUnit = m_pProgramWrapper->Set("TextureMap", m_pTextureBuffer);
			if (nTextureUnit >= 0)
			{
				// set sampler states
				m_pCurrentRenderer->SetSamplerState(nTextureUnit, Sampler::AddressU, TextureAddressing::Clamp);
				m_pCurrentRenderer->SetSamplerState(nTextureUnit, Sampler::AddressV, TextureAddressing::Clamp);
				m_pCurrentRenderer->SetSamplerState(nTextureUnit, Sampler::MagFilter, TextureFiltering::None);
				m_pCurrentRenderer->SetSamplerState(nTextureUnit, Sampler::MinFilter, TextureFiltering::None);
				m_pCurrentRenderer->SetSamplerState(nTextureUnit, Sampler::MipFilter, TextureFiltering::None);
			}

			// set vertex attributes
			m_pProgramWrapper->Set("VertexPosition", m_pVertexBuffer, VertexBuffer::Position);
			m_pProgramWrapper->Set("VertexTexCoord", m_pVertexBuffer, VertexBuffer::TexCoord);
		}

		// draw primitives
		m_pCurrentRenderer->DrawPrimitives(Primitive::TriangleStrip, 0, 4);
	}
}


bool SRPWindows::AddSceneRenderPass(SceneRenderer *pSceneRenderer)
{
	// add scene render pass
	if (pSceneRenderer->Add(*reinterpret_cast<SceneRendererPass*>(this)))
	{
		m_pCurrentSceneRenderer = pSceneRenderer;
		return true;
	}
	else
	{
		return false;
	}
}


bool SRPWindows::RemoveSceneRenderPass()
{
	if (m_pCurrentSceneRenderer)
	{
		// remove scene render pass
		if (m_pCurrentSceneRenderer->Remove(*reinterpret_cast<SceneRendererPass*>(this)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}


void SRPWindows::CreateAwesomiumWindow()
{
	// check if awesomium window is already created
	if (!m_pWindow)
	{
		// create awesomium window
		m_pWindow = m_pCurrentAwesomiumWebCore->CreateWebView(m_psWindowsData->nFrameWidth, m_psWindowsData->nFrameHeight);

		// setting listener for process callbacks
		m_pWindow->set_process_listener(this);

		// setting listener for load callbacks
		m_pWindow->set_load_listener(this);
	}
}


void SRPWindows::BufferUploadToGPU()
{
	if (m_bInitialized)
	{
		// upload data to GPU
		m_pTextureBuffer->CopyDataFrom(0, TextureBuffer::R8G8B8A8, m_pImageBuffer);
		// set state for future usage
		if (!m_bReadyToDraw) m_bReadyToDraw = true;
	}
}


void SRPWindows::MoveToFront()
{
	if (m_bInitialized && m_pCurrentSceneRenderer)
	{
		// move scene render pass to front
		m_pCurrentSceneRenderer->MoveElement(m_pCurrentSceneRenderer->GetIndex(*reinterpret_cast<SceneRendererPass*>(this)), m_pCurrentSceneRenderer->GetNumOfElements() - 1);
	}
}


void SRPWindows::RecreateWindow()
{
	m_bInitialized = false;
	// destroy the window
	DestroyWindow();
	// create a new window
	CreateAwesomiumWindow();
	// set the window settings
	SetWindowSettings();
	m_bInitialized = true;
}


Awesomium::WebView *SRPWindows::GetAwesomiumWindow() const
{
	return m_pWindow;
}


sWindowsData *SRPWindows::GetData() const
{
	return m_psWindowsData;
}


String SRPWindows::GetName() const
{
	return m_sWindowName;
}


void SRPWindows::DestroyWindow()
{
	if (m_pWindow)
	{
		// stop window navigation
		m_pWindow->Stop();
		// unfocus window
		m_pWindow->Unfocus();
		// destroy window
		m_pWindow->Destroy();
		m_pWindow = nullptr;
	}
}


void SRPWindows::SetWindowSettings()
{
	m_pWindow->SetTransparent(m_psWindowsData->bTransparent);
	m_pWindow->LoadURL(Awesomium::WebURL(Awesomium::WSLit(m_psWindowsData->sUrl.GetUTF8())));
	//m_pWindow->setDelegate(this); ??
}


Vector2i SRPWindows::GetPosition() const
{
	if (m_psWindowsData->nXPos && m_psWindowsData->nYPos)
	{
		// return the position of the window
		return Vector2i(m_psWindowsData->nXPos, m_psWindowsData->nYPos);
	}
	else
	{
		return Vector2i::NegativeOne;
	}
}


Vector2i SRPWindows::GetSize() const
{
	if (m_psWindowsData->nFrameWidth && m_psWindowsData->nFrameHeight)
	{
		// return the size of the window
		return Vector2i(m_psWindowsData->nFrameWidth, m_psWindowsData->nFrameHeight);
	}
	else
	{
		return Vector2i::NegativeOne;
	}
}


Vector2i SRPWindows::GetRelativeMousePosition(const Vector2i &vMousePos) const
{
	return Vector2i(vMousePos.x - m_psWindowsData->nXPos, vMousePos.y - m_psWindowsData->nYPos);
}


int SRPWindows::GetSceneRenderPassIndex()
{
	if (m_pCurrentSceneRenderer)
	{
		// return scene render pass index
		return m_pCurrentSceneRenderer->GetIndex(*reinterpret_cast<SceneRendererPass*>(this));
	}
	else
	{
		return -1;
	}
}


void SRPWindows::MoveWindow(const int &nX, const int &nY)
{
	/*i am not sure yet if this method is right for this use case*/

	m_psWindowsData->nXPos = nX;
	m_psWindowsData->nYPos = nY;
	UpdateVertexBuffer(m_pVertexBuffer, Vector2(float(nX), float(nY)), Vector2(float(m_psWindowsData->nFrameWidth), float(m_psWindowsData->nFrameHeight)));
}


void SRPWindows::ClearCallBacks() const
{
	m_pDefaultCallBacks->Clear();
}


uint32 SRPWindows::GetNumberOfCallBacks() const
{
	return m_pDefaultCallBacks->GetNumOfElements();
}


sCallBack *SRPWindows::GetCallBack(const String &sKey) const
{
	return m_pDefaultCallBacks->Get(sKey);
}


void SRPWindows::ResizeWindow(const int &nWidth, const int &nHeight)
{
	/*
	possible change
	- let (re)sizing be handled by the program uniform, see http://dev.pixellight.org/forum/viewtopic.php?f=6&t=503
	*/

	m_bIgnoreBufferUpdate = true; // still not optimal

	m_bReadyToDraw = false; // still not optimal

	m_psWindowsData->nFrameWidth = nWidth;
	m_psWindowsData->nFrameHeight = nHeight;

	m_cImage = Image::CreateImage(DataByte, ColorRGBA, Vector3i(m_psWindowsData->nFrameWidth, m_psWindowsData->nFrameHeight, 1));
	m_pTextureBuffer = reinterpret_cast<TextureBuffer*>(m_pCurrentRenderer->CreateTextureBuffer2D(m_cImage, TextureBuffer::Unknown, 0));
	m_pImageBuffer = m_cImage.GetBuffer()->GetData();

	UpdateVertexBuffer(m_pVertexBuffer, Vector2(float(m_psWindowsData->nXPos), float(m_psWindowsData->nYPos)), Vector2(float(m_psWindowsData->nFrameWidth), float(m_psWindowsData->nFrameHeight)));

	m_bReadyToDraw = true; // still not optimal

	m_pWindow->Resize(m_psWindowsData->nFrameWidth, m_psWindowsData->nFrameHeight);

	m_bIgnoreBufferUpdate = false; // still not optimal

	m_psWindowsData->bNeedsFullUpdate = true; // still not optimal
}


bool SRPWindows::AddCallBackFunction(const DynFuncPtr pDynFunc, String sJSFunctionName, bool bHasReturn)
{
	if (pDynFunc)
	{
		// create function descriptor
		const FuncDesc *pFuncDesc = pDynFunc->GetDesc();
		if (pFuncDesc)
		{
			if (m_pCallBackFunctions->Get(pFuncDesc->GetName()) == NULL)
			{
				if (sJSFunctionName == "")
				{
					// the function name is not defined so we use the method name
					sJSFunctionName = pFuncDesc->GetName();
				}
				// we bind the javascript function
				//GetAwesomiumWindow()->addBindOnStartLoading(Berkelium::WideString::point_to(sJSFunctionName.GetUnicode()), Berkelium::Script::Variant::bindFunction(Berkelium::WideString::point_to(pFuncDesc->GetName().GetUnicode()), bHasReturn));

				// we add the function pointer to the hashmap
				m_pCallBackFunctions->Add(pFuncDesc->GetName(), pDynFunc);
				return true;
			}
			else
			{
				// the function specified already exists in list
			}
		}
	}
	return false;
}


bool SRPWindows::RemoveCallBack(const String &sKey) const
{
	return m_pDefaultCallBacks->Remove(sKey);
}


void SRPWindows::SetDefaultCallBackFunctions()
{
	// bind the default javascript functions for use
	//GetAwesomiumWindow()->addBindOnStartLoading(Berkelium::WideString::point_to(String(DRAGWINDOW).GetUnicode()), Berkelium::Script::Variant::bindFunction(Berkelium::WideString::point_to(String(DRAGWINDOW).GetUnicode()), false));
	//GetAwesomiumWindow()->addBindOnStartLoading(Berkelium::WideString::point_to(String(HIDEWINDOW).GetUnicode()), Berkelium::Script::Variant::bindFunction(Berkelium::WideString::point_to(String(HIDEWINDOW).GetUnicode()), false));
	//GetAwesomiumWindow()->addBindOnStartLoading(Berkelium::WideString::point_to(String(CLOSEWINDOW).GetUnicode()), Berkelium::Script::Variant::bindFunction(Berkelium::WideString::point_to(String(CLOSEWINDOW).GetUnicode()), false));
}


void SRPWindows::ExecuteJavascript(const String &sJavascript) const
{
	// execute the javascript function
	m_pWindow->ExecuteJavascript(Awesomium::WebString::CreateFromUTF8(sJavascript.GetUTF8(), sJavascript.GetLength()), Awesomium::WebString());
}


void SRPWindows::UpdateCall()
{
	Awesomium::BitmapSurface* surface = static_cast<Awesomium::BitmapSurface*>(m_pWindow->surface());
	if (surface)
	{
		if (surface->is_dirty())
		{
			surface->CopyTo(m_pImageBuffer, m_psWindowsData->nFrameWidth * 4, 4, false, false);
			surface->set_is_dirty(false);
			BufferUploadToGPU();
		}
	}
}


void SRPWindows::SetAwesomiumWebCore(Awesomium::WebCore *pAwesomiumWebCore)
{
	m_pCurrentAwesomiumWebCore = pAwesomiumWebCore;
}


void SRPWindows::OnChangeTitle(Awesomium::WebView *caller, const Awesomium::WebString &title)
{
	DebugToConsole("OnChangeTitle()\n");
}


void SRPWindows::OnChangeAddressBar(Awesomium::WebView *caller, const Awesomium::WebURL &url)
{
	DebugToConsole("OnChangeAddressBar()\n");
}


void SRPWindows::OnChangeTooltip(Awesomium::WebView *caller, const Awesomium::WebString &tooltip)
{
	DebugToConsole("OnChangeTooltip()\n");
}


void SRPWindows::OnChangeTargetURL(Awesomium::WebView *caller, const Awesomium::WebURL &url)
{
	DebugToConsole("OnChangeTargetURL()\n");
}


void SRPWindows::OnChangeCursor(Awesomium::WebView *caller, Awesomium::Cursor cursor)
{
	DebugToConsole("OnChangeCursor()\n");
}


void SRPWindows::OnChangeFocus(Awesomium::WebView *caller, Awesomium::FocusedElementType focused_type)
{
	DebugToConsole("OnChangeFocus()\n");
}


void SRPWindows::OnShowCreatedWebView(Awesomium::WebView *caller, Awesomium::WebView *new_view, const Awesomium::WebURL &opener_url, const Awesomium::WebURL &target_url, const Awesomium::Rect &initial_pos, bool is_popup)
{
	DebugToConsole("OnShowCreatedWebView()\n");
}


void SRPWindows::OnUnresponsive(Awesomium::WebView *caller)
{
	DebugToConsole("OnUnresponsive()\n");
}


void SRPWindows::OnResponsive(Awesomium::WebView *caller)
{
	DebugToConsole("OnResponsive()\n");
}


void SRPWindows::OnCrashed(Awesomium::WebView *caller, Awesomium::TerminationStatus status)
{
	// the window has crashed so we should recreate it
	RecreateWindow();
}


void SRPWindows::OnBeginLoadingFrame(Awesomium::WebView *caller, PLCore::int64 frame_id, bool is_main_frame, const Awesomium::WebURL &url, bool is_error_page)
{
	m_psWindowsData->bLoaded = false;
}


void SRPWindows::OnFailLoadingFrame(Awesomium::WebView *caller, PLCore::int64 frame_id, bool is_main_frame, const Awesomium::WebURL &url, int error_code, const Awesomium::WebString &error_desc)
{
	DebugToConsole("OnFailLoadingFrame()\n");
}


void SRPWindows::OnFinishLoadingFrame(Awesomium::WebView *caller, PLCore::int64 frame_id, bool is_main_frame, const Awesomium::WebURL &url)
{
	m_psWindowsData->bLoaded = true;
}


void SRPWindows::OnDocumentReady(Awesomium::WebView *caller, const Awesomium::WebURL &url)
{
	DebugToConsole("OnDocumentReady()\n");
}


bool SRPWindows::IsLoaded() const
{
	return m_psWindowsData->bLoaded;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLAwesomium