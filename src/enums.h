#ifndef enumsH
#define enumsH
enum StartTimerOption
{
  WhenPumpIsOn,
  WhenPumpIsOff
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