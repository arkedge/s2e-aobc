#ifndef S2E_AOBC_COMPONENT_POWER_POWER_CONTROLLER_HPP_
#define S2E_AOBC_COMPONENT_POWER_POWER_CONTROLLER_HPP_

#include <components/base/gpio_connection_with_obc.hpp>
#include <components/real/power/power_control_unit.hpp>

class PowerController : public PowerControlUnit, public GpioConnectionWithObc {
 public:
  PowerController(PowerControlUnit pcu,
                  const std::vector<int> gpio_ports,              // GPIOのポート番号リスト
                  const std::vector<double> output_voltage_list,  // 出力電圧リスト
                  OnBoardComputer *obc);
  // Override: PCU functions
  void MainRoutine(int count) override;
  std::string GetLogHeader() const override;

 private:
  std::vector<double> output_voltage_list_;
};
#endif  // S2E_AOBC_COMPONENT_POWER_POWER_CONTROLLER_HPP_
