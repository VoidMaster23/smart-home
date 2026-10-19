import { AvailabilitySchema, LightConfigSchema, LightStateSchema, type Light } from '../types';

type ParseLightParams = {
  configTopic: string;
  payload: string;
};

export const parseLight = ({ configTopic, payload }: ParseLightParams): Light | null => {
  let data;
  try {
    data = JSON.parse(payload);
  } catch (error) {
    console.error('Failed to parse light payload:', error);
    return null;
  }

  const result = LightConfigSchema.safeParse(data);
  if (!result.success) {
    console.error('Invalid light config:', result.error);
    return null;
  }

  const config = result.data;
  const id = config.unique_id ?? config.state_topic;
  const [firstIdentifier] = [config.device?.identifiers].flat();

  return {
    availabilityTopics: config.availability.map(({ topic }) => topic),
    brightnessScale: config.brightness_scale,
    colorModes: config.supported_color_modes,
    commandTopic: config.command_topic,
    configTopic,
    deviceId: firstIdentifier ?? id,
    displayName: config.device?.name ?? id,
    id,
    maxMireds: config.max_mireds,
    minMireds: config.min_mireds,
    stateTopic: config.state_topic,
    supportsBrightness: config.brightness,
  };
};

export const parseLightState = (payload: string) => {
  let data;
  try {
    data = JSON.parse(payload);
  } catch (error) {
    console.error('Failed to parse light state payload:', error);
    return null;
  }

  const result = LightStateSchema.safeParse(data);
  if (!result.success) {
    console.error('Invalid light state:', result.error);
    return null;
  }

  return result.data;
};

export const parseAvailability = (payload: string) => {
  let data;
  try {
    data = JSON.parse(payload);
  } catch (error) {
    console.error('Failed to parse availability payload:', error);
    return null;
  }

  const result = AvailabilitySchema.safeParse(data);
  if (!result.success) {
    console.error('Invalid availability:', result.error);
    return null;
  }
  return result.data.state === 'online';
};
