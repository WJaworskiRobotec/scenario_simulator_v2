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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__PARAMETER_VALUE_DISTRIBUTION_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__PARAMETER_VALUE_DISTRIBUTION_HPP_

#include <openscenario_interpreter/scope.hpp>
#include <openscenario_interpreter/syntax/file.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ---- ParamterValueDistributionDefinition ---------------------------------
 *
 *  <xsd:complexType name="ParameterValueDistribution">
 *    <xsd:sequence>
 *      <xsd:element name="ScenarioFile" type="File"/>
 *      <xsd:group ref="DistributionDefinition"/>
 *    </xsd:sequence>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct ParameterValueDistribution
{
  explicit ParameterValueDistribution(const pugi::xml_node &, Scope &);

  File scenarioFile;
};
}  // namespace syntax
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__PARAMETER_VALUE_DISTRIBUTION_HPP_
