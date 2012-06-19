//[-------------------------------------------------------]
//[ Header                                                ]
//[-------------------------------------------------------]
#include "PLBerkelium/Gui.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLScene;
using namespace PLRenderer;
using namespace PLMath;
using namespace PLInput;
using namespace PLEngine;

namespace PLBerkelium {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(Gui)


//[-------------------------------------------------------]
//[ Functions		                                      ]
//[-------------------------------------------------------]
Gui::Gui() :
	m_bBerkeliumInitialized(false),
	m_bRenderersInitialized(false),
	m_pWindows(new HashMap<String, SRPWindows*>),
	m_pCurrentSceneRenderer(nullptr),
	m_pCurrentRenderer(nullptr),
	m_pSRPMousePointer(nullptr),
	m_pFocusedWindow(nullptr),
	m_bControlsEnabled(true),
	SlotOnUpdate(this),
	m_bIsUpdateConnected(false),
	m_bIsControllerConnected(false),
	SlotOnControl(this),
	m_sLastControl(),
	m_nLastMouseLeftReleaseTime(),
	m_pLastMouseWindow(nullptr),
	m_bMouseLeftDown(false),
	m_vLastKnownMousePos(Vector2i::Zero),
	m_pDragWindow(nullptr),
	m_pResizeWindow(nullptr),
	m_bMouseMoved(false),
	m_vLockMousePos(Vector2i::Zero),
	m_pTextButtonHandler(new HashMap<String, sButton*>),
	m_pKeyButtonHandler(new HashMap<String, sButton*>),
	m_nLastTextKeySendTime(0),
	m_nLastKeySendTime(0),
	m_nTextKeyHitCount(0),
	m_nKeyHitCount(0)
{
	Initialize();
}


Gui::~Gui()
{
	DebugToConsole("Shutting down..\n");
	DestroyWindows();
	RemoveMousePointer();
	StopBerkelium();
	DebugToConsole("Terminating..\n");
}


void Gui::DebugToConsole(const String &sString)
{
	System::GetInstance()->GetConsole().Print("PLBerkelium::Gui - " + sString);
}


bool Gui::AddWindow(const String &sName, const bool &pVisible, const String &sUrl, const int &nWidth, const int &nHeight, const int &nX, const int &nY, const bool &bTransparent, const bool &bEnabled)
{
	if (m_bBerkeliumInitialized)
	{
		if (!m_bRenderersInitialized)
		{
			DebugToConsole("The renderers are not set properly!\n");
			return false;
		}
		if (m_pWindows->Get(sName))
		{
			DebugToConsole("A window with this name already exists!\n");
			return false;
		}

		SRPWindows *pSRPWindows = new SRPWindows(sName);

		// data
		pSRPWindows->GetData()->bIsVisable = pVisible;
		pSRPWindows->GetData()->sUrl = sUrl;
		pSRPWindows->GetData()->nFrameWidth = nWidth;
		pSRPWindows->GetData()->nFrameHeight = nHeight;
		pSRPWindows->GetData()->nXPos = nX;
		pSRPWindows->GetData()->nYPos = nY;
		pSRPWindows->GetData()->bTransparent = bTransparent;
		pSRPWindows->GetData()->bKeyboardEnabled = bEnabled;
		pSRPWindows->GetData()->bMouseEnabled = bEnabled; // implement further
		pSRPWindows->GetData()->bNeedsFullUpdate = true;
		pSRPWindows->GetData()->bLoaded = false;
		// data

		// implement further
		if (pSRPWindows->Initialize(m_pCurrentRenderer, Vector2(float(nX), float(nY)), Vector2(float(nWidth), float(nHeight))))
		{
			pSRPWindows->AddSceneRenderPass(m_pCurrentSceneRenderer);
		}
		else
		{
			pSRPWindows->DestroyWindow();
			pSRPWindows->DestroyInstance();
			DebugToConsole("The window could not be created!\n");
			return false;
		}
		// implement further

		m_pWindows->Add(sName, pSRPWindows);

		DebugToConsole("Added created window '" + sName + "' to list..\n");
		return true;
	}
	else
	{
		DebugToConsole("Berkelium is not initialized properly!\n");
		return false;
	}
}


void Gui::DestroyWindows()
{
	if (m_bBerkeliumInitialized)
	{
		DebugToConsole("Destroying berkelium windows and data..\n");
		Iterator<SRPWindows*> cIterator = m_pWindows->GetIterator();
		while (cIterator.HasNext())
		{
			SRPWindows *pSRPWindows = cIterator.Next();
			pSRPWindows->RemoveSceneRenderPass();
			pSRPWindows->DestroyWindow();
			pSRPWindows->DestroyInstance();
		}
		m_pWindows->Clear();
	}
}


void Gui::Initialize()
{
	DebugToConsole("Initializing berkelium..\n");
	m_bBerkeliumInitialized = Berkelium::init(Berkelium::FileString::empty());
	if (m_bBerkeliumInitialized)
	{
		AddDummyWindow();
		DebugToConsole("Berkelium successfully initialized..\n");
	}
	else
	{
		DebugToConsole("Berkelium initialization failed..\n");
	}
}


void Gui::StopBerkelium() const
{
	if (m_bBerkeliumInitialized)
	{
		Berkelium::stopRunning();
	}
}


Berkelium::Window *Gui::GetBerkeliumWindow(const String &sName) const
{
	if (m_pWindows->Get(sName) == NULL)
	{
		return m_pWindows->Get(DUMMYWINDOW)->GetBerkeliumWindow();
	}
	else
	{
		return m_pWindows->Get(sName)->GetBerkeliumWindow();
	}
}


void Gui::UpdateBerkelium()
{
	Berkelium::update();
}


void Gui::DestroyInstance() const
{
	delete this;
}


void Gui::SetRenderers(Renderer *pRenderer, SceneRenderer *pSceneRenderer)
{
	if (!m_bRenderersInitialized)
	{
		if (pRenderer->IsInitialized())
		{
			if (pSceneRenderer)
			{
				m_pCurrentRenderer = pRenderer;
				m_pCurrentSceneRenderer = pSceneRenderer;
				m_bRenderersInitialized = true;
				CreateMousePointer();
			}
		}
	}
}


sWindowsData *Gui::GetWindowData(const String &sName) const
{
	if (m_pWindows->Get(sName) == NULL)
	{
		return m_pWindows->Get(DUMMYWINDOW)->GetData();
	}
	else
	{
		return m_pWindows->Get(sName)->GetData();
	}
}


void Gui::AddDummyWindow()
{
	SRPWindows *pSRPWindows = new SRPWindows(DUMMYWINDOW);

	pSRPWindows->GetData()->bIsVisable = false;
	pSRPWindows->GetData()->nFrameWidth = -1;
	pSRPWindows->GetData()->nFrameHeight = -1;
	pSRPWindows->GetData()->nXPos = -1;
	pSRPWindows->GetData()->nYPos = -1;
	pSRPWindows->GetData()->bKeyboardEnabled = false;
	pSRPWindows->GetData()->bMouseEnabled = false;

	pSRPWindows->CreateBerkeliumWindow();

	DebugToConsole("Added dummy window to list..\n");
	m_pWindows->Add(DUMMYWINDOW, pSRPWindows);
}


bool Gui::RemoveWindow(const String &sName)
{
	if (sName == DUMMYWINDOW)
	{
		DebugToConsole("You are not allowed to remove the dummy window!\n");
		return false;
	}
	if (m_pWindows->Get(sName) == NULL)
	{
		DebugToConsole("Could not find window!\n");
		return false;
	}
	else
	{
		DebugToConsole("Removing window '" + sName + "' from list..\n");

		SRPWindows *pSRPWindows = m_pWindows->Get(sName);
		pSRPWindows->RemoveSceneRenderPass();
		pSRPWindows->DestroyWindow();
		pSRPWindows->DestroyInstance();

		return m_pWindows->Remove(sName);
	}
}


void Gui::CreateMousePointer()
{
	m_pSRPMousePointer = new SRPMousePointer(m_pCurrentRenderer, m_pCurrentSceneRenderer);
}


void Gui::RemoveMousePointer() const
{
	if (m_bRenderersInitialized && m_pSRPMousePointer)
	{
		m_pSRPMousePointer->DestroyInstance();
	}
}


SRPWindows *Gui::GetWindow(const String &sName) const
{
	if (m_pWindows->Get(sName) == NULL)
	{
		return m_pWindows->Get(DUMMYWINDOW);
	}
	else
	{
		return m_pWindows->Get(sName);
	}
}


SRPMousePointer *Gui::GetMousePointer() const
{
	return m_pSRPMousePointer;
}


HashMap<String, SRPWindows*> *Gui::GetWindowsMap() const
{
	return m_pWindows;
}


bool Gui::IsBerkeliumInitialized() const
{
	return m_bBerkeliumInitialized;
}


void Gui::MouseMove(const SRPWindows *pSRPWindow, const Vector2i &vMousePos) const
{
	pSRPWindow->GetBerkeliumWindow()->mouseMoved(pSRPWindow->GetRelativeMousePosition(vMousePos).x, pSRPWindow->GetRelativeMousePosition(vMousePos).y);
}


void Gui::UnFocusAllWindows()
{
	m_pFocusedWindow = nullptr;
	Iterator<SRPWindows*> cIterator = m_pWindows->GetIterator();
	while (cIterator.HasNext())
	{
		cIterator.Next()->GetBerkeliumWindow()->unfocus();
	}
}


List<SRPWindows*> *Gui::GetMouseEnabledWindows()
{
	List<SRPWindows*> *pList = new List<SRPWindows*>;

	Iterator<SRPWindows*> cIterator = m_pWindows->GetIterator();
	while (cIterator.HasNext())
	{
		SRPWindows *pSRPWindows = cIterator.Next();
		if (pSRPWindows->GetData()->bIsVisable && pSRPWindows->GetData()->bMouseEnabled)
		{
			pList->Add(pSRPWindows);
		}
	}

	if (pList->GetNumOfElements() > 0)
	{
		return pList;
	}
	else
	{
		return nullptr;
	}
}


void Gui::FocusWindow(SRPWindows *pSRPWindows)
{
	if (m_pFocusedWindow != pSRPWindows)
	{
		if (m_pFocusedWindow != nullptr)
		{
			UnFocusAllWindows();
		}
		pSRPWindows->GetBerkeliumWindow()->focus();
		pSRPWindows->MoveToFront();
		m_pFocusedWindow = pSRPWindows;
	}
}


List<SRPWindows*> *Gui::GetMouseOverWindows(const List<SRPWindows*> *pEnabledWindows, const Vector2i &vMousePos)
{
	if (!pEnabledWindows)
	{
		return nullptr;
	}

	List<SRPWindows*> *pList = new List<SRPWindows*>;

	Iterator<SRPWindows*> cIterator = pEnabledWindows->GetIterator();
	while (cIterator.HasNext())
	{
		SRPWindows *pSRPWindows = cIterator.Next();

		Vector2i vRelativeMousePos = pSRPWindows->GetRelativeMousePosition(vMousePos);

		if (vRelativeMousePos.x > 0 && vRelativeMousePos.y > 0 && vRelativeMousePos.x < pSRPWindows->GetSize().x && vRelativeMousePos.y < pSRPWindows->GetSize().y)
		{
			pList->Add(pSRPWindows);
		}
		else
		{
			if (pSRPWindows->GetWidgets()->GetNumOfElements() > 0)
			{
				Iterator<sWidget*> cWidgetIterator = pSRPWindows->GetWidgets()->GetIterator();
				while (cWidgetIterator.HasNext())
				{
					sWidget *psWidget = cWidgetIterator.Next();

					Vector2i vRelativeMousePosWidget = pSRPWindows->GetRelativeMousePositionWidget(psWidget, vMousePos);

					if (vRelativeMousePosWidget.x > 0 && vRelativeMousePosWidget.y > 0 && vRelativeMousePosWidget.x < psWidget->nWidth && vRelativeMousePosWidget.y < psWidget->nHeight)
					{
						pList->Add(pSRPWindows);
					}
				}
			}
		}
	}

	if (pList->GetNumOfElements() > 0)
	{
		return pList;
	}
	else
	{
		return nullptr;
	}
}


SRPWindows *Gui::GetFocusedWindow() const
{
	return m_pFocusedWindow;
}


bool Gui::ConnectController(Controller *pController)
{
	if (pController && !m_bIsControllerConnected)
	{
		pController->SignalOnControl.Connect(SlotOnControl);
		m_bIsControllerConnected = true;
		return true;
	}
	return false;
}


void Gui::OnUpdate()
{
	UpdateBerkelium();
	// mouse handler?
	KeyboardHandler();
	DefaultCallBackHandler();
	DragWindowHandler();
	// resize window handler
}


bool Gui::ConnectEventUpdate(SceneContext *pSceneContext)
{
	if (pSceneContext && !m_bIsUpdateConnected)
	{
		pSceneContext->EventUpdate.Connect(SlotOnUpdate);
		m_bIsUpdateConnected = true;
		return true;
	}
	else
	{
		return false;
	}
}


void Gui::MouseEvents(Control &cControl)
{
	Frontend &cFrontend = static_cast<FrontendApplication*>(CoreApplication::GetApplication())->GetFrontend();
	if (cFrontend.IsRunning() && cFrontend.IsMouseOver())
	{
		Vector2i vMousePos(cFrontend.GetMousePositionX(), cFrontend.GetMousePositionY());
		m_vLastKnownMousePos = vMousePos;
		
		if (GetMousePointer()->IsVisible())
		{
			cFrontend.SetMouseVisible(false);
			GetMousePointer()->SetPosition(vMousePos.x, vMousePos.y);
			GetMousePointer()->MoveToFront();
		}
		else
		{
			cFrontend.SetMouseVisible(true);
		}

		SRPWindows *pSRPWindow = GetTopMostWindow(GetMouseOverWindows(GetMouseEnabledWindows(), vMousePos));
		if (pSRPWindow)
		{
			MouseMove(pSRPWindow, vMousePos);
			MouseClicks(pSRPWindow, cControl);
			MouseScrolls(pSRPWindow, cControl);
			m_pLastMouseWindow = pSRPWindow;
		}
		else
		{
			MouseScrolls(GetFocusedWindow(), cControl);
			if (m_pLastMouseWindow)
			{
				m_pLastMouseWindow->SetToolTip("");
			}
			if (m_pFocusedWindow)
			{
				if (cControl.GetName() == "MouseLeft")
				{
					UnFocusAllWindows();
				}
			}
			if (cControl.GetName() == "MouseLeft")
			{
				m_bMouseLeftDown = reinterpret_cast<Button&>(cControl).IsPressed();
			}
		}

		m_bMouseMoved = true;
	}
}


void Gui::MouseClicks(SRPWindows *pSRPWindow, Control &cControl)
{
	if (cControl.GetName() == "MouseLeft")
	{
		FocusWindow(pSRPWindow);
		if ((Timing::GetInstance()->GetPastTime() - m_nLastMouseLeftReleaseTime) > 0 && (Timing::GetInstance()->GetPastTime() - m_nLastMouseLeftReleaseTime) < 250)
		{
			pSRPWindow->GetBerkeliumWindow()->mouseButton(0, reinterpret_cast<Button&>(cControl).IsPressed(), 2);
			m_nLastMouseLeftReleaseTime = 0;
		}
		else
		{
			pSRPWindow->GetBerkeliumWindow()->mouseButton(0, reinterpret_cast<Button&>(cControl).IsPressed());
			if (!reinterpret_cast<Button&>(cControl).IsPressed())
			{
				m_bMouseLeftDown = false;
				m_nLastMouseLeftReleaseTime = Timing::GetInstance()->GetPastTime();
			}
			else
			{
				m_bMouseLeftDown = true;
				m_nLastMouseLeftReleaseTime = 0;
			}
		}
	}
	if (cControl.GetName() == "MouseRight")
	{
		FocusWindow(pSRPWindow);
		pSRPWindow->GetBerkeliumWindow()->mouseButton(2, reinterpret_cast<Button&>(cControl).IsPressed());
	}
}


void Gui::MouseScrolls(SRPWindows *pSRPWindow, Control &cControl)
{
	if (pSRPWindow)
	{
		if (pSRPWindow->GetData()->bIsVisable && pSRPWindow->GetData()->bMouseEnabled)
		{
			if (cControl.GetType() == ControlAxis)
			{
				if (cControl.GetName() == "MouseWheel")
				{
					pSRPWindow->GetBerkeliumWindow()->mouseWheel(0, int(static_cast<Axis&>(cControl).GetValue()));
				}
			}
		}
	}
}


void Gui::OnControl(Control &cControl)
{
	MouseEvents(cControl);
	KeyboardEvents(cControl);
}


SRPWindows *Gui::GetTopMostWindow(List<SRPWindows*> *pWindows)
{
	if (pWindows)
	{
		if (pWindows->GetNumOfElements() == 1)
		{
			return pWindows->Get(0);
		}
		else
		{
			SRPWindows *pTopMostWindow = nullptr;

			int nHigherIndex = -1;

			Iterator<SRPWindows*> cIterator = pWindows->GetIterator();
			while (cIterator.HasNext())
			{
				SRPWindows *pSRPWindows = cIterator.Next();
				if (pSRPWindows->GetSceneRenderPassIndex() > nHigherIndex)
				{
					nHigherIndex = pSRPWindows->GetSceneRenderPassIndex();
					pTopMostWindow = pSRPWindows;
				}
			}
			return pTopMostWindow;
		}
	}
	else
	{
		return nullptr;
	}
}


bool Gui::SetMousePointerVisible(const bool &bVisible) const
{
	if (m_bIsControllerConnected)
	{
		if (GetMousePointer())
		{
			GetMousePointer()->SetVisible(bVisible);
			return true;
		}
	}
	return false;
}


void Gui::DefaultCallBackHandler()
{
	Iterator<SRPWindows*> cIterator = m_pWindows->GetIterator();
	while (cIterator.HasNext())
	{
		SRPWindows *pSRPWindows = cIterator.Next();

		if (pSRPWindows->GetNumberOfCallBacks() > 0)
		{
			if (pSRPWindows->GetCallBack(DRAGWINDOW))
			{
				m_pDragWindow = pSRPWindows;
			}

			if (pSRPWindows->GetCallBack(HIDEWINDOW))
			{
				// hide window
			}

			if (pSRPWindows->GetCallBack(CLOSEWINDOW))
			{
				// close window
			}

			pSRPWindows->ClearCallBacks();
		}
	}
}


void Gui::KeyboardEvents(Control &cControl)
{
	if (m_pFocusedWindow)
	{
		if (m_pFocusedWindow->GetData()->bKeyboardEnabled)
		{
			if (cControl.GetName().GetSubstring(0, 8) == "Keyboard")
			{
				Button &cButton = reinterpret_cast<Button&>(cControl);

				if (cButton.IsPressed())
				{
					sButton *psButton = new sButton;

					if (String(cButton.GetCharacter()).IsAlphaNumeric())
					{
						AddTextKey(cButton.GetName(), cButton.GetCharacter(), psButton);
					}
					else if (cButton.GetName() == "KeyboardSpace")
					{
						AddTextKey(cButton.GetName(), cButton.GetCharacter(), psButton);
					}
					else if (cButton.GetName() == "KeyboardBackspace")
					{
						AddKey(cButton.GetName(), cButton.GetCharacter(), psButton);
					}
					else if (cButton.GetName() == "KeyboardTab")
					{
						AddKey(cButton.GetName(), cButton.GetCharacter(), psButton);
					}
					else
					{
						psButton->bValid = false;
					}
				}
				else
				{
					if (m_pTextButtonHandler->GetNumOfElements() == 1)
					{
						m_nTextKeyHitCount = 0;
					}
					m_pTextButtonHandler->Remove(cButton.GetName());

					if (m_pKeyButtonHandler->GetNumOfElements() == 1)
					{
						m_nKeyHitCount = 0;
					}
					m_pKeyButtonHandler->Remove(cButton.GetName());
				}

				//DebugToConsole("Window: '" + m_pFocusedWindow->GetName() + "', GetName(): '" + String(cButton.GetName()) + "'\n");
				//DebugToConsole("Window: '" + m_pFocusedWindow->GetName() + "', GetCharacter(): '" + String(cButton.GetCharacter()) + "'\n");
				//DebugToConsole("Window: '" + m_pFocusedWindow->GetName() + "', IsPressed(): '" + String(cButton.IsPressed()) + "'\n");

				DebugToConsole("Amount of text: " + String(m_pTextButtonHandler->GetNumOfElements()) + "\n");
				DebugToConsole("Hit count: " + String(m_nTextKeyHitCount) + "\n\n");
				DebugToConsole("Amount of keys: " + String(m_pKeyButtonHandler->GetNumOfElements()) + "\n");
				DebugToConsole("Hit count: " + String(m_nKeyHitCount) + "\n\n\n");
			}
		}
	}
}


bool Gui::SetWindowVisible(const String &sName, const bool &bVisible)
{
	if (sName == DUMMYWINDOW)
	{
		return false;
	}
	if (m_pWindows->Get(sName) == NULL)
	{
		return false;
	}
	else
	{
		if (m_pWindows->Get(sName) == m_pFocusedWindow && bVisible == false)
		{
			UnFocusAllWindows();
		}
		m_pWindows->Get(sName)->GetData()->bIsVisable = bVisible;
		return true;
	}
}


void Gui::DragWindowHandler()
{
	if (m_pDragWindow)
	{
		if (m_bMouseLeftDown)
		{
			if (m_bMouseMoved)
			{
				if (m_vLockMousePos == Vector2i::Zero)
				{
					m_pDragWindow->SetToolTip("");
					m_vLockMousePos = m_pDragWindow->GetRelativeMousePosition(m_vLastKnownMousePos);
				}
				m_pDragWindow->MoveWindow(m_vLastKnownMousePos.x - m_vLockMousePos.x, m_vLastKnownMousePos.y - m_vLockMousePos.y);
				m_bMouseMoved = false;
			}
		}
		else
		{
			m_vLockMousePos = Vector2i::Zero;
			m_pDragWindow = nullptr;
		}
	}
}


void Gui::KeyboardHandler()
{
	if (m_pFocusedWindow)
	{
		if (m_pFocusedWindow->GetData()->bKeyboardEnabled)
		{
			if (m_pTextButtonHandler->GetNumOfElements() > 0)
			{
				Iterator<sButton*> cIterator = m_pTextButtonHandler->GetIterator();
				while (cIterator.HasNext())
				{
					sButton *psButton = cIterator.Next();
					if (psButton->bValid)
					{
						if (m_nTextKeyHitCount == 0)
						{
							m_pFocusedWindow->GetBerkeliumWindow()->textEvent(psButton->sKey.GetUnicode(), psButton->sKey.GetLength());
							m_nLastTextKeySendTime = Timing::GetInstance()->GetPastTime();
							m_nTextKeyHitCount++;
						}
						else
						{
							if (m_nTextKeyHitCount == 1)
							{
								if ((Timing::GetInstance()->GetPastTime() - m_nLastTextKeySendTime) > 400)
								{
									m_pFocusedWindow->GetBerkeliumWindow()->textEvent(psButton->sKey.GetUnicode(), psButton->sKey.GetLength());
									m_nLastTextKeySendTime = Timing::GetInstance()->GetPastTime();
									m_nTextKeyHitCount++;
								}
							}
							else
							{
								if ((Timing::GetInstance()->GetPastTime() - m_nLastTextKeySendTime) > 50)
								{
									m_pFocusedWindow->GetBerkeliumWindow()->textEvent(psButton->sKey.GetUnicode(), psButton->sKey.GetLength());
									m_nLastTextKeySendTime = Timing::GetInstance()->GetPastTime();
									m_nTextKeyHitCount++;
								}
							}
						}
					}
				}
			}
			if (m_pKeyButtonHandler->GetNumOfElements() > 0)
			{
				Iterator<sButton*> cIterator = m_pKeyButtonHandler->GetIterator();
				while (cIterator.HasNext())
				{
					sButton *psButton = cIterator.Next();
					if (psButton->bValid)
					{
						if (m_nKeyHitCount == 0)
						{
							m_pFocusedWindow->GetBerkeliumWindow()->keyEvent(true, 0, psButton->nKey, 0);
							m_nLastKeySendTime = Timing::GetInstance()->GetPastTime();
							m_nKeyHitCount++;
						}
						else
						{
							if (m_nKeyHitCount == 1)
							{
								if ((Timing::GetInstance()->GetPastTime() - m_nLastKeySendTime) > 400)
								{
									m_pFocusedWindow->GetBerkeliumWindow()->keyEvent(true, 0, psButton->nKey, 0);
									m_nLastKeySendTime = Timing::GetInstance()->GetPastTime();
									m_nKeyHitCount++;
								}
							}
							else
							{
								if ((Timing::GetInstance()->GetPastTime() - m_nLastKeySendTime) > 50)
								{
									m_pFocusedWindow->GetBerkeliumWindow()->keyEvent(true, 0, psButton->nKey, 0);
									m_nLastKeySendTime = Timing::GetInstance()->GetPastTime();
									m_nKeyHitCount++;
								}
							}
						}
					}
				}
			}
		}
	}
}


void Gui::AddTextKey(const String &sName, const String &sKey, sButton *psButton)
{
	m_pTextButtonHandler->Clear();
	psButton->bValid = true;
	psButton->sKey = sKey;
	m_pTextButtonHandler->Add(sName, psButton);
	m_nTextKeyHitCount = 0;
}


void Gui::AddKey(const PLCore::String &sName, const char &nKey, sButton *psButton)
{
	psButton->bValid = true;
	psButton->nKey = nKey;
	m_pKeyButtonHandler->Add(sName, psButton);
	m_nKeyHitCount = 0;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLBerkelium