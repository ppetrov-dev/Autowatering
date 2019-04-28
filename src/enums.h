#ifndef enumsH
#define enumsH
enum StartTimerOption
{
  WhenPumpIsOn,
  WhenPumpIsOff
};

enum LcdCursorPosition
{
  SelectPauseDays,
  SelectPauseHours,
  SelectPauseMinutes,
  SelectWorkHours,
  SelectWorkMinutes,
  SelectWorkSeconds,
  SelectPump,
  SelectSettings,
  SelectBack,
};

enum AutoPumpState
{
  SelectPauseDaysState,
  SelectPauseHoursState,
  SelectPauseMinutesState,
  SelectWorkHoursState,
  SelectWorkMinutesState,
  SelectWorkSecondsState,
  SelectPumpState,
  SelectSettingsState,
  SelectBackState,
};

enum Command{
  EncoderLeftTurnCommand,
  EncoderRightTurnCommand,
  EncoderClickCommand,
  EncoderHoldLeftTurnCommand,
  EncoderHoldRightTurnCommand,
};

#endif