## Crop Disease Prediction API Doc

Base URL: `http://<backend-host>:3000/camera`
ML Service: `http://<ml-service-host>:5000`

### 1. POST `/camera/capture`

Captures current ESP32 frame, saves it, runs disease prediction (multi-model with Groq guidance).

Request: none (just POST)

Response:
```json
{
  "success": true,
  "filename": "ESP32_CAM_2025-11-13T11-11-25-785Z.jpg",
  "path": "/home/.../uploads/ESP32_CAM_2025-11-13T11-11-25-785Z.jpg",
  "url": "/uploads/ESP32_CAM_2025-11-13T11-11-25-785Z.jpg",
  "size": 234567,
  "timestamp": "2025-11-13T11-11-25-785Z",
  "disease_prediction": {
    "disease": "Tomato___Early_blight",
    "confidence": 0.9251,
    "model_used": "vit_base",
    "guidance": {
      "description": "Early blight causes concentric ring lesions on older tomato leaves.",
      "severity": "high",
      "remedies": [
        "Trim and dispose of infected leaves",
        "Apply copper-based fungicide every 7 days",
        "Avoid overhead irrigation",
        "Mulch soil to reduce splash spread"
      ],
      "follow_up": "Inspect daily for new lesions and remove infected leaves promptly.",
      "source": "groq",
      "model": "llama-3.3-70b-versatile"
    },
    "individual_predictions": null
  },
  "top_predictions": [
    {"disease": "Tomato___Early_blight", "confidence": 0.9251},
    {"disease": "Tomato___Late_blight", "confidence": 0.0512},
    {"disease": "Tomato___healthy", "confidence": 0.0237}
  ]
}
```

Notes:
- `disease` format: `Plant___Condition`. Split on `___` to get plant + condition.
- `model_used`: `vit_base`, `resnet50`, or `ensemble`.
- `guidance`: Groq-generated advice. Fields may be absent if Groq fallback was used; check with optional chaining.
- `individual_predictions`: only filled when multi-model mode is `ensemble`.
- `top_predictions`: confidence list direct from ML model.

### 2. POST `/camera/predict/:filename`

Runs prediction on saved image.

Response identical to `/capture`, plus request filename.

### 3. GET `/camera/images`

List saved images.

### 4. DELETE `/camera/images/:filename`

Remove image.

### 5. GET `/camera/status`

Returns ESP32 and socket status.

---

### Frontend Parsing Tips

```ts
const pred = result.disease_prediction;
const [plant, condition] = pred.disease.split('___');
const guidance = pred.guidance || {};

const severity = guidance.severity
  ? guidance.severity.toUpperCase()
  : 'UNKNOWN';

const remedies = Array.isArray(guidance.remedies)
  ? guidance.remedies
  : [];
```

Optional chaining prevents errors like `severity.toUpperCase()` or `remedies.map` when Groq returns fallback responses.

