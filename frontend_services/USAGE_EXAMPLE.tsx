/**
 * Complete Usage Example: Camera + Car Control
 * React/React Native Component
 */

import React, { useEffect, useState } from 'react';
import { cameraSocket } from './CameraSocketService';
import { carSocket } from './CarSocketService';

// Backend URL - Change this to your server IP
const BACKEND_URL = 'http://192.168.0.191:3000';

export default function IoTController() {
  // State
  const [cameraConnected, setCameraConnected] = useState(false);
  const [carConnected, setCarConnected] = useState(false);
  const [esp32CameraOnline, setEsp32CameraOnline] = useState(false);
  const [esp32CarOnline, setEsp32CarOnline] = useState(false);
  const [currentFrame, setCurrentFrame] = useState<string | null>(null);
  const [lastCommand, setLastCommand] = useState<string>('');
  const [frameCount, setFrameCount] = useState(0);

  useEffect(() => {
    // ===== CAMERA SETUP ===== //
    
    // Connect to camera endpoint
    cameraSocket.connect(BACKEND_URL);

    // Listen for camera WebSocket connection status
    cameraSocket.on('connected', (status) => {
      console.log('📷 Camera WebSocket:', status ? 'Connected' : 'Disconnected');
      setCameraConnected(status);
    });

    // Listen for ESP32-CAM device connection
    cameraSocket.on('device_connected', (data) => {
      console.log('✅ ESP32-CAM device connected:', data.device);
      setEsp32CameraOnline(true);
    });

    cameraSocket.on('device_disconnected', () => {
      console.log('❌ ESP32-CAM device disconnected');
      setEsp32CameraOnline(false);
    });

    // Listen for frames
    cameraSocket.on('frame', (data) => {
      const { image, timestamp } = data;
      setCurrentFrame(`data:image/jpeg;base64,${image}`);
      setFrameCount(prev => prev + 1);
    });

    // Handle camera errors
    cameraSocket.on('error', (error) => {
      console.error('📷 Camera error:', error);
    });

    // ===== CAR SETUP ===== //
    
    // Connect to car endpoint
    carSocket.connect(BACKEND_URL);

    // Listen for car WebSocket connection status
    carSocket.on('connected', (status) => {
      console.log('🚗 Car WebSocket:', status ? 'Connected' : 'Disconnected');
      setCarConnected(status);
    });

    // Listen for ESP32 car device connection
    carSocket.on('device_connected', (data) => {
      console.log('✅ ESP32 car device connected:', data.device, data.status);
      setEsp32CarOnline(true);
    });

    carSocket.on('device_disconnected', () => {
      console.log('❌ ESP32 car device disconnected');
      setEsp32CarOnline(false);
    });

    // Listen for command acknowledgments
    carSocket.on('acknowledgment', (data) => {
      console.log('✅ Command acknowledged:', data.command, data.status);
    });

    // Handle car errors
    carSocket.on('error', (error) => {
      console.error('🚗 Car error:', error);
    });

    // Cleanup on unmount
    return () => {
      cameraSocket.disconnect();
      carSocket.disconnect();
    };
  }, []);

  // ===== CAR CONTROL FUNCTIONS ===== //

  const sendCarCommand = (command: string, speed: number = 200) => {
    if (!carConnected) {
      alert('Car WebSocket not connected!');
      return;
    }

    if (!esp32CarOnline) {
      alert('ESP32 car not online!');
      return;
    }

    const success = carSocket.sendCommand(command as any, speed);
    if (success) {
      setLastCommand(command.toUpperCase());
    } else {
      alert('Failed to send command');
    }
  };

  const handleMoveForward = () => sendCarCommand('forward', 200);
  const handleMoveBackward = () => sendCarCommand('backward', 200);
  const handleTurnLeft = () => sendCarCommand('left', 180);
  const handleTurnRight = () => sendCarCommand('right', 180);
  const handleStop = () => sendCarCommand('stop', 0);

  // ===== RENDER ===== //

  return (
    <div style={{ padding: 20 }}>
      <h1>🚗 IoT Car Controller</h1>

      {/* Connection Status */}
      <div style={{ marginBottom: 20 }}>
        <h3>Connection Status</h3>
        <div>
          📷 Camera WebSocket: 
          <span style={{ color: cameraConnected ? 'green' : 'red', marginLeft: 10 }}>
            {cameraConnected ? '✅ Connected' : '❌ Disconnected'}
          </span>
        </div>
        <div>
          🚗 Car WebSocket: 
          <span style={{ color: carConnected ? 'green' : 'red', marginLeft: 10 }}>
            {carConnected ? '✅ Connected' : '❌ Disconnected'}
          </span>
        </div>
        <div>
          📹 ESP32-CAM Device: 
          <span style={{ color: esp32CameraOnline ? 'green' : 'red', marginLeft: 10 }}>
            {esp32CameraOnline ? '✅ Online' : '❌ Offline'}
          </span>
        </div>
        <div>
          🚙 ESP32 Car Device: 
          <span style={{ color: esp32CarOnline ? 'green' : 'red', marginLeft: 10 }}>
            {esp32CarOnline ? '✅ Online' : '❌ Offline'}
          </span>
        </div>
      </div>

      {/* Camera Feed */}
      <div style={{ marginBottom: 20 }}>
        <h3>📷 Camera Feed</h3>
        <div>Frames received: {frameCount}</div>
        {currentFrame ? (
          <img 
            src={currentFrame} 
            alt="ESP32-CAM Feed" 
            style={{ 
              width: '100%', 
              maxWidth: 640, 
              border: '2px solid #333',
              borderRadius: 8 
            }} 
          />
        ) : (
          <div style={{ 
            width: '100%', 
            maxWidth: 640, 
            height: 480, 
            backgroundColor: '#f0f0f0',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            border: '2px dashed #ccc',
            borderRadius: 8
          }}>
            {esp32CameraOnline ? 'Waiting for frames...' : 'ESP32-CAM not connected'}
          </div>
        )}
      </div>

      {/* Car Controls */}
      <div>
        <h3>🎮 Car Controls</h3>
        <div style={{ marginBottom: 10 }}>
          Last Command: <strong>{lastCommand || 'None'}</strong>
        </div>
        
        {/* D-Pad Style Controls */}
        <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'center', gap: 10 }}>
          {/* Forward */}
          <button
            onClick={handleMoveForward}
            disabled={!carConnected || !esp32CarOnline}
            style={buttonStyle}
          >
            ⬆️ FORWARD
          </button>

          {/* Left, Stop, Right */}
          <div style={{ display: 'flex', gap: 10 }}>
            <button
              onClick={handleTurnLeft}
              disabled={!carConnected || !esp32CarOnline}
              style={buttonStyle}
            >
              ⬅️ LEFT
            </button>
            <button
              onClick={handleStop}
              disabled={!carConnected || !esp32CarOnline}
              style={{ ...buttonStyle, backgroundColor: '#dc3545' }}
            >
              🛑 STOP
            </button>
            <button
              onClick={handleTurnRight}
              disabled={!carConnected || !esp32CarOnline}
              style={buttonStyle}
            >
              ➡️ RIGHT
            </button>
          </div>

          {/* Backward */}
          <button
            onClick={handleMoveBackward}
            disabled={!carConnected || !esp32CarOnline}
            style={buttonStyle}
          >
            ⬇️ BACKWARD
          </button>
        </div>
      </div>

      {/* Debug Info */}
      <div style={{ marginTop: 30, padding: 15, backgroundColor: '#f8f9fa', borderRadius: 8 }}>
        <h4>🔧 Debug Info</h4>
        <pre style={{ fontSize: 12 }}>
          {JSON.stringify({
            backend: BACKEND_URL,
            cameraEndpoint: `${BACKEND_URL.replace('http', 'ws')}/ws/camera`,
            carEndpoint: `${BACKEND_URL.replace('http', 'ws')}/ws/car`,
            cameraWS: cameraConnected,
            carWS: carConnected,
            esp32Cam: esp32CameraOnline,
            esp32Car: esp32CarOnline,
            frames: frameCount
          }, null, 2)}
        </pre>
      </div>
    </div>
  );
}

// Button style
const buttonStyle: React.CSSProperties = {
  padding: '15px 30px',
  fontSize: 18,
  fontWeight: 'bold',
  border: 'none',
  borderRadius: 8,
  backgroundColor: '#007bff',
  color: 'white',
  cursor: 'pointer',
  minWidth: 120,
  transition: 'all 0.2s',
};

// ===== REACT NATIVE VERSION ===== //

/**
 * For React Native, replace <div> with <View>, <button> with <TouchableOpacity>, etc.
 * 
 * Example:
 */

/*
import { View, Text, Image, TouchableOpacity, StyleSheet } from 'react-native';

export default function IoTController() {
  // ... same logic as above ...

  return (
    <View style={styles.container}>
      <Text style={styles.title}>🚗 IoT Car Controller</Text>

      {/* Camera Feed *\/}
      {currentFrame ? (
        <Image 
          source={{ uri: currentFrame }} 
          style={styles.cameraFeed}
        />
      ) : (
        <View style={styles.placeholder}>
          <Text>Waiting for camera...</Text>
        </View>
      )}

      {/* Controls *\/}
      <View style={styles.controls}>
        <TouchableOpacity 
          style={styles.button}
          onPress={handleMoveForward}
          disabled={!carConnected || !esp32CarOnline}
        >
          <Text style={styles.buttonText}>⬆️ FORWARD</Text>
        </TouchableOpacity>

        <View style={styles.row}>
          <TouchableOpacity 
            style={styles.button}
            onPress={handleTurnLeft}
            disabled={!carConnected || !esp32CarOnline}
          >
            <Text style={styles.buttonText}>⬅️ LEFT</Text>
          </TouchableOpacity>

          <TouchableOpacity 
            style={[styles.button, styles.stopButton]}
            onPress={handleStop}
            disabled={!carConnected || !esp32CarOnline}
          >
            <Text style={styles.buttonText}>🛑 STOP</Text>
          </TouchableOpacity>

          <TouchableOpacity 
            style={styles.button}
            onPress={handleTurnRight}
            disabled={!carConnected || !esp32CarOnline}
          >
            <Text style={styles.buttonText}>➡️ RIGHT</Text>
          </TouchableOpacity>
        </View>

        <TouchableOpacity 
          style={styles.button}
          onPress={handleMoveBackward}
          disabled={!carConnected || !esp32CarOnline}
        >
          <Text style={styles.buttonText}>⬇️ BACKWARD</Text>
        </TouchableOpacity>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 20,
    backgroundColor: '#fff',
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    marginBottom: 20,
  },
  cameraFeed: {
    width: '100%',
    height: 300,
    borderRadius: 10,
    marginBottom: 20,
  },
  placeholder: {
    width: '100%',
    height: 300,
    backgroundColor: '#f0f0f0',
    justifyContent: 'center',
    alignItems: 'center',
    borderRadius: 10,
    marginBottom: 20,
  },
  controls: {
    alignItems: 'center',
    gap: 10,
  },
  row: {
    flexDirection: 'row',
    gap: 10,
  },
  button: {
    backgroundColor: '#007bff',
    padding: 15,
    borderRadius: 10,
    minWidth: 100,
    alignItems: 'center',
  },
  stopButton: {
    backgroundColor: '#dc3545',
  },
  buttonText: {
    color: '#fff',
    fontSize: 16,
    fontWeight: 'bold',
  },
});
*/

