import { beforeEach, describe, expect, test, vi } from 'vitest';

import { parseLight } from './discovery';
import { mockDeviceConfigTopic, mockDevicePayload } from './mockData';

const uniqueId = '0x00e07291ff75002e_light_zigbee2mqtt';

const variant = (change: (config: Record<string, unknown>) => void): string => {
  const config = JSON.parse(mockDevicePayload) as Record<string, unknown>;
  change(config);
  return JSON.stringify(config);
};

describe('parseLight', () => {
  beforeEach(() => {
    vi.spyOn(console, 'error').mockImplementation(() => {});
  });

  test('parses the real config', () => {
    expect(parseLight({ configTopic: mockDeviceConfigTopic, payload: mockDevicePayload })).toEqual({
      availabilityTopics: [
        'zigbee2mqtt/bridge/state',
        'zigbee2mqtt/Kajplats_Room_Light/availability',
      ],
      brightnessScale: 254,
      colorModes: ['xy', 'color_temp'],
      commandTopic: 'zigbee2mqtt/Kajplats_Room_Light/set',
      configTopic: 'homeassistant/light/0x00e07291ff75002e/light/config',
      deviceId: 'zigbee2mqtt_0x00e07291ff75002e',
      displayName: 'Kajplats_Room_Light',
      id: uniqueId,
      maxMireds: 555,
      minMireds: 153,
      stateTopic: 'zigbee2mqtt/Kajplats_Room_Light',
      supportsBrightness: true,
    });
  });

  test.each(['garbage', '{}', 'null'])('returns null for %j', (payload) => {
    expect(parseLight({ configTopic: mockDeviceConfigTopic, payload })).toBeNull();
  });

  test('returns null when command_topic is missing', () => {
    expect(
      parseLight({
        configTopic: mockDeviceConfigTopic,
        payload: variant((c) => delete c.command_topic),
      }),
    ).toBeNull();
  });

  test('falls back to unique_id when device is missing', () => {
    const light = parseLight({
      configTopic: mockDeviceConfigTopic,
      payload: variant((c) => delete c.device),
    });
    expect(light?.deviceId).toBe(uniqueId);
    expect(light?.displayName).toBe(uniqueId);
  });

  test('leaves availabilityTopics undefined when availability is missing', () => {
    expect(
      parseLight({
        configTopic: mockDeviceConfigTopic,
        payload: variant((c) => delete c.availability),
      })?.availabilityTopics,
    ).toEqual([]);
  });

  test('drops unknown color modes', () => {
    const payload = variant((c) => (c.supported_color_modes = ['xy', 'disco']));
    expect(parseLight({ configTopic: mockDeviceConfigTopic, payload })).toMatchObject({
      colorModes: ['xy'],
    });
  });

  test('uses defaults for a minimal on/off light', () => {
    const light = parseLight({
      configTopic: mockDeviceConfigTopic,
      payload: '{"command_topic":"a/set","state_topic":"a"}',
    });
    expect(light).toMatchObject({
      brightnessScale: 255,
      colorModes: [],
      id: 'a',
      supportsBrightness: false,
    });
    expect(light?.minMireds).toBeUndefined();
  });
});
