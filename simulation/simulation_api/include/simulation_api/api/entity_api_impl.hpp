// Copyright 2015-2020 Autoware Foundation. All rights reserved.
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

#ifndef SIMULATION_API__API__ENTITY_API_IMPL_HPP_
#define SIMULATION_API__API__ENTITY_API_IMPL_HPP_

#include <simulation_api/api/api_impl_base.hpp>
#include <simulation_api/entity/vehicle_entity.hpp>
#include <simulation_api/entity/entity_status.hpp>
#include <simulation_api/entity/vehicle_parameter.hpp>
#include <simulation_api/entity/entity_manager.hpp>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <xmlrpcpp/XmlRpcClient.h>
#include <xmlrpcpp/XmlRpcValue.h>
#include <xmlrpcpp/XmlRpcException.h>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

#include <memory>
#include <limits>
#include <string>

namespace scenario_simulator
{
class EntityAPIImpl : public ApiImplBase
{
  using ApiImplBase::entity_manager_ptr_;
  using ApiImplBase::client_ptr_;

public:
  EntityAPIImpl(
    std::shared_ptr<XmlRpc::XmlRpcClient> client_ptr,
    std::shared_ptr<simulation_api::entity::EntityManager> entity_manager_ptr)
  : ApiImplBase(client_ptr, entity_manager_ptr) {}
  bool spawn(
    bool is_ego, std::string name,
    std::string catalog_xml,
    simulation_api::entity::EntityStatus status)
  {
    XmlRpc::XmlRpcValue value, status_value;
    status_value = toValue(name, status);
    value[0][0]["methodName"] = "spawn_entity";
    value[0][0]["params"] = status_value;
    value[0][0]["params"]["entity/is_ego"] = is_ego;
    value[0][0]["params"]["entity/catalog_xml"] = catalog_xml;
    pugi::xml_document catalog_xml_doc;
    catalog_xml_doc.load_string(catalog_xml.c_str());
    pugi::xml_node vehicle_node = catalog_xml_doc.child("Vehicle");
    // catalog_xml_doc.has("Vehicle");
    if (vehicle_node != NULL) {
      if (is_ego) {
        simulation_api::entity::EgoEntity ego(name, status, catalog_xml_doc);
        if (!entity_manager_ptr_->spawnEntity(ego)) {
          return false;
        }
      } else {
        simulation_api::entity::VehicleEntity npc(name, status, catalog_xml_doc);
        if (!entity_manager_ptr_->spawnEntity(npc)) {
          return false;
        }
      }
    }
    pugi::xml_node pedestrian_node = catalog_xml_doc.child("Pedestrian");
    if (pedestrian_node != NULL) {
      simulation_api::entity::PedestrianEntity pedestrian(name, status, catalog_xml_doc);
      if (!entity_manager_ptr_->spawnEntity(pedestrian)) {
        return false;
      }
    }

    XmlRpc::XmlRpcValue result;
    try {
      client_ptr_->execute("system.multicall", value, result);
    } catch (XmlRpc::XmlRpcException e) {
      throw XmlRpcRuntimeError(e.getMessage().c_str(), e.getCode());
    }
    return result[0][0]["success"];
  }
  bool spawn(
    bool is_ego, std::string name,
    simulation_api::entity::VehicleParameters params,
    simulation_api::entity::EntityStatus status)
  {
    return spawn(is_ego, name, params.toXml(), status);
  }
  bool spawn(
    bool is_ego, std::string name,
    simulation_api::entity::PedestrianParameters params,
    simulation_api::entity::EntityStatus status)
  {
    return spawn(is_ego, name, params.toXml(), status);
  }
  bool spawn(
    bool is_ego, std::string name,
    std::string catalog_xml)
  {
    XmlRpc::XmlRpcValue value;
    value[0][0]["methodName"] = "spawn_entity";
    value[0][0]["params"]["entity/is_ego"] = is_ego;
    value[0][0]["params"]["entity/catalog_xml"] = catalog_xml;

    pugi::xml_document catalog_xml_doc;
    catalog_xml_doc.load_string(catalog_xml.c_str());
    pugi::xml_node vehicle_node = catalog_xml_doc.child("Vehicle");
    // catalog_xml_doc.has("Vehicle");
    if (vehicle_node != NULL) {
      if (is_ego) {
        simulation_api::entity::EgoEntity ego(name, catalog_xml_doc);
        if (!entity_manager_ptr_->spawnEntity(ego)) {
          return false;
        }
      } else {
        simulation_api::entity::VehicleEntity npc(name, catalog_xml_doc);
        if (!entity_manager_ptr_->spawnEntity(npc)) {
          return false;
        }
      }
    }
    pugi::xml_node pedestrian_node = catalog_xml_doc.child("Pedestrian");
    if (pedestrian_node != NULL) {
      simulation_api::entity::PedestrianEntity pedestrian(name, catalog_xml_doc);
      if (!entity_manager_ptr_->spawnEntity(pedestrian)) {
        return false;
      }
    }

    XmlRpc::XmlRpcValue result;
    try {
      client_ptr_->execute("system.multicall", value, result);
    } catch (XmlRpc::XmlRpcException e) {
      throw XmlRpcRuntimeError(e.getMessage().c_str(), e.getCode());
    }
    return result[0][0]["success"];
  }
  bool spawn(
    bool is_ego, std::string name,
    simulation_api::entity::VehicleParameters params)
  {
    return spawn(is_ego, name, params.toXml());
  }
  bool spawn(
    bool is_ego, std::string name,
    simulation_api::entity::PedestrianParameters params)
  {
    return spawn(is_ego, name, params.toXml());
  }
  simulation_api::entity::EntityStatus getEntityStatus(
    std::string name,
    simulation_api::entity::CoordinateFrameTypes corrdinate =
    simulation_api::entity::CoordinateFrameTypes::LANE)
  {
    auto status = entity_manager_ptr_->getEntityStatus(name, corrdinate);
    if (!status) {
      throw simulation_api::SimulationRuntimeError(
              "error occurs while getting entity stauts, target entity : " + name);
    }
    return status.get();
  }
  bool setEntityStatus(std::string name, const simulation_api::entity::EntityStatus & status)
  {
    return entity_manager_ptr_->setEntityStatus(name, status);
  }
  boost::optional<double> getLongitudinalDistance(std::string from, std::string to)
  {
    return entity_manager_ptr_->getLongitudinalDistance(from, to);
  }
  boost::optional<double> getTimeHeadway(std::string from, std::string to)
  {
    if (!entity_manager_ptr_->entityStatusSetted(from) ||
      !entity_manager_ptr_->entityStatusSetted(to))
    {
      return boost::none;
    }
    geometry_msgs::msg::Pose pose = getRelativePose(from, to);
    if (pose.position.x > 0) {
      return boost::none;
    }
    simulation_api::entity::EntityStatus to_status = getEntityStatus(to);
    double ret = (pose.position.x * -1) / (to_status.twist.linear.x);
    if (std::isnan(ret)) {
      return std::numeric_limits<double>::infinity();
    }
    return ret;
  }
  void requestAcquirePosition(std::string name, std::int64_t lanelet_id, double s, double offset)
  {
    entity_manager_ptr_->requestAcquirePosition(name, lanelet_id, s, offset);
  }
  void requestLaneChange(std::string name, std::int64_t to_lanelet_id)
  {
    entity_manager_ptr_->requestLaneChange(name, to_lanelet_id);
  }
  void requestLaneChange(std::string name, simulation_api::entity::Direction direction)
  {
    entity_manager_ptr_->requestLaneChange(name, direction);
  }
  bool isInLanelet(std::string name, std::int64_t lanelet_id)
  {
    if (!entity_manager_ptr_->entityStatusSetted(name)) {
      return false;
    }
    try {
      simulation_api::entity::EntityStatus status = getEntityStatus(name,
          simulation_api::entity::CoordinateFrameTypes::LANE);
      if (status.lanelet_id == lanelet_id) {
        return true;
      }
    } catch (simulation_api::SimulationRuntimeError) {
      return false;
    }
    return false;
  }
  void setTargetSpeed(std::string name, double target_speed, bool continuous)
  {
    entity_manager_ptr_->setTargetSpeed(name, target_speed, continuous);
  }
  geometry_msgs::msg::Pose getRelativePose(std::string from, std::string to)
  {
    return entity_manager_ptr_->getRelativePose(from, to);
  }
  geometry_msgs::msg::Pose getRelativePose(geometry_msgs::msg::Pose from, std::string to)
  {
    return entity_manager_ptr_->getRelativePose(from, to);
  }
  geometry_msgs::msg::Pose getRelativePose(std::string from, geometry_msgs::msg::Pose to)
  {
    return entity_manager_ptr_->getRelativePose(from, to);
  }
  geometry_msgs::msg::Pose getRelativePose(
    geometry_msgs::msg::Pose from,
    geometry_msgs::msg::Pose to)
  {
    return entity_manager_ptr_->getRelativePose(from, to);
  }
  bool reachPosition(std::string name, geometry_msgs::msg::Pose target_pose, double tolerance)
  {
    if (!entity_manager_ptr_->entityStatusSetted(name)) {
      return false;
    }
    return entity_manager_ptr_->reachPosition(name, target_pose, tolerance);
  }
  bool reachPosition(
    std::string name, std::int64_t lanelet_id, double s, double offset,
    double tolerance)
  {
    if (!entity_manager_ptr_->entityStatusSetted(name)) {
      return false;
    }
    return entity_manager_ptr_->reachPosition(name, lanelet_id, s, offset, tolerance);
  }
  void setVerbose(bool verbose)
  {
    entity_manager_ptr_->setVerbose(verbose);
  }
  boost::optional<double> getStandStillDuration(std::string name) const
  {
    return entity_manager_ptr_->getStandStillDuration(name);
  }
  simulation_api::entity::EntityStatus toStatus(XmlRpc::XmlRpcValue param)
  {
    std::string coordinate = param["coordinate"];
    std::string name = param["entity/name"];
    geometry_msgs::msg::Pose pose;
    if (coordinate == "lane") {
      std::string lanelet_id_str = param["lanelet_id"];
      std::int64_t lanelet_id = boost::lexical_cast<std::int64_t>(lanelet_id_str);
      double s = param["s"];
      double offset = param["offset"];
      geometry_msgs::msg::Vector3 rpy;
      rpy.x = param["roll"];
      rpy.y = param["pitch"];
      rpy.z = param["yaw"];
      geometry_msgs::msg::Twist twist;
      twist.linear.x = param["twist/linear/x"];
      twist.linear.y = param["twist/linear/y"];
      twist.linear.z = param["twist/linear/z"];
      twist.angular.x = param["twist/angular/x"];
      twist.angular.y = param["twist/angular/y"];
      twist.angular.z = param["twist/angular/z"];
      geometry_msgs::msg::Accel accel;
      accel.linear.x = param["accel/linear/x"];
      accel.linear.y = param["accel/linear/y"];
      accel.linear.z = param["accel/linear/z"];
      accel.angular.x = param["accel/angular/x"];
      accel.angular.y = param["accel/angular/y"];
      accel.angular.z = param["accel/angular/z"];
      double time = param["time"];
      simulation_api::entity::EntityStatus status(time, lanelet_id, s, offset, rpy, twist,
        accel);
      return status;
    }
    if (coordinate == "world") {
      pose.position.x = param["pose/position/x"];
      pose.position.y = param["pose/position/y"];
      pose.position.z = param["pose/position/z"];
      if (param.hasMember("pose/orientation/x") ||
        param.hasMember("pose/orientation/y") ||
        param.hasMember("pose/orientation/z") ||
        param.hasMember("pose/orientation/w"))
      {
        pose.orientation.x = 0.0;
        pose.orientation.y = 0.0;
        pose.orientation.z = 0.0;
        pose.orientation.w = 1.0;
      } else {
        pose.orientation.x = param["pose/orientation/x"];
        pose.orientation.y = param["pose/orientation/y"];
        pose.orientation.z = param["pose/orientation/z"];
        pose.orientation.w = param["pose/orientation/w"];
      }
      geometry_msgs::msg::Twist twist;
      twist.linear.x = param["twist/linear/x"];
      twist.linear.y = param["twist/linear/y"];
      twist.linear.z = param["twist/linear/z"];
      twist.angular.x = param["twist/angular/x"];
      twist.angular.y = param["twist/angular/y"];
      twist.angular.z = param["twist/angular/z"];
      geometry_msgs::msg::Accel accel;
      accel.linear.x = param["accel/linear/x"];
      accel.linear.y = param["accel/linear/y"];
      accel.linear.z = param["accel/linear/z"];
      accel.angular.x = param["accel/angular/x"];
      accel.angular.y = param["accel/angular/y"];
      accel.angular.z = param["accel/angular/z"];
      double time = param["time"];
      simulation_api::entity::EntityStatus status(time, pose, twist, accel);
      return status;
    }
    throw(scenario_simulator::ExecutionFailedError("coordinate does not match, coordinate : " +
          coordinate));
  }
  XmlRpc::XmlRpcValue toValue(std::string name, simulation_api::entity::EntityStatus status)
  {
    if (status.coordinate == simulation_api::entity::CoordinateFrameTypes::WORLD) {
      XmlRpc::XmlRpcValue param;
      param["entity/name"] = name;
      param["coordinate"] = "world";
      param["pose/position/x"] = status.pose.position.x;
      param["pose/position/y"] = status.pose.position.y;
      param["pose/position/z"] = status.pose.position.z;
      param["pose/orientation/x"] = status.pose.orientation.x;
      param["pose/orientation/y"] = status.pose.orientation.y;
      param["pose/orientation/z"] = status.pose.orientation.z;
      param["pose/orientation/w"] = status.pose.orientation.w;
      param["twist/linear/x"] = status.twist.linear.x;
      param["twist/linear/y"] = status.twist.linear.y;
      param["twist/linear/z"] = status.twist.linear.z;
      param["twist/angular/x"] = status.twist.angular.x;
      param["twist/angular/y"] = status.twist.angular.y;
      param["twist/angular/z"] = status.twist.angular.z;
      param["accel/linear/x"] = status.accel.linear.x;
      param["accel/linear/y"] = status.accel.linear.y;
      param["accel/linear/z"] = status.accel.linear.z;
      param["accel/angular/x"] = status.accel.angular.x;
      param["accel/angular/y"] = status.accel.angular.y;
      param["accel/angular/z"] = status.accel.angular.z;
      param["time"] = status.time;
      return param;
    }
    if (status.coordinate == simulation_api::entity::CoordinateFrameTypes::LANE) {
      XmlRpc::XmlRpcValue param;
      param["entity/name"] = name;
      param["coordinate"] = "lane";
      param["lanelet_id"] = std::to_string(status.lanelet_id);
      param["s"] = status.s;
      param["offset"] = status.offset;
      param["roll"] = status.rpy.x;
      param["pitch"] = status.rpy.y;
      param["yaw"] = status.rpy.z;
      param["twist/linear/x"] = status.twist.linear.x;
      param["twist/linear/y"] = status.twist.linear.y;
      param["twist/linear/z"] = status.twist.linear.z;
      param["twist/angular/x"] = status.twist.angular.x;
      param["twist/angular/y"] = status.twist.angular.y;
      param["twist/angular/z"] = status.twist.angular.z;
      param["accel/linear/x"] = status.accel.linear.x;
      param["accel/linear/y"] = status.accel.linear.y;
      param["accel/linear/z"] = status.accel.linear.z;
      param["accel/angular/x"] = status.accel.angular.x;
      param["accel/angular/y"] = status.accel.angular.y;
      param["accel/angular/z"] = status.accel.angular.z;
      param["time"] = status.time;
      return param;
    }
    throw(scenario_simulator::ExecutionFailedError("coordinate does not match"));
  }
};
}  // namespace scenario_simulator

#endif  // SIMULATION_API__API__ENTITY_API_IMPL_HPP_
