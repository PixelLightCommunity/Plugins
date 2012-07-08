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
	SlotOnUpdate(this),
	SlotOnControl(this),
	m_bBerkeliumInitialized(false),
	m_bRenderersInitialized(false),
	m_pmapWindows(new HashMap<String, SRPWindow*>),
	m_pCurrentSceneRenderer(nullptr),
	m_pCurrentRenderer(nullptr),
	m_pSRPMousePointer(nullptr),
	m_pFocusedWindow(nullptr),
	m_bControlsEnabled(true),
	m_bIsUpdateConnected(false),
	m_bIsControllerConnected(false),
	m_sLastControl(""),
	m_nLastMouseLeftReleaseTime(0),
	m_pLastMouseWindow(nullptr),
	m_bMouseLeftDown(false),
	m_vLastKnownMousePos(Vector2i::Zero),
	m_pDragWindow(nullptr),
	m_pResizeWindow(nullptr),
	m_bMouseMoved(false),
	m_vLockMousePos(Vector2i::Zero),
	m_pmapTextButtonHandler(new HashMap<String, sButton*>),
	m_pmapKeyButtonHandler(new HashMap<String, sButton*>),
	m_nLastTextKeySendTime(0),
	m_nLastKeySendTime(0),
	m_nTextKeyHitCount(0),
	m_nKeyHitCount(0)
{
	// initialize everything need to run berkelium
	Initialize();
}


Gui::~Gui()
{
	// we should destroy all windows
	DestroyWindows();
	// we should destroy the mouse pointer
	DestroyMousePointer();
	// we should stop berkelium from doing anything else
	StopBerkelium();
	// cleanup
	delete m_pmapWindows;
	delete m_pmapTextButtonHandler;
	delete m_pmapKeyButtonHandler;
}


void Gui::DebugToConsole(const String &sString)
{
	/*this should be deprecated when not needed anymore*/
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
		if (m_pmapWindows->Get(sName))
		{
			// window name already exists, this is to prevent duplication and overrides in the hashmap
			return false;
		}

		// we create the window
		SRPWindow *pSRPWindow = new SRPWindow(sName);

		// we assign data to it
		pSRPWindow->GetData()->bIsVisable = pVisible;
		pSRPWindow->GetData()->sUrl = sUrl;
		pSRPWindow->GetData()->nFrameWidth = nWidth;
		pSRPWindow->GetData()->nFrameHeight = nHeight;
		pSRPWindow->GetData()->nXPos = nX;
		pSRPWindow->GetData()->nYPos = nY;
		pSRPWindow->GetData()->bTransparent = bTransparent;
		pSRPWindow->GetData()->bKeyboardEnabled = bEnabled;
		/*implement the option to disallow mouse events but still keep keyboard events going*/
		pSRPWindow->GetData()->bMouseEnabled = bEnabled;
		pSRPWindow->GetData()->bNeedsFullUpdate = true;
		pSRPWindow->GetData()->bLoaded = false;

		// we initialize the window
		if (pSRPWindow->Initialize(m_pCurrentRenderer, Vector2(float(nX), float(nY)), Vector2(float(nWidth), float(nHeight))))
		{
			// we add the window scene render pass to the renderer
			pSRPWindow->AddSceneRenderPass(m_pCurrentSceneRenderer);
		}
		else
		{
			// window cannot be initialized so we should destroy and cleanup the leftovers
			pSRPWindow->DestroyInstance();
			return false;
		}

		// we add the created window to the hashmap
		m_pmapWindows->Add(sName, pSRPWindow);

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
		Iterator<SRPWindow*> cIterator = m_pmapWindows->GetIterator();
		// loop trough the windows
		while (cIterator.HasNext())
		{
			SRPWindow *pSRPWindow = cIterator.Next();
			// remove the scene render pass from the renderer
			pSRPWindow->RemoveSceneRenderPass();
			// cleanup the instance
			pSRPWindow->DestroyInstance();
		}
		// clear the hashmap
		m_pmapWindows->Clear();
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
		// stop berkelium from running
		Berkelium::stopRunning();
	}
}


Berkelium::Window *Gui::GetBerkeliumWindow(const PLCore::String &sName)
{
	if (m_pmapWindows->Get(sName) == NULL)
	{
		/*test debug to make sure the dummy window is returned if the window is not found*/
		DebugToConsole("Could not find window, returning dummy instead\n");

		// we should return the berkelium window from the dummy window to prevent crashes on called functions
		// the end user should always verify that the returned object is the right one
		return m_pmapWindows->Get(BERKELIUMDUMMYWINDOW)->GetBerkeliumWindow();
	}
	else
	{
		// return the berkelium window
		return m_pmapWindows->Get(sName)->GetBerkeliumWindow();
	}
}


void Gui::UpdateBerkelium()
{
	// update berkelium
	Berkelium::update();
}


void Gui::DestroyInstance() const
{
	// cleanup this instance
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

				/*perhaps the following can be moved somewhere else*/
				// create the mouse pointer
				CreateMousePointer();
			}
		}
	}
}


sWindowsData *Gui::GetWindowData(const PLCore::String &sName)
{
	if (m_pmapWindows->Get(sName) == NULL)
	{
		/*test debug to make sure the dummy window is returned if the window is not found*/
		DebugToConsole("Could not find window, returning dummy instead\n");

		// we should return the data from the dummy window to prevent crashes on called functions
		// the end user should always verify that the returned data is the right one
		return m_pmapWindows->Get(BERKELIUMDUMMYWINDOW)->GetData();
	}
	else
	{
		// return the data
		return m_pmapWindows->Get(sName)->GetData();

	}
}


void Gui::AddDummyWindow()
{
	// we create the window
	SRPWindow *pSRPWindow = new SRPWindow(BERKELIUMDUMMYWINDOW);

	// we assign data to it
	pSRPWindow->GetData()->bIsVisable = false;
	pSRPWindow->GetData()->nFrameWidth = -1;
	pSRPWindow->GetData()->nFrameHeight = -1;
	pSRPWindow->GetData()->nXPos = -1;
	pSRPWindow->GetData()->nYPos = -1;
	pSRPWindow->GetData()->bKeyboardEnabled = false;
	pSRPWindow->GetData()->bMouseEnabled = false;

	// we create a berkelium window
	pSRPWindow->CreateBerkeliumWindow();

	// we add the created dummy window to the hashmap
	m_pmapWindows->Add(BERKELIUMDUMMYWINDOW, pSRPWindow);
}


bool Gui::RemoveWindow(const String &sName)
{
	if (sName == BERKELIUMDUMMYWINDOW)
	{
		// we should never remove the dummy window
		return false;
	}
	if (m_pmapWindows->Get(sName) == NULL)
	{
		// we cannot remove a window that cannot be found
		return false;
	}
	else
	{
		// get the window
		SRPWindow *pSRPWindow = m_pmapWindows->Get(sName);

		// unfocus window if its focused
		if (pSRPWindow == m_pFocusedWindow)
		{
			UnFocusAllWindows();
		}
		// reset last mouse window if its the window we are trying to remove
		if (pSRPWindow == m_pLastMouseWindow)
		{
			m_pLastMouseWindow = nullptr;
		}
		
		// remove the scene render pass from the renderer
		pSRPWindow->RemoveSceneRenderPass();
		// cleanup the instance
		pSRPWindow->DestroyInstance();

		// remove the window from the hashmap, should always be true
		return m_pmapWindows->Remove(sName);
	}
}


void Gui::CreateMousePointer()
{
	// we create a mouse pointer
	m_pSRPMousePointer = new SRPMousePointer(m_pCurrentRenderer, m_pCurrentSceneRenderer);
}


void Gui::DestroyMousePointer() const
{
	if (m_bRenderersInitialized && m_pSRPMousePointer)
	{
		// we destroy the instance of the mouse pointer to allow for a proper cleanup
		m_pSRPMousePointer->DestroyInstance();
	}
}


SRPWindow *Gui::GetWindow(const PLCore::String &sName)
{
	if (m_pmapWindows->Get(sName) == NULL)
	{
		/*test debug to make sure the dummy window is returned if the window is not found*/
		DebugToConsole("Could not find window, returning dummy instead\n");

		// we should return the dummy window to prevent crashes on called functions
		// the end user should always verify that the returned window is the right one
		return m_pmapWindows->Get(BERKELIUMDUMMYWINDOW);
	}
	else
	{
		// return the window
		return m_pmapWindows->Get(sName);
	}
}


SRPMousePointer *Gui::GetMousePointer() const
{
	return m_pSRPMousePointer;
}


HashMap<String, SRPWindow*> *Gui::GetWindowsMap() const
{
	return m_pmapWindows;
}


bool Gui::IsBerkeliumInitialized() const
{
	return m_bBerkeliumInitialized;
}


void Gui::MouseMove(const SRPWindow *pSRPWindow, const Vector2i &vMousePos) const
{
	// move the mouse for berkelium
	pSRPWindow->GetBerkeliumWindow()->mouseMoved(pSRPWindow->GetRelativeMousePosition(vMousePos).x, pSRPWindow->GetRelativeMousePosition(vMousePos).y);
}


void Gui::UnFocusAllWindows()
{
	// focused window needs to be a nullptr
	m_pFocusedWindow = nullptr;
	// get the iterator for all the windows
	Iterator<SRPWindow*> cIterator = m_pmapWindows->GetIterator();
	// loop trough the windows
	while (cIterator.HasNext())
	{
		// unfocus the window
		cIterator.Next()->GetBerkeliumWindow()->unfocus();
	}
}


List<SRPWindow*> *Gui::GetMouseEnabledWindows()
{
	// create an empty list
	List<SRPWindow*> *plstMouseEnabledWindows = new List<SRPWindow*>;

	// get the iterator for all the windows
	Iterator<SRPWindow*> cIterator = m_pmapWindows->GetIterator();
	// loop trough the windows
	while (cIterator.HasNext())
	{
		SRPWindow *pSRPWindow = cIterator.Next();
		if (pSRPWindow->GetData()->bIsVisable && pSRPWindow->GetData()->bMouseEnabled)
		{
			// if the window is visible and the mouse for the window is enabled then add the window to the list
			plstMouseEnabledWindows->Add(pSRPWindow);
		}
	}

	if (plstMouseEnabledWindows->GetNumOfElements() > 0)
	{
		// list has items so return the list
		return plstMouseEnabledWindows;
	}
	else
	{
		//question: [08-07-2012 Icefire] if we do not return the instance do we still need delete it?
		// we do not need the list any more
		delete plstMouseEnabledWindows;
		// return nothing because the list is empty
		return nullptr;
	}
}


void Gui::FocusWindow(SRPWindow *pSRPWindow)
{
	// check if the window is already focused
	if (m_pFocusedWindow != pSRPWindow)
	{
		if (m_pFocusedWindow != nullptr)
		{
			// unfocus all windows
			UnFocusAllWindows();
		}
		// focus the window
		pSRPWindow->GetBerkeliumWindow()->focus();
		// set the window to front
		pSRPWindow->MoveToFront();
		// set the new focused window
		m_pFocusedWindow = pSRPWindow;
	}
}


List<SRPWindow*> *Gui::GetMouseOverWindows(const List<SRPWindow*> *plstEnabledWindows, const Vector2i &vMousePos)
{
	if (!plstEnabledWindows)
	{
		// return nothing because the enabled windows list is not valid
		return nullptr;
	}

	// create an empty list
	List<SRPWindow*> *plstMouseOverWindows = new List<SRPWindow*>;

	// get the iterator for the enabled windows
	Iterator<SRPWindow*> cIterator = plstEnabledWindows->GetIterator();
	// loop trough the enabled windows
	while (cIterator.HasNext())
	{
		SRPWindow *pSRPWindow = cIterator.Next();

		// get the relative mouse position for the window
		Vector2i vRelativeMousePos = pSRPWindow->GetRelativeMousePosition(vMousePos);

		if (vRelativeMousePos.x > 0 && vRelativeMousePos.y > 0 && vRelativeMousePos.x < pSRPWindow->GetSize().x && vRelativeMousePos.y < pSRPWindow->GetSize().y)
		{
			// the mouse is currently over the window, so we add the window to the list
			plstMouseOverWindows->Add(pSRPWindow);
		}
		else
		{
			// check if any widgets are drawn for the window
			if (pSRPWindow->GetWidgets()->GetNumOfElements() > 0)
			{
				// get the iterator for the widgets
				Iterator<sWidget*> cWidgetIterator = pSRPWindow->GetWidgets()->GetIterator();
				// loop trough the widgets
				while (cWidgetIterator.HasNext())
				{
					sWidget *psWidget = cWidgetIterator.Next();

					// get the relative mouse position for the widget
					Vector2i vRelativeMousePosWidget = pSRPWindow->GetRelativeMousePositionWidget(psWidget, vMousePos);

					if (vRelativeMousePosWidget.x > 0 && vRelativeMousePosWidget.y > 0 && vRelativeMousePosWidget.x < psWidget->nWidth && vRelativeMousePosWidget.y < psWidget->nHeight)
					{
						// the mouse is currently over the widget, so we add the window to the list
						plstMouseOverWindows->Add(pSRPWindow);
					}
				}
			}
		}
	}

	if (plstMouseOverWindows->GetNumOfElements() > 0)
	{
		// list has items so return the list
		return plstMouseOverWindows;
	}
	else
	{
		//question: [08-07-2012 Icefire] if we do not return the instance do we still need delete it?
		// we do not need the list any more
		delete plstMouseOverWindows;
		// return nothing because the list is empty
		return nullptr;
	}
}


SRPWindow *Gui::GetFocusedWindow() const
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
	ResizeWindowHandler();
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
	return false;
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
		if (GetMousePointer()->IsVisible() && GetMousePointer()->IsInitialized())
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
		SRPWindow *pSRPWindow = GetTopMostWindow(GetMouseOverWindows(GetMouseEnabledWindows(), vMousePos));
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
				/*deprecate*/
				m_pLastMouseWindow->SetToolTip("");
			}

			if (m_pFocusedWindow)
			{
				// process mouse scrolls for the focused window
				MouseScrolls(m_pFocusedWindow, cControl);

				if (cControl.GetName() == "MouseLeft")
				{
					// we clicked outside a window so we need to unfocus it
					/*
					this should only happen on mouse down, also allow for more mouse buttons to unfocus a window (right, middle, etc)
					*/
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
		/*perhaps a better check for this*/
		m_bMouseMoved = true;
	}
}


void Gui::MouseClicks(SRPWindow *pSRPWindow, Control &cControl)
{
	if (cControl.GetName() == "MouseLeft")
	{
		// mouse clicked on a window so we need to focus it
		/*have this happen on mouse down only*/
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


void Gui::MouseScrolls(SRPWindow *pSRPWindow, Control &cControl)
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
	{
		/*filter out anything but mouse events*/
		MouseEvents(cControl);
	}

	{
		/*filter out anything but keyboard events*/
		KeyboardEvents(cControl);
	}
}


SRPWindow *Gui::GetTopMostWindow(List<SRPWindow*> *plstWindows)
{
	if (plstWindows)
	{
		if (plstWindows->GetNumOfElements() == 1)
		{
			// if the list has only one item return it
			return plstWindows->Get(0);
		}
		else
		{
			// prepare for the current topmost window
			SRPWindow *pTopMostWindow = nullptr;

			int nHigherIndex = -1;

			// get the iterator for the windows in the list
			Iterator<SRPWindow*> cIterator = plstWindows->GetIterator();
			// loop trough the windows
			while (cIterator.HasNext())
			{
				SRPWindow *pSRPWindow = cIterator.Next();
				if (pSRPWindow->GetSceneRenderPassIndex() > nHigherIndex)
				{
					// if the window is top most, keep it
					nHigherIndex = pSRPWindow->GetSceneRenderPassIndex();
					pTopMostWindow = pSRPWindow;
				}
			}

			// return the top most window, can be a nullptr
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
	Iterator<SRPWindow*> cIterator = m_pmapWindows->GetIterator();
	// loop trough the windows
	while (cIterator.HasNext())
	{
		SRPWindow *pSRPWindow = cIterator.Next();

		// check if callback is present
		if (pSRPWindow->GetNumberOfCallBacks() > 0)
		{
			if (pSRPWindow->GetCallBack(DRAGWINDOW))
			{
				m_pDragWindow = pSRPWindow;
				// call back is processed so we clear them
				pSRPWindow->ClearCallBacks();
			}
			else if (pSRPWindow->GetCallBack(HIDEWINDOW))
			{
				pSRPWindow->GetData()->bIsVisable = false;
				// call back is processed so we clear them
				pSRPWindow->ClearCallBacks();
			}
			else if (pSRPWindow->GetCallBack(CLOSEWINDOW))
			{
				RemoveWindow(pSRPWindow->GetName());
				// call back is processed and should get cleared by the remove window method
			}
			else if (pSRPWindow->GetCallBack(RESIZEWINDOW))
			{
				m_pResizeWindow = pSRPWindow;
				// call back is processed so we clear them
				pSRPWindow->ClearCallBacks();
			}
		}
	}
}


void Gui::KeyboardEvents(Control &cControl)
{
	/*i am not yet satisfied with this method, so expect this to change*/

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
					else if (cButton.GetName() == "KeyboardReturn")
					{
						AddTextKey(cButton.GetName(), cButton.GetCharacter(), psButton);
					}
					else
					{
						psButton->bValid = false;
					}
				}
				else
				{
					if (m_pmapTextButtonHandler->GetNumOfElements() == 1)
					{
						m_nTextKeyHitCount = 0;
					}
					m_pmapTextButtonHandler->Remove(cButton.GetName());

					if (m_pmapKeyButtonHandler->GetNumOfElements() == 1)
					{
						m_nKeyHitCount = 0;
					}
					m_pmapKeyButtonHandler->Remove(cButton.GetName());
				}
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
	if (m_pmapWindows->Get(sName) == NULL)
	{
		// we cannot find the window
		return false;
	}
	else
	{
		// check if the window we want to hide is focused
		if (m_pmapWindows->Get(sName) == m_pFocusedWindow && bVisible == false)
		{
			// unfocus the window before we hide it
			UnFocusAllWindows();
		}

		// set the visibility of the window
		m_pmapWindows->Get(sName)->GetData()->bIsVisable = bVisible;
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
					/*deprecate*/
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
	/*i am not yet satisfied with this method, so expect this to change*/

	if (m_pFocusedWindow)
	{
		if (m_pFocusedWindow->GetData()->bKeyboardEnabled)
		{
			if (m_pmapTextButtonHandler->GetNumOfElements() > 0)
			{
				Iterator<sButton*> cIterator = m_pmapTextButtonHandler->GetIterator();
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
			if (m_pmapKeyButtonHandler->GetNumOfElements() > 0)
			{
				Iterator<sButton*> cIterator = m_pmapKeyButtonHandler->GetIterator();
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

	m_pmapTextButtonHandler->Clear();
	psButton->bValid = true;
	psButton->sKey = sKey;
	m_pmapTextButtonHandler->Add(sName, psButton);
	m_nTextKeyHitCount = 0;
}


void Gui::AddKey(const PLCore::String &sName, const char &nKey, sButton *psButton)
{
	/*i am not yet satisfied with this method*/

	psButton->bValid = true;
	psButton->nKey = nKey;
	m_pmapKeyButtonHandler->Add(sName, psButton);
	m_nKeyHitCount = 0;
}


void Gui::DebugNamesOfWindows()
{
	if (m_pmapWindows->GetNumOfElements() > 1)
	{
		DebugToConsole("Amount of windows found: " + String(m_pmapWindows->GetNumOfElements() - 1) + "\n");

		Iterator<SRPWindow*> cIterator = m_pmapWindows->GetIterator();
		while (cIterator.HasNext())
		{
			SRPWindow *pSRPWindow = cIterator.Next();
			if (pSRPWindow->GetName() != BERKELIUMDUMMYWINDOW)
			{
				DebugToConsole("Window name: '" + pSRPWindow->GetName() + "'\n");
				DebugToConsole("\t- Visible?: " + String(pSRPWindow->GetData()->bIsVisable ? "True" : "False") + "\n");
				DebugToConsole("\t- Size: " + pSRPWindow->GetSize().ToString() + "\n");
				DebugToConsole("\t- Position: " + pSRPWindow->GetPosition().ToString() + "\n");
				DebugToConsole("\t- Loaded?: " + String(pSRPWindow->GetData()->bLoaded ? "True" : "False") + "\n\n");
			}
		}
	}
}


void Gui::ResizeWindowHandler()
{
	//todo: [06-07-2012 Icefire] this resizing handler works for now, however the way we resize the buffer now is still not acceptable.
	//							 also keep in mind that this current approach thinks the resize is triggered from the bottom right corner of the windows.

	if (m_pResizeWindow)
	{
		if (m_bMouseLeftDown)
		{
			if (m_bMouseMoved)
			{
				// check if we already locked the mouse position
				if (m_vLockMousePos == Vector2i::Zero)
				{
					// when resizing the window the tooltip should be empty
					/*deprecate*/
					m_pResizeWindow->SetToolTip("");
					// we need to lock the mouse position relative to the resizing window
					m_vLockMousePos = m_pResizeWindow->GetRelativeMousePosition(m_vLastKnownMousePos);
				}

				if (m_vLockMousePos.x == (m_pResizeWindow->GetRelativeMousePosition(m_vLastKnownMousePos)).x &&
					m_vLockMousePos.y == (m_pResizeWindow->GetRelativeMousePosition(m_vLastKnownMousePos)).y)
				{
					// locked mouse has not moved so we do nothing
				}
				else if (m_vLockMousePos.x <= m_pResizeWindow->GetRelativeMousePosition(m_vLastKnownMousePos).x &&
					m_vLockMousePos.y <= m_pResizeWindow->GetRelativeMousePosition(m_vLastKnownMousePos).y)
				{
					// we should make the window bigger
					Vector2i vNewSize = m_pResizeWindow->GetSize() - (m_vLockMousePos - m_pResizeWindow->GetRelativeMousePosition(m_vLastKnownMousePos));
					// check if the new size difference is bigger than 2px in any direction so to not call to many resize updates
					if ((vNewSize - m_pResizeWindow->GetSize()).x > 2 || (vNewSize - m_pResizeWindow->GetSize()).y > 2)
					{
						// check if new window size is at least 4 x 4 pixels, if you need smaller windows than that you should switch your field to nanotechnology
						if (vNewSize.x >= 4 && vNewSize.y >= 4)
						{
							// we resize the window
							m_pResizeWindow->ResizeWindow(vNewSize.x, vNewSize.y);
							// we should reset the locked mouse position since the window has been resized
							m_vLockMousePos = Vector2i::Zero;
						}
					}
				}
				else
				{
					// we should make the window smaller
					Vector2i vNewSize = m_pResizeWindow->GetSize() - (m_vLockMousePos - m_pResizeWindow->GetRelativeMousePosition(m_vLastKnownMousePos));
					// check if the new size difference is bigger than 2px in any direction so to not call to many resize updates
					if ((m_pResizeWindow->GetSize() - vNewSize).x > 2 ||
						(m_pResizeWindow->GetSize() - vNewSize).y > 2 ||
						(m_pResizeWindow->GetSize() - vNewSize).x < -2 ||
						(m_pResizeWindow->GetSize() - vNewSize).y < -2)
					{
						// check if new window size is at least 4 x 4 pixels, if you need smaller windows than that you should switch your field to nanotechnology
						if (vNewSize.x >= 4 && vNewSize.y >= 4)
						{
							// we resize the window
							m_pResizeWindow->ResizeWindow(vNewSize.x, vNewSize.y);
							// we should reset the locked mouse position since the window has been resized
							m_vLockMousePos = Vector2i::Zero;
						}
					}
				}
				
				// after its all done we reset the mouse moved status so that we do not keep doing this repeatedly
				m_bMouseMoved = false;
			}
		}
		else
		{
			// the left mouse is not pressed anymore so we can reset the following
			m_vLockMousePos = Vector2i::Zero;
			m_pResizeWindow = nullptr;
		}
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLBerkelium