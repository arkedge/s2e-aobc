#pragma once
#include "MagSensor.h"
#include "ObcI2cTargetCommunicationBase.h"

/* References
Manual: https://www.pnicorp.com/wp-content/uploads/RM3100-Testing-Boards-User-Manual-r04-1.pdf
HowToUse: NA
Note: Functions not used in the project are not implemented
*/

class RM3100: public MagSensor, public ObcI2cTargetCommunicationBase
{
public:
  RM3100(
    MagSensor mag_sensor,
    const int sils_port_id,
    const unsigned int hils_port_id,
    const unsigned char i2c_addr,
    OBC* obc,
    HilsPortManager* hils_port_manager
  );

  // Override: MagSensor functions
  void MainRoutine(int count) override;
  std::string GetLogHeader() const override;

private:
  // Mode setting
  unsigned char mode_ = 1;  //0: CMM, others: Polling  
  const unsigned char kModeSetRegId    = 0x01;
  const unsigned char kCmmModeSettings = 0x7d;

  double lsb2nT_ = 13.0;
  const unsigned int kStoredFrameSize = 3;

  // TLM
  int32_t ConvertMag2Tlm(double mag);
  int GenerateTelemetry();
};