/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "LightingDesignDay.hpp"
#include "LightingDesignDay_Impl.hpp"

#include "Model.hpp"
#include "ModelExtensibleGroup.hpp"
#include "Site.hpp"
#include "Site_Impl.hpp"

#include <utilities/idd/IddFactory.hxx>

#include <utilities/idd/OS_LightingDesignDay_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

#include "../utilities/time/DateTime.hpp"
#include "../utilities/core/Assert.hpp"

namespace openstudio {
namespace model {

  namespace detail {

    LightingDesignDay_Impl::LightingDesignDay_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle)
      : ModelObject_Impl(idfObject, model, keepHandle) {
      OS_ASSERT(idfObject.iddObject().type() == LightingDesignDay::iddObjectType());
    }

    LightingDesignDay_Impl::LightingDesignDay_Impl(const openstudio::detail::WorkspaceObject_Impl& other, Model_Impl* model, bool keepHandle)
      : ModelObject_Impl(other, model, keepHandle) {
      OS_ASSERT(other.iddObject().type() == LightingDesignDay::iddObjectType());
    }

    LightingDesignDay_Impl::LightingDesignDay_Impl(const LightingDesignDay_Impl& other, Model_Impl* model, bool keepHandle)
      : ModelObject_Impl(other, model, keepHandle) {}

    boost::optional<ParentObject> LightingDesignDay_Impl::parent() const {
      boost::optional<ParentObject> result(model().getOptionalUniqueModelObject<Site>());
      return result;
    }

    const std::vector<std::string>& LightingDesignDay_Impl::outputVariableNames() const {
      static const std::vector<std::string> result;
      return result;
    }

    IddObjectType LightingDesignDay_Impl::iddObjectType() const {
      return LightingDesignDay::iddObjectType();
    }

    std::string LightingDesignDay_Impl::cieSkyModel() const {
      boost::optional<std::string> value = getString(OS_LightingDesignDayFields::CIESkyModel, true);
      OS_ASSERT(value);
      return value.get();
    }

    int LightingDesignDay_Impl::snowIndicator() const {
      boost::optional<int> value = getInt(OS_LightingDesignDayFields::SnowIndicator, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool LightingDesignDay_Impl::isSnowIndicatorDefaulted() const {
      return isEmpty(OS_LightingDesignDayFields::SnowIndicator);
    }

    bool LightingDesignDay_Impl::setCIESkyModel(const std::string& cIESkyModel) {
      bool result = setString(OS_LightingDesignDayFields::CIESkyModel, cIESkyModel);
      return result;
    }

    bool LightingDesignDay_Impl::setSnowIndicator(int snowIndicator) {
      bool result = setInt(OS_LightingDesignDayFields::SnowIndicator, snowIndicator);
      return result;
    }

    void LightingDesignDay_Impl::resetSnowIndicator() {
      bool result = setString(OS_LightingDesignDayFields::SnowIndicator, "");
      OS_ASSERT(result);
    }

    openstudio::Date LightingDesignDay_Impl::date() const {
      OptionalInt month = getInt(OS_LightingDesignDayFields::Month, true);
      OptionalInt dayofMonth = getInt(OS_LightingDesignDayFields::DayofMonth, true);
      OS_ASSERT(month);
      OS_ASSERT(dayofMonth);
      return {MonthOfYear(*month), static_cast<unsigned int>(*dayofMonth)};
    }

    bool LightingDesignDay_Impl::setDate(const openstudio::Date& date) {
      bool test = setInt(OS_LightingDesignDayFields::Month, date.monthOfYear().value(), false);
      OS_ASSERT(test);
      test = setInt(OS_LightingDesignDayFields::DayofMonth, date.dayOfMonth());
      OS_ASSERT(test);
      return true;
    }

    std::vector<openstudio::Time> LightingDesignDay_Impl::simulationTimes() const {
      std::vector<openstudio::Time> result;

      for (const ModelExtensibleGroup& group : castVector<ModelExtensibleGroup>(extensibleGroups())) {
        OptionalInt hour = group.getInt(OS_LightingDesignDayExtensibleFields::HourtoSimulate, true);
        OptionalInt minute = group.getInt(OS_LightingDesignDayExtensibleFields::MinutetoSimulate, true);
        OS_ASSERT(hour);
        OS_ASSERT(minute);
        result.emplace_back(0, *hour, *minute);
      }

      return result;
    }

    std::vector<openstudio::DateTime> LightingDesignDay_Impl::simulationDateTimes() const {
      Date date = this->date();
      std::vector<openstudio::DateTime> result;

      for (const ModelExtensibleGroup& group : castVector<ModelExtensibleGroup>(extensibleGroups())) {
        OptionalInt hour = group.getInt(OS_LightingDesignDayExtensibleFields::HourtoSimulate, true);
        OptionalInt minute = group.getInt(OS_LightingDesignDayExtensibleFields::MinutetoSimulate, true);
        OS_ASSERT(hour);
        OS_ASSERT(minute);
        result.emplace_back(date, Time(0, *hour, *minute));
      }

      return result;
    }

    bool LightingDesignDay_Impl::addSimulationTime(const openstudio::Time& time) {
      double totalDays = time.totalDays();
      unsigned hours = time.hours();
      unsigned minutes = time.minutes();
      if ((totalDays > 1) || (totalDays < 0)) {
        return false;
      } else if (totalDays == 1) {
        hours = 24;
        minutes = 0;
      }

      std::vector<std::string> temp{std::to_string(hours), std::to_string(minutes)};

      auto group = pushExtensibleGroup(temp).cast<ModelExtensibleGroup>();
      return (!group.empty());
    }

    void LightingDesignDay_Impl::clearSimulationTimes() {
      clearExtensibleGroups();
    }

    void LightingDesignDay_Impl::ensureNoLeapDays() {
      boost::optional<int> month;
      boost::optional<int> day;

      month = getInt(OS_LightingDesignDayFields::Month);
      if (month && (month.get() == 2)) {
        day = this->getInt(OS_LightingDesignDayFields::DayofMonth);
        if (day && (day.get() == 29)) {
          this->setInt(OS_LightingDesignDayFields::DayofMonth, 28);
        }
      }
    }

  }  // namespace detail

  LightingDesignDay::LightingDesignDay(const std::string& cieSkyModel, const openstudio::Date& date, const Model& model)
    : ModelObject(LightingDesignDay::iddObjectType(), model) {
    OS_ASSERT(getImpl<detail::LightingDesignDay_Impl>());
    setCIESkyModel(cieSkyModel);
    setDate(date);
  }

  IddObjectType LightingDesignDay::iddObjectType() {
    IddObjectType result(IddObjectType::OS_LightingDesignDay);
    return result;
  }

  std::vector<std::string> LightingDesignDay::validCIESkyModelValues() {
    return getIddKeyNames(IddFactory::instance().getObject(iddObjectType()).get(), OS_LightingDesignDayFields::CIESkyModel);
  }

  std::string LightingDesignDay::cieSkyModel() const {
    return getImpl<detail::LightingDesignDay_Impl>()->cieSkyModel();
  }

  int LightingDesignDay::snowIndicator() const {
    return getImpl<detail::LightingDesignDay_Impl>()->snowIndicator();
  }

  bool LightingDesignDay::isSnowIndicatorDefaulted() const {
    return getImpl<detail::LightingDesignDay_Impl>()->isSnowIndicatorDefaulted();
  }

  bool LightingDesignDay::setCIESkyModel(const std::string& cIESkyModel) {
    return getImpl<detail::LightingDesignDay_Impl>()->setCIESkyModel(cIESkyModel);
  }

  bool LightingDesignDay::setSnowIndicator(int snowIndicator) {
    return getImpl<detail::LightingDesignDay_Impl>()->setSnowIndicator(snowIndicator);
  }

  void LightingDesignDay::resetSnowIndicator() {
    getImpl<detail::LightingDesignDay_Impl>()->resetSnowIndicator();
  }

  openstudio::Date LightingDesignDay::date() const {
    return getImpl<detail::LightingDesignDay_Impl>()->date();
  }

  bool LightingDesignDay::setDate(const openstudio::Date& date) {
    return getImpl<detail::LightingDesignDay_Impl>()->setDate(date);
  }

  std::vector<openstudio::Time> LightingDesignDay::simulationTimes() const {
    return getImpl<detail::LightingDesignDay_Impl>()->simulationTimes();
  }

  std::vector<openstudio::DateTime> LightingDesignDay::simulationDateTimes() const {
    return getImpl<detail::LightingDesignDay_Impl>()->simulationDateTimes();
  }

  bool LightingDesignDay::addSimulationTime(const openstudio::Time& time) {
    return getImpl<detail::LightingDesignDay_Impl>()->addSimulationTime(time);
  }

  void LightingDesignDay::clearSimulationTimes() {
    return getImpl<detail::LightingDesignDay_Impl>()->clearSimulationTimes();
  }

  void LightingDesignDay::ensureNoLeapDays() {
    getImpl<detail::LightingDesignDay_Impl>()->ensureNoLeapDays();
  }

  /// @cond
  LightingDesignDay::LightingDesignDay(std::shared_ptr<detail::LightingDesignDay_Impl> impl) : ModelObject(std::move(impl)) {}
  /// @endcond

}  // namespace model
}  // namespace openstudio
