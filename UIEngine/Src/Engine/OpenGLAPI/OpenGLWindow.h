#include <string>

struct GLFWwindow;

class OpenGLWindow
{
public:
	static OpenGLWindow& Instance()
	{
		static OpenGLWindow instance;
		return instance;
	}

	OpenGLWindow(const OpenGLWindow&)            = delete;
	OpenGLWindow& operator=(const OpenGLWindow&) = delete;

	// Initialise
	void Initialise(int width, int height, std::string_view name);

	// Window Functions
	int Window_intit(int width, int height, std::string_view name);

	void Update_Window();

	int Window_shouldClose();

	void Window_destroy();

	float GetDeltaTime() const;

	// Getters
	GLFWwindow* GetWindow() const
	{
		return window;
	}

	int m_Width  = 0;
	int m_Height = 0;

private:
	OpenGLWindow() = default;

	~OpenGLWindow();

	// Main Window
	GLFWwindow* window;

	// DeltaTime
	double last = 0.0;
	double now  = 0.0;
	float delta = 1.0f;
};
