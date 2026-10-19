import { beforeEach, describe, expect, test, vi } from 'vitest';

import { parseLight } from './discovery';
import { parseAvailability, parseLightState } from './discovery';
import { mockDeviceConfigTopic, mockDevicePayload } from './mockData';
import { mockStatePayload } from './mockData';

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

describe('parseLightState', () => {
  test('keeps the typed fields of a real payload', () => {
    expect(parseLightState(mockStatePayload)).toEqual({
      brightness: 254,
      color: { h: 123, s: 100, x: 0.17, y: 0.7 },
      color_mode: 'xy',
      color_temp: 153,
      state: 'ON',
    });
  });

  test('accepts a partial update', () => {
    expect(parseLightState('{"brightness":100}')).toEqual({ brightness: 100 });
  });

  test.each(['', 'garbage', 'null', '42', '[]'])('returns null for %j', (payload) => {
    expect(parseLightState(payload)).toBeNull();
  });

  test('returns null for a wrong type', () => {
    expect(parseLightState('{"brightness":"high"}')).toBeNull();
  });

  test('returns null for an unknown state value', () => {
    expect(parseLightState('{"state":"TOGGLE"}')).toBeNull();
  });

  test('drops an unknown color mode but keeps the rest', () => {
    expect(parseLightState('{"brightness":100,"color_mode":"plaid"}')).toStrictEqual({
      brightness: 100,
    });
  });
});

describe('parseAvailability', () => {
  test('online', () => {
    expect(parseAvailability('{"state":"online"}')).toBe(true);
  });

  test('offline', () => {
    expect(parseAvailability('{"state":"offline"}')).toBe(false);
  });

  test.each(['', 'garbage', 'online', '{}', '{"state":"maybe"}'])(
    'returns null for %j',
    (payload) => {
      expect(parseAvailability(payload)).toBeNull();
    },
  );
});
