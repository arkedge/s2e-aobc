#include "RM3100.h"

#include <library/utilities/macros.hpp>

RM3100::RM3100(Magnetometer mag_sensor, const int sils_port_id, const unsigned int hils_port_id, const unsigned char i2c_addr, OnBoardComputer *obc,
               HilsPortManager *hils_port_manager)
    : Magnetometer(mag_sensor), I2cTargetCommunicationWithObc(sils_port_id, hils_port_id, i2c_addr, obc, hils_port_manager) {}

void RM3100::MainRoutine(int count) {
  UNUSED(count);
  unsigned char mode_data[2] = {0, 0};
  ReadCommand(mode_data, 2);
  if (mode_data[0] == kModeSetRegId && mode_data[1] == kCmmModeSettings) mode_ = 0;

  if (mode_ == 0) {  // CMM
    // Convert frame
    magnetic_field_c_nT_ = quaternion_b2c_.FrameConversion(geomagnetic_field_->GetGeomagneticField_b_nT());
    magnetic_field_c_nT_ = Measure(magnetic_field_c_nT_);
    GenerateTelemetry();
  }

  int cmd_size = ReceiveCommand();
  if (cmd_size != 1) return;  // length == 1 means setting of read register address

  //  これ以降はHILS用に事前にテレメトリを溜めておく
  const int kTlmSize = 9;  //(24bit = 3 Byte) * 3 axis
  StoreTelemetry(kStoredFrameSize, kTlmSize);
  return;
}

std::string RM3100::GetLogHeader() const {
  std::string str_tmp = "";
  std::string MSSection = "RM3100";
  str_tmp += WriteVector(MSSection, "c", "nT", 3);

  return str_tmp;
}

int RM3100::GenerateTelemetry() {
  const int kTlmSize = 9;  //(24bit = 3 Byte) * 3 axis
  const int kByte2Bit = 8;
  unsigned char tlm[kTlmSize] = {1, 2, 3, 0, 0, 0, 0, 0, 4};
  int mag_c_tlm[kMagnetometerDimension] = {0, 0, 0};
  for (size_t i = 0; i < kMagnetometerDimension; i++) {
    mag_c_tlm[i] = ConvertMag2Tlm(magnetic_field_c_nT_[i]);
    for (int j = 0; j < 3; j++) {
      tlm[i * 3 + j] = (unsigned char)(mag_c_tlm[i] >> kByte2Bit * (3 - j - 1));
    }
  }
  WriteRegister(0x24, tlm, kTlmSize);

  return kTlmSize;
}

int32_t RM3100::ConvertMag2Tlm(double mag) {
  int32_t mag_c_bit = (int32_t)(mag / lsb2nT_);

  // Limits
  int32_t upper_limit = 0x007FFFFF;        // Signed 24bit max value
  int32_t lower_limit = -upper_limit - 1;  // Signed 24bit min value
  mag_c_bit = (std::min)(upper_limit, mag_c_bit);
  mag_c_bit = (std::max)(lower_limit, mag_c_bit);
  // 24 bit
  if (mag_c_bit >= 0) {
    mag_c_bit = mag_c_bit & upper_limit;
  } else {
    mag_c_bit = (mag_c_bit & upper_limit) | (upper_limit + 1);
  }
  return mag_c_bit;
}
