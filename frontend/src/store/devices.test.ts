import { describe, expect, test, beforeEach } from 'vitest';

import { isEntityOnline, useDevicesStore } from './devices';
import type { Entity } from './types';
import { mockDeviceConfigTopic } from './utils/mockData';

const makeLight = (overrides: Partial<Entity> = {}): Entity => ({
  availabilityTopics: ['zigbee2mqtt/bridge/state', 'zigbee2mqtt/Kajplats_Room_Light/availability'],
  brightnessScale: 254,
  colorModes: ['xy', 'color_temp'],
  commandTopic: 'zigbee2mqtt/Kajplats_Room_Light/set',
  configTopic: mockDeviceConfigTopic,
  deviceId: 'zigbee2mqtt_0x00e07291ff75002e',
  displayName: 'Kajplats_Room_Light',
  id: '0x00e07291ff75002e_light_zigbee2mqtt',
  kind: 'light',
  maxMireds: 555,
  minMireds: 153,
  stateTopic: 'zigbee2mqtt/Kajplats_Room_Light',
  supportsBrightness: true,
  ...overrides,
});

describe('device store', () => {
  const store = () => useDevicesStore.getState();

  beforeEach(() => {
    useDevicesStore.setState({
      availability: {},
      connected: false,
      entities: {},
      entityStates: {},
    });
  });

  describe('setConnected', () => {
    test('tracks the broker connection', () => {
      store().setConnected(true);
      expect(store().connected).toBe(true);
      store().setConnected(false);
      expect(store().connected).toBe(false);
    });
  });

  describe('upsertEntity', () => {
    test('stores an entity by id', () => {
      const entity = makeLight();
      store().upsertEntity(entity);
      expect(store().entities[entity.id]).toEqual(entity);
    });

    test('replaces an entity with the same id', () => {
      store().upsertEntity(makeLight());
      store().upsertEntity(makeLight({ displayName: 'Renamed' }));
      expect(Object.values(store().entities)).toHaveLength(1);
      expect(store().entities[makeLight().id]?.displayName).toBe('Renamed');
    });

    test('keeps entities with different ids apart', () => {
      store().upsertEntity(makeLight());
      store().upsertEntity(makeLight({ id: 'other' }));
      expect(Object.keys(store().entities)).toHaveLength(2);
    });

    test('does not touch entity state', () => {
      const entity = makeLight();
      store().upsertEntity(entity);
      store().applyState(entity, { state: 'ON' });
      store().upsertEntity(entity);
      expect(store().entityStates[entity.id]).toEqual({ state: 'ON' });
    });
  });

  describe('removeEntity', () => {
    test('removes the entity with that config topic', () => {
      const entity = makeLight();
      store().upsertEntity(entity);
      store().removeEntity(entity.configTopic);
      expect(store().entities).toEqual({});
    });

    test('ignores an unknown config topic', () => {
      store().upsertEntity(makeLight());
      store().removeEntity('homeassistant/light/unknown/light/config');
      expect(Object.keys(store().entities)).toHaveLength(1);
    });

    test('removes the state of the removed light', () => {
      const entity = makeLight();
      store().upsertEntity(entity);
      store().applyState(entity, { state: 'ON' });
      store().removeEntity(entity.configTopic);
      expect(store().entityStates).toEqual({});
    });

    test('leaves the store untouched for an unknown config topic', () => {
      store().upsertEntity(makeLight());
      const before = store();
      store().removeEntity('homeassistant/light/unknown/light/config');
      expect(store()).toBe(before);
    });

    test('removes the availability of the removed light', () => {
      const entity = makeLight();
      store().upsertEntity(entity);
      store().setAvailability(entity, entity.availabilityTopics[0]!, true);
      store().removeEntity(entity.configTopic);
      expect(store().availability).toEqual({});
    });
  });

  describe('applyState', () => {
    const entity = makeLight();

    beforeEach(() => {
      store().upsertEntity(entity);
    });

    test('stores the state under the entity id', () => {
      store().applyState(entity, { brightness: 254, state: 'ON' });
      expect(store().entityStates[entity.id]).toEqual({ brightness: 254, state: 'ON' });
    });

    test('merges partial updates into the existing state', () => {
      store().applyState(entity, { brightness: 254, state: 'ON' });
      store().applyState(entity, { brightness: 100 });
      expect(store().entityStates[entity.id]).toEqual({ brightness: 100, state: 'ON' });
    });
  });

  describe('setAvailability', () => {
    const entity = makeLight();
    const [bridge, device] = entity.availabilityTopics;

    test('stores availability per entity and topic', () => {
      store().setAvailability(entity, bridge!, true);
      expect(store().availability[entity.id]).toEqual({ [bridge!]: true });
    });

    test('keeps the other topics of the light', () => {
      store().setAvailability(entity, bridge!, true);
      store().setAvailability(entity, device!, false);
      expect(store().availability[entity.id]).toEqual({ [bridge!]: true, [device!]: false });
    });
  });

  describe('isLightOnline', () => {
    const entity = makeLight();
    const [bridge, device] = entity.availabilityTopics;

    test('is online when every topic is online', () => {
      store().setAvailability(entity, bridge!, true);
      store().setAvailability(entity, device!, true);
      expect(isEntityOnline(entity, store().availability)).toBe(true);
    });

    test('is offline when any topic is offline', () => {
      store().setAvailability(entity, bridge!, true);
      store().setAvailability(entity, device!, false);
      expect(isEntityOnline(entity, store().availability)).toBe(false);
    });

    test('is offline while a topic is still unknown', () => {
      store().setAvailability(entity, bridge!, true);
      expect(isEntityOnline(entity, store().availability)).toBe(false);
    });
  });
});
