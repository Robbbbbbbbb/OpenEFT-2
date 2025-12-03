import os
import cv2
import imutils
import numpy as np

# Read image, assume user uploads a resonably-aligned scan or uses the Crop/Rotate tool.
def align_image(img_path):
    # Return image as uploaded
    img = cv2.imread(img_path)
    if img is None:
        raise ValueError("Image not found")
    return img, True

# Logic to crop and rotate the image in case the user uploads something rotated 90/180/270 degrees or cropped out of alignment.
def apply_crop_and_rotate(img_path, rotate_angle, crop_rect):
    4# crop_rect: {x, y, w, h}
    print(f"Applying crop/rotate: path={img_path}, rot={rotate_angle}, rect={crop_rect}")
    img = cv2.imread(img_path)
    if img is None:
        raise ValueError("Image not found")
        
    # 1. Rotate
    if rotate_angle != 0:
        if rotate_angle == 90:
            img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
        elif rotate_angle == 180:
            img = cv2.rotate(img, cv2.ROTATE_180)
        elif rotate_angle == 270:
            img = cv2.rotate(img, cv2.ROTATE_90_COUNTERCLOCKWISE)
            
    # 2. Crop
    x, y, w, h = int(crop_rect['x']), int(crop_rect['y']), int(crop_rect['w']), int(crop_rect['h'])
    
    # Ensure bounds
    if x < 0: x = 0
    if y < 0: y = 0
    if x + w > img.shape[1]: w = img.shape[1] - x
    if y + h > img.shape[0]: h = img.shape[0] - y
    
    if w > 0 and h > 0:
        img = img[y:y+h, x:x+w]
        
    print(f"Resulting cropped shape: {img.shape}")
    # For session simplicity, overwrite aligned.png
    return img

# Returns the default bounding boxes scaled to the image size
def get_default_boxes(img_shape):
    # NOTE: This logic is based on the FD-258 format for the three Type 14 records, define Type-4 logic later.
    # Approximated relative coordinates for FD-258: (x_percent, y_percent, w_percent, h_percent)
    
    defaults = {
        "L_SLAP": (0.04, 0.74, 0.30, 0.22),
        "R_SLAP": (0.66, 0.74, 0.30, 0.22),
        "THUMBS": (0.36, 0.74, 0.28, 0.22)
    }
    
    h, w = img_shape[:2]
    boxes = []
    
    mapping = [
        ("L_SLAP", 14), 
        ("R_SLAP", 13), 
        ("THUMBS", 15)
    ]
    
    for name, num in mapping:
        xp, yp, wp, hp = defaults[name]
        boxes.append({
            "id": name,
            "fp_number": num,
            "x": int(xp * w),
            "y": int(yp * h),
            "w": int(wp * w),
            "h": int(hp * h)
        })
            
    return boxes
