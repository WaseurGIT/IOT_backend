var express = require('express');
var router = express.Router();

/**
 * Get car status
 */
router.get('/status', function(req, res) {
  var carStatus = req.app.get('carStatus') || {
    connected: false,
    lastUpdate: null,
    status: 'disconnected',
    lastCommand: null,
    lastCommandTime: null,
  };
  
  var esp32CarClient = req.app.get('esp32CarClient');
  var isConnected = esp32CarClient && esp32CarClient.readyState === 1;
  
  res.json({
    car: {
      ...carStatus,
      connected: isConnected,
    },
    uptime: process.uptime(),
  });
});

/**
 * Control car via REST API (alternative to WebSocket)
 * POST /car/control
 * Body: { "command": "forward|backward|left|right|stop", "speed": 200 }
 */
router.post('/control', function(req, res) {
  try {
    var command = req.body.command;
    var speed = req.body.speed || 200;
    
    // Validate command
    var validCommands = ['forward', 'backward', 'left', 'right', 'stop'];
    if (!command || validCommands.indexOf(command) === -1) {
      return res.status(400).json({
        error: 'Invalid command',
        validCommands: validCommands,
      });
    }
    
    // Get car WebSocket client
    var esp32CarClient = req.app.get('esp32CarClient');
    
    if (!esp32CarClient || esp32CarClient.readyState !== 1) {
      return res.status(503).json({
        error: 'Car not connected',
        message: 'ESP32 car is not connected to the server',
      });
    }
    
    // Send command via WebSocket
    var carCommand = JSON.stringify({
      command: command,
      speed: speed,
    });
    
    esp32CarClient.send(carCommand);
    
    // Update car status
    var carStatus = req.app.get('carStatus') || {};
    carStatus.lastCommand = command;
    carStatus.lastCommandTime = new Date();
    req.app.set('carStatus', carStatus);
    
    console.log('📤 Car command sent via REST API:', command, 'speed:', speed);
    
    res.json({
      success: true,
      command: command,
      speed: speed,
      timestamp: new Date(),
    });
  } catch (error) {
    console.error('Error controlling car:', error);
    res.status(500).json({
      error: 'Failed to control car',
      message: error.message,
    });
  }
});

module.exports = router;

