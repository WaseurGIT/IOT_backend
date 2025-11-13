# ESP32-CAM Socket.IO Backend

A real-time video streaming backend for ESP32-CAM using Socket.IO and Express.js.

## 🚀 Features

- ✅ Real-time video streaming via Socket.IO
- ✅ Multiple clients support
- ✅ Frame capture and storage
- ✅ **🌱 AI-Powered Crop Disease Detection**
- ✅ **📋 Treatment Recommendations**
- ✅ FPS monitoring
- ✅ ESP32 connection status tracking
- ✅ CORS enabled for cross-origin requests
- ✅ Image gallery management

## 📦 Installation

```bash
npm install
```

## 🌱 Disease Prediction Setup (Optional)

This backend includes AI-powered crop disease detection! To enable it:

### Quick Setup

```bash
# 1. Install Python dependencies
cd ml_service
python3 -m venv venv
source venv/bin/activate  # Windows: venv\Scripts\activate
pip install -r requirements.txt

# 2. Run without model (dummy predictions for testing)
python app.py
```

### For Real Predictions

1. **Download or train a model** - See [DISEASE_PREDICTION.md](./DISEASE_PREDICTION.md) for details
2. **Place model as** `ml_service/model.h5`
3. **Start both services** using `./start_services.sh` (or start manually)

**📖 Full Documentation**: See [DISEASE_PREDICTION.md](./DISEASE_PREDICTION.md) for:
- Complete setup instructions
- Model training guide
- API documentation
- Supported plants & diseases (38 classes)
- Treatment recommendations

## 🔧 Configuration

Create a `.env` file in the root directory (already created):

```env
PORT=3000
CORS_ORIGIN=*
NODE_ENV=development
```

## 🏃 Running the Server

### Development Mode (with auto-restart)
```bash
npm run dev
```

### Production Mode
```bash
npm start
```

The server will start on `http://localhost:3000`

## 📡 API Endpoints

### Status
- `GET /` - Backend status and available endpoints
- `GET /status` - Detailed ESP32 and client status

### Camera Operations
- `POST /frame` - ESP32 sends frames here (raw image data or base64)
- `GET /snapshot` - Get the latest frame as JPEG
- `POST /capture` - Save the current frame to uploads directory **+ Auto-predict disease**

### Disease Prediction 🌱
- `POST /predict/:filename` - Predict disease from a saved image
- Returns: disease name, confidence, remedies, severity level

### Image Management
- `GET /images` - List all saved images
- `DELETE /images/:filename` - Delete a saved image
- `GET /uploads/:filename` - Serve a saved image

### Socket.IO
Connect to Socket.IO for real-time streaming:
```javascript
const socket = io('http://localhost:3000');

socket.on('frame', (data) => {
  // data.image contains base64 encoded JPEG
  // data.timestamp contains the timestamp
});

socket.on('status', (status) => {
  // ESP32 connection status
});
```

## 🔌 ESP32-CAM Setup

Update your ESP32-CAM code to send frames to this backend:

```cpp
const char* backendURL = "http://YOUR_COMPUTER_IP:3000/frame";
```

Replace `YOUR_COMPUTER_IP` with your computer's local IP address.

### Find Your IP Address
- **Windows**: Run `ipconfig` and look for IPv4 Address
- **Mac/Linux**: Run `ifconfig` or `ip addr`

Example: `192.168.1.105`

## 📱 React Native App Integration

Install Socket.IO client in your React Native app:

```bash
npm install socket.io-client
```

Connect to the backend:

```typescript
import { io } from 'socket.io-client';

const socket = io('http://YOUR_COMPUTER_IP:3000', {
  transports: ['websocket'],
  reconnection: true,
});

socket.on('frame', (data) => {
  setFrameData(`data:image/jpeg;base64,${data.image}`);
});
```

## 📁 Directory Structure

```
iot_backend/
├── app.js                    # Main Express app configuration
├── bin/
│   └── www                  # Server startup with Socket.IO
├── routes/
│   ├── index.js             # Status endpoints
│   ├── users.js             # User routes
│   └── camera.js            # Camera + Disease prediction endpoints
├── ml_service/              # 🌱 AI Disease Detection Service
│   ├── app.py              # Flask ML service
│   ├── train_model.py      # Model training script
│   ├── test_service.py     # Testing utilities
│   ├── requirements.txt    # Python dependencies
│   ├── model.h5            # Trained model (not in git)
│   └── README.md           # ML service documentation
├── uploads/                 # Saved camera images
├── logs/                    # Service logs
├── public/                  # Static files
├── views/                   # Jade templates
├── .env                     # Environment variables
├── start_services.sh        # Startup script for both services
├── ecosystem.config.js      # PM2 configuration
├── DISEASE_PREDICTION.md    # Disease prediction documentation
└── package.json             # Dependencies and scripts
```

## 🧪 Testing

Test the backend with curl:

```bash
# Check status
curl http://localhost:3000/

# Get latest snapshot
curl http://localhost:3000/snapshot --output snapshot.jpg

# List saved images
curl http://localhost:3000/images

# Test disease prediction (if ML service is running)
curl http://localhost:5000/health
curl -X POST http://localhost:3000/camera/capture
curl -X POST http://localhost:3000/camera/predict/ESP32_CAM_2025-11-12T10-30-00-000Z.jpg
```

## 🐛 Troubleshooting

### Backend not receiving frames from ESP32

1. **Check ESP32 serial monitor** for connection status
2. **Verify IP address** in ESP32 code matches your computer's IP
3. **Check firewall** - Allow port 3000 through your firewall
4. **Ensure same network** - ESP32 and computer on same WiFi

### Mobile app not connecting

1. **Use IP address, not localhost** in mobile app
2. **Test backend URL** from mobile browser first
3. **Check CORS settings** in .env file
4. **Disable VPN** if active

### Low FPS / Lag

Adjust ESP32 settings:
```cpp
config.jpeg_quality = 15;  // Increase number (lower quality)
config.frame_size = FRAMESIZE_QVGA;  // Smaller resolution
const int frameInterval = 200;  // Lower FPS
```

## 📊 Performance

Expected performance:
- **FPS**: 8-15 frames per second
- **Latency**: 100-200ms
- **Image Quality**: VGA (640x480) with adjustable JPEG quality
- **Max Clients**: 10+ simultaneous viewers

## 🌐 Deployment

For production deployment, consider:
- **Railway.app** - Free tier available
- **Render.com** - Free tier available  
- **DigitalOcean** - $5/month
- **AWS EC2** - Variable pricing

Update ESP32 and mobile app to use the deployed URL.

## 📝 License

MIT

## 🤝 Contributing

Feel free to submit issues and enhancement requests!

---

**Happy Streaming! 🎉📹**

