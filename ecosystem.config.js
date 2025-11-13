module.exports = {
  apps: [
    {
      name: 'iot-backend',
      script: './bin/www',
      instances: 1,
      autorestart: true,
      watch: false,
      max_memory_restart: '1G',
      env: {
        NODE_ENV: 'production',
        PORT: 3000,
        ML_SERVICE_URL: 'http://localhost:5000'
      },
      error_file: './logs/backend-error.log',
      out_file: './logs/backend-out.log',
      log_date_format: 'YYYY-MM-DD HH:mm:ss Z',
      merge_logs: true
    },
    {
      name: 'ml-service',
      script: './ml_service/venv/bin/python',
      args: './ml_service/app.py',
      instances: 1,
      autorestart: true,
      watch: false,
      max_memory_restart: '2G',
      env: {
        PYTHONUNBUFFERED: '1'
      },
      error_file: './logs/ml-error.log',
      out_file: './logs/ml-out.log',
      log_date_format: 'YYYY-MM-DD HH:mm:ss Z',
      merge_logs: true
    }
  ]
};

