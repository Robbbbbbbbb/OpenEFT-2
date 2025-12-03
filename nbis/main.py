from fastapi import FastAPI, UploadFile, File, Form, HTTPException, Request
from fastapi.exceptions import RequestValidationError
from fastapi.staticfiles import StaticFiles
from fastapi.responses import JSONResponse, FileResponse
from pydantic import BaseModel
import shutil
import os
import uuid
import json
import base64
import cv2
from typing import List, Dict, Optional, Any, Union

from services.image_processing import align_image, get_default_boxes, apply_crop_and_rotate
from services.eft_generator import generate_eft
from services.fingerprint import Fingerprint

app = FastAPI()

@app.exception_handler(RequestValidationError)
async def validation_exception_handler(request: Request, exc: RequestValidationError):
    print(f"Validation Error: {exc.errors()}")
    print(f"Body: {await request.body()}")
    return JSONResponse(
        status_code=422,
        content={"detail": exc.errors(), "body": str(exc.body)},
    )

# Mount static files
app.mount("/static", StaticFiles(directory="static"), name="static")

# Temp storage
TMP_DIR = "/app/temp"
os.makedirs(TMP_DIR, exist_ok=True)

# In-memory session store
SESSIONS = {}

class Box(BaseModel):
    id: str
    fp_number: int
    x: float
    y: float
    w: float
    h: float

class CropRequest(BaseModel):
    session_id: str
    rotation: int
    x: int
    y: int
    w: int
    h: int

class GenerateRequest(BaseModel):
    session_id: str
    boxes: List[Box]
    type2_data: Dict[str, Any] # Changed from str to Any to be permissive

@app.get("/")
async def read_index():
    return FileResponse("static/index.html")

@app.post("/api/upload")
async def upload_image(file: UploadFile = File(...)):
    session_id = str(uuid.uuid4())
    session_dir = os.path.join(TMP_DIR, session_id)
    os.makedirs(session_dir, exist_ok=True)
    
    file_path = os.path.join(session_dir, "original.jpg")
    with open(file_path, "wb") as buffer:
        shutil.copyfileobj(file.file, buffer)
        
    # For Step 1.5, we just return the uploaded image as base64
    try:
        # Read the image to get base64
        with open(file_path, "rb") as f:
            img_bytes = f.read()
            img_base64 = base64.b64encode(img_bytes).decode('utf-8')
        
        SESSIONS[session_id] = {
            "image_path": file_path, # Temporary pointing to original
            "boxes": []
        }
        
        return {
            "session_id": session_id,
            "image_base64": img_base64
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/process_crop")
async def process_crop(data: CropRequest):
    session_id = data.session_id
    if session_id not in SESSIONS:
        raise HTTPException(status_code=404, detail="Session not found")
        
    session_dir = os.path.join(TMP_DIR, session_id)
    original_path = os.path.join(session_dir, "original.jpg")
    
    try:
        crop_rect = {'x': data.x, 'y': data.y, 'w': data.w, 'h': data.h}
        processed_img = apply_crop_and_rotate(original_path, data.rotation, crop_rect)
        
        # Save as aligned.png
        aligned_path = os.path.join(session_dir, "aligned.png")
        cv2.imwrite(aligned_path, processed_img)
        
        # Update session
        SESSIONS[session_id]["image_path"] = aligned_path
        
        # Get default boxes based on new image
        boxes = get_default_boxes(processed_img.shape)
        SESSIONS[session_id]["boxes"] = boxes
        
        # Return new base64 and boxes
        _, buffer = cv2.imencode('.png', processed_img)
        img_base64 = base64.b64encode(buffer).decode('utf-8')
        
        return {
            "image_base64": img_base64,
            "boxes": boxes
        }
    except Exception as e:
        print(e)
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/preview")
async def preview_crops(data: GenerateRequest):
    """
    Returns cropped images for the given boxes so the user can verify.
    """
    session_id = data.session_id
    if session_id not in SESSIONS:
        raise HTTPException(status_code=404, detail="Session not found")
    
    img_path = SESSIONS[session_id]["image_path"]
    img = cv2.imread(img_path)
    
    previews = {}
    for box in data.boxes:
        # Crop
        x, y, w, h = box.x, box.y, box.w, box.h
        # Ensure bounds
        x = max(0, x)
        y = max(0, y)
        w = min(w, img.shape[1] - x)
        h = min(h, img.shape[0] - y)
        
        crop = img[y:y+h, x:x+w]
        _, buffer = cv2.imencode('.jpg', crop)
        b64 = base64.b64encode(buffer).decode('utf-8')
        previews[box.id] = b64
        
    return {"previews": previews}

@app.post("/api/generate")
async def generate_eft_endpoint(data: GenerateRequest):
    session_id = data.session_id
    if session_id not in SESSIONS:
        raise HTTPException(status_code=404, detail="Session not found")
    
    session_dir = os.path.join(TMP_DIR, session_id)
    img_path = SESSIONS[session_id]["image_path"]
    img = cv2.imread(img_path)
    
    prints_map = {}
    
    # Process each box
    fp_objects = [] # Keep track of objects for re-compression if needed
    
    for box in data.boxes:
        # Cast to int for slicing
        x, y, w, h = int(box.x), int(box.y), int(box.w), int(box.h)
        crop = img[y:y+h, x:x+w]
        
        fp = Fingerprint(crop, box.fp_number, session_dir, session_id)
        fp_objects.append(fp)
        
        result_path = fp.process_and_convert(compression_ratio=10) # Default ratio
        
        if result_path:
            size = os.path.getsize(result_path)
            print(f"Processed FP {box.fp_number}: {result_path} ({size} bytes)")
            if size == 0:
                print(f"WARNING: FP {box.fp_number} is 0 bytes!")
            prints_map[box.fp_number] = fp
        else:
             print(f"ERROR: Failed to process FP {box.fp_number}")
            
    # Generate EFT with size safeguard
    try:
        # Initial generation with default compression
        eft_path = generate_eft(data.type2_data, session_id, {fp.fp_number: fp for fp in fp_objects})
        
        # Check size (Max 11MB, Min 1MB)
        max_size = 11 * 1024 * 1024
        min_size = 1 * 1024 * 1024
        current_size = os.path.getsize(eft_path)
        
        retries = 0
        ratios = [15, 20, 30] # Progressive compression ratios
        
        while current_size > max_size and retries < len(ratios):
            print(f"EFT size {current_size} exceeds limit. Re-compressing with ratio {ratios[retries]}...")
            
            # Re-compress all images
            for fp in fp_objects:
                fp.process_and_convert(compression_ratio=ratios[retries])
            
            # Re-generate EFT
            eft_path = generate_eft(data.type2_data, session_id, {fp.fp_number: fp for fp in fp_objects})
            current_size = os.path.getsize(eft_path)
            retries += 1
            
        if current_size > max_size:
            raise HTTPException(status_code=400, detail=f"EFT size ({current_size} bytes) exceeds 11MB limit even after compression.")
        
        if current_size < min_size:
            raise HTTPException(status_code=400, detail=f"EFT size ({current_size} bytes) is below 1MB, indicating a generation error.")
            
        filename = os.path.basename(eft_path)
        return {"download_url": f"/api/download/{session_id}/{filename}", "filename": filename}
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"EFT Generation failed: {str(e)}")

@app.get("/api/download/{session_id}/{filename}")
async def download_file(session_id: str, filename: str):
    file_path = os.path.join(TMP_DIR, session_id, filename)
    if os.path.exists(file_path):
        return FileResponse(file_path, filename=filename)
    raise HTTPException(status_code=404, detail="File not found")

@app.delete("/api/delete/{session_id}")
async def delete_session(session_id: str):
    # Validate session_id is a valid UUID to prevent directory traversal
    try:
        uuid.UUID(session_id)
    except ValueError:
        raise HTTPException(status_code=400, detail="Invalid session ID format")

    session_dir = os.path.join(TMP_DIR, session_id)
    # Double check path safety
    if not os.path.abspath(session_dir).startswith(os.path.abspath(TMP_DIR)):
         raise HTTPException(status_code=403, detail="Access denied")

    if os.path.exists(session_dir):
        shutil.rmtree(session_dir)
        if session_id in SESSIONS:
            del SESSIONS[session_id]
        return {"message": "Deleted"}
    raise HTTPException(status_code=404, detail="Session not found")
