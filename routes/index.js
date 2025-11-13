var express = require('express');
var router = express.Router();

/* GET home page - API Status */
router.get('/', function(req, res, next) {
  var esp32Status = req.app.get('esp32Status');
  var connectedClients = req.app.get('io') ? req.app.get('io').engine.clientsCount : 0;
  
  res.json({
    status: 'online',
    message: 'ESP32-CAM Socket.IO Backend',
    esp32: esp32Status,
    clients: connectedClients,
    uptime: process.uptime(),
    endpoints: {
      frame: 'POST /frame - ESP32 sends frames here',
      snapshot: 'GET /snapshot - Get latest frame',
      capture: 'POST /capture - Save current frame',
      images: 'GET /images - List saved images',
      status: 'GET /status - Get system status',
      socketio: 'Connect to Socket.IO for real-time streaming',
    }
  });
});

module.exports = router;
