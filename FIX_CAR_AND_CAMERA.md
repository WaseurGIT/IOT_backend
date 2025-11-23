# Fix: Car Not Moving + No Video

## 🔴 Your Problems

1. **Car not receiving commands** - ESP32 serial monitor shows no "📨 Car received" messages
2. **No video in app** - Camera sends frames but app doesn't display them

## 🔍 Root Cause

Your frontend is still using **WRONG services**:
- ❌ `CameraSocketService` → connects to `/ws/camera` (ESP32 device endpoint)
- ❌ `CarSocketService` → connects to `/ws/car` (ESP32 device endpoint)

**Should use:**
- ✅ `IoTSocketService` → connects to `/ws` (web client endpoint)

---

## ✅ Complete Fix

### Step 1: Replace Your Frontend Services

**❌ REMOVE THESE:**
```typescript
import { cameraSocket } from './CameraSocketService';
import { carSocket } from './CarSocketService';

cameraSocket.connect('http://192.168.0.115:3000');
carSocket.connect('http://192.168.0.115:3000');
```

**✅ USE THIS INSTEAD:**
```typescript
import { iotSocket } from './services/IoTSocketService';

// Single connection to /ws endpoint
const BACKEND_URL = 'http://192.168.0.115:3000';
iotSocket.connect(BACKEND_URL);
```

### Step 2: Update Your Component

```typescript
import React, { useEffect, useState } from 'react';
import { View, Image, TouchableOpacity, Text, StyleSheet } from 'react-native';
import { iotSocket } from './services/IoTSocketService';

const BACKEND_URL = 'http://192.168.0.115:3000';  // Your computer's IP

export default function CarController() {
  const [connected, setConnected] = useState(false);
  const [currentFrame, setCurrentFrame] = useState<string | null>(null);
  const [esp32CamOnline, setEsp32CamOnline] = useState(false);
  const [esp32CarOnline, setEsp32CarOnline] = useState(false);

  useEffect(() => {
    console.log('🔌 Connecting to:', BACKEND_URL);
    
    // Connect to /ws (web client endpoint)
    iotSocket.connect(BACKEND_URL);

    // ===== Connection Status =====
    iotSocket.on('connected', (status: boolean) => {
      console.log('📡 WebSocket:', status ? 'Connected ✅' : 'Disconnected ❌');
      setConnected(status);
    });

    // ===== Camera Events =====
    iotSocket.on('frame', (data: { image: string; timestamp: number }) => {
      console.log('📸 Frame received!', data.image.length, 'bytes');
      setCurrentFrame(`data:image/jpeg;base64,${data.image}`);
    });

    iotSocket.on('camera_connected', (data: { device: string }) => {
      console.log('✅ ESP32-CAM online:', data.device);
      setEsp32CamOnline(true);
    });

    iotSocket.on('camera_disconnected', () => {
      console.log('❌ ESP32-CAM offline');
      setEsp32CamOnline(false);
    });

    // ===== Car Events =====
    iotSocket.on('car_connected', (data: { device: string; status: string }) => {
      console.log('✅ ESP32 car online:', data.device);
      setEsp32CarOnline(true);
    });

    iotSocket.on('car_disconnected', () => {
      console.log('❌ ESP32 car offline');
      setEsp32CarOnline(false);
    });

    iotSocket.on('car_ack', (data: { command: string; status: string }) => {
      console.log('✅ Command executed:', data.command);
    });

    // ===== Errors =====
    iotSocket.on('error', (error: any) => {
      console.error('❌ Error:', error);
    });

    // Cleanup
    return () => {
      iotSocket.disconnect();
    };
  }, []);

  // ===== Send Car Commands =====
  const sendCommand = (command: string, speed: number = 200) => {
    if (!connected) {
      console.warn('⚠️ Not connected');
      return;
    }
    if (!esp32CarOnline) {
      console.warn('⚠️ ESP32 car not online');
      return;
    }

    console.log(`📤 Sending command: ${command}`);
    iotSocket.sendCarCommand(command, speed);
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>🚗 CropGuardian</Text>

      {/* Status */}
      <View style={styles.status}>
        <Text>WebSocket: {connected ? '✅' : '❌'}</Text>
        <Text>ESP32-CAM: {esp32CamOnline ? '✅' : '❌'}</Text>
        <Text>ESP32 Car: {esp32CarOnline ? '✅' : '❌'}</Text>
        <Text>{BACKEND_URL}</Text>
      </View>

      {/* Camera Feed */}
      {currentFrame ? (
        <Image source={{ uri: currentFrame }} style={styles.camera} />
      ) : (
        <View style={styles.placeholder}>
          <Text>
            {!connected ? 'Connecting...' : 
             !esp32CamOnline ? 'Waiting for ESP32-CAM...' : 
             'Waiting for frames...'}
          </Text>
        </View>
      )}

      {/* Controls */}
      <View style={styles.controls}>
        <TouchableOpacity
          style={[styles.button, (!connected || !esp32CarOnline) && styles.disabled]}
          onPress={() => sendCommand('forward', 200)}
          disabled={!connected || !esp32CarOnline}
        >
          <Text style={styles.buttonText}>⬆️ FORWARD</Text>
        </TouchableOpacity>

        <View style={styles.row}>
          <TouchableOpacity
            style={[styles.button, styles.small, (!connected || !esp32CarOnline) && styles.disabled]}
            onPress={() => sendCommand('left', 180)}
            disabled={!connected || !esp32CarOnline}
          >
            <Text style={styles.buttonText}>⬅️ LEFT</Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={[styles.button, styles.small, styles.stop, (!connected || !esp32CarOnline) && styles.disabled]}
            onPress={() => sendCommand('stop', 0)}
            disabled={!connected || !esp32CarOnline}
          >
            <Text style={styles.buttonText}>🛑 STOP</Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={[styles.button, styles.small, (!connected || !esp32CarOnline) && styles.disabled]}
            onPress={() => sendCommand('right', 180)}
            disabled={!connected || !esp32CarOnline}
          >
            <Text style={styles.buttonText}>➡️ RIGHT</Text>
          </TouchableOpacity>
        </View>

        <TouchableOpacity
          style={[styles.button, (!connected || !esp32CarOnline) && styles.disabled]}
          onPress={() => sendCommand('backward', 200)}
          disabled={!connected || !esp32CarOnline}
        >
          <Text style={styles.buttonText}>⬇️ BACKWARD</Text>
        </TouchableOpacity>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, padding: 20, backgroundColor: '#fff' },
  title: { fontSize: 24, fontWeight: 'bold', marginBottom: 15, textAlign: 'center' },
  status: { backgroundColor: '#f0f0f0', padding: 10, borderRadius: 8, marginBottom: 15 },
  camera: { width: '100%', height: 250, borderRadius: 10, marginBottom: 20, backgroundColor: '#000' },
  placeholder: { width: '100%', height: 250, backgroundColor: '#f0f0f0', justifyContent: 'center', alignItems: 'center', borderRadius: 10, marginBottom: 20 },
  controls: { alignItems: 'center', gap: 10 },
  row: { flexDirection: 'row', gap: 10 },
  button: { backgroundColor: '#007bff', padding: 15, borderRadius: 10, minWidth: 120, alignItems: 'center' },
  small: { minWidth: 100 },
  stop: { backgroundColor: '#dc3545' },
  disabled: { backgroundColor: '#ccc', opacity: 0.5 },
  buttonText: { color: '#fff', fontSize: 14, fontWeight: 'bold' },
});
```

---

## 🔍 Debugging Steps

### 1. Check Frontend Logs

After updating to `IoTSocketService`, you should see:

```
🔌 Connecting to: http://192.168.0.115:3000
📡 WebSocket: Connected ✅
✅ ESP32-CAM online: ESP32-CAM
✅ ESP32 car online: car_controller
📸 Frame received! 54321 bytes
```

### 2. Check Backend Logs

When you click forward, backend should show:

```
🔌 WebSocket upgrade request for: /ws
=== NEW WEBSOCKET CONNECTION ===
🌐 Web client registered
📤 Car command sent: forward speed: 200
```

### 3. Check ESP32 Car Serial Monitor

When you click forward, ESP32 should show:

```
📨 Car received: {"command":"forward","speed":200,"timestamp":...}
⚙️  Motor speed set to: 200
⬆️  Moving FORWARD
✅ Sent ACK for: forward
```

**If you DON'T see "📨 Car received":**
- Backend isn't forwarding commands
- Check backend logs for "📤 Car command sent"

**If you DO see "📨 Car received" but no movement:**
- Motor wiring issue
- PWM not configured
- Motor driver not powered

---

## 📊 Command Flow

```
Frontend App
    ↓
Sends: {"command":"forward","speed":200}
    ↓
Backend /ws endpoint (web client)
    ↓
Forwards to: esp32CarClient (ESP32 car on /ws/car)
    ↓
ESP32 Car receives command
    ↓
Executes: moveForward()
    ↓
Sends ACK back
    ↓
Backend forwards ACK to frontend
```

---

## ✅ Checklist

Before testing:

- [ ] Using `IoTSocketService` (NOT CameraSocketService + CarSocketService)
- [ ] Connecting to `/ws` endpoint (not `/ws/camera` or `/ws/car`)
- [ ] Correct IP address (192.168.0.115, not 10.0.2.2)
- [ ] Phone on same WiFi as computer
- [ ] Backend running (`npm start`)
- [ ] ESP32-CAM connected (backend logs show it)
- [ ] ESP32 car connected (backend logs show it)

---

## 🧪 Test Commands

### Test Backend from Phone Browser
```
Open: http://192.168.0.115:3000
Should see: Backend homepage
```

### Test Car Control via REST API
```bash
curl -X POST http://192.168.0.115:3000/car/control \
  -H "Content-Type: application/json" \
  -d '{"command": "forward", "speed": 200}'
```

**ESP32 serial monitor should show:**
```
📨 Car received: {"command":"forward","speed":200}
⬆️  Moving FORWARD
```

If this works, the issue is in your frontend code (wrong services).

---

## 🎯 Summary

**The Problem:**
- Frontend using wrong services (device endpoints instead of web endpoint)
- Commands not reaching ESP32 car
- Frames not reaching frontend

**The Solution:**
- Use `IoTSocketService` (single connection to `/ws`)
- Backend automatically forwards:
  - Frames from ESP32-CAM → Frontend
  - Commands from Frontend → ESP32 car

**After Fix:**
- ✅ Video appears in app
- ✅ Car responds to commands
- ✅ ESP32 serial monitor shows "📨 Car received"

---

**Copy `IoTSocketService.ts` to your project and use it!** 🚀

