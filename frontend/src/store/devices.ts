import { create } from 'zustand';

import type { Light, LightState } from './types';

interface DevicesState {
  connected: boolean;
  lights: Record<string, Light>;
  lightStates: Record<string, LightState>;
  availability: Record<string, Record<string, boolean>>; // light id → topic → online
  setConnected: (connected: boolean) => void;
  upsertLight: (light: Light) => void;
  removeLight: (configTopic: string) => void;
  setAvailability: (light: Light, topic: string, online: boolean) => void;
  applyState: (light: Light, state: LightState) => void;
}

const upsertLight = (state: DevicesState, light: Light) => {
  return {
    lights: {
      ...state.lights,
      [light.id]: light,
    },
  };
};

const removeLight = (state: DevicesState, configTopic: string) => {
  const lightToRemove = Object.values(state.lights).find(
    (light) => light.configTopic === configTopic,
  );
  if (!lightToRemove) {
    return state;
  }

  const { [lightToRemove.id]: _, ...remainingLights } = state.lights;
  const { [lightToRemove.id]: __, ...remainingStates } = state.lightStates;
  const { [lightToRemove.id]: ___, ...remainingAvailability } = state.availability;
  return {
    availability: remainingAvailability,
    lights: remainingLights,
    lightStates: remainingStates,
  };
};

const applyState = (state: DevicesState, light: Light, lightState: LightState) => {
  return {
    lightStates: {
      ...state.lightStates,
      [light.id]: { ...state.lightStates[light.id], ...lightState },
    },
  };
};

export const isLightOnline = (light: Light, availability: DevicesState['availability']) =>
  light.availabilityTopics.every((topic) => availability[light.id]?.[topic] === true);

const setAvailability = (state: DevicesState, light: Light, topic: string, online: boolean) => ({
  availability: {
    ...state.availability,
    [light.id]: { ...state.availability[light.id], [topic]: online },
  },
});

export const useDevicesStore = create<DevicesState>()((set) => ({
  applyState: (light: Light, lightState: LightState) =>
    set((state) => applyState(state, light, lightState)),
  availability: {},
  connected: false,
  lights: {},
  lightStates: {},
  removeLight: (configTopic: string) => set((state) => removeLight(state, configTopic)),
  setAvailability: (light, topic, online) =>
    set((state) => setAvailability(state, light, topic, online)),
  setConnected: (connected: boolean) => set({ connected }),
  upsertLight: (light: Light) => set((state) => upsertLight(state, light)),
}));
