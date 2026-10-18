import { beforeEach, describe, expect, test, vi } from 'vitest';

import { parseLight } from './devices';

// Real discovery payload from Kajplats_Room_Light (Z2M 2.7.1), exactly as received.
const realPayload = `{"availability":[{"topic":"zigbee2mqtt/bridge/state","value_template":"{{ value_json.state }}"}],"brightness":true,"brightness_scale":254,"command_topic":"zigbee2mqtt/Kajplats_Room_Light/set","default_entity_id":"light.kajplats_room_light","device":{"hw_version":1,"identifiers":["zigbee2mqtt_0x00e07291ff75002e"],"manufacturer":"","model":"Automatically generated definition","model_id":"","name":"Kajplats_Room_Light","sw_version":"","via_device":"zigbee2mqtt_bridge_0x00124b0037e739d7"},"effect":true,"effect_list":["blink","breathe","okay","channel_change","finish_effect","stop_effect","colorloop","stop_colorloop"],"max_mireds":555,"min_mireds":153,"name":null,"object_id":"kajplats_room_light","origin":{"name":"Zigbee2MQTT","sw":"2.7.1","url":"https://www.zigbee2mqtt.io"},"schema":"json","state_topic":"zigbee2mqtt/Kajplats_Room_Light","supported_color_modes":["xy","color_temp"],"unique_id":"0x00e07291ff75002e_light_zigbee2mqtt"}`;

const uniqueId = '0x00e07291ff75002e_light_zigbee2mqtt';

const variant = (change: (config: Record<string, unknown>) => void): string => {
    const config = JSON.parse(realPayload) as Record<string, unknown>;
    change(config);
    return JSON.stringify(config);
};

describe('parseLight', () => {
    beforeEach(() => {
        vi.spyOn(console, 'error').mockImplementation(() => { });
    });

    test('parses the real config', () => {
        expect(parseLight(realPayload)).toEqual({
            availabilityTopic: 'zigbee2mqtt/bridge/state',
            brightnessScale: 254,
            colorModes: ['xy', 'color_temp'],
            commandTopic: 'zigbee2mqtt/Kajplats_Room_Light/set',
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
        expect(parseLight(payload)).toBeNull();
    });

    test('returns null when command_topic is missing', () => {
        expect(parseLight(variant((c) => delete c.command_topic))).toBeNull();
    });

    test('falls back to unique_id when device is missing', () => {
        const light = parseLight(variant((c) => delete c.device));
        expect(light?.deviceId).toBe(uniqueId);
        expect(light?.displayName).toBe(uniqueId);
    });

    test('leaves availabilityTopic undefined when availability is missing', () => {
        expect(parseLight(variant((c) => delete c.availability))?.availabilityTopic).toBeUndefined();
    });

    test('drops unknown color modes', () => {
        const payload = variant((c) => (c.supported_color_modes = ['xy', 'disco']));
        expect(parseLight(payload)?.colorModes).toEqual(['xy']);
    });

    test('uses defaults for a minimal on/off light', () => {
        const light = parseLight('{"command_topic":"a/set","state_topic":"a"}');
        expect(light).toMatchObject({
            brightnessScale: 255,
            colorModes: [],
            id: 'a',
            supportsBrightness: false,
        });
        expect(light?.minMireds).toBeUndefined();
    });
});
