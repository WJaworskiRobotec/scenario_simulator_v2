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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__DETERMINISTIC_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__DETERMINISTIC_HPP_

#include <openscenario_interpreter/syntax/deterministic_multi_parameter_distribution.hpp>
#include <openscenario_interpreter/syntax/deterministic_single_parameter_distribution.hpp>
#include <openscenario_interpreter/syntax/scenario_definition.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
// clang-format off
/* ---- DeterministicParameterDistribution ------------------------------------
 *
 *  <xsd:group name="DeterministicParameterDistribution">
 *    <xsd:choice>
 *      <xsd:element name="DeterministicMultiParameterDistribution" type="DeterministicMultiParameterDistribution"/>
 *      <xsd:element name="DeterministicSingleParameterDistribution" type="DeterministicSingleParameterDistribution"/>
 *    </xsd:choice>
 *  </xsd:group>
 *
 * -------------------------------------------------------------------------- */
// clang-format on
struct DeterministicParameterDistribution : public ComplexType
{
  explicit DeterministicParameterDistribution(const pugi::xml_node & node, Scope & scope)
  // clang-format off
  : ComplexType(choice(node,
      std::make_pair("DeterministicMultiParameterDistribution" , [&](auto && node) { return make<DeterministicMultiParameterDistribution >(node, scope); }),
      std::make_pair("DeterministicSingleParameterDistribution", [&](auto && node) { return make<DeterministicSingleParameterDistribution>(node, scope); })
    ))
  // clang-format on
  {
  }

  auto sampling(Scope& scope) -> bool
  {
    if (is<DeterministicMultiParameterDistribution>()) {
      auto sample = as<DeterministicMultiParameterDistribution>().sampling();
      if (sample) {
        for (auto && [name, value] : *sample) {
          scope.insert(name, value);
        }
        return true;
      }
      return false;
    } else if (is<DeterministicMultiParameterDistribution>()) {
      auto sample = as<DeterministicSingleParameterDistribution>().sampling();
      if(sample){
        scope.insert(as<DeterministicSingleParameterDistribution>().parameterName, *sample);
        return true;
      }
      return false;
    } else {
      throw UNSUPPORTED_ELEMENT_SPECIFIED(type().name());
    }
  }
};

/* ---- Deterministic -----------------------------------------------------
 *
 *  <xsd:complexType name="Deterministic">
 *    <xsd:sequence>
 *      <xsd:group ref="DeterministicParameterDistribution" minOccurs="0" maxOccurs="unbounded"/>
 *    </xsd:sequence>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct Deterministic : public DeterministicParameterDistribution
{
  explicit Deterministic(const pugi::xml_node & node, Scope & scope)
  : DeterministicParameterDistribution(node, scope)
  {
  }
};

}  // namespace syntax
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__DETERMINISTIC_HPP_
