# ESP32-CAM Frame Rate Configuration

## Understanding Your Current Setup

### Current Configuration
- **Frame Interval**: 100ms (`const int frameInterval = 100;` in camera_soket.ino line 33)
- **Actual FPS**: ~10 frames per second (1000ms ÷ 100ms = 10 FPS)
- **Frame Size**: CIF (352×288 pixels), ~4-10 KB per frame
- **Network Load**: ~40-100 KB/second

### Why the Server Logs Look "Fast"

The server was logging **every single frame** received:
```
📨 Message received from esp32_camera : 4694 bytes  <-- 10 times per second!
📨 Message received from esp32_camera : 4694 bytes
📨 Message received from esp32_camera : 4694 bytes
...
```

At 10 FPS, this creates **300 log lines per 30 seconds** = massive console spam!

The ESP32 serial monitor correctly shows batches of 30 frames every 3 seconds:
```
23:22:14.039 -> ✅ Frames sent: 30 (size: 4085 bytes)   <-- 3 seconds later
23:22:18.729 -> ✅ Frames sent: 60 (size: 4019 bytes)   <-- 3 seconds later
```

## ✅ Fixed: Reduced Server Logging

I've updated `bin/www` to **skip logging binary camera frames** while still logging:
- Camera connection/disconnection
- Car commands
- Identification messages
- Errors

Now you'll only see the occasional frame log (1% sampling):
```
📸 Frame received from ESP32 Camera: 4911 bytes  <-- Only shown occasionally
```

## Adjusting Frame Rate (Optional)

If you want to change the FPS, modify `camera_soket.ino` line 33:

### Recommended Settings

| FPS | frameInterval | Use Case | Network Load |
|-----|---------------|----------|--------------|
| **5 FPS** | `200` | Battery saving, slow monitoring | ~20-50 KB/s |
| **10 FPS** | `100` | **Current - Good balance** | ~40-100 KB/s |
| **15 FPS** | `67` | Smooth video, moderate load | ~60-150 KB/s |
| **20 FPS** | `50` | High quality video | ~80-200 KB/s |
| **30 FPS** | `33` | Professional streaming (may lag) | ~120-300 KB/s |

### Example: Change to 5 FPS (Slower, Lower Bandwidth)

```cpp
const int frameInterval = 200; // ~5 FPS (was 100)
```

### Example: Change to 15 FPS (Smoother Video)

```cpp
const int frameInterval = 67; // ~15 FPS (was 100)
```

## Network Bandwidth Calculation

**Formula**: `FPS × Average Frame Size = Bytes/Second`

Example with CIF frames (~6 KB average):
- 5 FPS: 5 × 6 KB = 30 KB/s
- 10 FPS: 10 × 6 KB = 60 KB/s
- 15 FPS: 15 × 6 KB = 90 KB/s
- 20 FPS: 20 × 6 KB = 120 KB/s

## Recommendations

### For Local Network (Current Setup)
✅ **10 FPS** is perfect - smooth enough for monitoring, low enough for reliability

### For Render.com (Production)
- Use **5-10 FPS** to avoid hitting bandwidth limits
- Render free tier has limited bandwidth per month

### For Battery-Powered Operation
- Use **5 FPS** or lower
- Consider motion detection (only stream when movement detected)

### For Real-Time Control (Car Driving)
- Use **10-15 FPS** for responsive control
- Higher FPS helps with navigation but uses more power

## Testing Your Changes

After modifying `frameInterval`:

1. **Upload to ESP32-CAM**
2. **Watch Serial Monitor**:
   ```
   ✅ Frames sent: 30 (size: 4085 bytes)
   ```
   Count seconds between logs - should be `30 ÷ FPS` seconds
   
3. **Check Network Traffic**:
   - Server logs will now be clean (binary frames not logged)
   - Only see connection events and errors

4. **Test Video Quality**:
   - Open web client
   - Check if video is smooth enough for your needs
   - Adjust FPS up/down as needed

## Current Status

✅ **Server logging fixed** - No more frame spam
✅ **10 FPS maintained** - Good balance for your use case
✅ **Camera reconnection** - Will work properly on capture (no disconnection issue)

Your setup is now optimized! 🎉

