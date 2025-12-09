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
try:
    import cv2
except ImportError:
    cv2 = None
from typing import List, Dict, Optional, Any, Union

from services.image_processing import align_image, get_default_boxes, apply_crop_and_rotate
from services.eft_generator import generate_eft
from services.fingerprint import Fingerprint
from services.eft_parser import EFTParser
from services.eft_editor import EFTEditor
from services.fd258_generator import FD258Generator
from services.nbis_helper import decode_wsq


app = FastAPI()

# Logging handler for validation errors
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

# Model selection box on the fingerprint card image.
class Box(BaseModel):
    id: str
    fp_number: int
    x: float
    y: float
    w: float
    h: float

# Request model for the initial crop and rotate step.
class CropRequest(BaseModel):
    session_id: str
    rotation: int
    x: int
    y: int
    w: int
    h: int

# Request model for the final EFT generation step.
class GenerateRequest(BaseModel):
    session_id: str
    boxes: List[Box]
    type2_data: Dict[str, Any]
    mode: Optional[str] = "atf" # 'atf' or 'rolled'

class CaptureSessionRequest(BaseModel):
    l_slap: str
    r_slap: str
    thumbs: str

# Request model for saving edited EFT.
class SaveEFTRequest(BaseModel):
    session_id: str
    type2_data: Dict[str, Any]

# Serves the main SPA.
@app.get("/")
async def read_index():
    return FileResponse("static/index.html")

# Step 1: Uploads the raw fingerprint card image.
# Creates a new session and saves the original image.

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
            "image_path": file_path, # Temporary path pointing to original uploaded image
            "boxes": []
        }
        
        return {
            "session_id": session_id,
            "image_base64": img_base64
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Creates a session from captured live scans.
@app.post("/api/start_capture_session")
async def start_capture_session(data: CaptureSessionRequest):
    session_id = str(uuid.uuid4())
    session_dir = os.path.join(TMP_DIR, session_id)
    os.makedirs(session_dir, exist_ok=True)
    
    images_map = {}
    
    # Save print images
    try:
        def save_b64(b64_str, name):
            path = os.path.join(session_dir, name)
            with open(path, "wb") as f:
                f.write(base64.b64decode(b64_str))
            return path
        
        # 14 = L_SLAP, 13 = R_SLAP, 15 = THUMBS
        images_map[14] = save_b64(data.l_slap, "14.png") # 14 = L_SLAP
        images_map[13] = save_b64(data.r_slap, "13.png") # 13 = R_SLAP
        images_map[15] = save_b64(data.thumbs, "15.png") # 15 = THUMBS
        
        # Save session and return session id
        SESSIONS[session_id] = {
            "mode": "capture",
            "images": images_map
        }
        
        return {"session_id": session_id}
    
    # Exception handling in case of an error
    except Exception as e:
        import traceback
        traceback.print_exc()
        raise HTTPException(status_code=500, detail=str(e))

# Step 2: Applies user-defined crop and rotation to the original image.
# Calculates default fingerprint boxes for the newly aligned image.
@app.post("/api/process_crop")
async def process_crop(data: CropRequest):

    # Get session
    session_id = data.session_id
    if session_id not in SESSIONS:
        raise HTTPException(status_code=404, detail="Session not found")
        
    # Get session directory
    session_dir = os.path.join(TMP_DIR, session_id)
    original_path = os.path.join(session_dir, "original.jpg")
    
    # Process crop
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
    
    # Exception handling in case of an error
    except Exception as e:
        print(e)
        raise HTTPException(status_code=500, detail=str(e))


# Returns cropped images for the given boxes so the user can verify.
@app.post("/api/preview")
async def preview_crops(data: GenerateRequest):
    session_id = data.session_id
    if session_id not in SESSIONS:
        raise HTTPException(status_code=404, detail="Session not found")
    
    # Get session directory
    img_path = SESSIONS[session_id]["image_path"]
    img = cv2.imread(img_path)
    
    # Generate print previews
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
    
"""
> Step 3 (Final): Processes individual fingerprint images and generates the EFT file.

This endpoint does the following:
    1. Crops each finger based on the user-adjusted boxes.
    2. Converts/segments the images (RGB -> Gray -> JP2).
    3. Assembles the EFT file.
    4. Handles re-compression if the file exceeds the 11MB size limit.
"""

@app.post("/api/generate")
async def generate_eft_endpoint(data: GenerateRequest):

    # Get session
    session_id = data.session_id
    if session_id not in SESSIONS:
        raise HTTPException(status_code=404, detail="Session not found")
    
    # Get session data
    session_data = SESSIONS[session_id]
    session_dir = os.path.join(TMP_DIR, session_id)
    
    # Check if cv2 is installed, handle error if not
    if cv2 is None:
        raise HTTPException(status_code=500, detail="cv2 not installed")

    # Initialize variables
    prints_map = {}
    fp_objects = [] 

    # Check session mode (Capture or Upload)
    if session_data.get("mode") == "capture":

        # If Capture Mode: Load individual images based on box.fp_number
        images_map = session_data["images"]
        for box in data.boxes:
            if box.fp_number in images_map:
                img_path = images_map[box.fp_number]
                img = cv2.imread(img_path)

                # Create Fingerprint object
                fp = Fingerprint(img, box.fp_number, session_dir, session_id)
                fp_objects.append(fp)

                # Capture mode currently only supports Type-14 Capture
                result_path = fp.process_and_convert(compression_ratio=10)
                if result_path:
                    prints_map[box.fp_number] = fp
    else:
        # Upload Mode: Crop from master image
        img_path = session_data["image_path"]
        img = cv2.imread(img_path)
        
        for box in data.boxes:
            # Cast to int for slicing
            x, y, w, h = int(box.x), int(box.y), int(box.w), int(box.h)
            crop = img[y:y+h, x:x+w]
            
            fp = Fingerprint(crop, box.fp_number, session_dir, session_id)
            fp_objects.append(fp)
            
            # Select processing method based on requested mode (rolled or flat)
            if data.mode == "rolled":
                result_path = fp.process_and_convert_type4(compression_ratio=10)
            else:
                result_path = fp.process_and_convert(compression_ratio=10) # Default ratio
            
            # Add processed fingerprint to prints_map
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
        eft_path = generate_eft(data.type2_data, session_id, {fp.fp_number: fp for fp in fp_objects}, mode=data.mode)
        
        # Check size (Max 11MB)
        max_size = 11 * 1024 * 1024
        current_size = os.path.getsize(eft_path)
        
        retries = 0
        ratios = [15, 20, 30] # Progressive compression ratios in case file exceeds limit
        
        # Re-compress and re-generate EFT if file exceeds limit
        while current_size > max_size and retries < len(ratios):
            print(f"EFT size {current_size} exceeds limit. Re-compressing with ratio {ratios[retries]}...")
            
            # Re-compress all images
            for fp in fp_objects:
                if data.mode == "rolled":
                    fp.process_and_convert_type4(compression_ratio=ratios[retries])
                else:
                    fp.process_and_convert(compression_ratio=ratios[retries])
            
            # Re-generate EFT
            eft_path = generate_eft(data.type2_data, session_id, {fp.fp_number: fp for fp in fp_objects}, mode=data.mode)
            current_size = os.path.getsize(eft_path)
            retries += 1
        
        # If file still exceeds limit after all retries, raise error
        if current_size > max_size:
            raise HTTPException(status_code=400, detail=f"EFT size ({current_size} bytes) exceeds 11MB limit even after compression.")
        
        # Determine Filename
        fname = data.type2_data.get("fname", "Unknown")
        lname = data.type2_data.get("lname", "Unknown")

        # Sanitize
        safe_fname = "".join(c for c in fname if c.isalnum() or c in ('-', '_'))
        safe_lname = "".join(c for c in lname if c.isalnum() or c in ('-', '_'))
        
        # Generate filename
        filename = f"oeft-{safe_fname}-{safe_lname}.eft"
        
        # Rename the generated file to the user-friendly name
        new_path = os.path.join(session_dir, filename)
        shutil.move(eft_path, new_path)
        
        # Return download URL with session path and filename
        return {"download_url": f"/api/download/{session_id}/{filename}", "filename": filename}
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"EFT Generation failed: {str(e)}")

# View/Edit EFT Endpoints
# Upload an existing EFT file for viewing/editing
@app.post("/api/upload_eft")
async def upload_eft(file: UploadFile = File(...)):
    session_id = str(uuid.uuid4())
    session_dir = os.path.join(TMP_DIR, session_id)
    # Create session directory
    os.makedirs(session_dir, exist_ok=True)

    # Save uploaded file to session directory
    file_path = os.path.join(session_dir, "original.eft")
    with open(file_path, "wb") as buffer:
        shutil.copyfileobj(file.file, buffer)
        
    # Store session data
    SESSIONS[session_id] = {
        "eft_path": file_path,
        "mode": "view_edit"
    }
    
    # Return session ID
    return {"session_id": session_id}

# Parse the uploaded EFT and return data for the UI
@app.get("/api/eft_session/{session_id}")
async def get_eft_session(session_id: str):
    # Check if session exists
    if session_id not in SESSIONS or "eft_path" not in SESSIONS[session_id]:
        raise HTTPException(status_code=404, detail="Session not found")
        
    # Get session directory and EFT path
    session_dir = os.path.join(TMP_DIR, session_id)
    eft_path = SESSIONS[session_id]["eft_path"]
    
    # Parse EFT
    try:
        parser = EFTParser(eft_path)
        
        # 1. Type 2 Data
        type2_data = parser.get_type2_data()
        
        # 2. Extract Images
        images_dir = os.path.join(session_dir, "images")
        images = parser.extract_images(images_dir)
        
        # Prepare image URLs by mapping raw local path to endpoint        
        image_data = []
        for img in images:
            image_data.append({
                "fgp": img["fgp"],
                "url": f"/api/image/{session_id}/{os.path.basename(img['display_path'])}" if img['display_path'] else None,
                "width": img["width"],
                "height": img["height"]
            })
            
        # 3. Text Dump
        text_dump = parser.get_text_dump()
        return {
            "type2_data": type2_data,
            "images": image_data,
            "text_dump": text_dump
        }
    # Catch any errors and throw console message if present
    except Exception as e:
        import traceback
        traceback.print_exc()
        raise HTTPException(status_code=500, detail=f"Failed to parse EFT: {str(e)}")

@app.get("/api/image/{session_id}/{filename}")
async def get_image(session_id: str, filename: str):
    file_path = os.path.join(TMP_DIR, session_id, "images", filename)
    if os.path.exists(file_path):
        return FileResponse(file_path)
    raise HTTPException(status_code=404, detail="Image not found")

# Reconstruct the EFT with updated Type 2 data.
@app.post("/api/save_eft")
async def save_eft(data: SaveEFTRequest):
    # Get session ID and throw error if not present
    session_id = data.session_id
    if session_id not in SESSIONS or "eft_path" not in SESSIONS[session_id]:
        raise HTTPException(status_code=404, detail="Session not found")
        
    session_dir = os.path.join(TMP_DIR, session_id)
    eft_path = SESSIONS[session_id]["eft_path"]
    
    # Generate new filename
    output_path = os.path.join(session_dir, "edited.eft")
    
    try:
        editor = EFTEditor(eft_path, output_path)
        editor.save(data.type2_data)
        
        # Determine nicer filename if possible
        fname = data.type2_data.get("2.018", "edited")
        # Sanitize filename
        safe_fname = "".join(c for c in fname if c.isalnum() or c in ('-', '_', ','))
        final_name = f"edited-{safe_fname}.eft"
        
        final_path = os.path.join(session_dir, final_name)
        shutil.move(output_path, final_path)
        # Create download URL for edited EFT
        return {"download_url": f"/api/download/{session_id}/{final_name}", "filename": final_name}
    
    # Throw error if EFT can't be saved
    except Exception as e:
        import traceback
        traceback.print_exc()
        raise HTTPException(status_code=500, detail=f"Failed to save EFT: {str(e)}")

# Create download endpoint
@app.get("/api/download/{session_id}/{filename}")
async def download_file(session_id: str, filename: str):
    file_path = os.path.join(TMP_DIR, session_id, filename)
    if os.path.exists(file_path):
        return FileResponse(file_path, filename=filename)
    raise HTTPException(status_code=404, detail="File not found")

# Destroy session
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

class RawFP:
    def __init__(self, p, w=0, h=0, is_raw=False):
        self.img_path = p
        self.w = w
        self.h = h
        self.is_raw = is_raw

class SimpleFP:
    def __init__(self, p, w=0, h=0): 
        self.img_path = p
        self.w = w
        self.h = h


@app.post("/api/generate_fd258")
async def generate_fd258(data: GenerateRequest):
    # Get session
    session_id = data.session_id
    if session_id not in SESSIONS:
        raise HTTPException(status_code=404, detail="Session not found")
    
    session_data = SESSIONS[session_id]
    session_dir = os.path.join(TMP_DIR, session_id)
    
    # FD258 generation is only available for capture mode
    if session_data.get("mode") != "capture":
        raise HTTPException(status_code=400, detail="Only available for capture sessions")

    # Load images_map
    images_map = session_data["images"]
    print(f"DEBUG: images_map keys: {list(images_map.keys())}")
    
    # Process slaps (13, 14, 15) to get segments
    fp_objects = {}
    
    for fp_num in [13, 14, 15]:
        target_path = None
        # Robust key (int or str) and path check
        if fp_num in images_map:
            target_path = images_map[fp_num]
        elif str(fp_num) in images_map:
            target_path = images_map[str(fp_num)]
            
        if target_path:
             if not os.path.exists(target_path):
                 print(f"DEBUG: Image path not found: {target_path}")
                 continue
                 
             img = cv2.imread(target_path)
             if img is None: 
                 print(f"DEBUG: Failed to load image with cv2: {target_path}")
                 continue
             
             print(f"DEBUG: Loaded FP {fp_num} from {target_path}, shape={img.shape}")
             fp = Fingerprint(img, fp_num, session_dir, session_id)

             # Saves png and runs segment()
             # Note: Using a lower compression ratio for intermediate processing, 
             # but here we just want segmentation side effects.
             fp.process_and_convert(compression_ratio=10) 
             
             # Double check segmentation didn't fail silently
             if not fp.fingers:
                 print(f"FP {fp_num} has no segments after process. Forcing segment().")
                 fp.segment()
                 
             fp_objects[fp_num] = fp
             print(f"FP {fp_num} has {len(fp.fingers)} segments: {[f.n for f in fp.fingers]}")



    # Collect printable images
    prints_map = {}
    
    for fp in fp_objects.values():
         # 1. Plain boxes (Slaps)
         # Map the full slap images to their respective plain codes
         if fp.fp_number == 13:
             prints_map[13] = fp # R Slap -> P_R4
         elif fp.fp_number == 14:
             prints_map[14] = fp # L Slap -> P_L4
             
         # 2. Segments (Rolled boxes 1-10)
         for finger in fp.fingers:
             try:
                 fn = int(finger.n)
                 seg_path = os.path.join(session_dir, finger.name)
                 if fp.fp_number == 14:
                     # Swap 7 <-> 10 to properly place prints in order
                     if fn == 7: fn = 10
                     elif fn == 10: fn = 7
                     # Swap 8 <-> 9  to properly place prints in order
                     elif fn == 8: fn = 9
                     elif fn == 9: fn = 8
                     print(f"Swapped Left Hand Segment {finger.n} -> {fn}")

                 

                 # Check/Decode WSQ or RAW
                 if seg_path.endswith('.wsq'):
                     if os.path.exists(seg_path):
                         # Decode to RAW
                         raw_path = decode_wsq(seg_path)
                         sfp = RawFP(raw_path, finger.sw, finger.sh, is_raw=True)
                     else:
                         print(f"WSQ not found: {seg_path}")
                         continue
                 elif seg_path.endswith('.raw'):
                     if os.path.exists(seg_path):
                         sfp = RawFP(seg_path, finger.sw, finger.sh, is_raw=True)
                     else:
                          print(f"RAW not found: {seg_path}")
                          continue
                 elif os.path.exists(seg_path):
                     # Assume standard image and not WSQ or RAW
                     sfp = RawFP(seg_path)
                 else:
                     print(f"Segment file not found: {seg_path}")
                     continue
                     
                 # Map 1-10 (Rolled)
                 if 1 <= fn <= 10:
                     prints_map[fn] = sfp
                     print(f"Mapped Segment {fn} from {seg_path}")
                 
                 # Handling Thumbs from FP 15 (which return segments 11 and 12)
                 # Map 11 -> 1 (Rolled R Thumb) and 11 (Plain R Thumb)
                 # Map 12 -> 6 (Rolled L Thumb) and 12 (Plain L Thumb)
                 if fn == 11:
                     prints_map[1] = sfp  # Rolled R Thumb
                     prints_map[11] = sfp # Plain R Thumb
                     print(f"Mapped Segment {fn} to 1 and 11")
                 elif fn == 12:
                     prints_map[6] = sfp  # Rolled L Thumb
                     prints_map[12] = sfp # Plain L Thumb
                     print(f"Mapped Segment {fn} to 6 and 12")
                     
                 # Map Segments to Plain Thumbs 11/12 (Legacy checking 1/6)
                 if fn == 1:
                     prints_map[11] = sfp # P_RT (11) mapping to layout "P_RT"
                 elif fn == 6:
                     prints_map[12] = sfp # P_LT (12) mapping to layout "P_LT"

                             
             except Exception as e:
                 print(f"Error processing segment for FP {fp.fp_number}: {e}")

    print(f"DEBUG: Final prints_map keys: {list(prints_map.keys())}")



                 
    # Generate FD258
    try:
        generator = FD258Generator("static/img/fd258-blank.jpg")
        img_bytes = generator.generate(data.type2_data, prints_map)
        
        # Save
        filename = f"fd258-{session_id}.jpg"
        out_path = os.path.join(session_dir, filename)
        with open(out_path, "wb") as f:
            f.write(img_bytes)
            
        return {"download_url": f"/api/download/{session_id}/{filename}", "filename": filename}
    except Exception as e:
        import traceback
        traceback.print_exc()
        raise HTTPException(status_code=500, detail=f"FD258 Generation failed: {str(e)}")

