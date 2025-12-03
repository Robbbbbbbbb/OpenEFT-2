import os
import cv2
import math
import numpy as np
from services.eft_helper import US_CHAR
from services.nbis_helper import segment_fingerprints, get_nfiq_quality

class Finger:
    def __init__(self, str_data):
        self.orgID = "15" # Vendor ID for NFIQv1
        self.algID = "14205" # Algorithm ID for NFIQv1
        self.str = str_data
        self.n = "0"
        self.sw = 0
        self.sh = 0
        self.sx = 0
        self.sy = 0
        self.t = 0.0
        self.readString()
        self.computeBox()
        self.segmentQuality()

    def readString(self):
        # Example: FILE lfour_10.raw -> e 3 sw 168 sh 280 sx 120 sy 256 th -28.3
        vals = self.str.split(' ')
        try:
            self.name = vals[1]
            # Try to infer finger number from filename if possible
            # Usually nfseg output: basename_XX.ext
            # We will default to 0 if unknown
            base = os.path.splitext(self.name)[0]
            if '_' in base:
                parts = base.split('_')
                if parts[-1].isdigit():
                    self.n = str(int(parts[-1]))
            
            if 'sw' in vals:
                idx_sw = vals.index('sw') + 1
                self.sw = int(vals[idx_sw])
            if 'sh' in vals:
                idx_sh = vals.index('sh') + 1
                self.sh = int(vals[idx_sh])
            if 'sx' in vals:
                idx_sx = vals.index('sx') + 1
                self.sx = int(vals[idx_sx])
            if 'sy' in vals:
                idx_sy = vals.index('sy') + 1
                self.sy = int(vals[idx_sy])
            if 'th' in vals:
                idx_th = vals.index('th') + 1
                self.t = float(vals[idx_th])
            
        except Exception as e:
            print(f"Error parsing Finger string: {e}")

    def computeBox(self):
        self.x1 = str(abs(int((self.sw / 2) * math.cos(self.t) - (self.sh / 2)))) 
        self.y1 = str(abs(int((self.sh / 2) * math.cos(self.t) - (self.sw / 2)))) 
        self.x2 = str(abs(int((self.sw / 2) * math.cos(self.t) + (self.sh / 2)))) 
        self.y2 = str(abs(int((self.sh / 2) * math.cos(self.t) - (self.sw / 2)))) 

    def segmentQuality(self):
        try:
            self.score = str(get_nfiq_quality(self.name))
        except Exception as e:
            # print(f"NFIQ failed: {e}") # Silent fail
            self.score = "255"

    def getScoreString(self):
        return self.n + chr(US_CHAR) + self.score + chr(US_CHAR) + self.orgID + chr(US_CHAR) + self.algID

    def getPosString(self):
        return self.n + chr(US_CHAR) + self.x1 + chr(US_CHAR) + self.x2 + chr(US_CHAR) + self.y1 + chr(US_CHAR) + self.y2


class Fingerprint:
    def __init__(self, src_img, fp_number, tmpdir, session_id):
        self.tmpdir = tmpdir
        self.session_id = session_id
        self.fp_number = fp_number
        self.name = f"{session_id}_{fp_number}"
        self.encoding = 'png'
        self.converted = ""
        
        # Force 8-bit Grayscale
        if len(src_img.shape) == 3 and src_img.shape[2] == 3:
            print(f"Converting FP {fp_number} from RGB {src_img.shape} to Grayscale")
            self.img = cv2.cvtColor(src_img, cv2.COLOR_BGR2GRAY)
        else:
            self.img = src_img
            
        print(f"FP {fp_number} Image Shape: {self.img.shape}")
        self.fingers = []
        
        self.hps = 500
        self.vps = 500
        self.cga = "JP2"
        self.bpx = 8

    def process_and_convert(self, compression_ratio=10):
        """
        Resizes (if needed), saves, converts to JP2.
        compression_ratio: passed to opj_compress -r
        """
        png_path = os.path.join(self.tmpdir, self.name + ".png")
        if not os.path.exists(png_path):
            cv2.imwrite(png_path, self.img)
            
        png_size = os.path.getsize(png_path)
        print(f"FP {self.fp_number} PNG Saved: {png_path} ({png_size} bytes)")
        
        jp2_path = os.path.join(self.tmpdir, self.name + ".jp2")
        
        # Command: opj_compress -i input.png -o output.jp2 -r ratio -n 2
        cmd = ["opj_compress", "-i", png_path, "-o", jp2_path, "-r", str(compression_ratio), "-n", "2"]
        
        try:
            # Capture stdout/stderr
            import subprocess
            res = subprocess.run(cmd, capture_output=True, text=True)
            if res.returncode != 0:
                print(f"opj_compress failed for FP {self.fp_number}: {res.stderr}")
                return None
            
            self.converted = jp2_path
        except Exception as e:
            print(f"Conversion failed: {e}")
            return None

        if int(self.fp_number) >= 13:
            self.segment()
            
        return self.converted

    def segment(self):
        png_path = os.path.join(self.tmpdir, self.name + ".png")
        try:
            segments = segment_fingerprints(png_path, self.fp_number)
            for segment in segments:
                # The Finger class expects a string, so we need to reconstruct it
                line = f"FILE {segment['file']} e 3 sw {segment['sw']} sh {segment['sh']} sx {segment['sx']} sy {segment['sy']} th {segment['th']}"
                self.fingers.append(Finger(line))
        except Exception as e:
            print(f"Segmentation failed: {e}")
