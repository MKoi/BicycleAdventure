#include "pch.h"
#include "BicycleAdventure.h"
#include "BasicTimer.h"
#include "Renderer.h"
#include "Block.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace concurrency;

BicycleAdventure::BicycleAdventure() :
	m_windowClosed(false),
	m_windowVisible(true)
{
	
}

void BicycleAdventure::Initialize(CoreApplicationView^ applicationView)
{
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &BicycleAdventure::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &BicycleAdventure::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &BicycleAdventure::OnResuming);

	m_renderer = ref new Renderer();
	Graphics::InitColors();
	m_disprequest.RequestActive();
	m_game.reset(new Game(m_renderer));
}

void BicycleAdventure::SetWindow(CoreWindow^ window)
{
	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &BicycleAdventure::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &BicycleAdventure::OnWindowClosed);

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &BicycleAdventure::OnPointerPressed);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &BicycleAdventure::OnPointerMoved);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &BicycleAdventure::OnPointerReleased);

	m_renderer->Initialize(CoreWindow::GetForCurrentThread());
	m_game->UpdateScreenDimensions();
}

void BicycleAdventure::Load(Platform::String^ entryPoint)
{
}

void BicycleAdventure::Run()
{
	BasicTimer^ timer = ref new BasicTimer();

	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			timer->Update();
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			
			if (m_game->Ready())
			{
				m_game->Update(timer->Delta);
				m_renderer->Update(timer->Total, timer->Delta);
				m_game->Draw();
				m_renderer->Present(); // This call is synchronized to the display frame rate.
			}			
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

void BicycleAdventure::Uninitialize()
{
}

void BicycleAdventure::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void BicycleAdventure::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
	
}


void BicycleAdventure::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
	if (m_game && m_game->Ready())
	{
		m_game->OnPressed(args->CurrentPoint->Position);
	}
}

void BicycleAdventure::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{

}

void BicycleAdventure::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{	
	if (m_game && m_game->Ready())
	{
		m_game->OnReleased();
	}
}

void BicycleAdventure::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	CoreWindow::GetForCurrentThread()->Activate();
//	OutputDebugString(TEXT("OnActivated Activate\n"));
//	m_disprequest.RequestActive();
}

void BicycleAdventure::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	m_disprequest.RequestRelease();

	if (m_game->Ready())
	{
		SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();
		m_game->Save(deferral);
	}
	m_renderer->ReleaseResourcesForSuspending();
}
 
void BicycleAdventure::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.
	m_disprequest.RequestActive();
	m_renderer->CreateWindowSizeDependentResources();
	m_game->ResumeSuspend();
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new BicycleAdventure();
}

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}