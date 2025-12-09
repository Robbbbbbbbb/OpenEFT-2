from PIL import Image, ImageDraw, ImageFont, ImageOps, ImageStat

import os
import io
from datetime import datetime


class FD258Generator:
    def __init__(self, blank_path):
        self.blank_path = blank_path
        self.width = 0
        self.height = 0
        if os.path.exists(blank_path):
            with Image.open(blank_path) as img:
                self.width = img.width
                self.height = img.height

        # Define Layout (Normalized coordinates)
        self.layout = {
            # Personal Details (approximate percentage coordinates based on FD-258)
            # Name: "LAST NAME ... FIRST NAME ... MIDDLE NAME"
            "name": {"x": 0.38, "y": 0.050, "size": 0.024}, 
            # Row: Aliases (AKA)
            "aliases": {"x": 0.05, "y": 0.115, "size": 0.018},
            "ori": {"x": 0.53, "y": 0.115, "size": 0.018},
            # Row: DOB, Sex, etc.
            "dob": {"x": 0.81, "y": 0.14, "size": 0.018},
            "sex": {"x": 0.55, "y": 0.172, "size": 0.018},
            "race": {"x": 0.58, "y": 0.172, "size": 0.018},
            "hgt": {"x": 0.62, "y": 0.172, "size": 0.018},
            "wgt": {"x": 0.67, "y": 0.172, "size": 0.018},
            "eyes": {"x": 0.71, "y": 0.172, "size": 0.018},
            "hair": {"x": 0.76, "y": 0.172, "size": 0.018},       
            # POB
            "pob": {"x": 0.86, "y": 0.172, "size": 0.018}, 
            # Citizenship (CTZ)
            "ctz": {"x": 0.36, "y": 0.172, "size": 0.018}, 
            # Residence
            "residence": {"x": 0.02, "y": 0.155, "size": 0.018},
            # Date generated
            "date": {"x": 0.00, "y": 0.19, "size": 0.01},
            # Reason Fingerprinted
            "reason": {"x": 0.02, "y": 0.32, "size": 0.018},
            # SOC
            "soc": {"x": 0.36, "y": 0.31, "size": 0.024}, # 
            # Fingerprints (box: x, y, w, h)
            # R1-R5 (Row 1) - y~0.37 approx correct
            "R1": {"x": 0.010, "y": 0.370, "w": 0.190, "h": 0.180}, # R. Thumb
            "R2": {"x": 0.205, "y": 0.370, "w": 0.190, "h": 0.180}, # R. Index
            "R3": {"x": 0.400, "y": 0.370, "w": 0.190, "h": 0.180}, # R. Middle
            "R4": {"x": 0.595, "y": 0.370, "w": 0.190, "h": 0.180}, # R. Ring
            "R5": {"x": 0.790, "y": 0.370, "w": 0.190, "h": 0.180}, # R. Little
            # L6-L10 (Row 2) - y~0.555 approx correct
            "L1": {"x": 0.010, "y": 0.555, "w": 0.190, "h": 0.180}, # L. Thumb
            "L2": {"x": 0.205, "y": 0.555, "w": 0.190, "h": 0.180}, # L. Index
            "L3": {"x": 0.400, "y": 0.555, "w": 0.190, "h": 0.180}, # L. Middle
            "L4": {"x": 0.595, "y": 0.555, "w": 0.190, "h": 0.180}, # L. Ring
            "L5": {"x": 0.790, "y": 0.555, "w": 0.190, "h": 0.180}, # L. Little
            # Row 3: Plain (slaps) - y~0.745 approx correct
            "P_L4": {"x": 0.010, "y": 0.745, "w": 0.385, "h": 0.230}, # Left 4 Fingers
            "P_LT": {"x": 0.400, "y": 0.745, "w": 0.095, "h": 0.230}, # L Thumb
            "P_RT": {"x": 0.500, "y": 0.745, "w": 0.095, "h": 0.230}, # R Thumb
            "P_R4": {"x": 0.600, "y": 0.745, "w": 0.385, "h": 0.230}  # Right 4 Fingers
        }
    def generate(self, type2_data, prints_map):
        """
        type2_data: dict of text fields
        prints_map: dict of Fingerprint objects or images
        """
        print(f"Generating FD258 with {len(prints_map)} prints...")
        img = Image.open(self.blank_path).convert("RGB")
        draw = ImageDraw.Draw(img)
        # Load font
        font_large = None
        font_small = None
        font_debug = None
        # List of candidate paths
        font_candidates = [
            "arial.ttf",
            r"C:\Windows\Fonts\arial.ttf",
            r"C:\Windows\Fonts\Arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            "FreeSans.ttf"
        ]
        for fpath in font_candidates:
            try:
                # Calculate size
                size_large = int(self.height * 0.018) # ~53px
                size_small = int(self.height * 0.015) # ~40px
                font_large = ImageFont.truetype(fpath, size_large)
                font_small = ImageFont.truetype(fpath, size_small)
                font_debug = ImageFont.truetype(fpath, 20)
                print(f"Success: Loaded font from {fpath}")
                break
            except IOError:
                continue
        if font_large is None:
            print("WARNING: Could not load any TrueType font. Falling back to default (tiny) bitmap font.") # Catch error if font fails to load
            font_large = ImageFont.load_default()
            font_small = ImageFont.load_default()
            font_debug = ImageFont.load_default()
        # 1. Draw Text
        # Mapping type2 keys to layout keys
        # 2.018 Name
        name = type2_data.get("2.018", "")
        # 2.022 DOB
        # Convert standard YYYYMMDD format -> MM/DD/YYYY (eg: 20250101 -> 01/01/2025)
        dob_raw = type2_data.get("2.022", "")
        dob = dob_raw
        if len(dob_raw) == 8 and dob_raw.isdigit():
            dob = f"{dob_raw[4:6]}/{dob_raw[6:8]}/{dob_raw[0:4]}"
        # 2.016 SOC
        soc = type2_data.get("2.016", "")
        # 2.024 Sex
        sex = type2_data.get("2.024", "")
        # 2.025 Race
        race = type2_data.get("2.025", "")
        # 2.027 Hgt
        hgt = type2_data.get("2.027", "")
        # 2.029 Wgt
        wgt = type2_data.get("2.029", "")
        # 2.031 Eyes
        eyes = type2_data.get("2.031", "")
        # 2.032 Hair
        hair = type2_data.get("2.032", "")
        # 2.020 POB
        pob = type2_data.get("2.020", "")
        # 2.021 CTZ
        ctz = type2_data.get("2.021", "")
        # 2.041 Residence
        residence = type2_data.get("2.041", "")
        # 2.038 Date Fingerprinted
        # Convert standard YYYYMMDD format -> MM/DD/YYYY (eg: 20250101 -> 01/01/2025)
        date_txt = type2_data.get("2.038", "")
        if len(date_txt) == 8 and date_txt.isdigit():
             date_txt = f"{date_txt[4:6]}/{date_txt[6:8]}/{date_txt[0:4]}"
        elif not date_txt:
             # Default to current date if missing
             date_txt = datetime.now().strftime("%m/%d/%Y")             
        # 2.049 Reason Fingerprinted
        reason = type2_data.get("2.049", "FAUF") # Default to FAUF to be more useful if not used with ATF
        fields = [
             ("name", name), ("dob", dob), ("soc", soc), ("sex", sex),
             ("race", race), ("hgt", hgt), ("wgt", wgt), ("eyes", eyes),
             ("hair", hair), ("pob", pob), ("ctz", ctz),
             ("residence", residence), ("date", date_txt), ("reason", reason)
        ]
        for key, val in fields:
            if key in self.layout:
                conf = self.layout[key]
                x = int(conf["x"] * self.width)
                y = int(conf["y"] * self.height)
                f = font_large if conf.get("size", 0) >= 0.015 else font_small
                draw.text((x, y), str(val), fill="black", font=f)

        # 2. Draw Fingerprints
        # prints_map keys: 1-10 (Rolled), 11(RT Plain), 12(LT Plain), 13(R Plain 4), 14(L Plain 4), 15 (Thumbs Plain)
        # Map FGPs to layout keys
        fgp_map = {
            1: "R1", 2: "R2", 3: "R3", 4: "R4", 5: "R5",
            6: "L1", 7: "L2", 8: "L3", 9: "L4", 10: "L5",
            # Plain
            13: "P_R4", 14: "P_L4", 
            11: "P_RT", 12: "P_LT"
        }
        
        # FD258 expects separate plain thumbs (11, 12); splitting record is required
        # Iterate over all expected keys in layout to ensure we draw something (image or error) and check all mapping keys
        for fgp, layout_key in fgp_map.items():
            if layout_key not in self.layout: continue
            fp_obj = prints_map.get(fgp)
            if fp_obj:
                # Get the image from fp_obj
                # fp_obj is expected to be a Fingerprint object which has an image (numpy array or path)
                fp_img = None
                img_error_msg = ""
                if hasattr(fp_obj, 'is_raw') and fp_obj.is_raw:
                    # Load Raw
                    try:
                        with open(fp_obj.img_path, 'rb') as f:
                            raw_data = f.read()
                        # Validate expected size
                        expected_size = fp_obj.w * fp_obj.h
                        if len(raw_data) > expected_size:
                             # Header expected
                             print(f"Trimming raw data: {len(raw_data)} -> {expected_size}")
                             raw_data = raw_data[-expected_size:]
                        if len(raw_data) != expected_size:
                             raise ValueError(f"Size mismatch: {len(raw_data)} != {fp_obj.w}*{fp_obj.h}")
                        fp_img = Image.frombytes('L', (fp_obj.w, fp_obj.h), raw_data)
                    # Error handling if raw data fails
                    except Exception as e:
                        print(f"Failed to load raw image: {e}")
                        img_error_msg = str(e)
                elif hasattr(fp_obj, 'img_path') and os.path.exists(fp_obj.img_path):
                    try:
                         # Force verify
                         img_verify = Image.open(fp_obj.img_path)
                         img_verify.verify()
                         # Load image
                         fp_img = Image.open(fp_obj.img_path)
                    # Error handling if image fails
                    except Exception as e:
                        print(f"Failed to load image: {e}")
                        img_error_msg = str(e)

                elif hasattr(fp_obj, 'img') or hasattr(fp_obj, 'image'): # numpy array
                    # Convert cv2 (BGR) to RGB
                    import cv2
                    try:
                        # Handle both naming conventions
                        img_arr = getattr(fp_obj, 'img', None)
                        if img_arr is None:
                            img_arr = getattr(fp_obj, 'image', None)
                        # Error handling if numpy array fails
                        if img_arr is not None and img_arr.size > 0:
                            if len(img_arr.shape) == 3:
                                rgb = cv2.cvtColor(img_arr, cv2.COLOR_BGR2RGB)
                            else:
                                rgb = cv2.cvtColor(img_arr, cv2.COLOR_GRAY2RGB)
                            fp_img = Image.fromarray(rgb)
                        else:
                             img_error_msg = "Empty CV2 Image"
                    # Error handling if numpy array fails
                    except Exception as e:
                        print(f"Failed to load CV2 image: {e}")
                        img_error_msg = str(e)

                if fp_img:
                    # Convert to grayscale
                    # Detect contrast and auto-invert if mostly dark (white ridges on black background)
                    if fp_img.mode != 'L':
                        fp_img = fp_img.convert('L')
                    stat = ImageStat.Stat(fp_img)
                    if stat.mean[0] < 128:
                        # Mostly dark, invert
                        fp_img = ImageOps.invert(fp_img)
                    conf = self.layout[layout_key]
                    # Get target dimensions
                    target_w = int(conf["w"] * self.width)
                    target_h = int(conf["h"] * self.height)
                    target_x = int(conf["x"] * self.width)
                    target_y = int(conf["y"] * self.height)
                    # Resize to fit within aspect ratio
                    fp_img.thumbnail((target_w, target_h), Image.Resampling.LANCZOS)
                    # Center in box
                    paste_x = target_x + (target_w - fp_img.width) // 2
                    paste_y = target_y + (target_h - fp_img.height) // 2
                    # Paste image in box
                    img.paste(fp_img, (paste_x, paste_y))
                else:
                    # Debug: Draw red X if image missing
                    # TODO: Not sure how this might handle missing images, may need to adjust in the future
                    conf = self.layout[layout_key]
                    target_w = int(conf["w"] * self.width)
                    target_h = int(conf["h"] * self.height)
                    target_x = int(conf["x"] * self.width)
                    target_y = int(conf["y"] * self.height)
                    draw.rectangle([target_x, target_y, target_x + target_w, target_y + target_h], outline="red", width=5)
                    # Wrap error text
                    err_lines = [img_error_msg[i:i+20] for i in range(0, len(img_error_msg), 20)]
                    y_off = 10
                    for line in err_lines:
                         draw.text((target_x+10, target_y+y_off), line, fill="red", font=font_debug)
                         y_off += 25
            else:
                 # Missing Content
                 img_error_msg = "MISSING DATA"
                 conf = self.layout[layout_key]
                 target_w = int(conf["w"] * self.width)
                 target_h = int(conf["h"] * self.height)
                 target_x = int(conf["x"] * self.width)
                 target_y = int(conf["y"] * self.height)
                 draw.rectangle([target_x, target_y, target_x + target_w, target_y + target_h], outline="blue", width=3)
                 draw.text((target_x+10, target_y+target_h/2), "MISSING", fill="blue", font=font_debug)
        # Return bytes
        buf = io.BytesIO()
        img.save(buf, format="JPEG", quality=90)
        print("FD258 Generation Complete")
        return buf.getvalue()
