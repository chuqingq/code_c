#include <termio.h>
#include <stdio.h>

// 初始化键盘事件
static void initkeyboard()
{
	struct termios new_settings;
	struct termios stored_settings;
	tcgetattr(0,&stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ICANON);
	new_settings.c_lflag&=~ECHO;
	new_settings.c_cc[VTIME] = 0;
	tcgetattr(0,&stored_settings);
	new_settings.c_cc[VMIN] = 1;
	tcsetattr(0,TCSANOW,&new_settings);
}

// 执行意图
static void doaction(int target_speed, int wheel_angle) {
	// TODO 参数转换。可能还需要调整档位
}

int main(){
	// 按键常量
	const int kKeyW = 119;
	const int kKeyS = 115;
	const int kKeyA = 97;
	const int kKeyD = 100;
	const int kKeyUp = 65;
	const int kKeyDown = 66;
	const int kKeyLeft = 68;
	const int kKeyRight = 67;

	// 目标速度
	int target_speed = 0;
	const int kTargetSpeedMax = 5;
	const int kTargetSpeedMin = -5;

	// 转向角度
	int wheel_angle = 0;
	const int kWheelAngleMax = 5;
	const int kWheelAngleMin = -5;

	// 初始化键盘事件
	initkeyboard();

	int ch;
	while (1) {
		// 通过control+c退出
		ch = getchar();
		// printf("pressed: %d\n", ch);
		switch (ch) {
		// 上
		case kKeyW:
		case kKeyUp:
			if (target_speed < kTargetSpeedMax) target_speed += 1;
			break;
		// 下
		case kKeyS:
		case kKeyDown:
			if (target_speed > kTargetSpeedMin) target_speed -= 1;
			break;
		// 左
		case kKeyA:
		case kKeyLeft:
			if (wheel_angle > kWheelAngleMin) wheel_angle -= 1;
			break;
		// 右
		case kKeyD:
		case kKeyRight:
			if (wheel_angle < kWheelAngleMax) wheel_angle += 1;
			break;
		default: continue;
		}
		printf("speed: %d, angle: %d\n", target_speed, wheel_angle);
		doaction(target_speed, wheel_angle);
	}

	return 0;
}