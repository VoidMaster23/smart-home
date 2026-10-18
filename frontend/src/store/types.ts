type Device = {
  id: string;
  displayName: string;
  deviceId: string;
  stateTopic: string;
  availabilityTopic?: string;
  commandTopic: string;
};

const COLOR_MODES = [
  'xy',
  'color_temp',
  'hs',
  'rgb',
  'rgbw',
  'rgbww',
  'brightness',
  'white',
  'onoff',
] as const;
export type ColorMode = (typeof COLOR_MODES)[number];
export const isColorMode = (value: unknown): value is ColorMode => {
  return typeof value === 'string' && (COLOR_MODES as readonly string[]).includes(value);
};

export type LightState = {
  state?: 'ON' | 'OFF';
  brightness?: number;
  color_mode?: ColorMode;
  color_temp?: number;
  color?: { x?: number; y?: number; h?: number; s?: number };
};

export type Light = Device & {
  supportsBrightness: boolean;
  brightnessScale: number;
  minMireds?: number;
  maxMireds?: number;
  colorModes: ColorMode[];
};
