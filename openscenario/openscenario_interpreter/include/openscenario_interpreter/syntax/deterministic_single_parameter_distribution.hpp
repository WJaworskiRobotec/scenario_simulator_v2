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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__DETERMINISTIC_SINGLE_PARAMETER_DISTRIBUTION_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__DETERMINISTIC_SINGLE_PARAMETER_DISTRIBUTION_HPP_

#include "openscenario_interpreter/error.hpp"
#include "openscenario_interpreter/reader/attribute.hpp"

#include <openscenario_interpreter/reader/element.hpp>
#include <openscenario_interpreter/syntax/parameter_assignment.hpp>
#include <openscenario_interpreter/syntax/range.hpp>
#include <openscenario_interpreter/syntax/scenario_definition.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/optional/optional.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ---- DistributionSet ---------------------------------------------------
 *
 *  <xsd:complexType name="DistributionSetElement">
 *    <xsd:attribute name="value" type="String" use="required"/>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct DistributionSetElement
{
  explicit DistributionSetElement(const pugi::xml_node & node, Scope & scope)
  : value(readAttribute<String>("value", node, scope))
  {
  }

  String value;
};

/* ---- DistributionSet ---------------------------------------------------
 *
 *  <xsd:complexType name="DistributionSet">
 *    <xsd:sequence>
 *      <xsd:element name="Element" type="DistributionSetElement" maxOccurs="unbounded"/>
 *    </xsd:sequence>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct DistributionSet : std::list<DistributionSetElement>
{
  explicit DistributionSet(const pugi::xml_node & node, Scope & scope)
  : std::list<DistributionSetElement>(
      readElements<DistributionSetElement, 1>("DistributionSetElement", node, scope)),
    current(begin())
  {
  }

  auto sampling() -> boost::optional<Object>
  {
    return current == end() ? boost::none : boost::make_optional(make((current++)->value));
  }

private:
  const_iterator current;
};

/* ---- DistributionRange --------------------------------------------------
 *
 *  <xsd:complexType name="DistributionRange">
 *    <xsd:all>
 *      <xsd:element name="Range" type="Range"/>
 *    </xsd:all>
 *    <xsd:attribute name="stepWidth" type="Double" use="required"/>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct DistributionRange
{
  explicit DistributionRange(const pugi::xml_node & node, Scope & scope)
  : range(readElement<Range>("Range", node, scope)),
    stepWidth(readAttribute<Double>("stepWidth", node, scope)),
    current(range.lowerLimit)
  {
  }

  auto sampling() -> boost::optional<Object>
  {
    if (current > range.upperLimit) {
      return boost::none;
    } else {
      auto ret = boost::make_optional(make(current));
      current += stepWidth;
      return ret;
    }
  }

  const Range range;
  const Double stepWidth;

private:
  Double current;
};

/* ---- UserDefinedDistribution -------------------------------------------
 *
 *  <xsd:complexType name="UserDefinedDistribution">
 *    <xsd:simpleContent>
 *      <xsd:extension base="xsd:string">
 *        <xsd:attribute name="type" type="String" use="required"/>
 *      </xsd:extension>
 *    </xsd:simpleContent>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct UserDefinedDistribution
{
  explicit UserDefinedDistribution(const pugi::xml_node & node, Scope & scope)
  : type(readAttribute<String>("type", node, scope)),
    content(readAttribute<String>("content", node, scope, ""))
  {
  }

  auto sampling() -> boost::optional<Object>
  {
    auto iter = distributions.find(type);
    if (iter != distributions.end()) {
      return iter->second(content);
    } else {
      return boost::none;
    }
  }

  const String type;
  const String content;

private:
  std::unordered_map<String, std::function<boost::optional<Object>(const String &)>> distributions;
};

/* ---- DeterministicSingleParameterDistribution ------------------------------------
 *
 *  <xsd:group name="DeterministicSingleParameterDistributionType">
 *    <xsd:choice>
 *      <xsd:element name="DistributionSet" type="DistributionSet"/>
 *      <xsd:element name="DistributionRange" type="DistributionRange"/>
 *      <xsd:element name="UserDefinedDistribution" type="UserDefinedDistribution"/>
 *    </xsd:choice>
 *  </xsd:group>
 *
 * -------------------------------------------------------------------------- */
struct DeterministicSingleParameterDistributionType : public ComplexType
{
  explicit DeterministicSingleParameterDistributionType(const pugi::xml_node & node, Scope & scope)
  // clang-format off
  : ComplexType(choice(node,
      std::make_pair("DistributionSet",         [&](auto && node) { return make<DistributionSet        >(node, scope); }),
      std::make_pair("DistributionRange",       [&](auto && node) { return make<DistributionRange      >(node, scope); }),
      std::make_pair("UserDefinedDistribution", [&](auto && node) { return make<UserDefinedDistribution>(node, scope); })))
  // clang-format on
  {
  }

  auto sampling() -> boost::optional<Object>
  {
    if (is<DistributionSet>()) {
      return as<DistributionSet>().sampling();
    } else if (is<DistributionRange>()) {
      return as<DistributionRange>().sampling();
    } else if (is<UserDefinedDistribution>()) {
      return as<UserDefinedDistribution>().sampling();
    } else {
      throw UNSUPPORTED_ELEMENT_SPECIFIED(type().name());
    }
  }
};

/* ---- DeterministicSingleParameterDistribution ------------------------------------
 *
 *  <xsd:complexType name="DeterministicSingleParameterDistribution">
 *    <xsd:sequence>
 *      <xsd:group ref="DeterministicSingleParameterDistributionType"/>
 *    </xsd:sequence>
 *    <xsd:attribute name="parameterName" type="String" use="required"/>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct DeterministicSingleParameterDistribution
: public DeterministicSingleParameterDistributionType
{
  explicit DeterministicSingleParameterDistribution(const pugi::xml_node & node, Scope & scope)
  : DeterministicSingleParameterDistributionType(node, scope),
    parameterName(readAttribute<String>("parameterName", node, scope))
  {
  }

  String parameterName;
};

}  // namespace syntax
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__DETERMINISTIC_SINGLE_PARAMETER_DISTRIBUTION_HPP_
