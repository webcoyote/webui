// Include the WebUI header
#include "webui.hpp"

// Include C++ STD
#include <iostream>
#include <filesystem>

// Making this object global so show_second_window() can access it.
webui::window my_second_window;

const char s_icon[] = "<svg class=\"w-6 h-6 text-gray-800 dark:text-white\" aria-hidden=\"true\" xmlns=\"http://www.w3.org/2000/svg\" fill=\"none\" viewBox=\"0 0 21 20\"><path stroke=\"currentColor\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" d=\"m11.479 1.712 2.367 4.8a.532.532 0 0 0 .4.292l5.294.769a.534.534 0 0 1 .3.91l-3.83 3.735a.534.534 0 0 0-.154.473l.9 5.272a.535.535 0 0 1-.775.563l-4.734-2.49a.536.536 0 0 0-.5 0l-4.73 2.487a.534.534 0 0 1-.775-.563l.9-5.272a.534.534 0 0 0-.154-.473L2.158 8.48a.534.534 0 0 1 .3-.911l5.294-.77a.532.532 0 0 0 .4-.292l2.367-4.8a.534.534 0 0 1 .96.004Z\"/></svg>";
const char s_icontype[] = "image/svg+xml";


// Example of a simple Class
class MyClass {
	public:
	// This method gets called every time the
	// user clicks on "OpenNewWindow"
	void show_second_window(webui::window::event* e) {
		// Show a new window, and navigate to `/second.html`
		// if the window is already opened, then switch in the same window
		my_second_window.show("second.html");
	}

	// This method gets called every time the
	// user clicks on "SwitchToSecondPage"
	void switch_to_second_page(webui::window::event* e) {
		// Switch to `/second.html` in the same opened window.
		e->get_window().show("second.html");
	}

	// Example of a simple function (Not a method)
	// This function receives all events because
	// it's get bind with an empty HTML ID.
	void events(webui::window::event* e) {
		if (e->event_type == webui::CONNECTED)
			std::cout << "Window Connected." << std::endl;
		else if (e->event_type == webui::DISCONNECTED)
			std::cout << "Window Disconnected." << std::endl;
		else if (e->event_type == webui::MOUSE_CLICK)
			std::cout << "Click on element: " << e->element << std::endl;
		else if (e->event_type == webui::NAVIGATION) {
			std::string url = e->get_string();
			std::cout << "Starting navigation to: " << url << std::endl;
			e->get_window().navigate(url);
		}
	}

	// Example of a simple function (Not a method)
	void exit_app(webui::window::event* e) {
		// Close all opened windows
		webui::exit();
	}
};

// -- MyClass C Wrapper ------------------------------------------------------------------------
// Because WebUI is written in C, so it can not access `MyClass` directly.
// That's why we should create a simple C++ wrapper.
MyClass myClassObj;
void show_second_window_wrp(webui::window::event* e) { myClassObj.show_second_window(e); }
void switch_to_second_page_wrp(webui::window::event* e) { myClassObj.switch_to_second_page(e); }
void events_wrp(webui::window::event* e) { myClassObj.events(e); }
void exit_app_wrp(webui::window::event* e) { myClassObj.exit_app(e); }
// ---------------------------------------------------------------------------------------------

#ifdef _WIN32
std::filesystem::path get_app_filename() {
  char path[MAX_PATH];
  BOOL success = GetModuleFileName(NULL, path, sizeof(path));
  return  std::filesystem::path(path);
}
#endif

#ifdef __linux__
std::filesystem::path get_app_filename() {
  char path[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
  return new std::filesystem::path(path);
}
#endif

#ifdef __APPLE__
std::filesystem::path get_app_filename() {
  	char path[PATH_MAX];
  	uint32_t size = sizeof(path);
	int count = _NSGetExecutablePath(path, &size);
  	return new std::filesystem::path(path);
}
#endif

int main() {

	// Print logs (debug build only)
	std::cout << "Starting..." << std::endl;

	// Set the path to the folder containing the application
	auto path = get_app_filename();
	std::filesystem::current_path(path.remove_filename());

	// Create a new window
	webui::window my_window;
	my_window.set_icon(s_icon, s_icontype);

	// Bind HTML element IDs with a C functions
	my_window.bind("SwitchToSecondPage", switch_to_second_page_wrp);
	my_window.bind("OpenNewWindow", show_second_window_wrp);
	my_window.bind("Exit", exit_app_wrp);
	my_second_window.bind("Exit", exit_app_wrp);

	// Bind all events
	my_window.bind("", events_wrp);
	my_second_window.bind("", events_wrp);

	// Show a new window
	my_window.show("index.html"); // my_window.show_browser("index.html", Chrome);

	// Wait until all windows get closed
	webui::wait();

	// Print logs (debug build only)
	std::cout << "Thank you." << std::endl;

	return 0;
}

#ifdef _WIN32
// Release build
int WINAPI
    wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	return main();
}
#endif
