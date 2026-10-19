
import './App.css';
import { startMqtt } from './mqtt/client';

function App() {
  startMqtt();
  return (
    <div></div>
  );
}

export default App;
