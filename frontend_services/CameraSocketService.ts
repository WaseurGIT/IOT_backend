/**
 * Camera WebSocket Service
 * Connects to dedicated camera endpoint: /ws/camera
 * Receives real-time video frames from ESP32-CAM
 */

class CameraSocketService {
  private socket: WebSocket | null = null;
  private listeners: Map<string, Function[]> = new Map();
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  private reconnectTimeout: ReturnType<typeof setTimeout> | null = null;
  private reconnectDelay = 2000; // 2 seconds
  private wsUrl: string = '';

  /**
   * Convert HTTP/HTTPS URL to WebSocket URL for CAMERA endpoint
   * Uses dedicated /ws/camera path for optimal performance
   */
  private convertToWebSocketUrl(backendUrl: string): string {
    try {
      const url = new URL(backendUrl);
      const protocol = url.protocol === 'https:' ? 'wss:' : 'ws:';
      return `${protocol}//${url.host}/ws/camera`; // 📷 Dedicated camera endpoint
    } catch (error) {
      // If URL parsing fails, try simple string replacement
      if (backendUrl.startsWith('http://')) {
        return backendUrl.replace('http://', 'ws://') + '/ws/camera';
      } else if (backendUrl.startsWith('https://')) {
        return backendUrl.replace('https://', 'wss://') + '/ws/camera';
      } else if (backendUrl.startsWith('ws://') || backendUrl.startsWith('wss://')) {
        // Already a WebSocket URL, ensure /ws/camera path
        return backendUrl.replace(/\/ws\/?$/, '') + '/ws/camera';
      }
      // Default to ws://
      return `ws://${backendUrl}/ws/camera`;
    }
  }

  /**
   * Connect to camera WebSocket endpoint
   * @param backendUrl - Backend server URL (http:// or https://)
   */
  connect(backendUrl: string) {
    if (this.socket?.readyState === WebSocket.OPEN) {
      console.log('📷 Already connected to camera WebSocket');
      return this.socket;
    }

    // Convert HTTP URL to WebSocket URL
    this.wsUrl = this.convertToWebSocketUrl(backendUrl);
    console.log(`📷 Connecting camera to: ${this.wsUrl}`);

    this.connectWebSocket();
    return this.socket;
  }

  private connectWebSocket() {
    try {
      // Close existing connection if any
      if (this.socket) {
        this.socket.close();
        this.socket = null;
      }

      const ws = new WebSocket(this.wsUrl);
      this.socket = ws;

      ws.onopen = () => {
        console.log('✅ Camera WebSocket connected to dedicated endpoint');
        this.reconnectAttempts = 0;
        
        // Clear any pending reconnection
        if (this.reconnectTimeout) {
          clearTimeout(this.reconnectTimeout);
          this.reconnectTimeout = null;
        }
        
        this.emit('connected', true);
      };

      ws.onmessage = (event) => {
        try {
          // Server sends JSON messages with a type field
          const message = JSON.parse(event.data);
          
          switch (message.type) {
            case 'frame':
              // Frame data with base64 image
              this.emit('frame', {
                image: message.image,
                timestamp: message.timestamp || Date.now()
              });
              break;
              
            case 'camera_status':
              // ESP32 camera status update
              this.emit('status', message.data);
              break;
              
            case 'camera_connected':
              // ESP32-CAM device connected
              this.emit('device_connected', {
                device: message.device
              });
              break;
              
            case 'camera_disconnected':
              // ESP32-CAM device disconnected
              this.emit('device_disconnected', true);
              break;
              
            default:
              console.log('📨 Unknown camera message type:', message.type);
          }
        } catch (error) {
          console.error('❌ Failed to parse camera WebSocket message:', error);
        }
      };

      ws.onerror = (error) => {
        console.error('🚨 Camera WebSocket error:', error);
        this.emit('error', error);
      };

      ws.onclose = (event) => {
        console.log('❌ Camera WebSocket disconnected', event.code, event.reason);
        this.emit('connected', false);
        
        // Auto-reconnect if not manually closed
        if (event.code !== 1000) { // Not a normal closure
          this.scheduleReconnect();
        }
      };
    } catch (error) {
      console.error('❌ Failed to create camera WebSocket:', error);
      this.emit('error', error);
      this.scheduleReconnect();
    }
  }

  private scheduleReconnect() {
    if (this.reconnectAttempts >= this.maxReconnectAttempts) {
      console.error('❌ Max camera reconnection attempts reached');
      this.emit('error', new Error('Max reconnection attempts reached'));
      return;
    }

    this.reconnectAttempts++;
    console.log(`🔄 Reconnecting camera... (attempt ${this.reconnectAttempts}/${this.maxReconnectAttempts})`);
    
    // Clear any existing timeout
    if (this.reconnectTimeout) {
      clearTimeout(this.reconnectTimeout);
    }
    
    this.reconnectTimeout = setTimeout(() => {
      this.connectWebSocket();
    }, this.reconnectDelay);
  }

  /**
   * Register event listener
   * @param event - Event name ('connected', 'frame', 'status', 'device_connected', 'device_disconnected', 'error')
   * @param callback - Callback function
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

  private emit(event: string, data: any) {
    const callbacks = this.listeners.get(event);
    if (callbacks) {
      callbacks.forEach(callback => callback(data));
    }
  }

  /**
   * Request a single frame (optional - frames are automatically streamed)
   */
  requestFrame() {
    if (this.isConnected()) {
      const message = JSON.stringify({ type: 'requestFrame' });
      this.socket?.send(message);
    }
  }

  /**
   * Capture image and get prediction
   * Note: Use HTTP API endpoint instead for capture with prediction
   * This is here for backwards compatibility
   */
  capture(callback: (result: any) => void) {
    if (this.isConnected()) {
      const message = JSON.stringify({ type: 'capture' });
      this.socket?.send(message);
      
      callback({ success: true, pending: true, message: 'Capture request sent via WebSocket' });
    } else {
      callback({ success: false, error: 'Camera WebSocket not connected' });
    }
  }

  /**
   * Disconnect from camera WebSocket
   */
  disconnect() {
    if (this.socket) {
      console.log('📷 Disconnecting camera WebSocket...');
      
      // Clear reconnection timeout
      if (this.reconnectTimeout) {
        clearTimeout(this.reconnectTimeout);
        this.reconnectTimeout = null;
      }
      
      // Close with normal closure code (1000) to prevent auto-reconnect
      this.socket.close(1000, 'Manual disconnect');
      this.socket = null;
    }
    this.listeners.clear();
    this.reconnectAttempts = 0;
  }

  /**
   * Check if camera WebSocket is connected
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
   * Reset reconnection attempts counter
   */
  resetReconnectAttempts() {
    this.reconnectAttempts = 0;
  }
}

// Export singleton instance
export const cameraSocket = new CameraSocketService();

