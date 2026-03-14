#pragma once

#include "smart_device.h"
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QPointer>

/*
Note: my logic assumes mireds, since my lights operate with mireds,
i am too lazy to handle kelvin lol so thats a future me problem
*/
struct ColorTempRange {
  int min{};
  int max{};
  int neutral{};
};

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
 * Abstract base class representing a controllable smart light device.
 *
 * Encapsulates brightness and color temperature state and exposes an interface
 * for platform-specific implementations to apply changes and handle updates.
 */

/**
 * Construct a SmartLight from a parameter bundle.
 * @param params Parameters used to initialize the smart light (id, name, model,
 *               initial brightness, initial color temperature, color temp range, etc.).
 */

/**
 * Get the current brightness level.
 * @returns The current brightness value in the device's scale (between min_brightness and max_brightness).
 */

/**
 * Get the current color temperature.
 * @returns The current color temperature value expressed in mireds.
 */

/**
 * Retrieve the allowable color temperature range.
 * @returns A ColorTempRange struct containing the minimum, maximum, and neutral mired values.
 */

/**
 * Set the target brightness for the light.
 * @param brightness Desired brightness value; implementations should clamp or validate against min_brightness and max_brightness.
 */

/**
 * Set the target color temperature for the light.
 * @param color_temp Desired color temperature in mireds; implementations should respect the configured color temperature range.
 */

/**
 * Handle an incoming device update payload.
 * @param payload JSON object containing update fields from the device; implementations must apply relevant changes and emit corresponding signals.
 */
class SmartLight : public SmartDevice {
  Q_OBJECT
  Q_PROPERTY(int brightness READ brightness WRITE set_brightness NOTIFY
                 brightness_changed)
  Q_PROPERTY(int color_temp READ color_temp WRITE set_color_temp NOTIFY
                 color_temp_changed)

public:
  explicit SmartLight(SmartLightParams params);

  [[nodiscard]] int brightness() const;
  [[nodiscard]] int color_temp() const;
  [[nodiscard]] ColorTempRange get_color_temp_range() const;

  virtual void set_brightness(int brightness) = 0;
  virtual void set_color_temp(int color_temp) = 0;

  void handle_update(QJsonObject const &payload) override = 0;

  static constexpr int min_brightness = 0;
  static constexpr int max_brightness = 254;

signals:
  void brightness_changed(int brightness);
  void color_temp_changed(int color_temp);

protected:
  int m_brightness;
  int m_color_temp;
  ColorTempRange color_temp_range;
};