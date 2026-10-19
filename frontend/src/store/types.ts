import * as z from 'zod';

export const LightConfigSchema = z.object({
  availability: z.array(z.object({ topic: z.string() })).default([]),
  brightness: z.boolean().default(false),
  brightness_scale: z.number().default(255),
  command_topic: z.string(),
  device: z
    .object({
      identifiers: z.union([z.string(), z.array(z.string())]).optional(),
      name: z.string().optional(),
    })
    .optional(),
  max_mireds: z.number().optional(),
  min_mireds: z.number().optional(),
  state_topic: z.string(),
  // Accept any strings, then drop unknown modes: one odd mode must not reject the whole light.
  supported_color_modes: z
    .array(z.string())
    .default([])
    .transform((modes) => modes.filter(isColorMode)),
  unique_id: z.string().optional(),
});

export const AvailabilitySchema = z.object({ state: z.enum(['online', 'offline']) });

type Device = {
  id: string;
  displayName: string;
  deviceId: string;
  stateTopic: string;
  availabilityTopics: string[];
  commandTopic: string;
  configTopic: string;
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

export const LightStateSchema = z
  .object({
    brightness: z.number().optional(),
    color: z
      .object({
        h: z.number().optional(),
        s: z.number().optional(),
        x: z.number().optional(),
        y: z.number().optional(),
      })
      .optional(),
    color_mode: z.enum(COLOR_MODES).optional().catch(undefined),
    color_temp: z.number().optional(),
    state: z.enum(['ON', 'OFF']).optional(),
  })
  .transform(({ color_mode, ...rest }) => {
    return color_mode === undefined ? rest : { color_mode, ...rest };
  });

export type Light = Device & {
  supportsBrightness: boolean;
  brightnessScale: number;
  minMireds?: number;
  maxMireds?: number;
  colorModes: ColorMode[];
};
