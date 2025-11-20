# CropGuardian Conference Paper - References Explained

## Overview
This document explains each reference used in the CropGuardian conference paper, what it covers, and why it's cited.

---

## References Used in the Paper

### [b1] Savary et al., 2012 - "Crop losses due to diseases"
**Journal:** Food Security  
**Topic:** Global impact of crop diseases on food production  
**Why cited:** Supports the introduction's claim about 20-40% yield losses due to crop diseases  
**Where to find:** https://link.springer.com/article/10.1007/s12571-012-0200-5  
**Key point:** Establishes the problem statement - crop diseases are a major global challenge

---

### [b2] Oerke, 2006 - "Crop losses to pests"
**Journal:** Journal of Agricultural Science  
**Topic:** Statistical analysis of agricultural losses  
**Why cited:** Reinforces the economic and productivity impact of crop diseases  
**Where to find:** https://www.cambridge.org/core/journals/journal-of-agricultural-science  
**Key point:** Provides authoritative data on the scale of the problem

---

### [b3] Farooq et al., 2019 - "Role of IoT in Agriculture"
**Journal:** IEEE Access  
**Topic:** Comprehensive survey of IoT applications in smart farming  
**Why cited:** Shows recent advances in IoT for agriculture, setting context for our solution  
**Where to find:** https://ieeexplore.ieee.org/document/8889866  
**Key point:** Establishes that IoT is an emerging solution for agricultural challenges  
**Note:** This is an IEEE publication, which strengthens your IEEE conference paper

---

### [b4] Mohanty et al., 2016 - "Using Deep Learning for Image-Based Plant Disease Detection"
**Journal:** Frontiers in Plant Science  
**Topic:** Deep learning for plant disease detection using PlantVillage dataset  
**Why cited:** This is THE foundational paper for using PlantVillage dataset with deep learning  
**Where to find:** https://www.frontiersin.org/articles/10.3389/fpls.2016.01419/full  
**Key point:** 
- They achieved 99.35% accuracy on PlantVillage
- Your model (96.5%) is competitive and more practical
- Validates your choice of PlantVillage dataset
**IMPORTANT:** This is a MUST-CITE paper for any work using PlantVillage dataset

---

### [b5] Rangarajan et al., 2018 - "Tomato crop disease classification"
**Journal:** Procedia Computer Science  
**Topic:** Pre-trained deep learning for tomato disease detection  
**Why cited:** Related work showing transfer learning approach (similar to your EfficientNet approach)  
**Where to find:** https://www.sciencedirect.com/science/article/pii/S1877050918308305  
**Key point:** Shows precedent for using pre-trained models in crop disease detection

---

### [b6] Too et al., 2019 - "Fine-tuning deep learning models for plant disease"
**Journal:** Computers and Electronics in Agriculture  
**Topic:** Comparative study of different deep learning models for disease detection  
**Why cited:** Related work comparing different architectures (helps justify your EfficientNet choice)  
**Where to find:** https://www.sciencedirect.com/science/article/pii/S0168169918311390  
**Key point:** Demonstrates that fine-tuning (transfer learning) is effective approach

---

### [b7] Farooq et al., 2022 - "Role of IoT in Smart Livestock Environment"
**Journal:** IEEE Internet of Things Journal  
**Topic:** IoT frameworks for agricultural monitoring  
**Why cited:** Related work in IoT-based agricultural systems  
**Where to find:** IEEE Xplore  
**Key point:** Shows IoT monitoring systems but identifies gap in treatment/actuation
**Note:** Another IEEE publication strengthening your related work

---

### [b8] Subeesh & Mehta, 2021 - "Automation and digitization of agriculture"
**Journal:** Artificial Intelligence in Agriculture  
**Topic:** Survey of AI and IoT in agriculture  
**Why cited:** Identifies need for integrated systems (monitoring + actuation)  
**Where to find:** https://www.sciencedirect.com/science/article/pii/S2589721721000301  
**Key point:** Points out the gap that CropGuardian fills - most systems only monitor, not treat

---

### [b9] Shamshiri et al., 2018 - "Agricultural robotics research and development"
**Journal:** International Journal of Agricultural and Biological Engineering  
**Topic:** Review of agricultural robots and autonomous systems  
**Why cited:** Related work in mobile robotics for agriculture  
**Where to find:** http://www.ijabe.org/  
**Key point:** Validates use of mobile robots in agriculture, shows precedent

---

### [b10] Bawden et al., 2017 - "Robot for weed species management"
**Journal:** Journal of Field Robotics  
**Topic:** Autonomous robot for weed detection and treatment  
**Why cited:** Similar concept (robot + detection + treatment) but for weeds not diseases  
**Where to find:** https://onlinelibrary.wiley.com/journal/15564967  
**Key point:** Shows successful implementation of detection+treatment robotics in agriculture

---

### [b11] Tan & Le, 2019 - "EfficientNet"
**Conference:** International Conference on Machine Learning (ICML)  
**Topic:** The original EfficientNet paper by Google Research  
**Why cited:** MUST cite when using EfficientNet architecture  
**Where to find:** http://proceedings.mlr.press/v97/tan19a.html  
**Key point:** Gives credit to the EfficientNet architecture you're using
**IMPORTANT:** Always cite the original paper when using someone's model architecture

---

### [b12] Hughes & Salathe, 2015 - "Open access repository on plant health"
**Source:** arXiv preprint  
**Topic:** The creation and release of PlantVillage dataset  
**Why cited:** MUST cite when using PlantVillage dataset - this is the original dataset paper  
**Where to find:** https://arxiv.org/abs/1511.08060  
**Key point:** Gives credit to dataset creators
**IMPORTANT:** Always cite the dataset paper when using a public dataset

---

## Citation Strategy Explained

### Problem Statement Citations (Introduction)
- **[b1], [b2]** - Establish that crop diseases are a significant problem
- Provides context and motivation for your work

### Technology Background (Introduction)
- **[b3]** - Shows IoT is being used in agriculture
- Sets up that your work is part of a larger trend

### Related Work (Section II)
- **[b4], [b5], [b6]** - Prior work in disease detection using deep learning
- Shows what others have done and how your work differs
- **[b7], [b8]** - IoT systems for agriculture
- Identifies gaps (monitoring without treatment)
- **[b9], [b10]** - Agricultural robotics
- Shows robots are used in agriculture but not for disease-specific treatment

### Technical Justification
- **[b11]** - EfficientNet architecture (REQUIRED when using EfficientNet)
- **[b12]** - PlantVillage dataset (REQUIRED when using this dataset)

---

## Important Notes

### 1. **Must-Cite References**
These are REQUIRED if you're using the technology/data:
- **[b11]** - EfficientNet (you MUST cite if using EfficientNet)
- **[b12]** - PlantVillage dataset (you MUST cite if using this dataset)
- **[b4]** - Highly recommended as the landmark PlantVillage deep learning paper

### 2. **IEEE Publications Used**
The paper cites 2 IEEE publications ([b3], [b7]):
- Shows familiarity with IEEE research
- Appropriate for an IEEE conference submission

### 3. **Recent References**
Most references are from 2015-2022:
- Shows current/recent work
- 2015-2016 for foundational dataset/methods
- 2017-2022 for recent advances

### 4. **Diverse Sources**
References come from various journals:
- IEEE (computer science/engineering)
- Agricultural journals (domain expertise)
- Computer science conferences (methodology)
- This diversity shows comprehensive literature review

---

## How to Access These Papers

### Option 1: Institutional Access
- Use your university library access
- Most universities have subscriptions to IEEE, Springer, Elsevier

### Option 2: Google Scholar
1. Go to https://scholar.google.com/
2. Search for paper title
3. Look for [PDF] link on right side (free versions)
4. Check author's personal website (often have free copies)

### Option 3: ResearchGate
- Many authors upload their papers to ResearchGate
- Create free account and request full-text

### Option 4: ArXiv (for preprints)
- [b12] is on arxiv.org (completely free)
- Some others may have arxiv versions

### Option 5: Contact Authors
- Most authors happy to share their papers
- Email addresses usually in the paper or on their university page

---

## Additional References You Might Want to Add

If you have space for more references, consider adding:

### For React Native / Mobile Development:
```
"React Native: Cross-platform mobile development framework"
- Strengthens mobile app section
```

### For WebSocket Communication:
```
"WebSocket: Real-time bidirectional communication protocol"
- Supports your communication architecture
```

### For Groq API (if you can find technical documentation):
```
"Groq: Large language model for agricultural recommendations"
- Justifies AI-powered remedy generation
```

### For ESP32-CAM:
```
A technical paper or documentation about ESP32-CAM in IoT applications
- Validates hardware choice
```

---

## Reference Format Notes

The references in your paper follow IEEE citation style:
- Authors, "Title," Journal/Conference, volume, pages, year
- Use proper quotation marks for paper titles
- Use italics for journal/conference names (LaTeX handles this)
- Include DOI or URL when available (optional but helpful)

---

## Verification Checklist

Before submission, verify:
- [ ] All citations [b1]-[b12] are referenced in the text
- [ ] No citation in text that isn't in bibliography
- [ ] Author names spelled correctly (check original papers)
- [ ] Journal/conference names accurate
- [ ] Year and volume numbers correct
- [ ] Proper LaTeX special characters (é in Salathé, etc.)

---

## Summary

Your paper has **12 solid references** covering:
- ✅ Problem motivation (2 refs)
- ✅ IoT in agriculture (2 refs)  
- ✅ Deep learning for disease detection (3 refs)
- ✅ Agricultural robotics (2 refs)
- ✅ Technical methods used: EfficientNet (1 ref)
- ✅ Dataset source: PlantVillage (1 ref)
- ✅ Automation/AI in agriculture (1 ref)

This is a **strong reference list** for a conference paper! Most IEEE conference papers have 10-20 references, so you're in good range.

---

## Questions?

If you need help:
1. Finding any of these papers
2. Understanding why a reference was chosen
3. Adding additional references
4. Formatting citations correctly

Feel free to ask!

