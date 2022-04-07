
extern pfVec3	CameraSwingVeloc;
extern char		KeysDown[256];
extern char		KeysPressed[256];
extern char		KeysReleased[256];

extern float	JoystickVals[4];
extern uint32	JoystickButtons;
extern uint32	JoystickButtonsPrevious;

#define BUTTON_X		0x00000004
#define BUTTON_TRI		0x00000001
#define BUTTON_SQUARE	0x00000008
#define BUTTON_CIRCLE	0x00000002

#define BUTTON_L1		0x00000040
#define BUTTON_L2		0x00000010
#define BUTTON_L3		0x00000200
#define BUTTON_R1		0x00000080
#define BUTTON_R2		0x00000020
#define BUTTON_R3		0x00000400

#define BUTTON_UP		0x00001000
#define BUTTON_DOWN		0x00004000
#define BUTTON_LEFT		0x00008000
#define BUTTON_RIGHT	0x00002000

#define BUTTON_START	0x00000100
#define BUTTON_SELECT	0x00000800

#define CONTROL_BUTTON_THRUST	(BUTTON_X | BUTTON_TRI | BUTTON_R2)
#define CONTROL_BUTTON_BRAKES	(BUTTON_L3 | BUTTON_R1)
#define CONTROL_BUTTON_FIRE1	BUTTON_SQUARE
#define CONTROL_BUTTON_FIRE2	BUTTON_CIRCLE

#define ButtonDown(_x)		((JoystickButtons & (_x)) != 0)
#define ButtonPressed(_x)	(((JoystickButtons & (_x)) != 0) && ((JoystickButtonsPrevious & (_x)) == 0))
#define ButtonReleased(_x)	(((JoystickButtons & (_x)) == 0) && ((JoystickButtonsPrevious & (_x)) != 0))

void HandleKeyboard(unsigned char key, int x, int y);
void HandleMouse(int button, int state, int x, int y);
void HandleMouseMotion(int x, int y);
void ControlPressKey(int key, bool down);
void ControlJoystickData(uint32 buttons, float x, float y, float z, float r);
void CheckKeyDiff(int key);

