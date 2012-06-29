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
	m_sLastControl(""),
	m_nLastMouseLeftReleaseTime(0),
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
	// we should destroy all windows
	DestroyWindows();
	// we should remove the mouse pointer
	RemoveMousePointer();
	// we should stop berkelium from doing anything else
	StopBerkelium();
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
			// we need to add a scene render pass to the renderer, therefor renderers must be initialized
			return false;
		}
		if (m_pWindows->Get(sName))
		{
			// window name already exists, this is to prevent duplication and overrides in the hashmap
			return false;
		}

		// we create the window
		SRPWindows *pSRPWindows = new SRPWindows(sName);

		// we assign data to it
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

		// we initialize the window
		if (pSRPWindows->Initialize(m_pCurrentRenderer, Vector2(float(nX), float(nY)), Vector2(float(nWidth), float(nHeight))))
		{
			// we add the window scene render pass to the renderer
			pSRPWindows->AddSceneRenderPass(m_pCurrentSceneRenderer);
		}
		else
		{
			// window cannot be initialized so we should destroy and cleanup the leftovers
			pSRPWindows->DestroyWindow();
			pSRPWindows->DestroyInstance();
			return false;
		}

		// we add the created window to the hashmap
		m_pWindows->Add(sName, pSRPWindows);

		// we return successfully
		return true;
	}
	else
	{
		// berkelium is not initialized, therefor we should not do anything
		return false;
	}
}


void Gui::DestroyWindows()
{
	if (m_bBerkeliumInitialized)
	{
		// get the iterator for all the windows created
		Iterator<SRPWindows*> cIterator = m_pWindows->GetIterator();
		// loop trough the windows
		while (cIterator.HasNext())
		{
			SRPWindows *pSRPWindows = cIterator.Next();
			// remove the scene render pass from the renderer
			pSRPWindows->RemoveSceneRenderPass();
			// destroy the window
			pSRPWindows->DestroyWindow();
			// cleanup the instance
			pSRPWindows->DestroyInstance();
		}
		// clear the hashmap
		m_pWindows->Clear();
	}
}


void Gui::Initialize()
{
	// we try to initialize berkelium
	m_bBerkeliumInitialized = Berkelium::init(Berkelium::FileString::empty());
	if (m_bBerkeliumInitialized)
	{
		// we need to have a dummy window that returns from certain methods
		AddDummyWindow();
	}
	else
	{
		// berkelium could not be initialized
	}
}


void Gui::StopBerkelium() const
{
	if (m_bBerkeliumInitialized)
	{
		// stop berkelium
		Berkelium::stopRunning();
	}
}


Berkelium::Window *Gui::GetBerkeliumWindow(const String &sName) const
{
	if (m_pWindows->Get(sName) == NULL)
	{
		// we should return the berkelium window from the dummy window to prevent crashes on called functions
		// the end user should always verify that the returned object is the right one
		return m_pWindows->Get(BERKELIUMDUMMYWINDOW)->GetBerkeliumWindow();
	}
	else
	{
		// return the berkelium window
		return m_pWindows->Get(sName)->GetBerkeliumWindow();
	}
}


void Gui::UpdateBerkelium()
{
	// update berkelium
	Berkelium::update();
}


void Gui::DestroyInstance() const
{
	// cleanup
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
				// assign the renderers for future usage
				m_pCurrentRenderer = pRenderer;
				m_pCurrentSceneRenderer = pSceneRenderer;
				m_bRenderersInitialized = true;
				// create the mouse pointer
				CreateMousePointer();
			}
		}
	}
}


sWindowsData *Gui::GetWindowData(const String &sName) const
{
	if (m_pWindows->Get(sName) == NULL)
	{
		// we should return the data from the dummy window to prevent crashes on called functions
		// the end user should always verify that the returned data is the right one
		return m_pWindows->Get(BERKELIUMDUMMYWINDOW)->GetData();
	}
	else
	{
		// return the data
		return m_pWindows->Get(sName)->GetData();
	}
}


void Gui::AddDummyWindow()
{
	// we create the window
	SRPWindows *pSRPWindows = new SRPWindows(BERKELIUMDUMMYWINDOW);

	// we assign data to it
	pSRPWindows->GetData()->bIsVisable = false;
	pSRPWindows->GetData()->nFrameWidth = -1;
	pSRPWindows->GetData()->nFrameHeight = -1;
	pSRPWindows->GetData()->nXPos = -1;
	pSRPWindows->GetData()->nYPos = -1;
	pSRPWindows->GetData()->bKeyboardEnabled = false;
	pSRPWindows->GetData()->bMouseEnabled = false;

	// we create a berkelium window
	pSRPWindows->CreateBerkeliumWindow();

	// we add the created dummy window to the hashmap
	m_pWindows->Add(BERKELIUMDUMMYWINDOW, pSRPWindows);
}


bool Gui::RemoveWindow(const String &sName)
{
	if (sName == BERKELIUMDUMMYWINDOW)
	{
		// we should never remove the dummy window
		return false;
	}
	if (m_pWindows->Get(sName) == NULL)
	{
		// we cannot remove a window that cannot be found
		return false;
	}
	else
	{
		// get the window
		SRPWindows *pSRPWindows = m_pWindows->Get(sName);
		// remove the scene render pass from the renderer
		pSRPWindows->RemoveSceneRenderPass();
		// destroy the window
		pSRPWindows->DestroyWindow();
		// cleanup the instance
		pSRPWindows->DestroyInstance();

		// remove the window from the hashmap, should always be true
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
		// we should return the dummy window to prevent crashes on called functions
		// the end user should always verify that the returned window is the right one
		return m_pWindows->Get(BERKELIUMDUMMYWINDOW);
	}
	else
	{
		// return the window
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
	// move the mouse for berkelium
	pSRPWindow->GetBerkeliumWindow()->mouseMoved(pSRPWindow->GetRelativeMousePosition(vMousePos).x, pSRPWindow->GetRelativeMousePosition(vMousePos).y);
}


void Gui::UnFocusAllWindows()
{
	// focused window needs to be a nullptr
	m_pFocusedWindow = nullptr;
	// get the iterator for all the windows
	Iterator<SRPWindows*> cIterator = m_pWindows->GetIterator();
	// loop trough the windows
	while (cIterator.HasNext())
	{
		// unfocus the window
		cIterator.Next()->GetBerkeliumWindow()->unfocus();
	}
}


List<SRPWindows*> *Gui::GetMouseEnabledWindows()
{
	// create an empty list
	List<SRPWindows*> *pList = new List<SRPWindows*>;

	// get the iterator for all the windows
	Iterator<SRPWindows*> cIterator = m_pWindows->GetIterator();
	// loop trough the windows
	while (cIterator.HasNext())
	{
		SRPWindows *pSRPWindows = cIterator.Next();
		if (pSRPWindows->GetData()->bIsVisable && pSRPWindows->GetData()->bMouseEnabled)
		{
			// if the window is visible and the mouse for the window is enabled then add the window to the list
			pList->Add(pSRPWindows);
		}
	}

	if (pList->GetNumOfElements() > 0)
	{
		// list has items so return the list
		return pList;
	}
	else
	{
		// return nothing because the list is empty
		return nullptr;
	}
}


void Gui::FocusWindow(SRPWindows *pSRPWindows)
{
	// check if the window is already focused
	if (m_pFocusedWindow != pSRPWindows)
	{
		if (m_pFocusedWindow != nullptr)
		{
			// unfocus all windows
			UnFocusAllWindows();
		}
		// focus the window
		pSRPWindows->GetBerkeliumWindow()->focus();
		// set the window to front
		pSRPWindows->MoveToFront();
		// set the new focused window
		m_pFocusedWindow = pSRPWindows;
	}
}


List<SRPWindows*> *Gui::GetMouseOverWindows(const List<SRPWindows*> *pEnabledWindows, const Vector2i &vMousePos)
{
	if (!pEnabledWindows)
	{
		// return nothing because the enabled windows list is not valid
		return nullptr;
	}

	// create an empty list
	List<SRPWindows*> *pList = new List<SRPWindows*>;

	// get the iterator for the enabled windows
	Iterator<SRPWindows*> cIterator = pEnabledWindows->GetIterator();
	// loop trough the enabled windows
	while (cIterator.HasNext())
	{
		SRPWindows *pSRPWindows = cIterator.Next();

		// get the relative mouse position for the window
		Vector2i vRelativeMousePos = pSRPWindows->GetRelativeMousePosition(vMousePos);

		if (vRelativeMousePos.x > 0 && vRelativeMousePos.y > 0 && vRelativeMousePos.x < pSRPWindows->GetSize().x && vRelativeMousePos.y < pSRPWindows->GetSize().y)
		{
			// the mouse is currently over the window, so we add the window to the list
			pList->Add(pSRPWindows);
		}
		else
		{
			// check if any widgets are drawn for the window
			if (pSRPWindows->GetWidgets()->GetNumOfElements() > 0)
			{
				// get the iterator for the widgets
				Iterator<sWidget*> cWidgetIterator = pSRPWindows->GetWidgets()->GetIterator();
				// loop trough the widgets
				while (cWidgetIterator.HasNext())
				{
					sWidget *psWidget = cWidgetIterator.Next();

					// get the relative mouse position for the widget
					Vector2i vRelativeMousePosWidget = pSRPWindows->GetRelativeMousePositionWidget(psWidget, vMousePos);

					if (vRelativeMousePosWidget.x > 0 && vRelativeMousePosWidget.y > 0 && vRelativeMousePosWidget.x < psWidget->nWidth && vRelativeMousePosWidget.y < psWidget->nHeight)
					{
						// the mouse is currently over the widget, so we add the window to the list
						pList->Add(pSRPWindows);
					}
				}
			}
		}
	}

	if (pList->GetNumOfElements() > 0)
	{
		// list has items so return the list
		return pList;
	}
	else
	{
		// return nothing because the list is empty
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
		// connect the controller to the handler
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
		// connect the update event handler
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
	// get the frontend
	Frontend &cFrontend = static_cast<FrontendApplication*>(CoreApplication::GetApplication())->GetFrontend();

	if (cFrontend.IsRunning() && cFrontend.IsMouseOver())
	{
		// get the mouse position
		Vector2i vMousePos(cFrontend.GetMousePositionX(), cFrontend.GetMousePositionY());
		// set the last known mouse position
		m_vLastKnownMousePos = vMousePos;
		
		// check if mouse pointer is visible
		if (GetMousePointer()->IsVisible())
		{
			// hide the native mouse pointer
			// this should be a setting so that the user can decide if they want to hide the native mouse pointer
			cFrontend.SetMouseVisible(false);

			// set the position of the mouse pointer
			GetMousePointer()->SetPosition(vMousePos.x, vMousePos.y);
			// move the mouse pointer to front so that its always visible
			GetMousePointer()->MoveToFront();
		}
		else
		{
			// show the native mouse pointer
			// this should be a setting so that the user can decide if they want to show the native mouse pointer
			cFrontend.SetMouseVisible(true);
		}

		// get the window that the mouse is over
		// if there are more window under the mouse then it will return the top most
		SRPWindows *pSRPWindow = GetTopMostWindow(GetMouseOverWindows(GetMouseEnabledWindows(), vMousePos));
		if (pSRPWindow)
		{
			// move the mouse on the window
			MouseMove(pSRPWindow, vMousePos);
			// process mouse clicks on the window
			MouseClicks(pSRPWindow, cControl);
			// process mouse scrolls on the window
			MouseScrolls(pSRPWindow, cControl);
			// set the window that the mouse last had contact with
			m_pLastMouseWindow = pSRPWindow;
		}
		else
		{
			if (m_pLastMouseWindow)
			{
				// the mouse has left the window so the tooltip should be empty
				m_pLastMouseWindow->SetToolTip("");
			}

			if (m_pFocusedWindow)
			{
				// process mouse scrolls for the focused window
				MouseScrolls(m_pFocusedWindow, cControl);

				if (cControl.GetName() == "MouseLeft")
				{
					// we clicked outside a window so we need to unfocus it
					UnFocusAllWindows();
				}
			}

			if (cControl.GetName() == "MouseLeft")
			{
				// set the state for the left mouse button
				m_bMouseLeftDown = reinterpret_cast<Button&>(cControl).IsPressed();
			}
		}

		// the mouse supposedly has moved so we wanna know about it
		m_bMouseMoved = true;
	}
}


void Gui::MouseClicks(SRPWindows *pSRPWindow, Control &cControl)
{
	if (cControl.GetName() == "MouseLeft")
	{
		// mouse clicked on a window so we need to focus it
		/*have this happen on mouse down*/
		FocusWindow(pSRPWindow);

		if ((Timing::GetInstance()->GetPastTime() - m_nLastMouseLeftReleaseTime) > 0 && (Timing::GetInstance()->GetPastTime() - m_nLastMouseLeftReleaseTime) < 250)
		{
			// we should send a double click
			pSRPWindow->GetBerkeliumWindow()->mouseButton(0, reinterpret_cast<Button&>(cControl).IsPressed(), 2);
			m_nLastMouseLeftReleaseTime = 0;
		}
		else
		{
			// send a single click
			pSRPWindow->GetBerkeliumWindow()->mouseButton(0, reinterpret_cast<Button&>(cControl).IsPressed());
			if (!reinterpret_cast<Button&>(cControl).IsPressed())
			{
				// mouse button has been released
				m_bMouseLeftDown = false;
				m_nLastMouseLeftReleaseTime = Timing::GetInstance()->GetPastTime();
			}
			else
			{
				// mouse button has been pressed
				m_bMouseLeftDown = true;
				m_nLastMouseLeftReleaseTime = 0;
			}
		}
	}
	if (cControl.GetName() == "MouseRight")
	{
		// mouse clicked on a window so we need to focus it
		/*have this happen on mouse down*/
		FocusWindow(pSRPWindow);

		// send a right mouse click
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
					// if all of the above is true, send mouse scroll
					pSRPWindow->GetBerkeliumWindow()->mouseWheel(0, int(static_cast<Axis&>(cControl).GetValue()));
				}
			}
		}
	}
}


void Gui::OnControl(Control &cControl)
{
	/*we wanna know about any mouse stuff*/
	MouseEvents(cControl);

	/*keyboard stuff*/
	KeyboardEvents(cControl);
}


SRPWindows *Gui::GetTopMostWindow(List<SRPWindows*> *pWindows)
{
	if (pWindows)
	{
		if (pWindows->GetNumOfElements() == 1)
		{
			// if the list has only one item return it
			return pWindows->Get(0);
		}
		else
		{
			// clear the current topmost window
			SRPWindows *pTopMostWindow = nullptr;

			int nHigherIndex = -1;

			// get the iterator for the windows in the list
			Iterator<SRPWindows*> cIterator = pWindows->GetIterator();
			// loop trough the windows
			while (cIterator.HasNext())
			{
				SRPWindows *pSRPWindows = cIterator.Next();
				if (pSRPWindows->GetSceneRenderPassIndex() > nHigherIndex)
				{
					// if the window is top most, keep it
					nHigherIndex = pSRPWindows->GetSceneRenderPassIndex();
					pTopMostWindow = pSRPWindows;
				}
			}

			// return the top most window
			return pTopMostWindow;
		}
	}
	else
	{
		// return nothing
		return nullptr;
	}
}


bool Gui::SetMousePointerVisible(const bool &bVisible) const
{
	if (m_bIsControllerConnected)
	{
		if (GetMousePointer())
		{
			// set the visibility for the mouse pointer
			GetMousePointer()->SetVisible(bVisible);
			return true;
		}
	}
	return false;
}


void Gui::DefaultCallBackHandler()
{
	// get the iterator for the windows
	Iterator<SRPWindows*> cIterator = m_pWindows->GetIterator();
	// loop trough the windows
	while (cIterator.HasNext())
	{
		SRPWindows *pSRPWindows = cIterator.Next();

		// check if callback is present
		if (pSRPWindows->GetNumberOfCallBacks() > 0)
		{
			if (pSRPWindows->GetCallBack(DRAGWINDOW))
			{
				m_pDragWindow = pSRPWindows;
			}

			if (pSRPWindows->GetCallBack(HIDEWINDOW))
			{
				/*hide window*/
			}

			if (pSRPWindows->GetCallBack(CLOSEWINDOW))
			{
				/*close window*/
			}

			// call back is processed so we clear them
			pSRPWindows->ClearCallBacks();
		}
	}
}


void Gui::KeyboardEvents(Control &cControl)
{
	/*i am not yet satisfied with this method*/

	if (m_pFocusedWindow)
	{
		// check if the focused window allows for key events
		if (m_pFocusedWindow->GetData()->bKeyboardEnabled)
		{
			// check if the event is for the keyboard
			if (cControl.GetName().GetSubstring(0, 8) == "Keyboard")
			{
				// get the button class
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
	if (sName == BERKELIUMDUMMYWINDOW)
	{
		// we should not do this on the dummy window
		return false;
	}
	if (m_pWindows->Get(sName) == NULL)
	{
		// we cannot find the window
		return false;
	}
	else
	{
		// check if the window we want to hide is focused
		if (m_pWindows->Get(sName) == m_pFocusedWindow && bVisible == false)
		{
			// unfocus the window before we hide it
			UnFocusAllWindows();
		}

		// set the visibility of the window
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
				// check if we already locked the mouse position
				if (m_vLockMousePos == Vector2i::Zero)
				{
					// when dragging the window the tooltip should be empty
					m_pDragWindow->SetToolTip("");
					// we need to lock the mouse position relative to the dragging window
					m_vLockMousePos = m_pDragWindow->GetRelativeMousePosition(m_vLastKnownMousePos);
				}
				// we move the window
				m_pDragWindow->MoveWindow(m_vLastKnownMousePos.x - m_vLockMousePos.x, m_vLastKnownMousePos.y - m_vLockMousePos.y);
				// after its all done we reset the mouse moved status so that we do not keep doing this repeatedly
				m_bMouseMoved = false;
			}
		}
		else
		{
			// the left mouse is not pressed anymore so we can reset the following
			m_vLockMousePos = Vector2i::Zero;
			m_pDragWindow = nullptr;
		}
	}
}


void Gui::KeyboardHandler()
{
	/*i am not yet satisfied with this method*/

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
	/*i am not yet satisfied with this method*/

	m_pTextButtonHandler->Clear();
	psButton->bValid = true;
	psButton->sKey = sKey;
	m_pTextButtonHandler->Add(sName, psButton);
	m_nTextKeyHitCount = 0;
}


void Gui::AddKey(const PLCore::String &sName, const char &nKey, sButton *psButton)
{
	/*i am not yet satisfied with this method*/

	psButton->bValid = true;
	psButton->nKey = nKey;
	m_pKeyButtonHandler->Add(sName, psButton);
	m_nKeyHitCount = 0;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLBerkelium