module;

// #include <type_traits>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// NOLINTBEGIN(misc-unused-using-decls)
export module glfw;

export  {
using ::GLFWwindow;
using ::GLFWcursor;
using ::GLFWmonitor;

using ::GLFWvidmode;
using ::GLFWmonitor;
using ::GLFWmonitor;
using ::GLFWmonitor;
using ::GLFWmonitor;
using ::GLFWmonitor;
using ::GLFWmonitor;
using ::GLFWmonitor;

using ::glfwInit;
using ::glfwTerminate;
using ::glfwInitHint;
using ::glfwInitAllocator;
#if defined(VK_VERSION_1_0)
using ::glfwInitVulkanLoader;
#endif // VK_VERSION_1_0
using ::glfwGetVersion;
using ::glfwGetVersionString;
using ::glfwGetError;
using ::glfwSetErrorCallback;
using ::glfwGetPlatform;
using ::glfwPlatformSupported;
using ::glfwGetMonitors;
using ::glfwGetPrimaryMonitor;
using ::glfwGetMonitorPos;
using ::glfwGetMonitorWorkarea;
using ::glfwGetMonitorPhysicalSize;
using ::glfwGetMonitorContentScale;
using ::glfwGetMonitorName;
using ::glfwSetMonitorUserPointer;
using ::glfwGetMonitorUserPointer;
using ::glfwSetMonitorCallback;
using ::glfwGetVideoModes;
using ::glfwGetVideoMode;
using ::glfwSetGamma;
using ::glfwGetGammaRamp;
using ::glfwSetGammaRamp;
using ::glfwDefaultWindowHints;
using ::glfwWindowHint;
using ::glfwWindowHintString;
using ::glfwCreateWindow;
using ::glfwDestroyWindow;
using ::glfwWindowShouldClose;
using ::glfwSetWindowShouldClose;
using ::glfwGetWindowTitle;
using ::glfwSetWindowTitle;
using ::glfwSetWindowIcon;
using ::glfwGetWindowPos;
using ::glfwSetWindowPos;
using ::glfwGetWindowSize;
using ::glfwSetWindowSizeLimits;
using ::glfwSetWindowAspectRatio;
using ::glfwSetWindowSize;
using ::glfwGetFramebufferSize;
using ::glfwGetWindowFrameSize;
using ::glfwGetWindowContentScale;
using ::glfwGetWindowOpacity;
using ::glfwSetWindowOpacity;
using ::glfwIconifyWindow;
using ::glfwRestoreWindow;
using ::glfwMaximizeWindow;
using ::glfwShowWindow;
using ::glfwHideWindow;
using ::glfwFocusWindow;
using ::glfwRequestWindowAttention;
using ::glfwGetWindowMonitor;
using ::glfwSetWindowMonitor;
using ::glfwGetWindowAttrib;
using ::glfwSetWindowAttrib;
using ::glfwSetWindowUserPointer;
using ::glfwGetWindowUserPointer;
using ::glfwSetWindowPosCallback;
using ::glfwSetWindowSizeCallback;
using ::glfwSetWindowCloseCallback;
using ::glfwSetWindowRefreshCallback;
using ::glfwSetWindowFocusCallback;
using ::glfwSetWindowIconifyCallback;
using ::glfwSetWindowMaximizeCallback;
using ::glfwSetFramebufferSizeCallback;
using ::glfwSetWindowContentScaleCallback;
using ::glfwPollEvents;
using ::glfwWaitEvents;
using ::glfwWaitEventsTimeout;
using ::glfwPostEmptyEvent;
using ::glfwGetInputMode;
using ::glfwSetInputMode;
using ::glfwRawMouseMotionSupported;
using ::glfwGetKeyName;
using ::glfwGetKeyScancode;
using ::glfwGetKey;
using ::glfwGetMouseButton;
using ::glfwGetCursorPos;
using ::glfwSetCursorPos;
using ::glfwCreateCursor;
using ::glfwCreateStandardCursor;
using ::glfwDestroyCursor;
using ::glfwSetCursor;
using ::glfwSetKeyCallback;
using ::glfwSetCharCallback;
using ::glfwSetCharModsCallback;
using ::glfwSetMouseButtonCallback;
using ::glfwSetCursorPosCallback;
using ::glfwSetCursorEnterCallback;
using ::glfwSetScrollCallback;
using ::glfwSetDropCallback;
using ::glfwJoystickPresent;
using ::glfwGetJoystickAxes;
using ::glfwGetJoystickButtons;
using ::glfwGetJoystickHats;
using ::glfwGetJoystickName;
using ::glfwGetJoystickGUID;
using ::glfwSetJoystickUserPointer;
using ::glfwGetJoystickUserPointer;
using ::glfwJoystickIsGamepad;
using ::glfwSetJoystickCallback;
using ::glfwUpdateGamepadMappings;
using ::glfwGetGamepadName;
using ::glfwGetGamepadState;
using ::glfwSetClipboardString;
using ::glfwGetClipboardString;
using ::glfwGetTime;
using ::glfwSetTime;
using ::glfwGetTimerValue;
using ::glfwGetTimerFrequency;
using ::glfwMakeContextCurrent;
using ::glfwGetCurrentContext;
using ::glfwSwapBuffers;
using ::glfwSwapInterval;
using ::glfwExtensionSupported;
using ::glfwGetProcAddress;
using ::glfwVulkanSupported;
using ::glfwGetRequiredInstanceExtensions;
#if defined(VK_VERSION_1_0)
using ::glfwGetInstanceProcAddress;
using ::glfwGetPhysicalDevicePresentationSupport;
using ::glfwCreateWindowSurface;
#endif // VK_VERSION_1_0
}

#define PRAGMA(...) _Pragma(#__VA_ARGS__)
#define PUSH_MACRO(NAME) PRAGMA(push_macro(#NAME))
#define EXPORT_POP_MACRO(NAME) export inline constexpr auto NAME = PRAGMA(pop_macro(#NAME)) NAME;
// NOLINTEND(misc-unused-using-decls)

export
namespace GLFW {
inline constexpr auto VersionMajor = GLFW_VERSION_MAJOR;
inline constexpr auto VersionMinor = GLFW_VERSION_MINOR;
inline constexpr auto VersionRevision = GLFW_VERSION_REVISION;
inline constexpr auto True = GLFW_TRUE;
inline constexpr auto False = GLFW_FALSE;
inline constexpr auto Release = GLFW_RELEASE;
inline constexpr auto Press = GLFW_PRESS;
inline constexpr auto Repeat = GLFW_REPEAT;

inline constexpr auto Connected = GLFW_CONNECTED;
inline constexpr auto Disconnected = GLFW_DISCONNECTED;
inline constexpr auto JoystickHatButtons = GLFW_JOYSTICK_HAT_BUTTONS;
inline constexpr auto AnglePlatformType = GLFW_ANGLE_PLATFORM_TYPE;
inline constexpr auto Platform = GLFW_PLATFORM;
inline constexpr auto CocoaChdirResources = GLFW_COCOA_CHDIR_RESOURCES;
inline constexpr auto CocoaMenubar = GLFW_COCOA_MENUBAR;
inline constexpr auto X11XcbVulkanSurface = GLFW_X11_XCB_VULKAN_SURFACE;
inline constexpr auto WaylandLibdecor = GLFW_WAYLAND_LIBDECOR;
inline constexpr auto AnyPlatform = GLFW_ANY_PLATFORM;
inline constexpr auto PlatformWin32 = GLFW_PLATFORM_WIN32;
inline constexpr auto PlatformCocoa = GLFW_PLATFORM_COCOA;
inline constexpr auto PlatformWayland = GLFW_PLATFORM_WAYLAND;
inline constexpr auto PlatformX11 = GLFW_PLATFORM_X11;
inline constexpr auto PlatformNull = GLFW_PLATFORM_NULL;
inline constexpr auto DontCare = GLFW_DONT_CARE;

inline constexpr auto NoError = GLFW_NO_ERROR;
inline constexpr auto NotInitialized = GLFW_NOT_INITIALIZED;
inline constexpr auto NoCurrentContext = GLFW_NO_CURRENT_CONTEXT;
inline constexpr auto InvalidEnum = GLFW_INVALID_ENUM;
inline constexpr auto InvalidValue = GLFW_INVALID_VALUE;
inline constexpr auto OutOfMemory = GLFW_OUT_OF_MEMORY;
inline constexpr auto ApiUnavailable = GLFW_API_UNAVAILABLE;
inline constexpr auto VersionUnavailable = GLFW_VERSION_UNAVAILABLE;
inline constexpr auto PlatformError = GLFW_PLATFORM_ERROR;
inline constexpr auto FormatUnavailable = GLFW_FORMAT_UNAVAILABLE;
inline constexpr auto NoWindowContext = GLFW_NO_WINDOW_CONTEXT;
inline constexpr auto CursorUnavailable = GLFW_CURSOR_UNAVAILABLE;
inline constexpr auto FeatureUnavailable = GLFW_FEATURE_UNAVAILABLE;
inline constexpr auto FeatureUnimplemented = GLFW_FEATURE_UNIMPLEMENTED;
inline constexpr auto PlatformUnavailable = GLFW_PLATFORM_UNAVAILABLE;

// Window
inline constexpr auto Focused = GLFW_FOCUSED;
inline constexpr auto Iconified = GLFW_ICONIFIED;
inline constexpr auto Resizable = GLFW_RESIZABLE;
inline constexpr auto Visible = GLFW_VISIBLE;
inline constexpr auto Decorated = GLFW_DECORATED;
inline constexpr auto AutoIconify = GLFW_AUTO_ICONIFY;
inline constexpr auto Floating = GLFW_FLOATING;
inline constexpr auto Maximized = GLFW_MAXIMIZED;
inline constexpr auto CenterCursor = GLFW_CENTER_CURSOR;
inline constexpr auto TransparentFramebuffer = GLFW_TRANSPARENT_FRAMEBUFFER;
inline constexpr auto Hovered = GLFW_HOVERED;
inline constexpr auto FocusOnShow = GLFW_FOCUS_ON_SHOW;
inline constexpr auto MousePassthrough = GLFW_MOUSE_PASSTHROUGH;
inline constexpr auto PositionX = GLFW_POSITION_X;
inline constexpr auto PositionY = GLFW_POSITION_Y;
inline constexpr auto RedBits = GLFW_RED_BITS;
inline constexpr auto GreenBits = GLFW_GREEN_BITS;
inline constexpr auto BlueBits = GLFW_BLUE_BITS;
inline constexpr auto AlphaBits = GLFW_ALPHA_BITS;
inline constexpr auto DepthBits = GLFW_DEPTH_BITS;
inline constexpr auto StencilBits = GLFW_STENCIL_BITS;
inline constexpr auto AccumRedBits = GLFW_ACCUM_RED_BITS;
inline constexpr auto AccumGreenBits = GLFW_ACCUM_GREEN_BITS;
inline constexpr auto AccumBlueBits = GLFW_ACCUM_BLUE_BITS;
inline constexpr auto AccumAlphaBits = GLFW_ACCUM_ALPHA_BITS;
inline constexpr auto AuxBuffers = GLFW_AUX_BUFFERS;
inline constexpr auto Stereo = GLFW_STEREO;
inline constexpr auto Samples = GLFW_SAMPLES;
inline constexpr auto SrgbCapable = GLFW_SRGB_CAPABLE;
inline constexpr auto RefreshRate = GLFW_REFRESH_RATE;
inline constexpr auto DoubleBuffer = GLFW_DOUBLEBUFFER;
inline constexpr auto ClientApi = GLFW_CLIENT_API;
inline constexpr auto ContextVersionMajor = GLFW_CONTEXT_VERSION_MAJOR;
inline constexpr auto ContextVersionMinor = GLFW_CONTEXT_VERSION_MINOR;
inline constexpr auto ContextRevision = GLFW_CONTEXT_REVISION;
inline constexpr auto ContextRobustness = GLFW_CONTEXT_ROBUSTNESS;
inline constexpr auto OpenglForwardCompat = GLFW_OPENGL_FORWARD_COMPAT;
inline constexpr auto ContextDebug = GLFW_CONTEXT_DEBUG;
inline constexpr auto OpenglDebugContext = GLFW_OPENGL_DEBUG_CONTEXT;
inline constexpr auto OpenglProfile = GLFW_OPENGL_PROFILE;
inline constexpr auto ContextReleaseBehavior = GLFW_CONTEXT_RELEASE_BEHAVIOR;
inline constexpr auto ContextNoError = GLFW_CONTEXT_NO_ERROR;
inline constexpr auto ContextCreationApi = GLFW_CONTEXT_CREATION_API;
inline constexpr auto ScaleToMonitor = GLFW_SCALE_TO_MONITOR;
inline constexpr auto ScaleFramebuffer = GLFW_SCALE_FRAMEBUFFER;
inline constexpr auto CocoaRetinaFramebuffer = GLFW_COCOA_RETINA_FRAMEBUFFER;
inline constexpr auto CocoaFrameName = GLFW_COCOA_FRAME_NAME;
inline constexpr auto CocoaGraphicsSwitching = GLFW_COCOA_GRAPHICS_SWITCHING;
inline constexpr auto X11ClassName = GLFW_X11_CLASS_NAME;
inline constexpr auto X11InstanceName = GLFW_X11_INSTANCE_NAME;

inline constexpr auto Win32KeyboardMenu = GLFW_WIN32_KEYBOARD_MENU;
inline constexpr auto Win32ShowDefault = GLFW_WIN32_SHOWDEFAULT;
inline constexpr auto WaylandAppId = GLFW_WAYLAND_APP_ID;

inline constexpr auto NoApi = GLFW_NO_API;
inline constexpr auto OpenglApi = GLFW_OPENGL_API;
inline constexpr auto OpenglEsApi = GLFW_OPENGL_ES_API;
inline constexpr auto NoRobustness = GLFW_NO_ROBUSTNESS;
inline constexpr auto NoResetNotification = GLFW_NO_RESET_NOTIFICATION;
inline constexpr auto LoseContextOnReset = GLFW_LOSE_CONTEXT_ON_RESET;
inline constexpr auto OpenglAnyProfile = GLFW_OPENGL_ANY_PROFILE;
inline constexpr auto OpenglCoreProfile = GLFW_OPENGL_CORE_PROFILE;
inline constexpr auto OpenglCompatProfile = GLFW_OPENGL_COMPAT_PROFILE;
inline constexpr auto Cursor = GLFW_CURSOR;
inline constexpr auto StickyKeys = GLFW_STICKY_KEYS;
inline constexpr auto StickyMouseButton = GLFW_STICKY_MOUSE_BUTTONS;
inline constexpr auto LockKeyMods = GLFW_LOCK_KEY_MODS;
inline constexpr auto RawMouseMotion = GLFW_RAW_MOUSE_MOTION;
inline constexpr auto UnlimitedMouseButton = GLFW_UNLIMITED_MOUSE_BUTTONS;
inline constexpr auto CursorNormal = GLFW_CURSOR_NORMAL;
inline constexpr auto CursorHidden = GLFW_CURSOR_HIDDEN;
inline constexpr auto CursorDisabled = GLFW_CURSOR_DISABLED;
inline constexpr auto CursorCaptured = GLFW_CURSOR_CAPTURED;
inline constexpr auto AnyReleaseBehavior = GLFW_ANY_RELEASE_BEHAVIOR;
inline constexpr auto ReleaseBehaviorFlush = GLFW_RELEASE_BEHAVIOR_FLUSH;
inline constexpr auto ReleaseBehaviorNone = GLFW_RELEASE_BEHAVIOR_NONE;
inline constexpr auto NativeContextApi = GLFW_NATIVE_CONTEXT_API;
inline constexpr auto EglContextApi = GLFW_EGL_CONTEXT_API;
inline constexpr auto OsmesaContextApi = GLFW_OSMESA_CONTEXT_API;
inline constexpr auto AnglePlatformTypeNone = GLFW_ANGLE_PLATFORM_TYPE_NONE;
inline constexpr auto AnglePlatformTypeOpengl = GLFW_ANGLE_PLATFORM_TYPE_OPENGL;
inline constexpr auto AnglePlatformTypeOpenGles = GLFW_ANGLE_PLATFORM_TYPE_OPENGLES;
inline constexpr auto AnglePlatformTypeD3d9 = GLFW_ANGLE_PLATFORM_TYPE_D3D9;
inline constexpr auto AnglePlatformTypeD3d11 = GLFW_ANGLE_PLATFORM_TYPE_D3D11;
inline constexpr auto AnglePlatformTypeVulkan = GLFW_ANGLE_PLATFORM_TYPE_VULKAN;
inline constexpr auto AnglePlatformTypeMetal = GLFW_ANGLE_PLATFORM_TYPE_METAL;
inline constexpr auto WaylandPreferLibdecor = GLFW_WAYLAND_PREFER_LIBDECOR;
inline constexpr auto WaylandDisableLibdecor = GLFW_WAYLAND_DISABLE_LIBDECOR;
inline constexpr auto AnyPosition = GLFW_ANY_POSITION;

class MouseButton {
public:
    enum Value : unsigned int {
        Button1 = GLFW_MOUSE_BUTTON_1,
        Button2 = GLFW_MOUSE_BUTTON_2,
        Button3 = GLFW_MOUSE_BUTTON_3,
        Button4 = GLFW_MOUSE_BUTTON_4,
        Button5 = GLFW_MOUSE_BUTTON_5,
        Button6 = GLFW_MOUSE_BUTTON_6,
        Button7 = GLFW_MOUSE_BUTTON_7,
        Button8 = GLFW_MOUSE_BUTTON_8,
        Last    = GLFW_MOUSE_BUTTON_LAST,
        Left    = GLFW_MOUSE_BUTTON_LEFT,
        Right   = GLFW_MOUSE_BUTTON_RIGHT,
        Middle  = GLFW_MOUSE_BUTTON_MIDDLE,
    };

    constexpr explicit MouseButton(Value value) : value_(value) {}

    constexpr operator unsigned() const {
        return static_cast<unsigned>(value_);
    }

private:
    Value value_;
};


/* Printable keys */
enum class Key {
	Unknown = GLFW_KEY_UNKNOWN,
	Space = GLFW_KEY_SPACE,
	Apostrophe = GLFW_KEY_APOSTROPHE,
	Comma = GLFW_KEY_COMMA,
	Minus = GLFW_KEY_MINUS,
	Period = GLFW_KEY_PERIOD,
	Slash = GLFW_KEY_SLASH,
	_0 = GLFW_KEY_0,
	_1 = GLFW_KEY_1,
	_2 = GLFW_KEY_2,
	_3 = GLFW_KEY_3,
	_4 = GLFW_KEY_4,
	_5 = GLFW_KEY_5,
	_6 = GLFW_KEY_6,
	_7 = GLFW_KEY_7,
	_8 = GLFW_KEY_8,
	_9 = GLFW_KEY_9,
	Semicolon = GLFW_KEY_SEMICOLON,
	Equal = GLFW_KEY_EQUAL,
	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,
	LeftBracket = GLFW_KEY_LEFT_BRACKET,
	Backslash = GLFW_KEY_BACKSLASH,
	RightBracket = GLFW_KEY_RIGHT_BRACKET,
	GraveAccent = GLFW_KEY_GRAVE_ACCENT,
	World1 = GLFW_KEY_WORLD_1,
	World2 = GLFW_KEY_WORLD_2,

	Escape = GLFW_KEY_ESCAPE,
	Enter = GLFW_KEY_ENTER,
	Tab = GLFW_KEY_TAB,
	Backspace = GLFW_KEY_BACKSPACE,
	Insert = GLFW_KEY_INSERT,
	Delete = GLFW_KEY_DELETE,
	Right = GLFW_KEY_RIGHT,
	Left = GLFW_KEY_LEFT,
	Down = GLFW_KEY_DOWN,
	Up = GLFW_KEY_UP,
	PageUp = GLFW_KEY_PAGE_UP,
	PageDown = GLFW_KEY_PAGE_DOWN,
	Home = GLFW_KEY_HOME,
	End = GLFW_KEY_END,
	CapsLock = GLFW_KEY_CAPS_LOCK,
	ScrollLock = GLFW_KEY_SCROLL_LOCK,
	NumLock = GLFW_KEY_NUM_LOCK,
	PrintScreen = GLFW_KEY_PRINT_SCREEN,
	Pause = GLFW_KEY_PAUSE,
	F1 = GLFW_KEY_F1,
	F2 = GLFW_KEY_F2,
	F3 = GLFW_KEY_F3,
	F4 = GLFW_KEY_F4,
	F5 = GLFW_KEY_F5,
	F6 = GLFW_KEY_F6,
	F7 = GLFW_KEY_F7,
	F8 = GLFW_KEY_F8,
	F9 = GLFW_KEY_F9,
	F10 = GLFW_KEY_F10,
	F11 = GLFW_KEY_F11,
	F12 = GLFW_KEY_F12,
	F13 = GLFW_KEY_F13,
	F14 = GLFW_KEY_F14,
	F15 = GLFW_KEY_F15,
	F16 = GLFW_KEY_F16,
	F17 = GLFW_KEY_F17,
	F18 = GLFW_KEY_F18,
	F19 = GLFW_KEY_F19,
	F20 = GLFW_KEY_F20,
	F21 = GLFW_KEY_F21,
	F22 = GLFW_KEY_F22,
	F23 = GLFW_KEY_F23,
	F24 = GLFW_KEY_F24,
	F25 = GLFW_KEY_F25,
	KP0 = GLFW_KEY_KP_0,
	KP1 = GLFW_KEY_KP_1,
	KP2 = GLFW_KEY_KP_2,
	KP3 = GLFW_KEY_KP_3,
	KP4 = GLFW_KEY_KP_4,
	KP5 = GLFW_KEY_KP_5,
	KP6 = GLFW_KEY_KP_6,
	KP7 = GLFW_KEY_KP_7,
	KP8 = GLFW_KEY_KP_8,
	KP9 = GLFW_KEY_KP_9,
	KPDecimal = GLFW_KEY_KP_DECIMAL,
	KPDivide = GLFW_KEY_KP_DIVIDE,
	KPMultiply = GLFW_KEY_KP_MULTIPLY,
	KPSubtract = GLFW_KEY_KP_SUBTRACT,
	KPAdd = GLFW_KEY_KP_ADD,
	KPEnter = GLFW_KEY_KP_ENTER,
	KPEqual = GLFW_KEY_KP_EQUAL,
	LeftShift = GLFW_KEY_LEFT_SHIFT,
	LeftControl = GLFW_KEY_LEFT_CONTROL,
	LeftAlt = GLFW_KEY_LEFT_ALT,
	LeftSuper = GLFW_KEY_LEFT_SUPER,
	RightShift = GLFW_KEY_RIGHT_SHIFT,
	RightControl = GLFW_KEY_RIGHT_CONTROL,
	RightAlt = GLFW_KEY_RIGHT_ALT,
	RightSuper = GLFW_KEY_RIGHT_SUPER,
	Menu = GLFW_KEY_MENU,
	Last = GLFW_KEY_LAST
};

enum class Mod : unsigned int {
    Shift = GLFW_MOD_SHIFT,
    Control = GLFW_MOD_CONTROL,
    Alt = GLFW_MOD_ALT,
    Super = GLFW_MOD_SUPER,
    CapsLock = GLFW_MOD_CAPS_LOCK,
    NumLock = GLFW_MOD_NUM_LOCK,
};
/* 
using ModFlags = std::underlying_type_t<Mod>;
static_assert(std::is_unsigned_v<ModFlags>);

inline ModFlags operator|(Mod lhs, Mod rhs) {
    return static_cast<ModFlags>(lhs) | static_cast<ModFlags>(rhs);
}
 */
enum class Hat : unsigned int {
	Centered = GLFW_HAT_CENTERED,
	Up = GLFW_HAT_UP,
	Right = GLFW_HAT_RIGHT,
	Down = GLFW_HAT_DOWN,
	Left = GLFW_HAT_LEFT,
	RightUp = GLFW_HAT_RIGHT_UP,
	RightDown = GLFW_HAT_RIGHT_DOWN,
	LeftUp = GLFW_HAT_LEFT_UP,
	LeftDown = GLFW_HAT_LEFT_DOWN,
};

enum class Cursor : unsigned int {
	Arrow = GLFW_ARROW_CURSOR,
	Ibeam = GLFW_IBEAM_CURSOR,
	Crosshair = GLFW_CROSSHAIR_CURSOR,
	PointingHand = GLFW_POINTING_HAND_CURSOR,
	ResizeEW = GLFW_RESIZE_EW_CURSOR,
	ResizeNS = GLFW_RESIZE_NS_CURSOR,
	ResizeNWSE = GLFW_RESIZE_NWSE_CURSOR,
	ResizeNESW = GLFW_RESIZE_NESW_CURSOR,
	ResizeAll = GLFW_RESIZE_ALL_CURSOR,
	NotAllowed = GLFW_NOT_ALLOWED_CURSOR,
	HResize = GLFW_HRESIZE_CURSOR,
	VResize = GLFW_VRESIZE_CURSOR,
	Hand = GLFW_HAND_CURSOR,
};

enum class Joystick : unsigned int {
	_1 = GLFW_JOYSTICK_1,
	_2 = GLFW_JOYSTICK_2,
	_3 = GLFW_JOYSTICK_3,
	_4 = GLFW_JOYSTICK_4,
	_5 = GLFW_JOYSTICK_5,
	_6 = GLFW_JOYSTICK_6,
	_7 = GLFW_JOYSTICK_7,
	_8 = GLFW_JOYSTICK_8,
	_9 = GLFW_JOYSTICK_9,
	_10 = GLFW_JOYSTICK_10,
	_11 = GLFW_JOYSTICK_11,
	_12 = GLFW_JOYSTICK_12,
	_13 = GLFW_JOYSTICK_13,
	_14 = GLFW_JOYSTICK_14,
	_15 = GLFW_JOYSTICK_15,
	_16 = GLFW_JOYSTICK_16,
	Last = GLFW_JOYSTICK_LAST,
};

enum class GamepadButton: unsigned int {
	A           = GLFW_GAMEPAD_BUTTON_A,
	B           = GLFW_GAMEPAD_BUTTON_B,
	X           = GLFW_GAMEPAD_BUTTON_X,
	Y           = GLFW_GAMEPAD_BUTTON_Y,
	LeftBumper  = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
	RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
	Back        = GLFW_GAMEPAD_BUTTON_BACK,
	Start       = GLFW_GAMEPAD_BUTTON_START,
	Guide       = GLFW_GAMEPAD_BUTTON_GUIDE,
	LeftThumb   = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
	RightThumb  = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
	DpadUp      = GLFW_GAMEPAD_BUTTON_DPAD_UP,
	DpadRight   = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
	DpadDown    = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
	DpadLeft    = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
	Last        = GLFW_GAMEPAD_BUTTON_LAST,
	Cross       = GLFW_GAMEPAD_BUTTON_CROSS,
	Circle      = GLFW_GAMEPAD_BUTTON_CIRCLE,
	Square      = GLFW_GAMEPAD_BUTTON_SQUARE,
	Triangle    = GLFW_GAMEPAD_BUTTON_TRIANGLE,
};


enum class GamepadAxis : unsigned int {
	LeftX        = GLFW_GAMEPAD_AXIS_LEFT_X,
	LeftY        = GLFW_GAMEPAD_AXIS_LEFT_Y,
	RightX       = GLFW_GAMEPAD_AXIS_RIGHT_X,
	RightY       = GLFW_GAMEPAD_AXIS_RIGHT_Y,
	LeftTrigger  = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
	RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
	Last         = GLFW_GAMEPAD_AXIS_LAST,
};

} // namespace GLFW 


