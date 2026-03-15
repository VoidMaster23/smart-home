#pragma once

#include "smart_device.h"
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QPointer>

/**
 * @brief Represents a range of color temperatures.
 * 
 * Note: Assumes values are in mireds.
 */
struct ColorTempRange {
  int min{};
  int max{};
  int neutral{};
};

/**
 * @brief Parameters for constructing a SmartLight.
 */
struct SmartLightParams {
  QString id;
  QString name;
  QString model;
  ColorTempRange color_temp_range;
  int color_temp{};
  int brightness{};
  QPointer<QObject> parent = nullptr;
};

/**
 * @brief Base class for smart lights.
 */
class SmartLight : public SmartDevice {
  Q_OBJECT
  Q_PROPERTY(int brightness READ brightness WRITE set_brightness NOTIFY
                 brightness_changed)
  Q_PROPERTY(int color_temp READ color_temp WRITE set_color_temp NOTIFY
                 color_temp_changed)

public:
  /**
   * @brief Construct a SmartLight.
   * @param params Construction parameters.
   */
  explicit SmartLight(SmartLightParams params);

  /**
   * @brief Get the current brightness.
   * @returns Brightness level (0-254).
   */
  [[nodiscard]] int brightness() const;

  /**
   * @brief Get the current color temperature.
   * @returns Color temperature in mireds.
   */
  [[nodiscard]] int color_temp() const;

  /**
   * @brief Get the supported color temperature range.
   * @returns ColorTempRange for the device.
   */
  [[nodiscard]] ColorTempRange get_color_temp_range() const;

  /**
   * @brief Set the brightness level.
   * @param brightness Level to set (0-254).
   */
  virtual void set_brightness(int brightness) = 0;

  /**
   * @brief Set the color temperature.
   * @param color_temp Temperature in mireds.
   */
  virtual void set_color_temp(int color_temp) = 0;

  /**
   * @brief Handle an incoming JSON payload.
   */
  void handle_update(QJsonObject const &payload) override = 0;

  static constexpr int min_brightness = 0;
  static constexpr int max_brightness = 254;

signals:
  /**
   * @brief Emitted when the brightness changes.
   * @param brightness New brightness level.
   */
  void brightness_changed(int brightness);

  /**
   * @brief Emitted when the color temperature changes.
   * @param color_temp New color temperature level.
   */
  void color_temp_changed(int color_temp);

protected:
  /**
   * @brief Internal method to update the brightness state.
   * @param brightness New brightness level.
   */
  void update_brightness(int brightness);

  /**
   * @brief Internal method to update the color temperature state.
   * @param color_temp New color temperature level.
   */
  void update_color_temp(int color_temp);

  int m_brightness;
  int m_color_temp;
  ColorTempRange m_color_temp_range;
};
