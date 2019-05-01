#ifndef enumsH
#define enumsH

enum AutoWateringState
{
  SelectWaitDaysState,
  SelectWaitHoursState,
  SelectWaitMinutesState,
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

enum PumpMode{
  Normal,
  ForcedlyStarted,
  ForcedlyStartedWithTimer
};

#endif