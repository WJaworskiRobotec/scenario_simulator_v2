// Copyright 2015-2021 Tier IV, Inc. All rights reserved.
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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__DETERMINISTIC_MULTI_PARAMETER_DISTRIBUTION_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__DETERMINISTIC_MULTI_PARAMETER_DISTRIBUTION_HPP_

#include <openscenario_interpreter/reader/element.hpp>
#include <openscenario_interpreter/syntax/parameter_assignment.hpp>
#include <openscenario_interpreter/syntax/scenario_definition.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ---- ParameterValueSet ------------------------------------
 *
 *  <xsd:complexType name="ParameterValueSet">
 *    <xsd:sequence>
 *      <xsd:element name="ParameterAssignment" type="ParameterAssignment" maxOccurs="unbounded"/>
 *    </xsd:sequence>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct ParameterValueSet : public std::list<ParameterAssignment>
{
  explicit ParameterValueSet(const pugi::xml_node & node, const Scope & scope)
  : std::list<ParameterAssignment>(
      readElements<ParameterAssignment, 1>("ParameterAssignment", node, scope))
  {
  }

  auto sampling() const -> std::vector<std::pair<String, Object>>
  {
    std::vector<std::pair<String, Object>> sample;
    for (auto & assignment : *this) {
      sample.emplace_back(assignment.parameterRef, make(assignment.value));
    }
    return sample;
  }
};

/* ---- ValueSetDistribution ---------------------------------------------
 *
 *  <xsd:complexType name="ValueSetDistribution">
 *    <xsd:sequence>
 *      <xsd:element name="ParameterValueSet" type="ParameterValueSet" maxOccurs="unbounded"/>
 *    </xsd:sequence>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct ValueSetDistribution : public std::list<ParameterValueSet>
{
  explicit ValueSetDistribution(const pugi::xml_node & node, Scope & scope)
  : std::list<ParameterValueSet>(
      readElements<ParameterValueSet, 1>("ParameterValueSet", node, scope)),
    current(begin())
  {
  }

  auto sampling() -> boost::optional<std::vector<std::pair<String, Object>>>
  {
    return current == end() ? boost::none : boost::make_optional((current++)->sampling());
  }

private:
  const_iterator current;
};

/* ---- DeterministicMultiParameterDistributionType ------------------------------------
 *
 *  <xsd:group name="DeterministicMultiParameterDistributionType">
 *    <xsd:sequence>
 *      <xsd:element name="ValueSetDistribution" type="ValueSetDistribution"/>
 *    </xsd:sequence>
 *  </xsd:group>
 *
 * -------------------------------------------------------------------------- */
struct DeterministicMultiParameterDistributionType : public ValueSetDistribution
{
  explicit DeterministicMultiParameterDistributionType(const pugi::xml_node & node, Scope & scope)
  : ValueSetDistribution(node, scope)
  {
  }
};

/* ---- DeterministicMultiParameterDistribution ------------------------------------
 *
 *  <xsd:complexType name="DeterministicMultiParameterDistribution">
 *    <xsd:sequence>
 *      <xsd:group ref="DeterministicMultiParameterDistributionType"/>
 *    </xsd:sequence>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct DeterministicMultiParameterDistribution : public DeterministicMultiParameterDistributionType
{
  explicit DeterministicMultiParameterDistribution(const pugi::xml_node & node, Scope & scope)
  : DeterministicMultiParameterDistributionType(node, scope)
  {
  }
};

}  // namespace syntax
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__DETERMINISTIC_MULTI_PARAMETER_DISTRIBUTION_HPP_
