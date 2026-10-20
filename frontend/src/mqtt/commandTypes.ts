import type { Entity } from '../store/types';

export type LightCommand = {
  brightness?: number;
  color?: { hex: string };
  color_temp?: number;
  state?: 'ON' | 'OFF';
};

export type Commandable = Extract<Entity, { commandTopic: string }>;

export type Commands = { light: LightCommand }; // later: switch: SwitchCommand
