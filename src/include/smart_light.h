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
 * Represents an abstract smart light device exposing brightness and color temperature controls.
 *
 * Provides read accessors and abstract mutators for brightness and color temperature,
 * enforces a color temperature range, and declares signals for changes. Concrete
 * implementations must apply state changes and handle incoming update payloads.
 */

/**
 * Initialize a SmartLight with the provided parameters.
 * @param params Initialization parameters including id, name, model, initial brightness,
 *               initial color temperature, and color temperature range.
 */

/**
 * Get the current brightness level.
 * @return The brightness value in the device's scale (between min_brightness and max_brightness).
 */

/**
 * Get the current color temperature.
 * @return The color temperature in mireds.
 */

/**
 * Get the supported color temperature range.
 * @return A ColorTempRange struct with `min`, `max`, and `neutral` values (mireds).
 */

/**
 * Set the brightness to the specified value.
 * @param brightness Desired brightness value; implementations should clamp or validate
 *                   it within [min_brightness, max_brightness] as appropriate.
 */

/**
 * Set the color temperature to the specified value.
 * @param color_temp Desired color temperature in mireds; implementations should clamp or
 *                   validate it within the device's color temperature range as appropriate.
 */

/**
 * Handle an incoming update payload from the device or network.
 * @param payload JSON object containing update fields; implementations must parse and
 *                apply relevant state changes and emit the corresponding signals.
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