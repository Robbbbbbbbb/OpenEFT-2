# OpenEFT Converter

A lightweight web application for converting scanned FD-258 fingerprint cards into EFT files (FBI/NBIS standard) using the NIST Biometric Image Software (NBIS).

## Features

1.  **Upload**: Supports high-resolution scans of FD-258 cards.
2.  **Crop & Rotate**: Built-in tool to manually rotate and crop the image to the card boundary.
3.  **Align & Segment**: Suggests default fingerprint locations based on standard card layout.
4.  **Interactive Editor**: Allows users to adjust bounding boxes for individual prints to ensure accuracy.
5.  **Data Entry**: Validates and collects required Type-2 demographic data (Name, DOB, SSN, etc.).
6.  **EFT Generation**: Uses compiled NBIS tools (`an2k`, `nfiq`, `opj_compress`) to generate compliant EFT files.
7.  **Smart Compression**: Automatically ensures the final EFT file is under 12MB by adjusting compression ratios if needed.

## Prerequisites

- **Docker**: The application is containerized. Ensure Docker is installed on your machine.
- **Source Code**: You must have the full repository (including the NBIS source directories `an2k`, `nfiq`, etc.) present locally.

## Build & Run

### 1. Build the Docker Image
**Important:** You must run this command from the **root directory** of the repository.

```bash
docker build -t openeft2 .
```

*Note: The build process compiles NBIS tools from source and may take a few minutes.*

### 2. Run the Container
```bash
docker run -p 8080:8080 openeft
```

### 3. Access the Application
Open your browser and navigate to:
[http://localhost:8080](http://localhost:8080)

## Usage Guide

1.  **Upload Card**: Select your scanned FD-258 image (JPG/PNG).
2.  **Crop & Rotate**: 
    - Use the **Rotate** buttons to orient the card upright.
    - Drag a box around the actual card area (excluding scanner bed background).
    - Click "Next".
3.  **Verify Boxes**: You will see the aligned image with boxes around the expected fingerprint locations.
    -   **Drag** boxes to move them.
    -   **Resize** boxes using the corners (Top-Left or Bottom-Right).
    -   Ensure the boxes capture the full print.
4.  **Enter Data**: Fill out the required fields.
    -   **Dates** must be in `YYYYMMDD` format.
    -   **SSN** should be 9 digits.
    -   **Codes**: Use standard 2-3 letter codes (e.g., `US` for citizenship, `BLK` for hair).
5.  **Download**: Click "Generate EFT". The system will process the images and generate the file.
6.  **Cleanup**: Click "Delete File & Start Over" to remove the temporary session data.

## Technical Details

-   **Backend**: FastAPI (Python)
-   **Image Processing**: OpenCV, NumPy
-   **Biometrics**: NBIS (NIST Biometric Image Software) - `opj_compress` (JPEG 2000), `nfseg`, `an2k`.
-   **Frontend**: Vanilla HTML/JS/CSS.

## Troubleshooting

**Build Fails with "setup.sh not found":**
Ensure you are running `docker build .` from the root of the project folder, not inside a subdirectory like.