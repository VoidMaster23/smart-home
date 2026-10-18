import { describe, expect, test, beforeEach } from 'vitest';

import { isLightOnline, useDevicesStore } from './devices';
import type { Light } from './types';
import { mockDeviceConfigTopic } from './utils/mockData';

const makeLight = (overrides: Partial<Light> = {}): Light => ({
  availabilityTopics: ['zigbee2mqtt/bridge/state', 'zigbee2mqtt/Kajplats_Room_Light/availability'],
  brightnessScale: 254,
  colorModes: ['xy', 'color_temp'],
  commandTopic: 'zigbee2mqtt/Kajplats_Room_Light/set',
  configTopic: mockDeviceConfigTopic,
  deviceId: 'zigbee2mqtt_0x00e07291ff75002e',
  displayName: 'Kajplats_Room_Light',
  id: '0x00e07291ff75002e_light_zigbee2mqtt',
  maxMireds: 555,
  minMireds: 153,
  stateTopic: 'zigbee2mqtt/Kajplats_Room_Light',
  supportsBrightness: true,
  ...overrides,
});

describe('device store', () => {
  const store = () => useDevicesStore.getState();

  beforeEach(() => {
    useDevicesStore.setState({ availability: {}, connected: false, lights: {}, lightStates: {} });
  });

  describe('setConnected', () => {
    test('tracks the broker connection', () => {
      store().setConnected(true);
      expect(store().connected).toBe(true);
      store().setConnected(false);
      expect(store().connected).toBe(false);
    });
  });

  describe('upsertLight', () => {
    test('stores a light by id', () => {
      const light = makeLight();
      store().upsertLight(light);
      expect(store().lights[light.id]).toEqual(light);
    });

    test('replaces a light with the same id', () => {
      store().upsertLight(makeLight());
      store().upsertLight(makeLight({ displayName: 'Renamed' }));
      expect(Object.values(store().lights)).toHaveLength(1);
      expect(store().lights[makeLight().id]?.displayName).toBe('Renamed');
    });

    test('keeps lights with different ids apart', () => {
      store().upsertLight(makeLight());
      store().upsertLight(makeLight({ id: 'other' }));
      expect(Object.keys(store().lights)).toHaveLength(2);
    });

    test('does not touch light state', () => {
      const light = makeLight();
      store().upsertLight(light);
      store().applyState(light, { state: 'ON' });
      store().upsertLight(light);
      expect(store().lightStates[light.id]).toEqual({ state: 'ON' });
    });
  });

  describe('removeLight', () => {
    test('removes the light with that config topic', () => {
      const light = makeLight();
      store().upsertLight(light);
      store().removeLight(light.configTopic);
      expect(store().lights).toEqual({});
    });

    test('ignores an unknown config topic', () => {
      store().upsertLight(makeLight());
      store().removeLight('homeassistant/light/unknown/light/config');
      expect(Object.keys(store().lights)).toHaveLength(1);
    });

    test('removes the state of the removed light', () => {
      const light = makeLight();
      store().upsertLight(light);
      store().applyState(light, { state: 'ON' });
      store().removeLight(light.configTopic);
      expect(store().lightStates).toEqual({});
    });

    test('leaves the store untouched for an unknown config topic', () => {
      store().upsertLight(makeLight());
      const before = store();
      store().removeLight('homeassistant/light/unknown/light/config');
      expect(store()).toBe(before);
    });

    test('removes the availability of the removed light', () => {
      const light = makeLight();
      store().upsertLight(light);
      store().setAvailability(light, light.availabilityTopics[0]!, true);
      store().removeLight(light.configTopic);
      expect(store().availability).toEqual({});
    });
  });

  describe('applyState', () => {
    const light = makeLight();

    beforeEach(() => {
      store().upsertLight(light);
    });

    test('stores the state under the light id', () => {
      store().applyState(light, { brightness: 254, state: 'ON' });
      expect(store().lightStates[light.id]).toEqual({ brightness: 254, state: 'ON' });
    });

    test('merges partial updates into the existing state', () => {
      store().applyState(light, { brightness: 254, state: 'ON' });
      store().applyState(light, { brightness: 100 });
      expect(store().lightStates[light.id]).toEqual({ brightness: 100, state: 'ON' });
    });
  });

  describe('setAvailability', () => {
    const light = makeLight();
    const [bridge, device] = light.availabilityTopics;

    test('stores availability per light and topic', () => {
      store().setAvailability(light, bridge!, true);
      expect(store().availability[light.id]).toEqual({ [bridge!]: true });
    });

    test('keeps the other topics of the light', () => {
      store().setAvailability(light, bridge!, true);
      store().setAvailability(light, device!, false);
      expect(store().availability[light.id]).toEqual({ [bridge!]: true, [device!]: false });
    });
  });

  describe('isLightOnline', () => {
    const light = makeLight();
    const [bridge, device] = light.availabilityTopics;

    test('is online when every topic is online', () => {
      store().setAvailability(light, bridge!, true);
      store().setAvailability(light, device!, true);
      expect(isLightOnline(light, store().availability)).toBe(true);
    });

    test('is offline when any topic is offline', () => {
      store().setAvailability(light, bridge!, true);
      store().setAvailability(light, device!, false);
      expect(isLightOnline(light, store().availability)).toBe(false);
    });

    test('is offline while a topic is still unknown', () => {
      store().setAvailability(light, bridge!, true);
      expect(isLightOnline(light, store().availability)).toBe(false);
    });
  });
});
