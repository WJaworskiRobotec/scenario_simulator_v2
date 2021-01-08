// Copyright 2015-2020 Tier IV, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SIMULATION_API__BEHAVIOR__VEHICLE__FOLLOW_LANE_SEQUENCE__STOP_AT_CROSSING_ENTITY_ACTION_HPP_
#define SIMULATION_API__BEHAVIOR__VEHICLE__FOLLOW_LANE_SEQUENCE__STOP_AT_CROSSING_ENTITY_ACTION_HPP_

#include <simulation_api/entity/entity_base.hpp>
#include <simulation_api/behavior/vehicle/vehicle_action_node.hpp>

#include <openscenario_msgs/msg/entity_trajectory.hpp>

#include <string>
#include <vector>

namespace entity_behavior
{
namespace vehicle
{
namespace follow_lane_sequence
{
class StopAtCrossingEntityAction : public entity_behavior::VehicleActionNode
{
public:
  StopAtCrossingEntityAction(const std::string & name, const BT::NodeConfiguration & config);
  BT::NodeStatus tick() override;
  static BT::PortsList providedPorts()
  {
    BT::PortsList ports = {};
    BT::PortsList parent_ports = entity_behavior::VehicleActionNode::providedPorts();
    for (const auto & parent_port : parent_ports) {
      ports.emplace(parent_port.first, parent_port.second);
    }
    return ports;
  }
  boost::optional<double> calculateTargetSpeed(
    const std::vector<std::int64_t> & following_lanelets,
    double current_velocity);
  const openscenario_msgs::msg::WaypointsArray calculateWaypoints() override;
  const boost::optional<openscenario_msgs::msg::Obstacle> calculateObstacle(
    const openscenario_msgs::msg::WaypointsArray & waypoints) override;

private:
  boost::optional<double> distance_to_stop_target_;
};
}  // namespace follow_lane_sequence
}  // namespace vehicle
}  // namespace entity_behavior

#endif  // SIMULATION_API__BEHAVIOR__VEHICLE__FOLLOW_LANE_SEQUENCE__STOP_AT_CROSSING_ENTITY_ACTION_HPP_
