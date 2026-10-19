import mqtt from 'mqtt';

import { useDevicesStore } from '../store/devices';
import { isEntityKind } from '../store/types';
import { configParsers, parseAvailability, stateParsers } from '../store/utils/discovery';
import type { Commandable, Commands } from './commandTypes';

const HA_TOPIC = 'homeassistant/';
const CONFIG_SUFFIX = '/config';
const url = `ws://${window.location.hostname}:9001`;

const { upsertEntity, removeEntity, applyState, setAvailability, setConnected } =
  useDevicesStore.getState();

let client: mqtt.MqttClient | undefined;

const isDiscoveryTopic = (topic: string) =>
  topic.startsWith(HA_TOPIC) && topic.endsWith(CONFIG_SUFFIX);

const handleDiscoveryMessage = (topic: string, payload: string) => {
  if (payload === '') {
    removeEntity(topic);
    return;
  }

  const [, kind] = topic.split('/');
  if (!isEntityKind(kind)) return;
  const entity = configParsers[kind]({ configTopic: topic, payload });

  if (entity) {
    upsertEntity(entity);
    client?.subscribe([entity.stateTopic, ...entity.availabilityTopics]);
  }
};

const onConnect = () => {
  setConnected(true);
  client?.subscribe(`${HA_TOPIC}#`);
};

const onMessage = (topic: string, message: Buffer) => {
  const payload = message.toString();

  if (isDiscoveryTopic(topic)) {
    handleDiscoveryMessage(topic, payload);
    return;
  }

  const entities = Object.values(useDevicesStore.getState().entities);

  const stateEntity = entities.find((e) => e.stateTopic === topic);

  if (stateEntity) {
    const deviceState = stateParsers[stateEntity.kind](payload);
    if (deviceState) {
      applyState(stateEntity, deviceState);
    }
  }

  const availabilityEntities = entities.filter((e) => e.availabilityTopics.includes(topic));
  if (availabilityEntities.length > 0) {
    const online = parseAvailability(payload);
    if (online !== null) {
      availabilityEntities.forEach((entity) => {
        setAvailability(entity, topic, online);
      });
    }
  }
};

const onClose = () => {
  setConnected(false);
};

export const startMqtt = () => {
  client = mqtt.connect(url);

  client.on('connect', onConnect);

  client.on('message', onMessage);

  client.on('close', onClose);

  client.on('error', (error) => {
    console.error('MQTT error:', error);
  });
};

export const sendCommand = <E extends Commandable>(entity: E, command: Commands[E['kind']]) => {
  if (!client?.connected) {
    return;
  }

  client.publish(entity.commandTopic, JSON.stringify(command));
};

import.meta.hot?.dispose(() => client?.end());
