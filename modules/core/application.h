#ifndef MODULE_APPLICATION_H
#define MODULE_APPLICATION_H

namespace module
{
	class DX12Device;

	LRESULT windowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	class Application
	{
	public:
		static Application* instance()
		{
			if (!Application::mpInstance)
			{
				mpInstance = new Application();
			}
			return mpInstance;
		}

		long getWindowWidth() { return mpWindowWidth; }
		long getWindowHeight() { return mpWindowHeight; }
		RECT getWindowRect() { return { 0, 0, mpWindowWidth, mpWindowHeight }; }
		HWND getHwnd() const { return mHwnd; }

		void createWindow();
		void initialize();
		void run();
		void finalize();
	private:
		Application();
		static Application* mpInstance;
		DX12Device* mpDevice;

		HWND mHwnd;
		WNDCLASSEX mpWndClass;
		long mpWindowWidth;
		long mpWindowHeight;
		const float mClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
}

#endif // !MODULE_APPLICATION_H



