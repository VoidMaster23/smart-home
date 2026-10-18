import { isColorMode, type Light } from './types';

export const parseLight = (payload: string): Light | null => {
  let data;
  try {
    data = JSON.parse(payload);
  } catch (error) {
    console.error('Failed to parse light payload:', error);
    return null;
  }

  if (typeof data !== 'object' || data === null) {
    console.error('Invalid light payload format');
    return null;
  }

  if (typeof data.state_topic !== 'string' || typeof data.command_topic !== 'string') {
    console.error('Missing required topics in light payload');
    return null;
  }

  const id = data.unique_id ?? data.state_topic;
  return {
    availabilityTopic: data.availability?.[0]?.topic,
    brightnessScale: data.brightness_scale ?? 255,
    colorModes: Array.isArray(data.supported_color_modes)
      ? data.supported_color_modes.filter(isColorMode)
      : [],
    commandTopic: data.command_topic,
    deviceId: data.device?.identifiers?.[0] ?? id,
    displayName: data.device?.name ?? id,
    id,
    maxMireds: data.max_mireds,
    minMireds: data.min_mireds,
    stateTopic: data.state_topic,
    supportsBrightness: data.brightness === true,
  };
};
