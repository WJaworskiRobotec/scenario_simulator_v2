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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__DISTRIBUTION_DEFINITION_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__DISTRIBUTION_DEFINITION_HPP_

#include "openscenario_interpreter/object.hpp"

#include <openscenario_interpreter/reader/element.hpp>
#include <openscenario_interpreter/syntax/deterministic.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ---- DistributionDefinition ---------------------------------
 *
 *  <xsd:group name="DistributionDefinition">
 *    <xsd:choice>
 *      <xsd:element name="Deterministic" type="Deterministic"/>
 *      <xsd:element name="Stochastic" type="Stochastic"/>
 *    </xsd:choice>
 *  </xsd:group>
 *
 * -------------------------------------------------------------------------- */
struct DistributionDefinition : public ComplexType
{
  explicit DistributionDefinition(const pugi::xml_node & node, Scope & scope)
  // clang-format off
  : ComplexType(choice(node,
      std::make_pair("Deterministic", [&](auto && node) { return make<Deterministic>(node, scope); }),
      std::make_pair("Stochastic"   , [&](auto &&     ) { throw UNSUPPORTED_ELEMENT_SPECIFIED("Stochastic"); return unspecified;})))
  // clang-format on
  {
  }

  auto sampling(Scope & scope) -> bool
  {
    if (is<Deterministic>()) {
      return as<Deterministic>().sampling(scope);
    } else {
      throw UNSUPPORTED_ELEMENT_SPECIFIED(type().name());
    }
  }
};
}  // namespace syntax
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__DISTRIBUTION_DEFINITION_HPP_
