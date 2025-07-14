#pragma once
#include "SinglePortModule.h"
#include "input/InputBroker.h"

class DetectionSensorModule : public SinglePortModule, private concurrency::OSThread
{
  public:
    DetectionSensorModule();
#ifdef MESHTASTIC_EXCLUDE_SCREEN
    int handleInputEvent(const InputEvent *event);
#endif

  protected:
    virtual int32_t runOnce() override;

  private:
    bool firstTime = true;
    uint32_t lastSentToMesh = 0;
    bool wasDetected = false;
    void sendDetectionMessage();
    void sendCurrentStateMessage(bool state);
    bool hasDetectionEvent();
#ifdef MESHTASTIC_EXCLUDE_SCREEN
    CallbackObserver<DetectionSensorModule, const InputEvent *> inputObserver =
        CallbackObserver<DetectionSensorModule, const InputEvent *>(this, &DetectionSensorModule::handleInputEvent);
#endif
};

extern DetectionSensorModule *detectionSensorModule;