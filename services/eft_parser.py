import os
import subprocess
import shutil
try:
    import cv2
except ImportError:
    cv2 = None
from typing import Dict, List, Tuple, Optional, Any
from services.eft_helper import FS_CHAR, GS_CHAR, RS_CHAR, US_CHAR

class EFTParser:
    def __init__(self, file_path: str):
        self.file_path = file_path
        self.records = []
        self._parse()

    def _parse(self):
        if not os.path.exists(self.file_path):
            raise FileNotFoundError(f"EFT file not found: {self.file_path}")
        
        with open(self.file_path, 'rb') as f:
            data = f.read()
        
        offset = 0
        file_len = len(data)
        
        # Safety break
        while offset < file_len:
            # Special Handling for Record 1 (Type 1 Header)
            # 1.001 contains total FILE length, NOT just Type-1 record length.
            # Record 1 ends at the first FS separator.
            # Check if this is potentially Record 1
            # We assume Record 1 starts at offset 0, or after previous record.
            try:
                # Find first colon
                first_colon = data.find(b':', offset)
                if first_colon != -1:
                    tag_bytes = data[offset:first_colon]
                    tag_str = tag_bytes.decode('ascii', errors='ignore')
                    
                    if tag_str == "1.001":
                        # This is Type 1 Record.
                        # Scan for FS break
                        fs_index = data.find(bytes([FS_CHAR]), offset)
                        if fs_index == -1:
                             # Catch invalid file. This should not happen in valid EFT file.
                             print("Error: Type 1 record has no FS terminator.")
                             break
                        
                        record_len = (fs_index - offset) + 1 # Include FS
                        
                        # Extract and parse
                        record_data = data[offset : offset + record_len]
                        parsed_record = self._parse_record(record_data)
                        self.records.append(parsed_record)
                        
                        offset += record_len
                        continue
            except Exception as e:
                print(f"Error checking for Type 1 at {offset}: {e}")

            # Standard Logic for other records (Type 2..99)
            # Find the first GS separator to extract LEN
            try:
                gs_index = data.index(bytes([GS_CHAR]), offset)
            except ValueError:
                break
            
            # Extract the first field content to get length
            first_field_bytes = data[offset:gs_index]
            try:
                first_field_str = first_field_bytes.decode('ascii', errors='ignore')
                if ':' not in first_field_str:
                     print(f"Malformed header at {offset}: {first_field_str}")
                     break
                tag, length_str = first_field_str.split(':')
                record_len = int(length_str)
            except Exception as e:
                print(f"Error parsing record length at offset {offset}: {e}")
                break
                
            if offset + record_len > file_len:
                print(f"Record length {record_len} exceeds file size. Truncating.")
                record_len = file_len - offset
            
            record_data = data[offset : offset + record_len]
            parsed_record = self._parse_record(record_data)
            self.records.append(parsed_record)
            
            offset += record_len

    def _parse_record(self, data: bytes) -> Dict[str, Any]:
        fields = {}
        
        # Get Record Type from header
        gs_index = data.find(bytes([GS_CHAR]))
        if gs_index == -1:
             if data.endswith(bytes([FS_CHAR])):
                 content = data[:-1]
             else:
                 content = data
             key, val = self._parse_field_entry(content)
             if key: fields[key] = val
             return fields

        first_field = data[:gs_index].decode('ascii', errors='ignore')
        rec_type = first_field.split('.')[0]
        
        # Identify binary records.
        # Only expecting Type 4 (Fingerprint) and 14 (Fingerprint). If anything else, skip the record type, but identify it to avoid crashing and unhandled exceptions.
        is_binary = rec_type in ['4', '7', '8', '10', '13', '14', '15', '16', '17']
        
        # Strip trailing FS for processing
        processing_data = data
        if processing_data.endswith(bytes([FS_CHAR])):
             processing_data = processing_data[:-1]

        if not is_binary:
            # Text Record: split by GS
            parts = processing_data.split(bytes([GS_CHAR]))
            for p in parts:
                k, v = self._parse_field_entry(p)
                if k: fields[k] = v
        else:
            # Binary Record: carefully parse fields until we find the image blob
            curr = 0
            
            while curr < len(processing_data):
                # Try to find a valid tag pattern "N.NNN:"
                # Search for next colon (":") (and GS)
                # Check if segment between curr and colon is a valid text field.
                # Note that image data can contain GS, meaning that the record must be inspeced to make sure it's at the start of a tag (eg: "rec_type.field_id:")
                colon_pos = processing_data.find(b':', curr)
                if colon_pos == -1:
                    break
                
                tag_candidate = processing_data[curr:colon_pos]
                
                valid_tag = False
                try:
                    tag_str = tag_candidate.decode('ascii')
                    if '.' in tag_str:
                         parts = tag_str.split('.')
                         if len(parts) == 2 and parts[0].isdigit() and parts[1].isdigit():
                             valid_tag = True
                             field_id = parts[1]
                except:
                    pass
                
                if valid_tag:
                    if field_id == '999':
                         # Image data present here
                         # Consumes rest of processing_data
                         value_bytes = processing_data[colon_pos+1:]
                         fields[tag_str] = value_bytes
                         break
                    else:
                        # Normal text field
                        # Ends at next GS
                        next_gs = processing_data.find(bytes([GS_CHAR]), colon_pos)
                        if next_gs == -1: next_gs = len(processing_data)
                        
                        val_bytes = processing_data[colon_pos+1:next_gs]
                        fields[tag_str] = val_bytes.decode('utf-8', errors='replace')
                        curr = next_gs + 1
                else:
                    curr = colon_pos + 1 # Advance and retry
        
        return fields

    def _parse_field_entry(self, data: bytes) -> Tuple[Optional[str], Any]:
        try:
            if b':' in data:
                parts = data.split(b':', 1)
                key = parts[0].decode('ascii')
                if key.endswith('.999'):
                     return key, parts[1] # Return bytes for 999
                else:
                     return key, parts[1].decode('utf-8', errors='replace')
            return None, None
        except:
            return None, None

    def get_type2_data(self) -> Dict[str, str]:
        t2 = next((r for r in self.records if any(k.startswith('2.') for k in r.keys())), None)
        if t2:
            # Filter for Type 2 keys and return
            return {k: v for k, v in t2.items() if k.startswith('2.') and k != '2.001'} # Exclude LEN
        return {}

    def extract_images(self, output_dir: str) -> List[Dict[str, Any]]:
        images = []
        os.makedirs(output_dir, exist_ok=True)
        
        for r in self.records:
            first_key = list(r.keys())[0]
            rec_type = first_key.split('.')[0]
            
            if rec_type in ['4', '14']:
                img_key = f"{rec_type}.999"
                if img_key in r:
                    data = r[img_key]
                    
                    # Metadata
                    fgp_key = f"{rec_type}.013" if rec_type == '14' else "4.004"
                    fgp = r.get(fgp_key, "0")
                    
                    cga_key = f"{rec_type}.011" if rec_type == '14' else "4.008"
                    cga = r.get(cga_key, "RAW")
                    
                    ext = "raw"
                    if isinstance(cga, str):
                        if "JP2" in cga: ext = "jp2"
                        elif "WSQ" in cga: ext = "wsq"
                    
                    filename = f"fp_{fgp}.{ext}"
                    out_path = os.path.join(output_dir, filename)
                    
                    with open(out_path, 'wb') as f:
                        f.write(data)
                        
                    png_filename = f"fp_{fgp}.png"
                    png_path = os.path.join(output_dir, png_filename)
                    
                    width = r.get(f"{rec_type}.006", "0")
                    height = r.get(f"{rec_type}.007", "0")
                    
                    # Convert to png
                    converted = False
                    if cv2 is not None:
                        try:
                            if ext == "jp2":
                                img = cv2.imread(out_path)
                                if img is not None:
                                    cv2.imwrite(png_path, img)
                                    converted = True
                            elif ext == "wsq":
                                # If WSQ filetype, use NBIS dwsq to preview. If tool is not available due to compile problem, skip preview.
                                pass
                        except Exception as e:
                            print(f"Error converting {filename}: {e}")
                    
                    if not converted and ext == "jp2":
                        # Fallback: Maybe generic PIL?
                        pass

                    images.append({
                        "fgp": fgp,
                        "original_path": out_path,
                        "display_path": png_path if converted else None,
                        "width": width,
                        "height": height,
                        "cga": cga
                    })
        return images
    
    def get_text_dump(self) -> str:
        # Use an2k2txt to dump the file output
        try:
             cmd = ["an2k2txt", self.file_path]
             res = subprocess.run(cmd, capture_output=True, text=True)
             if res.returncode == 0:
                 return res.stdout
        except:
            pass
            
        # Fallback: Generate dump from parsed records
        out = []
        for i, r in enumerate(self.records):
            out.append(f"Record {i+1}")
            # Sort keys by field number
            def sort_key(k):
                try:
                    parts = k.split('.')
                    return (int(parts[0]), int(parts[1]))
                except:
                    return (0,0)
                    
            keys = sorted(r.keys(), key=sort_key)
            for k in keys:
                val = r[k]
                if isinstance(val, bytes):
                    val = f"<Binary Data: {len(val)} bytes>"
                out.append(f"{k} : {val}")
            out.append("-" * 20)
        return "\n".join(out)
