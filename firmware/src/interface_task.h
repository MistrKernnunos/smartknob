#pragma once

// #include <AceButton.h>
#include <Arduino.h>

// #include "StepEncoder.h"
#include "USBHID.h"
#include "display_task.h"
#include "logger.h"
#include "motor_task.h"
#include "serial/serial_protocol_plaintext.h"
#include "serial/serial_protocol_protobuf.h"
#include "serial/uart_stream.h"
#include "task.h"

static const uint8_t report_descriptor[] = { TUD_HID_REPORT_DESC_GENERIC_INOUT(
  64) };
class HIDRaw : public USBHIDDevice
{
  USBHID hid;
  MotorTask& motor_task;
  Logger& logger;
  PB_SmartKnobState old_state;

  static constexpr uint8_t SWITCH_ID = 3;
  static constexpr uint8_t ENCODER_ID = 2;
  static constexpr uint8_t BUTTON_ID = 1;

public:
  void begin(void);
  void end(void);
  HIDRaw(MotorTask& task, Logger& logger);

  void send(uint8_t* data, size_t len);
  void sendEncoderUpdate(uint8_t id, int8_t diff);

  void StateUpdate(PB_SmartKnobState state);

  uint16_t _onGetDescriptor(uint8_t* buffer);
  void _onOutput(uint8_t report_id, const uint8_t* buffer, uint16_t len);
};
class InterfaceTask
  : public Task<InterfaceTask>
  , public Logger
{
  friend class Task<InterfaceTask>; // Allow base Task to invoke protected run()

public:
  InterfaceTask(const uint8_t task_core,
                MotorTask& motor_task,
                DisplayTask* display_task);
  virtual ~InterfaceTask(){};

  void log(const char* msg) override;

protected:
  void run();

private:
  HIDRaw hid;
  // StepEncoder enc;

  UartStream stream_;
  MotorTask& motor_task_;
  DisplayTask* display_task_;
  char buf_[64];

  int current_config_ = 0;
  int64_t last_enc_val = 0;

  QueueHandle_t log_queue_;
  QueueHandle_t knob_state_queue_;
  SerialProtocolPlaintext plaintext_protocol_;
  SerialProtocolProtobuf proto_protocol_;

  void changeConfig(bool next);
  void updateHardware();
};
