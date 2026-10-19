import { create } from 'zustand';

import type { Entity, EntityState } from './types';

interface DevicesState {
  connected: boolean;
  entities: Record<string, Entity>;
  entityStates: Record<string, EntityState>;
  availability: Record<string, Record<string, boolean>>; // light id → topic → online
  setConnected: (connected: boolean) => void;
  upsertEntity: (entity: Entity) => void;
  removeEntity: (configTopic: string) => void;
  setAvailability: (entity: Entity, topic: string, online: boolean) => void;
  applyState: (entity: Entity, state: EntityState) => void;
}

const upsertEntity = (state: DevicesState, entity: Entity) => {
  return {
    entities: {
      ...state.entities,
      [entity.id]: entity,
    },
  };
};

const removeEntity = (state: DevicesState, configTopic: string) => {
  const entityToRemove = Object.values(state.entities).find(
    (entity) => entity.configTopic === configTopic,
  );
  if (!entityToRemove) {
    return state;
  }

  const { [entityToRemove.id]: _, ...remainingEntities } = state.entities;
  const { [entityToRemove.id]: __, ...remainingStates } = state.entityStates;
  const { [entityToRemove.id]: ___, ...remainingAvailability } = state.availability;
  return {
    availability: remainingAvailability,
    entities: remainingEntities,
    entityStates: remainingStates,
  };
};

const applyState = (state: DevicesState, entity: Entity, entityState: EntityState) => {
  return {
    entityStates: {
      ...state.entityStates,
      [entity.id]: { ...state.entityStates[entity.id], ...entityState },
    },
  };
};

export const isEntityOnline = (entity: Entity, availability: DevicesState['availability']) =>
  entity.availabilityTopics.every((topic) => availability[entity.id]?.[topic] === true);

const setAvailability = (state: DevicesState, entity: Entity, topic: string, online: boolean) => ({
  availability: {
    ...state.availability,
    [entity.id]: { ...state.availability[entity.id], [topic]: online },
  },
});

export const useDevicesStore = create<DevicesState>()((set) => ({
  applyState: (entity: Entity, entityState: EntityState) =>
    set((state) => applyState(state, entity, entityState)),
  availability: {},
  connected: false,
  entities: {},
  entityStates: {},
  removeEntity: (configTopic: string) => set((state) => removeEntity(state, configTopic)),
  setAvailability: (entity: Entity, topic: string, online: boolean) =>
    set((state) => setAvailability(state, entity, topic, online)),
  setConnected: (connected: boolean) => set({ connected }),
  upsertEntity: (entity: Entity) => set((state) => upsertEntity(state, entity)),
}));
