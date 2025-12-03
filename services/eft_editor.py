import os
from services.eft_helper import Type1, Type2, Type14Raw, get_date
from services.eft_parser import EFTParser

class EFTEditor:
    def __init__(self, original_path: str, output_path: str):
        self.original_path = original_path
        self.output_path = output_path
        self.parser = EFTParser(original_path)
        
    # Reconstruct EFT with updated Type 2 data (use eft_helper classes)
    def save(self, type2_updates: dict):
        print(f"Starting EFT Save/Reconstruction: {self.original_path} -> {self.output_path}")
        
        # 1. Parse Records
        records = self.parser.records
        if not records:
            raise ValueError("No records found in original file.")
        
        print(f"Parsed {len(records)} records from original file.")
            
        # 2. Extract Type-1
        t1_data = records[0]
        t1 = Type1()
        t1.from_dict(t1_data)
        print(f"Initialized Type 1 Record (TCN: {t1.tcn})")
        
        # 3. Process Other Records
        # Rebuild the sequence of records in t1.cnt ( T1 > T2 > T4 -or- T14)
        
        # Find Type 2 index
        t2_idx = -1
        for i, r in enumerate(records):
            if any(k.startswith('2.') for k in r.keys()):
                t2_idx = i
                break
        
        if t2_idx == -1:
             raise ValueError("No Type 2 record found.")
             
        # Create Type 2 Object
        t2_data = records[t2_idx].copy()
        
        # Merge Updates
        print("Applying Type 2 Updates:")
        for k, v in type2_updates.items():
            if k.startswith('2.'):
                print(f"  - {k}: {v}")
                t2_data[k] = str(v)
                
        t2 = Type2(1) # IDC 1
        t2.from_dict(t2_data)
        
        t1.add_record(t2)
        print("Added Updated Type 2 Record.")
        
        # Process Type-14 (or other binary records)
        # Iterate records, skipping T1 (idx 0) and T2 (idx t2_idx)
        # Make sure records are terated in order
        
        current_idc = 2
        
        for i, r in enumerate(records):
            if i == 0 or i == t2_idx:
                continue
            
            # Determine type
            keys = list(r.keys())
            if not keys: continue
            rec_type = keys[0].split('.')[0]
            
            if rec_type == '14':
                print(f"Processing Record {i} (Type 14, IDC {current_idc})...")
                try:
                    t14 = Type14Raw(r, current_idc)
                    t1.add_record(t14)
                    current_idc += 1
                except Exception as e:
                    print(f"Error creating Type14Raw for record {i}: {e}")
                    raise e
            else:
                # Check for unsupported record type
                # If Type 4 (or other) records are encountered, preserve them
                print(f"Warning: Skipping unsupported record type {rec_type} at index {i}")
        
        # 4. Write to file
        # t1.write_to_file calls get_len recursively, recalculating everything correctly.
        print("Writing finalized EFT file...")
        t1.write_to_file(self.output_path)
        print("EFT Save Complete.")
        
        return self.output_path
