/**
 * Unified IoT WebSocket Service
 * Single connection to /ws endpoint for both camera frames and car control
 * 
 * This is the RECOMMENDED approach for web/mobile clients.
 * Simpler than managing separate camera and car connections.
 * 
 * Usage:
 *   import { iotSocket } from './IoTSocketService';
 *   
 *   iotSocket.connect('http://192.168.0.115:3000');
 *   iotSocket.on('frame', (data) => setImage(data.image));
 *   iotSocket.sendCarCommand('forward', 200);
 */

export interface IoTMessage {
  type: string;
  [key: string]: any;
}

class IoTSocketService {
  private socket: WebSocket | null = null;
  private listeners: Map<string, Function[]> = new Map();
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  private reconnectTimeout: ReturnType<typeof setTimeout> | null = null;
  private reconnectDelay = 2000;
  private wsUrl: string = '';

  /**
   * Convert HTTP/HTTPS URL to WebSocket URL
   */
  private convertToWebSocketUrl(backendUrl: string): string {
    try {
      const url = new URL(backendUrl);
      const protocol = url.protocol === 'https:' ? 'wss:' : 'ws:';
      return `${protocol}//${url.host}/ws`; // Web client endpoint
    } catch (error) {
      if (backendUrl.startsWith('http://')) {
        return backendUrl.replace('http://', 'ws://') + '/ws';
      } else if (backendUrl.startsWith('https://')) {
        return backendUrl.replace('https://', 'wss://') + '/ws';
      } else if (backendUrl.startsWith('ws://') || backendUrl.startsWith('wss://')) {
        return backendUrl.endsWith('/ws') ? backendUrl : backendUrl + '/ws';
      }
      return `ws://${backendUrl}/ws`;
    }
  }

  /**
   * Connect to backend WebSocket
   * @param backendUrl - Backend URL (http:// or https://)
   */
  connect(backendUrl: string) {
    if (this.socket?.readyState === WebSocket.OPEN) {
      console.log('🔌 Already connected to WebSocket');
      return this.socket;
    }

    this.wsUrl = this.convertToWebSocketUrl(backendUrl);
    console.log(`🔌 Connecting to: ${this.wsUrl}`);

    this.connectWebSocket();
    return this.socket;
  }

  private connectWebSocket() {
    try {
      if (this.socket) {
        this.socket.close();
        this.socket = null;
      }

      const ws = new WebSocket(this.wsUrl);
      this.socket = ws;

      ws.onopen = () => {
        console.log('✅ WebSocket connected');
        this.reconnectAttempts = 0;
        
        if (this.reconnectTimeout) {
          clearTimeout(this.reconnectTimeout);
          this.reconnectTimeout = null;
        }
        
        this.emit('connected', true);
      };

      ws.onmessage = (event) => {
        try {
          const message: IoTMessage = JSON.parse(event.data);
          
          switch (message.type) {
            case 'frame':
              // Camera frame (base64 JPEG)
              this.emit('frame', {
                image: message.image,
                timestamp: message.timestamp || Date.now()
              });
              break;
              
            case 'camera_status':
              // ESP32 camera status
              this.emit('camera_status', message.data);
              break;
              
            case 'camera_connected':
              // ESP32-CAM device connected
              console.log('📷 ESP32-CAM online:', message.device);
              this.emit('camera_connected', {
                device: message.device
              });
              break;
              
            case 'camera_disconnected':
              // ESP32-CAM device disconnected
              console.log('📷 ESP32-CAM offline');
              this.emit('camera_disconnected', true);
              break;
              
            case 'car_status':
              // ESP32 car status
              this.emit('car_status', message.data);
              break;
              
            case 'car_connected':
              // ESP32 car device connected
              console.log('🚗 ESP32 car online:', message.device, message.status);
              this.emit('car_connected', {
                device: message.device,
                status: message.status
              });
              break;
              
            case 'car_disconnected':
              // ESP32 car device disconnected
              console.log('🚗 ESP32 car offline');
              this.emit('car_disconnected', true);
              break;
              
            case 'car_ack':
              // Car command acknowledgment
              console.log('✅ Car ACK:', message.command, message.status);
              this.emit('car_ack', {
                command: message.command,
                status: message.status
              });
              break;
              
            case 'error':
              // Server error
              console.error('❌ Server error:', message.message);
              this.emit('error', message.message);
              break;
              
            default:
              console.log('📨 Unknown message type:', message.type);
          }
        } catch (error) {
          console.error('❌ Failed to parse WebSocket message:', error);
        }
      };

      ws.onerror = (error) => {
        console.error('🚨 WebSocket error:', error);
        this.emit('error', error);
      };

      ws.onclose = (event) => {
        console.log('❌ WebSocket disconnected', event.code, event.reason);
        this.emit('connected', false);
        
        // Auto-reconnect if not manually closed
        if (event.code !== 1000) {
          this.scheduleReconnect();
        }
      };
    } catch (error) {
      console.error('❌ Failed to create WebSocket:', error);
      this.emit('error', error);
      this.scheduleReconnect();
    }
  }

  private scheduleReconnect() {
    if (this.reconnectAttempts >= this.maxReconnectAttempts) {
      console.error('❌ Max reconnection attempts reached');
      this.emit('error', new Error('Max reconnection attempts reached'));
      return;
    }

    this.reconnectAttempts++;
    console.log(`🔄 Reconnecting... (attempt ${this.reconnectAttempts}/${this.maxReconnectAttempts})`);
    
    if (this.reconnectTimeout) {
      clearTimeout(this.reconnectTimeout);
    }
    
    this.reconnectTimeout = setTimeout(() => {
      this.connectWebSocket();
    }, this.reconnectDelay);
  }

  /**
   * Send car control command
   * @param command - Command ('forward', 'backward', 'left', 'right', 'stop')
   * @param speed - Motor speed (0-255, default: 200)
   * @returns true if sent successfully
   */
  sendCarCommand(command: string, speed: number = 200): boolean {
    if (!this.isConnected()) {
      console.error('❌ Cannot send command: WebSocket not connected');
      this.emit('error', new Error('WebSocket not connected'));
      return false;
    }

    try {
      this.socket?.send(JSON.stringify({
        command,
        speed,
        timestamp: Date.now()
      }));
      console.log(`📤 Sent command: ${command.toUpperCase()} (speed: ${speed})`);
      return true;
    } catch (error) {
      console.error('❌ Failed to send command:', error);
      this.emit('error', error);
      return false;
    }
  }

  /**
   * Convenience methods for car control
   */
  moveForward(speed: number = 200): boolean {
    return this.sendCarCommand('forward', speed);
  }

  moveBackward(speed: number = 200): boolean {
    return this.sendCarCommand('backward', speed);
  }

  turnLeft(speed: number = 200): boolean {
    return this.sendCarCommand('left', speed);
  }

  turnRight(speed: number = 200): boolean {
    return this.sendCarCommand('right', speed);
  }

  stop(): boolean {
    return this.sendCarCommand('stop', 0);
  }

  /**
   * Register event listener
   * 
   * Events:
   * - connected(status: boolean) - WebSocket connection status
   * - frame({ image: string, timestamp: number }) - Camera frame
   * - camera_connected({ device: string }) - ESP32-CAM online
   * - camera_disconnected() - ESP32-CAM offline
   * - camera_status(data: object) - Camera status update
   * - car_connected({ device: string, status: string }) - ESP32 car online
   * - car_disconnected() - ESP32 car offline
   * - car_status(data: object) - Car status update
   * - car_ack({ command: string, status: string }) - Command acknowledged
   * - error(error: any) - Error occurred
   */
  on(event: string, callback: Function) {
    if (!this.listeners.has(event)) {
      this.listeners.set(event, []);
    }
    this.listeners.get(event)?.push(callback);
  }

  /**
   * Unregister event listener
   */
  off(event: string, callback?: Function) {
    if (!callback) {
      this.listeners.delete(event);
    } else {
      const callbacks = this.listeners.get(event);
      if (callbacks) {
        const index = callbacks.indexOf(callback);
        if (index > -1) {
          callbacks.splice(index, 1);
        }
      }
    }
  }

  /**
   * Emit event to listeners
   */
  private emit(event: string, data: any) {
    const callbacks = this.listeners.get(event);
    if (callbacks) {
      callbacks.forEach(callback => callback(data));
    }
  }

  /**
   * Disconnect from WebSocket
   */
  disconnect() {
    if (this.socket) {
      console.log('🔌 Disconnecting WebSocket...');
      
      if (this.reconnectTimeout) {
        clearTimeout(this.reconnectTimeout);
        this.reconnectTimeout = null;
      }
      
      this.socket.close(1000, 'Manual disconnect');
      this.socket = null;
    }
    this.listeners.clear();
    this.reconnectAttempts = 0;
  }

  /**
   * Check if WebSocket is connected
   */
  isConnected(): boolean {
    return this.socket?.readyState === WebSocket.OPEN;
  }

  /**
   * Get WebSocket instance
   */
  getSocket(): WebSocket | null {
    return this.socket;
  }

  /**
   * Reset reconnection attempts
   */
  resetReconnectAttempts() {
    this.reconnectAttempts = 0;
  }
}

// Export singleton instance
export const iotSocket = new IoTSocketService();


