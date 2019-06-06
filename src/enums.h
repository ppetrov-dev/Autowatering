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

enum RelayType
{
  LowLevel = 0, //low signal when start
  HighLevel = 1 //high signal when start
};

#endif