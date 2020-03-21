#pragma once

#include "Delegate.h"
#include "Math/Vector2.h"

class nApplication
{
public:
	virtual ~nApplication() = default;

	enum class GamepadButtonState : uint8
	{
		PRESSED, RELEASED
	};
protected:
	Delegate<void(float, float)> onRightTriggerChanged;
	Delegate<void(float, float)> onLeftTriggerChanged;
	Delegate<void(const Vector2&, const Vector2&)> onRightStickMove;
	Delegate<void(const Vector2&, const Vector2&)> onLeftStickMove;
	Delegate<void(GamepadButtonState)> onRightHatChanged;
	Delegate<void(GamepadButtonState)> onLeftHatChanged;
	Delegate<void(GamepadButtonState)> onRightStickButtonChanged;
	Delegate<void(GamepadButtonState)> onLeftStickButtonChanged;
	Delegate<void(GamepadButtonState)> onTopFaceButtonChanged;
	Delegate<void(GamepadButtonState)> onRightFaceButtonChanged;
	Delegate<void(GamepadButtonState)> onBottomFaceButtonChanged;
	Delegate<void(GamepadButtonState)> onLeftFaceButtonChanged;
	Delegate<void(GamepadButtonState)> onTopDPadButtonChanged;
	Delegate<void(GamepadButtonState)> onRightDPadButtonChanged;
	Delegate<void(GamepadButtonState)> onBottomDPadButtonChanged;
	Delegate<void(GamepadButtonState)> onLeftDPadButtonChanged;
	Delegate<void(GamepadButtonState)> onStartButtonChanged;
	Delegate<void(GamepadButtonState)> onBackButtonChanged;
	Delegate<void(uint8)> onControllerConnected;
	Delegate<void(uint8)> onControllerDisconnected;

public:
	struct ApplicationCreateInfo
	{
	};
	explicit nApplication(const ApplicationCreateInfo& applicationCreateInfo);

	virtual void Update() = 0;
	
	virtual void CheckForNewControllers() = 0;
	
	virtual void Close() = 0;
};
