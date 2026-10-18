import { LightConfigSchema, type Light } from './types';

export const parseLight = (payload: string): Light | null => {
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
        availabilityTopic: config.availability?.[0]?.topic,
        brightnessScale: config.brightness_scale,
        colorModes: config.supported_color_modes,
        commandTopic: config.command_topic,
        deviceId: firstIdentifier ?? id,
        displayName: config.device?.name ?? id,
        id,
        maxMireds: config.max_mireds,
        minMireds: config.min_mireds,
        stateTopic: config.state_topic,
        supportsBrightness: config.brightness,
    };
};
