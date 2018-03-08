#pragma once

#include "pch.h"
#include "Renderer.h"
#include "Graphics.h"
#include "Game.h"
#include "Menu.h"

ref class BicycleAdventure sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public:
	BicycleAdventure();
	
	// IFrameworkView Methods.
	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
	virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
	virtual void Load(Platform::String^ entryPoint);
	virtual void Run();
	virtual void Uninitialize();

protected:
	// Event Handlers.
	void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
	void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
	void OnResuming(Platform::Object^ sender, Platform::Object^ args);
	void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);
	void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
	void OnPointerPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerReleased(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);

private:
	Renderer^ m_renderer;
	bool m_windowClosed;
	bool m_windowVisible;
//	Graphics m_debugDraw;
	std::unique_ptr<Game> m_game;
	Windows::System::Display::DisplayRequest m_disprequest;

};

ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};