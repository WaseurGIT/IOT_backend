# CropGuardian IEEE Paper - Images & Diagrams Guide

## Required Images for the Conference Paper

### 1. **System Architecture Diagram** (Referenced as Fig. 1 in paper)
**Filename:** `system_architecture.png`
**What to include:**
- Four main components in blocks:
  - **Hardware Platform** (left): 4-wheel robot, ESP32-CAM, motors, water pump
  - **Backend Server** (center-top): Node.js WebSocket server, REST API
  - **ML Model** (center-bottom): EfficientNet-B3, CropGuardian.h5
  - **Mobile App** (right): React Native interface
- Arrows showing data flow:
  - Camera → Backend (video frames)
  - Mobile App → Backend (control commands)
  - Backend → Robot (movement commands)
  - Image → ML Model → Backend → Mobile (prediction results)
- Use professional colors (blue for IoT components, green for ML, orange for mobile)

**Tools to create:**
- Draw.io (https://app.diagrams.net/)
- Microsoft Visio
- Lucidchart
- PowerPoint/Google Slides

---

### 2. **Hardware Component Diagram** (Suggested addition)
**Filename:** `hardware_components.png`
**What to include:**
- Labeled photo or diagram of the actual robot showing:
  - ESP32-CAM module position
  - Four wheels
  - Motor drivers (L298N)
  - Water pump system
  - Battery pack
  - Annotations with arrows pointing to each component

**How to create:**
- Take a clear photo of your actual robot
- Add labels and arrows using:
  - Adobe Photoshop
  - GIMP (free)
  - PowerPoint (take photo, insert arrows and text boxes, export as PNG)

---

### 3. **System Workflow Diagram** (Suggested addition)
**Filename:** `workflow_diagram.png`
**What to include:**
- Flowchart showing the 7-step process:
  1. Image Acquisition (camera icon)
  2. Image Transmission (upload arrow)
  3. Preprocessing (resize/normalize)
  4. Inference (neural network icon)
  5. Post-processing (Groq AI)
  6. Result Delivery (mobile screen)
  7. Treatment Application (water pump)
- Use standard flowchart symbols
- Different colors for different stages (capture=blue, processing=green, action=orange)

**Tools to create:**
- Lucidchart
- Draw.io
- Microsoft Visio
- PlantUML (code-based)

---

### 4. **Mobile App Screenshots** (Suggested addition)
**Filename:** `mobile_app_interface.png`
**What to include:**
- Composite image showing 3-4 app screens:
  - **Screen 1:** Live video streaming view
  - **Screen 2:** Control buttons (forward/backward/left/right)
  - **Screen 3:** Disease detection result with prediction
  - **Screen 4:** Remedy recommendations from Groq API
- Arrange in a grid or horizontal layout

**How to create:**
- Take screenshots from your React Native app
- Use image editing tool to create composite:
  - Photoshop/GIMP for professional look
  - PowerPoint to arrange screenshots
  - Online tools like Canva

---

### 5. **Model Performance Graphs** (Suggested addition)
**Filename:** `model_performance.png`
**What to include:**
Two graphs side-by-side:
- **Left:** Training/Validation accuracy over epochs (line graph)
- **Right:** Confusion matrix for disease classes (heatmap)

**How to create:**
- Python with matplotlib/seaborn:

```python
import matplotlib.pyplot as plt
import seaborn as sns

# Training history plot
plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
plt.plot(history['accuracy'], label='Training')
plt.plot(history['val_accuracy'], label='Validation')
plt.xlabel('Epoch')
plt.ylabel('Accuracy')
plt.legend()
plt.title('Model Training Performance')

# Confusion matrix
plt.subplot(1, 2, 2)
sns.heatmap(confusion_matrix, annot=True, fmt='d', cmap='Blues')
plt.title('Confusion Matrix')
plt.xlabel('Predicted')
plt.ylabel('Actual')

plt.tight_layout()
plt.savefig('model_performance.png', dpi=300, bbox_inches='tight')
```

---

### 6. **Field Test Results Photo** (Suggested addition)
**Filename:** `field_deployment.png`
**What to include:**
- Real photo of the robot in a crop field
- Inset showing mobile app screen with detection result
- Annotation showing the detected disease on actual plant

**How to create:**
- Take photo during actual field testing
- Add overlays in Photoshop/GIMP showing:
  - Detected disease name
  - Confidence score
  - Affected plant highlighted

---

### 7. **Comparison Bar Chart** (Suggested addition)
**Filename:** `performance_comparison.png`
**What to include:**
- Bar chart comparing Manual vs CropGuardian on:
  - Inspection Time
  - Detection Rate
  - Response Time
  - Treatment Accuracy
- Use data from Table II in the paper
- Different colors for Manual (red/gray) and CropGuardian (green/blue)

**Python code:**
```python
import matplotlib.pyplot as plt
import numpy as np

categories = ['Inspection\nTime (min)', 'Detection\nRate (%)', 'Response\nTime (min)', 'Treatment\nAccuracy (%)']
manual = [120, 73, 2880, 65]  # Response time in minutes (48 hours)
cropguardian = [35, 94, 15, 91]

x = np.arange(len(categories))
width = 0.35

fig, ax = plt.subplots(figsize=(10, 6))
bars1 = ax.bar(x - width/2, manual, width, label='Manual Method', color='#FF6B6B')
bars2 = ax.bar(x + width/2, cropguardian, width, label='CropGuardian', color='#4ECDC4')

ax.set_xlabel('Metrics', fontsize=12)
ax.set_ylabel('Value', fontsize=12)
ax.set_title('Performance Comparison: Manual vs CropGuardian', fontsize=14, fontweight='bold')
ax.set_xticks(x)
ax.set_xticklabels(categories)
ax.legend()
ax.grid(axis='y', alpha=0.3)

plt.tight_layout()
plt.savefig('performance_comparison.png', dpi=300, bbox_inches='tight')
```

---

## Image Specifications for IEEE Conference

- **Format:** PNG or JPEG (PNG preferred for diagrams)
- **Resolution:** Minimum 300 DPI for print quality
- **Width:** Single column = 3.5 inches (8.9 cm), Double column = 7 inches (17.8 cm)
- **File size:** Keep under 1 MB per image if possible
- **Color:** RGB color mode (for color printing)
- **Fonts:** Use clear, readable fonts (Arial, Helvetica, Times) minimum 8pt

---

## LaTeX Image Insertion Reference

### Single column figure:
```latex
\begin{figure}[htbp]
\centerline{\includegraphics[width=0.45\textwidth]{filename.png}}
\caption{Your caption here describing the figure.}
\label{fig:yourlabel}
\end{figure}
```

### Double column figure (wide):
```latex
\begin{figure*}[htbp]
\centerline{\includegraphics[width=0.9\textwidth]{filename.png}}
\caption{Your caption here for wide figure.}
\label{fig:widefig}
\end{figure*}
```

### Multiple subfigures:
```latex
\usepackage{subcaption} % Add to preamble

\begin{figure}[htbp]
\centering
\begin{subfigure}{0.22\textwidth}
    \includegraphics[width=\textwidth]{image1.png}
    \caption{Screen 1}
\end{subfigure}
\begin{subfigure}{0.22\textwidth}
    \includegraphics[width=\textwidth]{image2.png}
    \caption{Screen 2}
\end{subfigure}
\caption{Mobile app interface screenshots}
\label{fig:appscreens}
\end{figure}
```

---

## Priority Order

1. **MUST HAVE** (Required for paper):
   - System Architecture Diagram (already referenced in paper)

2. **HIGHLY RECOMMENDED** (Will significantly improve paper quality):
   - Hardware Component Diagram
   - Mobile App Screenshots
   - System Workflow Diagram

3. **NICE TO HAVE** (Additional visual support):
   - Model Performance Graphs
   - Comparison Bar Chart
   - Field Test Photos

---

## Quick Creation Tips

1. **For Diagrams**: Use Draw.io (free, online, easy to use)
   - Go to https://app.diagrams.net/
   - Choose "Blank Diagram"
   - Use shapes from left sidebar
   - Export as PNG (File → Export as → PNG, 300 DPI)

2. **For Screenshots**: Use device emulator or actual phone
   - Android Studio emulator for testing
   - Use ADB to take screenshots
   - Combine in PowerPoint or GIMP

3. **For Graphs**: Use Python matplotlib
   - Install: `pip install matplotlib seaborn`
   - Run the code examples above
   - Adjust colors/labels as needed

4. **For Photos**: Use good lighting
   - Take photos in bright daylight
   - Multiple angles
   - Clean background
   - Add annotations later in editing software

---

## Need Help?

If you need assistance creating any of these images:
1. I can provide more detailed Python code for graphs
2. I can help with Draw.io diagram specifications
3. I can suggest online tools for specific needs
4. I can provide more specific LaTeX code for image placement

---

Good luck with your conference paper! 📝🌱🤖

