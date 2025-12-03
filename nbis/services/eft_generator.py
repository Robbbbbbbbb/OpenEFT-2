import os
import cv2
import shutil
import subprocess
import uuid
from services.eft_helper import Type1, Type2, Type14, Type4, get_date
from services.fingerprint import Fingerprint 

# We need to define settings.TMP_DIR logic since we aren't in Django
TMP_DIR = "/app/temp"

from services.nbis_helper import verify_eft

def generate_eft(data, session_id, prints_map):
    """
    Generates the EFT file.
    data: Type 2 record data (dict).
    session_id: UUID string for the session.
    prints_map: Dict of {fp_number: Fingerprint object}.
    """
    session_dir = os.path.join(TMP_DIR, session_id)
    
    t1 = Type1()
    t2 = Type2(1) # IDC starts at 1 for the first record
    
    # Map data to Type 2 fields according to EFT Record Type Explanations.txt
    t2.name = data.get("2.018", "")
    t2.dob = data.get("2.022", "")
    t2.addr = data.get("2.041", "")
    t2.pob = data.get("2.020", "")
    t2.ctz = data.get("2.021", "")
    t2.ssn = data.get("2.016", "")
    t2.race = data.get("2.025", "")
    t2.eye = data.get("2.031", "")
    t2.hair = data.get("2.032", "")
    t2.height = data.get("2.027", "")
    t2.weight = data.get("2.029", "")
    t2.sex = data.get("2.024", "")
    
    t1.add_record(t2)
    
    # Generate Transaction Control Number (TCN)
    n = f"TCN-{uuid.uuid4().hex[:10]}"
    t1.set_tcn(n)
    
    fname = f"{n}.eft"
    
    # Create Type 14 records only
    sorted_prints = sorted(prints_map.items(), key=lambda item: int(item[0]))
    
    idc = 2 # IDC continues from the Type 2 record
    for fp_num, fp_obj in sorted_prints:
        if int(fp_num) in [13, 14, 15]: # Only Left Slap, Right Slap, and Thumbs
            t14 = Type14(fp_obj, idc)
            t14.fcd = get_date().split(':')[0]
            t14.build()
            t1.add_record(t14)
            idc += 1
            
    output_path = os.path.join(session_dir, fname)
    t1.write_to_file(output_path)
    
    # Verify the generated EFT file
    is_valid, message = verify_eft(output_path)
    if not is_valid:
        raise Exception(f"EFT verification failed: {message}")
        
    return output_path

