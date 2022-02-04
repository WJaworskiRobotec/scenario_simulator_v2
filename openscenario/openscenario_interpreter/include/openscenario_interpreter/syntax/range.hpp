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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__RANGE_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__RANGE_HPP_

#include <openscenario_interpreter/reader/attribute.hpp>
#include <openscenario_interpreter/scope.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ---- Range --------------------------------------------------------------
 *
 *  <xsd:complexType name="Range">
 *    <xsd:attribute name="lowerLimit" type="Double" use="required"/>
 *    <xsd:attribute name="upperLimit" type="Double" use="required"/>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct Range
{
  explicit Range(const pugi::xml_node & node, Scope & scope)
  : lowerLimit(readAttribute<Double>("lowerLimit", node, scope)),
    upperLimit(readAttribute<Double>("upperLimit", node, scope))
  {
  }

  Double lowerLimit;
  Double upperLimit;
};

}  // namespace syntax
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__RANGE_HPP_
