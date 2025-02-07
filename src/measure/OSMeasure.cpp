/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "OSMeasure.hpp"

namespace openstudio {
namespace measure {

  OSMeasure::OSMeasure(MeasureType measureType) : m_measureType(measureType) {}

  MeasureType OSMeasure::measureType() const {
    return m_measureType;
  }

  std::string OSMeasure::name() const {
    return "";
  }

  std::string OSMeasure::description() const {
    return "";
  }

  std::string OSMeasure::taxonomy() const {
    return "";
  }

  std::string OSMeasure::modeler_description() const {
    return "";
  }

  bool OSMeasure::registerWithSketchUp() const {
    return false;
  }

  bool OSMeasure::registerWithApplication() const {
    return false;
  }

}  // namespace measure
}  // namespace openstudio
