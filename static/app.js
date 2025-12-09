
// Global State
let sessionId = null;
let image = new Image(); // For verification/box selection (Step 1)
let cropImage = new Image(); // For crop step (Step 1)
let boxes = []; // Array of fingerprint boxes
let activeBoxIndex = -1;
let isCaptureSession = false; // Track if session is from capture

// Crop State
let cropRotation = 0;
let cropBox = null;
let isCropDragging = false;
let cropStart = null;
let cropScaleFactor = 1;

// Box Selection State
let isDragging = false;
let isResizing = false;
let resizeHandle = null;
let scaleFactor = 1;
let selectedGenMode = 'atf'; // 'atf' or 'rolled'

// Edit/View State
let isEditMode = false;
let editSessionId = null;

// UI References
const pStep1 = document.getElementById('p-step-1');
const pStep2 = document.getElementById('p-step-2');
const pStep3 = document.getElementById('p-step-3');
const wizardProgressBar = document.getElementById('wizard-progress-bar');

const panelStep1 = document.getElementById('step-1-panel');
const panelStep2 = document.getElementById('step-2-panel');
const panelStep3 = document.getElementById('step-3-panel');
const panelDownload = document.getElementById('step-download-panel');

const viewUpload = document.getElementById('view-upload');
const viewCrop = document.getElementById('view-crop');
const viewModeSelect = document.getElementById('view-mode-select');
const viewBox = document.getElementById('view-box');

// New Section References
const sectionNewEft = document.getElementById('section-new-eft');
const sectionCapture = document.getElementById('section-capture-prints');
const sectionEditEft = document.getElementById('section-edit-eft');
const sectionDynamic = document.getElementById('section-dynamic-content');
const editViewUpload = document.getElementById('edit-view-upload');
const editViewMode = document.getElementById('edit-view-mode');
const editViewMain = document.getElementById('edit-view-main');

const btnBack = document.getElementById('btn-back');
const btnNext = document.getElementById('btn-next');
const btnSubmit = document.getElementById('btn-submit');
const loading = document.getElementById('loading');

const navNew = document.querySelector('.nav-links li:first-child');
const navCapture = document.getElementById('nav-capture');
const navEdit = document.getElementById('nav-edit');
const navInfo = document.getElementById('nav-info');
const navAbout = document.getElementById('nav-about');

const dynamicTitle = document.getElementById('dynamic-title');
const dynamicBody = document.getElementById('dynamic-content-body');

// Canvases
const cropCanvas = document.getElementById('crop-canvas');
const cropCtx = cropCanvas.getContext('2d');
const verifyCanvas = document.getElementById('editor-canvas');
const verifyCtx = verifyCanvas.getContext('2d');

// Navigation Wizard
let currentAppMode = 'new'; // 'new' or 'edit'
let currentStep = 1; // 1, 2, 3
let currentSubStep = 'upload'; // 'upload', 'crop', 'mode', 'box'

// Navigation Handling
navEdit.onclick = () => {
    currentAppMode = 'edit';
    updateAppMode();
};

navCapture.onclick = () => {
    currentAppMode = 'capture';
    updateAppMode();
};

navInfo.onclick = () => {
    currentAppMode = 'info';
    updateAppMode();
};

navAbout.onclick = () => {
    currentAppMode = 'about';
    updateAppMode();
};

function updateAppMode() {
    navNew.classList.remove('active');
    navEdit.classList.remove('active');
    navCapture.classList.remove('active');
    navInfo.classList.remove('active');
    navAbout.classList.remove('active');

    // Reset Views
    sectionNewEft.classList.add('hidden');
    sectionCapture.classList.add('hidden');
    sectionEditEft.classList.add('hidden');
    sectionDynamic.classList.add('hidden');
    wizardProgressBar.classList.add('hidden');
    document.getElementById('main-footer').classList.add('hidden');

    if (currentAppMode === 'new') {
        navNew.classList.add('active');
        sectionNewEft.classList.remove('hidden');
        wizardProgressBar.classList.remove('hidden');
        document.getElementById('main-footer').classList.remove('hidden');
        updateWizardUI();
    } else if (currentAppMode === 'capture') {
        navCapture.classList.add('active');
        sectionCapture.classList.remove('hidden');
        initCaptureMode();
    } else if (currentAppMode === 'edit') {
        navEdit.classList.add('active');
        sectionEditEft.classList.remove('hidden');
        // Reset Edit Flow
        editViewUpload.classList.remove('hidden');
        editViewMode.classList.add('hidden');
        editViewMain.classList.add('hidden');
        panelDownload.classList.add('hidden');
    } else if (currentAppMode === 'info') {
        navInfo.classList.add('active');
        sectionDynamic.classList.remove('hidden');
        loadDynamicContent('https://raw.githubusercontent.com/Robbbbbbbbb/OpenEFT-2/main/dynamic/info.md', 'Info');
    } else if (currentAppMode === 'about') {
        navAbout.classList.add('active');
        sectionDynamic.classList.remove('hidden');
        loadDynamicContent('https://raw.githubusercontent.com/Robbbbbbbbb/OpenEFT-2/main/dynamic/about.md', 'About');
    }
}

async function loadDynamicContent(url, title) {
    dynamicTitle.textContent = title;
    dynamicBody.innerHTML = '';
    showLoading(true);

    try {
        // Append timestamp to prevent caching
        const res = await fetch(url + "?t=" + new Date().getTime(), {
            cache: "no-store"
        });
        if (res.ok) {
            const text = await res.text();
            dynamicBody.innerHTML = marked.parse(text);

            // Open links in new tab
            dynamicBody.querySelectorAll('a').forEach(a => {
                a.setAttribute('target', '_blank');
                a.setAttribute('rel', 'noopener noreferrer');
            });
        } else {
            throw new Error("Content unavailable");
        }
    } catch (e) { // If the dynamic content from GitHub can't be displayed, link the repo instead
        dynamicBody.innerHTML = `
            <div style="text-align:center; padding: 20px;">
                <p>Content unavailable.</p>
                <p><a href="https://github.com/Robbbbbbbbb/OpenEFT-2/" target="_blank" style="color:var(--accent-color)">Visit Repository</a></p>
            </div>
        `;
    } finally {
        showLoading(false);
    }
}

// NEW EFT LOGIC

function updateWizardUI() {
    if (currentAppMode !== 'new') return;

    // Hide all panels
    [panelStep1, panelStep2, panelStep3, panelDownload].forEach(p => p.classList.add('hidden'));

    // Update Progress Bar
    [pStep1, pStep2, pStep3].forEach((el, idx) => {
        el.classList.remove('active', 'completed');
        if (idx + 1 < currentStep) el.classList.add('completed');
        if (idx + 1 === currentStep) el.classList.add('active');
    });

    // Update Bar Fill
    const fill = document.getElementById('progress-fill');
    if (fill) {
        let pct = 0;
        if (currentStep === 1) pct = 33;
        else if (currentStep === 2) pct = 66;
        else if (currentStep >= 3) pct = 100;
        fill.style.width = pct + "%";
    }

    // Reset Buttons
    btnBack.classList.add('hidden');
    btnNext.classList.add('hidden');
    btnSubmit.classList.add('hidden');

    if (currentStep === 1) {
        panelStep1.classList.remove('hidden');

        // Sub-step logic
        [viewUpload, viewCrop, viewModeSelect, viewBox].forEach(v => v.classList.add('hidden'));

        if (currentSubStep === 'upload') {
            viewUpload.classList.remove('hidden');
            // No buttons (upload is action)
        } else if (currentSubStep === 'crop') {
            viewCrop.classList.remove('hidden');
            btnNext.classList.remove('hidden');
            btnNext.textContent = "Confirm Crop";
            btnBack.classList.remove('hidden');
            btnBack.onclick = () => window.location.reload();
        } else if (currentSubStep === 'mode') {
            viewModeSelect.classList.remove('hidden');
            btnBack.classList.remove('hidden');
            btnBack.onclick = () => { currentSubStep = 'crop'; updateWizardUI(); };
        } else if (currentSubStep === 'box') {
            viewBox.classList.remove('hidden');
            btnBack.classList.remove('hidden');
            btnBack.onclick = () => { currentSubStep = 'mode'; updateWizardUI(); };
            btnNext.classList.remove('hidden');
            btnNext.textContent = "Next Step";
        }

    } else if (currentStep === 2) {
        panelStep2.classList.remove('hidden');
        btnBack.classList.remove('hidden');
        btnNext.classList.remove('hidden');
        btnNext.textContent = "Review";

        btnBack.onclick = () => {
            currentStep = 1;
            currentSubStep = 'box';
            updateWizardUI();
            requestAnimationFrame(initVerifyStep); // Redraw canvas
        };

    } else if (currentStep === 3) {
        panelStep3.classList.remove('hidden');
        btnBack.classList.remove('hidden');
        btnSubmit.classList.remove('hidden');

        btnBack.onclick = () => {
            currentStep = 2;
            updateWizardUI();
        };

        populateReview();

    } else if (currentStep === 4) { // Download
        panelDownload.classList.remove('hidden');
        document.getElementById('main-footer').classList.add('hidden');
        loadSupportInfo();
    }
}

async function loadSupportInfo() {
    const el = document.getElementById('support-content');
    el.innerHTML = '<div style="text-align:center; color:#aaa;">Loading info...</div>';

    const url = "https://raw.githubusercontent.com/Robbbbbbbbb/OpenEFT-2/refs/heads/main/dynamic/supportme.md";

    try {
        const res = await fetch(url + "?t=" + new Date().getTime());
        if (res.ok) {
            const text = await res.text();
            el.innerHTML = marked.parse(text);

            // Style links
            el.querySelectorAll('a').forEach(a => {
                a.setAttribute('target', '_blank');
                a.setAttribute('rel', 'noopener noreferrer');
                a.style.color = 'var(--accent-color)';
                a.style.textDecoration = 'none';
            });
        } else {
            throw new Error("Content unavailable");
        }
    } catch (e) {
        el.innerHTML = `
            <div style="text-align:center;">
                <h3>OpenEFT-2</h3>
                <p>Check out the project on GitHub:</p>
                <p><a href="https://github.com/Robbbbbbbbb/OpenEFT-2/" target="_blank" style="color:var(--accent-color); text-decoration:none;">https://github.com/Robbbbbbbbb/OpenEFT-2/</a></p>
            </div>
        `;
    }
}



// Next Button Logic
btnNext.onclick = async () => {
    if (currentStep === 1) {
        if (currentSubStep === 'crop') {
            await confirmCrop();
        } else if (currentSubStep === 'box') {
            currentStep = 2;
            updateWizardUI();
        }
    } else if (currentStep === 2) {
        // Validate Form
        const form = document.getElementById('type2-form');
        if (form.checkValidity()) {
            currentStep = 3;
            updateWizardUI();
        } else {
            form.reportValidity();
        }
    }
};

// Mode Selection Handlers
document.getElementById('btn-gen-atf').onclick = () => {
    selectedGenMode = 'atf';
    boxes = getBoxesForMode('atf');
    currentSubStep = 'box';
    updateWizardUI();
    requestAnimationFrame(initVerifyStep);
};
document.getElementById('btn-gen-rolled').onclick = () => {
    selectedGenMode = 'rolled';
    boxes = getBoxesForMode('rolled');
    currentSubStep = 'box';
    updateWizardUI();
    requestAnimationFrame(initVerifyStep);
};

// STEP 1: Upload
const fileInput = document.getElementById('file-input');
const uploadArea = document.querySelector('.upload-area');

// Drag & Drop
uploadArea.addEventListener('dragover', (e) => {
    e.preventDefault();
    uploadArea.classList.add('dragover');
});

uploadArea.addEventListener('dragleave', (e) => {
    e.preventDefault();
    uploadArea.classList.remove('dragover');
});

uploadArea.addEventListener('drop', (e) => {
    e.preventDefault();
    uploadArea.classList.remove('dragover');
    if (e.dataTransfer.files.length > 0) {
        handleFileUpload(e.dataTransfer.files[0]);
    }
});

fileInput.addEventListener('change', (e) => {
    if (e.target.files.length > 0) {
        handleFileUpload(e.target.files[0]);
    }
});

async function handleFileUpload(file) {
    const formData = new FormData();
    formData.append("file", file);

    showLoading(true);
    try {
        const res = await fetch('/api/upload', { method: 'POST', body: formData });
        if (!res.ok) throw new Error("Upload failed");
        const data = await res.json();

        sessionId = data.session_id;
        isCaptureSession = false;

        cropImage.onload = () => {
            currentSubStep = 'crop';
            updateWizardUI();
            requestAnimationFrame(initCropStep);
        };
        cropImage.src = "data:image/png;base64," + data.image_base64;

    } catch (e) {
        alert(e.message);
    } finally {
        showLoading(false);
    }
}

// STEP 1.5: Crop Logic
function initCropStep() {
    cropRotation = 0;
    cropBox = null;
    drawCropCanvas();
}

function getRotatedDimensions() {
    if (cropRotation % 180 === 0) {
        return { w: cropImage.naturalWidth, h: cropImage.naturalHeight };
    } else {
        return { w: cropImage.naturalHeight, h: cropImage.naturalWidth };
    }
}

function drawCropCanvas() {
    const container = cropCanvas.parentElement;
    const maxWidth = container.clientWidth;
    const maxHeight = container.clientHeight;

    const dims = getRotatedDimensions();

    const scaleX = maxWidth / dims.w;
    const scaleY = maxHeight / dims.h;

    cropScaleFactor = Math.min(scaleX, scaleY);

    cropCanvas.width = dims.w * cropScaleFactor;
    cropCanvas.height = dims.h * cropScaleFactor;

    cropCtx.save();
    cropCtx.clearRect(0, 0, cropCanvas.width, cropCanvas.height);
    cropCtx.scale(cropScaleFactor, cropScaleFactor);

    cropCtx.translate(dims.w / 2, dims.h / 2);
    cropCtx.rotate(cropRotation * Math.PI / 180);
    cropCtx.drawImage(cropImage, -cropImage.naturalWidth / 2, -cropImage.naturalHeight / 2);

    cropCtx.restore();

    // Draw Boxes
    if (cropBox) {
        cropCtx.strokeStyle = '#bada55';
        cropCtx.lineWidth = 2;
        cropCtx.strokeRect(
            cropBox.x * cropScaleFactor,
            cropBox.y * cropScaleFactor,
            cropBox.w * cropScaleFactor,
            cropBox.h * cropScaleFactor
        );

        cropCtx.fillStyle = 'rgba(0,0,0,0.7)';
        cropCtx.fillRect(0, 0, cropCanvas.width, cropBox.y * cropScaleFactor); // Top
        cropCtx.fillRect(0, (cropBox.y + cropBox.h) * cropScaleFactor, cropCanvas.width, cropCanvas.height - (cropBox.y + cropBox.h) * cropScaleFactor); // Bottom
        cropCtx.fillRect(0, cropBox.y * cropScaleFactor, cropBox.x * cropScaleFactor, cropBox.h * cropScaleFactor); // Left
        cropCtx.fillRect((cropBox.x + cropBox.w) * cropScaleFactor, cropBox.y * cropScaleFactor, cropCanvas.width - (cropBox.x + cropBox.w) * cropScaleFactor, cropBox.h * cropScaleFactor); // Right
    }
}

// Crop Controls
document.getElementById('btn-rotate-left').onclick = () => { cropRotation = (cropRotation - 90) % 360; cropBox = null; drawCropCanvas(); };
document.getElementById('btn-rotate-right').onclick = () => { cropRotation = (cropRotation + 90) % 360; cropBox = null; drawCropCanvas(); };
document.getElementById('btn-reset-crop').onclick = initCropStep;

cropCanvas.onmousedown = (e) => {
    const rect = cropCanvas.getBoundingClientRect();
    const x = (e.clientX - rect.left) / cropScaleFactor;
    const y = (e.clientY - rect.top) / cropScaleFactor;
    isCropDragging = true;
    cropStart = { x, y };
    cropBox = { x, y, w: 0, h: 0 };
    drawCropCanvas();
};

cropCanvas.onmousemove = (e) => {
    if (!isCropDragging) return;
    const rect = cropCanvas.getBoundingClientRect();
    const x = (e.clientX - rect.left) / cropScaleFactor;
    const y = (e.clientY - rect.top) / cropScaleFactor;

    const w = x - cropStart.x;
    const h = y - cropStart.y;

    cropBox = {
        x: w > 0 ? cropStart.x : x,
        y: h > 0 ? cropStart.y : y,
        w: Math.abs(w),
        h: Math.abs(h)
    };
    drawCropCanvas();
};

cropCanvas.onmouseup = () => isCropDragging = false;

async function confirmCrop() {
    const dims = getRotatedDimensions();
    const finalCrop = cropBox || { x: 0, y: 0, w: dims.w, h: dims.h };

    if (finalCrop.w < 100 || finalCrop.h < 100) return alert("Selection too small");

    showLoading(true);
    try {
        const payload = {
            session_id: sessionId,
            rotation: cropRotation,
            x: Math.round(finalCrop.x),
            y: Math.round(finalCrop.y),
            w: Math.round(finalCrop.w),
            h: Math.round(finalCrop.h)
        };

        const res = await fetch('/api/process_crop', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (!res.ok) throw new Error("Processing failed");
        const data = await res.json();

        // boxes = data.boxes; // Do not use default boxes from backend

        image.onload = () => {
            currentSubStep = 'mode';
            updateWizardUI();
            // requestAnimationFrame(initVerifyStep);
        };
        image.src = "data:image/png;base64," + data.image_base64;

    } catch (e) {
        alert(e.message);
    } finally {
        showLoading(false);
    }
}


// Box Selection Logic
function initVerifyStep() {
    const container = verifyCanvas.parentElement;
    const maxWidth = container.clientWidth;
    const maxHeight = container.clientHeight;

    const scaleX = maxWidth / image.width;
    const scaleY = maxHeight / image.height;

    scaleFactor = Math.min(scaleX, scaleY);

    verifyCanvas.width = image.width * scaleFactor;
    verifyCanvas.height = image.height * scaleFactor;

    drawVerifyCanvas();
}

function getVerifyMousePos(evt) {
    const rect = verifyCanvas.getBoundingClientRect();
    const scaleX = verifyCanvas.width / rect.width;
    const scaleY = verifyCanvas.height / rect.height;
    const cvsX = (evt.clientX - rect.left) * scaleX;
    const cvsY = (evt.clientY - rect.top) * scaleY;
    return { x: cvsX / scaleFactor, y: cvsY / scaleFactor };
}

function getBoxesForMode(mode) {
    const w = image.width;
    const h = image.height;

    if (mode === 'atf') {
        return [
            { id: 'L_SLAP', fp_number: 14, x: w * 0.05, y: h * 0.45, w: w * 0.3, h: h * 0.4 },
            { id: 'R_SLAP', fp_number: 13, x: w * 0.65, y: h * 0.45, w: w * 0.3, h: h * 0.4 },
            { id: 'THUMBS', fp_number: 15, x: w * 0.3, y: h * 0.75, w: w * 0.4, h: h * 0.2 },
        ];
    } else {
        const rowH = h * 0.18;
        const boxW = w * 0.16;
        const y1 = h * 0.35;
        const y2 = h * 0.55;
        const y3 = h * 0.78;

        const list = [];
        // Row 1 (Right Hand Rolled) 1-5
        for (let i = 1; i <= 5; i++) {
            list.push({ id: `R${i}`, fp_number: i, x: (i - 1) * boxW + w * 0.1, y: y1, w: boxW * 0.9, h: rowH });
        }
        // Row 2 (Left Hand Rolled) 6-10
        for (let i = 6; i <= 10; i++) {
            list.push({ id: `L${i - 5}`, fp_number: i, x: (i - 6) * boxW + w * 0.1, y: y2, w: boxW * 0.9, h: rowH });
        }

        // Row 3 (Plain)
        const pW = w * 0.2;
        const tW = w * 0.1;

        // 14: Plain Left 4
        list.push({ id: 'P_L4', fp_number: 14, x: w * 0.05, y: y3, w: pW, h: rowH });
        // 12: Plain Left Thumb
        list.push({ id: 'P_LT', fp_number: 12, x: w * 0.28, y: y3, w: tW, h: rowH });
        // 11: Plain Right Thumb
        list.push({ id: 'P_RT', fp_number: 11, x: w * 0.40, y: y3, w: tW, h: rowH });
        // 13: Plain Right 4
        list.push({ id: 'P_R4', fp_number: 13, x: w * 0.55, y: y3, w: pW, h: rowH });

        return list;
    }
}

function drawVerifyCanvas() {
    verifyCtx.clearRect(0, 0, verifyCanvas.width, verifyCanvas.height);
    verifyCtx.save();
    verifyCtx.scale(scaleFactor, scaleFactor);
    verifyCtx.drawImage(image, 0, 0);
    verifyCtx.lineWidth = 10;

    boxes.forEach((box, index) => {
        // High contrast colors: Active=Magenta (#FF00FF), Inactive=Cyan (#00FFFF) or Lime (#00FF00)
        // User requested high contrast, not blue/black.
        const isActive = index === activeBoxIndex;
        verifyCtx.strokeStyle = isActive ? '#FF00FF' : '#00FF00';
        verifyCtx.strokeRect(box.x, box.y, box.w, box.h);
        verifyCtx.fillStyle = verifyCtx.strokeStyle;
        verifyCtx.font = 'bold 40px Arial';
        verifyCtx.fillText(box.id, box.x, box.y - 10);

        if (isActive) {
            // Drag Handles: Bright Yellow/Orange for contrast
            verifyCtx.fillStyle = '#FFD700'; // Gold
            const handleSize = 40;
            verifyCtx.fillRect(box.x - handleSize / 2, box.y - handleSize / 2, handleSize, handleSize);
            verifyCtx.fillRect(box.x + box.w - handleSize / 2, box.y - handleSize / 2, handleSize, handleSize);
            verifyCtx.fillRect(box.x - handleSize / 2, box.y + box.h - handleSize / 2, handleSize, handleSize);
            verifyCtx.fillRect(box.x + box.w - handleSize / 2, box.y + box.h - handleSize / 2, handleSize, handleSize);
        }
    });

    verifyCtx.restore();
}


verifyCanvas.onmousedown = (e) => {
    const { x, y } = getVerifyMousePos(e);
    if (activeBoxIndex !== -1) {
        const handle = getResizeHandle(boxes[activeBoxIndex], x, y);
        if (handle) {
            isResizing = true;
            resizeHandle = handle;
            return;
        }
    }
    for (let i = boxes.length - 1; i >= 0; i--) {
        if (isInside(boxes[i], x, y)) {
            activeBoxIndex = i;
            isDragging = true;
            drawVerifyCanvas();
            return;
        }
    }
    activeBoxIndex = -1;
    drawVerifyCanvas();
};

verifyCanvas.onmousemove = (e) => {
    const { x, y } = getVerifyMousePos(e);
    if (isResizing && activeBoxIndex !== -1) {
        resizeBox(boxes[activeBoxIndex], resizeHandle, x, y);
        drawVerifyCanvas();
    } else if (isDragging && activeBoxIndex !== -1) {
        const box = boxes[activeBoxIndex];
        box.x += e.movementX / scaleFactor;
        box.y += e.movementY / scaleFactor;
        drawVerifyCanvas();
    } else {
        if (activeBoxIndex !== -1) {
            const handle = getResizeHandle(boxes[activeBoxIndex], x, y);
            if (handle) {
                verifyCanvas.style.cursor = getCursorForHandle(handle);
                return;
            }
        }
        verifyCanvas.style.cursor = 'default';
    }
};

verifyCanvas.onmouseup = () => { isDragging = false; isResizing = false; };

function isInside(box, x, y) {
    return x >= box.x && x <= box.x + box.w && y >= box.y && y <= box.y + box.h;
}

function getResizeHandle(box, x, y) {
    const d = 40;
    if (Math.abs(x - box.x) < d && Math.abs(y - box.y) < d) return 'tl';
    if (Math.abs(x - (box.x + box.w)) < d && Math.abs(y - box.y) < d) return 'tr';
    if (Math.abs(x - box.x) < d && Math.abs(y - (box.y + box.h)) < d) return 'bl';
    if (Math.abs(x - (box.x + box.w)) < d && Math.abs(y - (box.y + box.h)) < d) return 'br';
    return null;
}

function resizeBox(box, handle, mx, my) {
    if (handle === 'tl') {
        const newW = (box.x + box.w) - mx;
        const newH = (box.y + box.h) - my;
        if (newW > 10 && newH > 10) { box.x = mx; box.y = my; box.w = newW; box.h = newH; }
    } else if (handle === 'tr') {
        const newW = mx - box.x;
        const newH = (box.y + box.h) - my;
        if (newW > 10 && newH > 10) { box.y = my; box.w = newW; box.h = newH; }
    } else if (handle === 'bl') {
        const newW = (box.x + box.w) - mx;
        const newH = my - box.y;
        if (newW > 10 && newH > 10) { box.x = mx; box.w = newW; box.h = newH; }
    } else if (handle === 'br') {
        const newW = mx - box.x;
        const newH = my - box.y;
        if (newW > 10 && newH > 10) { box.w = newW; box.h = newH; }
    }
}


function getCursorForHandle(handle) {
    return (handle === 'tl' || handle === 'br') ? 'nwse-resize' : 'nesw-resize';
}


// STEP 3: Review Logic
function populateReview() {
    const summary = document.getElementById('summary-content');
    const form = document.getElementById('type2-form');
    const formData = new FormData(form);
    const data = Object.fromEntries(formData.entries());

    const noMn = document.getElementById('no-mn').checked;
    let mname = data.mname || "";
    if (noMn) mname = "NMN";
    const fullName = `${data.lname}, ${data.fname} ${mname}`;

    const cleanAddr = [data.addr_street, data.addr_city, data.addr_state, data.addr_zip]
        .map(s => (s || "").trim())
        .filter(s => s)
        .join(" ");

    const displayFields = [
        { num: "2.018", name: "NAM", val: fullName },
        { num: "2.022", name: "DOB", val: data["dob"] },
        { num: "2.016", name: "SOC", val: data["2.016"] },
        { num: "2.024", name: "SEX", val: data["2.024"] },
        { num: "2.025", name: "RAC", val: data["2.025"] },
        { num: "2.027", name: "HGT", val: data["2.027"] },
        { num: "2.029", name: "WGT", val: data["2.029"] },
        { num: "2.031", name: "EYE", val: data["2.031"] },
        { num: "2.032", name: "HAI", val: data["2.032"] },
        { num: "2.020", name: "POB", val: data["2.020"] },
        { num: "2.021", name: "CTZ", val: data["2.021"] },
        { num: "2.041", name: "RES", val: cleanAddr },
    ];

    let html = '<ul style="list-style:none; padding:0;">';
    displayFields.forEach(f => {
        if (f.val) {
            html += `<li><strong style="color:var(--text-muted)">${f.num} - ${f.name}:</strong> <span style="color:var(--text-color)">${f.val}</span></li>`;
        }
    });
    html += '</ul>';
    summary.innerHTML = html;

    // Check if we have captured images in session, otherwise crop from card
    if (capturedPrints.L_SLAP) {
        setReviewImage('review-img-lslap', capturedPrints.L_SLAP);
        setReviewImage('review-img-rslap', capturedPrints.R_SLAP);
        setReviewImage('review-img-thumbs', capturedPrints.THUMBS);
    } else {
        if (selectedGenMode === 'rolled') {
            generateReviewCrop('P_L4', 'review-img-lslap');
            generateReviewCrop('P_R4', 'review-img-rslap');
            generateReviewCrop('P_RT', 'review-img-thumbs');
        } else {
            generateReviewCrop('L_SLAP', 'review-img-lslap');
            generateReviewCrop('R_SLAP', 'review-img-rslap');
            generateReviewCrop('THUMBS', 'review-img-thumbs');
        }
    }
}

function setReviewImage(elemId, base64) {
    const container = document.getElementById(elemId);
    container.innerHTML = '';
    const img = document.createElement('img');
    img.src = "data:image/png;base64," + base64;
    img.style.maxWidth = '100%';
    img.style.height = 'auto';
    container.appendChild(img);
}

function generateReviewCrop(boxId, elemId) {
    const box = boxes.find(b => b.id === boxId);
    const container = document.getElementById(elemId);
    container.innerHTML = '';

    if (!box) {
        container.textContent = "Not found";
        return;
    }
    const cvs = document.createElement('canvas');
    cvs.width = box.w;
    cvs.height = box.h;
    const ctx = cvs.getContext('2d');
    ctx.drawImage(image, box.x, box.y, box.w, box.h, 0, 0, box.w, box.h);
    cvs.style.maxWidth = '100%';
    cvs.style.height = 'auto';
    container.appendChild(cvs);
}

// Submit (New EFT)
btnSubmit.onclick = async () => {
    const formData = new FormData(document.getElementById('type2-form'));
    const data = Object.fromEntries(formData.entries());

    const noMn = document.getElementById('no-mn').checked;
    let mname = data.mname || "";
    if (noMn) mname = "NMN";

    const fullName = `${data.lname},${data.fname},${mname}`;
    data["2.018"] = fullName;

    const cleanAddr = [data.addr_street, data.addr_city, data.addr_state, data.addr_zip]
        .map(s => s.replace(/[^a-zA-Z0-9\s]/g, "").trim())
        .join(" ");
    data["2.041"] = cleanAddr;

    const dobInput = document.getElementById('dob-input').value;
    if (dobInput) data["2.022"] = dobInput.replace(/-/g, "");

    const payload = {
        session_id: sessionId,
        boxes: boxes,
        type2_data: data,
        mode: selectedGenMode
    };

    showLoading(true);
    try {
        const res = await fetch('/api/generate', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (!res.ok) throw new Error((await res.json()).detail || "Generation failed");

        const result = await res.json();
        const link = document.getElementById('download-link');
        link.href = result.download_url;
        link.setAttribute('download', result.filename);

        currentStep = 4;
        updateWizardUI();

        // Show FD258 button if capture session
        if (isCaptureSession) {
            document.getElementById('btn-dl-fd258').classList.remove('hidden');
        } else {
            document.getElementById('btn-dl-fd258').classList.add('hidden');
        }

    } catch (e) {
        alert(e.message);
    } finally {
        showLoading(false);
    }
};


document.getElementById('btn-dl-fd258').onclick = async () => {

    const formData = new FormData(document.getElementById('type2-form'));
    const data = Object.fromEntries(formData.entries());

    const noMn = document.getElementById('no-mn').checked;
    let mname = data.mname || "";
    if (noMn) mname = "NMN";

    const fullName = `${data.lname},${data.fname},${mname}`;
    data["2.018"] = fullName;

    const cleanAddr = [data.addr_street, data.addr_city, data.addr_state, data.addr_zip]
        .map(s => s.replace(/[^a-zA-Z0-9\s]/g, "").trim())
        .join(" ");
    data["2.041"] = cleanAddr;

    const dobInput = document.getElementById('dob-input').value;
    if (dobInput) data["2.022"] = dobInput.replace(/-/g, "");

    const payload = {
        session_id: sessionId,
        boxes: boxes,
        type2_data: data,
        mode: selectedGenMode
    };

    showLoading(true);
    try {
        const res = await fetch('/api/generate_fd258', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (!res.ok) throw new Error((await res.json()).detail || "Generation failed");

        const result = await res.json();
        // Trigger download
        const a = document.createElement('a');
        a.href = result.download_url;
        a.download = result.filename;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);

    } catch (e) {
        alert(e.message);
    } finally {
        showLoading(false);
    }
}

document.getElementById('btn-restart').onclick = () => window.location.reload();


// CAPTURE PRINTS LOGIC
let ws = null;
let captureStep = 1; // 1=L_SLAP, 2=R_SLAP, 3=THUMBS
let capturedPrints = { L_SLAP: null, R_SLAP: null, THUMBS: null };
let currentCaptureImage = null;
let reconnectTimer = null;

function initCaptureMode() {
    const statusEl = document.getElementById('scanner-status');
    const previewEl = document.getElementById('scanner-preview');

    if (ws && (ws.readyState === WebSocket.OPEN || ws.readyState === WebSocket.CONNECTING)) return;

    // Console logging to end user; display websocket status
    logToConsole("Connecting to Scanner Helper...");
    ws = new WebSocket('ws://localhost:8888/');

    // Scanner is connected! We can capture prints
    ws.onopen = () => {
        logToConsole("WS Connected");
        statusEl.textContent = '🟢 Scanner Helper Connected';
        statusEl.style.background = '#27ae60';
        updateCaptureUI();
        if (reconnectTimer) { clearInterval(reconnectTimer); reconnectTimer = null; }
    };

    // Scanner is disconnected - display notice to user
    ws.onclose = () => {
        logToConsole("WS Disconnected");
        statusEl.textContent = '⚪ Scanner Disconnected (Is Helper Running?)';
        statusEl.style.background = '#e74c3c';
        ws = null;
        // Auto Reconnect
        if (!reconnectTimer) {
            reconnectTimer = setInterval(initCaptureMode, 3000);
        }
    };

    ws.onerror = (e) => logToConsole("WS Error");

    ws.onmessage = (evt) => {
        const msg = JSON.parse(evt.data);

        if (msg.type === 'log') {
            logToConsole("[HELPER]: " + msg.message);
        } else if (msg.type === 'preview') {
            previewEl.src = "data:image/png;base64," + msg.image;
        } else if (msg.type === 'status') {
            logToConsole("STATUS: " + msg.message);
        } else if (msg.type === 'result') {
            logToConsole("Image Captured");
            currentCaptureImage = msg.image;
            showCaptureResult(msg.image);
        }
    };

    document.getElementById('btn-cap-start').onclick = startCapture;
    document.getElementById('btn-cap-cancel').onclick = cancelCapture;
    document.getElementById('btn-cap-retry').onclick = retryCapture;
    document.getElementById('btn-cap-reset').onclick = resetCapture;
    document.getElementById('btn-cap-accept').onclick = acceptCapture;
    document.getElementById('btn-cap-finalize').onclick = finalizeCapture;
}

function updateCaptureUI() {
    const s1 = document.getElementById('cap-step-1');
    const s2 = document.getElementById('cap-step-2');
    const s3 = document.getElementById('cap-step-3');

    [s1, s2, s3].forEach(el => el.classList.remove('active', 'completed'));

    if (captureStep === 1) {
        s1.classList.add('active');
        document.getElementById('capture-instruction').textContent = "Place Left 4 Fingers";
    } else if (captureStep === 2) {
        s1.classList.add('completed');
        s2.classList.add('active');
        document.getElementById('capture-instruction').textContent = "Place Right 4 Fingers";
    } else if (captureStep === 3) {
        s1.classList.add('completed');
        s2.classList.add('completed');
        s3.classList.add('active');
        document.getElementById('capture-instruction').textContent = "Place Thumbs";
    }

    resetCaptureButtons(true);
}

function resetCaptureButtons(readyToStart) {
    document.getElementById('btn-cap-start').classList.toggle('hidden', !readyToStart);
    document.getElementById('btn-cap-cancel').classList.add('hidden');
    document.getElementById('btn-cap-accept').classList.add('hidden');
    document.getElementById('btn-cap-retry').classList.add('hidden');
    document.getElementById('btn-cap-reset').classList.toggle('hidden', readyToStart && captureStep === 1);
    document.getElementById('scanner-preview').style.opacity = '0.5';
}

function startCapture() {
    if (!ws) return;

    document.getElementById('btn-cap-start').classList.add('hidden');
    document.getElementById('btn-cap-reset').classList.add('hidden');
    document.getElementById('btn-cap-cancel').classList.remove('hidden');
    document.getElementById('scanner-preview').style.opacity = '1.0';

    let seq = "";
    if (captureStep === 1) seq = "L_SLAP";
    else if (captureStep === 2) seq = "R_SLAP";
    else if (captureStep === 3) seq = "THUMBS";

    ws.send(JSON.stringify({ action: "START_CAPTURE", param: seq }));
}

function cancelCapture() {
    ws.send(JSON.stringify({ action: "CANCEL" }));
    resetCaptureButtons(true);
}

function showCaptureResult(base64) {
    document.getElementById('scanner-preview').src = "data:image/png;base64," + base64;
    document.getElementById('btn-cap-cancel').classList.add('hidden');
    document.getElementById('btn-cap-accept').classList.remove('hidden');
    document.getElementById('btn-cap-retry').classList.remove('hidden');
}

function retryCapture() {
    currentCaptureImage = null;
    startCapture();
}

function resetCapture() {
    captureStep = 1;
    capturedPrints = { L_SLAP: null, R_SLAP: null, THUMBS: null };
    currentCaptureImage = null;

    // Clear thumbs
    document.getElementById('thumb-l-slap').src = "";
    document.getElementById('thumb-r-slap').src = "";
    document.getElementById('thumb-thumbs').src = "";
    document.getElementById('scanner-preview').src = "";

    document.getElementById('btn-cap-finalize').classList.add('hidden');

    updateCaptureUI();
}

function acceptCapture() {
    if (captureStep === 1) {
        capturedPrints.L_SLAP = currentCaptureImage;
        document.getElementById('thumb-l-slap').src = "data:image/png;base64," + currentCaptureImage;
        captureStep = 2;
        updateCaptureUI();
    } else if (captureStep === 2) {
        capturedPrints.R_SLAP = currentCaptureImage;
        document.getElementById('thumb-r-slap').src = "data:image/png;base64," + currentCaptureImage;
        captureStep = 3;
        updateCaptureUI();
    } else if (captureStep === 3) {
        capturedPrints.THUMBS = currentCaptureImage;
        document.getElementById('thumb-thumbs').src = "data:image/png;base64," + currentCaptureImage;

        // Done
        document.getElementById('btn-cap-accept').classList.add('hidden');
        document.getElementById('btn-cap-retry').classList.add('hidden');
        document.getElementById('btn-cap-start').classList.add('hidden');
        document.getElementById('capture-instruction').textContent = "Capture Complete";

        document.getElementById('btn-cap-finalize').classList.remove('hidden');
    }
}

function logToConsole(msg) {
    const el = document.getElementById('console-output');
    if (el) {
        const line = document.createElement('div');
        line.textContent = `[${new Date().toLocaleTimeString()}] ${msg}`;
        el.appendChild(line);
        el.parentElement.scrollTop = el.parentElement.scrollHeight;
    }
}

async function finalizeCapture() {
    // 1. Send images to backend to create a session
    showLoading(true);
    try {
        const payload = {
            l_slap: capturedPrints.L_SLAP,
            r_slap: capturedPrints.R_SLAP,
            thumbs: capturedPrints.THUMBS
        };

        const res = await fetch('/api/start_capture_session', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (!res.ok) throw new Error("Failed to start session");
        const data = await res.json();
        sessionId = data.session_id;
        isCaptureSession = true;

        // Populate boxes for generator
        boxes = [
            { id: 'L_SLAP', fp_number: 14, x: 0, y: 0, w: 0, h: 0 },
            { id: 'R_SLAP', fp_number: 13, x: 0, y: 0, w: 0, h: 0 },
            { id: 'THUMBS', fp_number: 15, x: 0, y: 0, w: 0, h: 0 }
        ];

        // 2. Switch to New EFT -> Step 2
        currentAppMode = 'new';
        currentStep = 2;

        // Update UI
        navCapture.classList.remove('active');
        sectionCapture.classList.add('hidden');

        navNew.classList.add('active');
        sectionNewEft.classList.remove('hidden');
        wizardProgressBar.classList.remove('hidden');
        document.getElementById('main-footer').classList.remove('hidden');

        updateWizardUI();

    } catch (e) {
        alert(e.message);
    } finally {
        showLoading(false);
    }
}

// EDIT/VIEW EFT LOGIC

// 1. Upload EFT
const eftFileInput = document.getElementById('eft-file-input');
eftFileInput.addEventListener('change', async (e) => {
    if (e.target.files.length > 0) {
        const file = e.target.files[0];
        const formData = new FormData();
        formData.append("file", file);

        showLoading(true);
        try {
            const res = await fetch('/api/upload_eft', { method: 'POST', body: formData });
            if (!res.ok) throw new Error("Upload failed");
            const data = await res.json();

            editSessionId = data.session_id;

            // Move to mode selection
            editViewUpload.classList.add('hidden');
            editViewMode.classList.remove('hidden');

        } catch (e) {
            alert(e.message);
        } finally {
            showLoading(false);
        }
    }
});

// 2. Mode Selection
document.getElementById('btn-mode-read').onclick = () => loadEFT(false);
document.getElementById('btn-mode-edit').onclick = () => loadEFT(true);

async function loadEFT(editMode) {
    isEditMode = editMode;

    showLoading(true);
    try {
        const res = await fetch(`/api/eft_session/${editSessionId}`);
        if (!res.ok) throw new Error("Failed to load EFT data");
        const data = await res.json();

        renderEFTData(data, editMode);

        editViewMode.classList.add('hidden');
        editViewMain.classList.remove('hidden');

    } catch (e) {
        alert(e.message);
    } finally {
        showLoading(false);
    }
}

function renderEFTData(data, editMode) {
    // 1. Raw Dump
    document.getElementById('raw-content').textContent = data.text_dump || "No raw data available.";

    // 2. Form Fields (Type 2)
    const formGrid = document.getElementById('edit-form-grid');
    formGrid.innerHTML = '';

    const t2 = data.type2_data;

    // Common fields to show with labels (others at bottom)
    // Map field number to label
    const fieldLabels = {
        "2.018": "Name",
        "2.016": "SSN",
        "2.022": "DOB",
        "2.024": "Sex",
        "2.025": "Race",
        "2.027": "Height",
        "2.029": "Weight",
        "2.031": "Eye Color",
        "2.032": "Hair Color",
        "2.020": "Place of Birth",
        "2.021": "Citizenship",
        "2.041": "Address",
    };

    // Hiding certain Type-2 fields from being editable
    const hiddenFields = ["2.002", "2.005", "2.037", "2.038", "2.073"];

    // Sort keys: Known first, then others
    const keys = Object.keys(t2).sort();
    const knownKeys = Object.keys(fieldLabels);
    let orderedKeys = [...knownKeys.filter(k => t2[k] !== undefined), ...keys.filter(k => !knownKeys.includes(k))];

    // Filter hidden
    orderedKeys = orderedKeys.filter(k => !hiddenFields.includes(k));

    orderedKeys.forEach(k => {
        const val = t2[k];
        const label = fieldLabels[k] ? `${k} - ${fieldLabels[k]}` : k;

        const div = document.createElement('div');
        div.className = 'form-group';

        const lbl = document.createElement('label');
        lbl.textContent = label;
        div.appendChild(lbl);

        const inp = document.createElement('input');
        inp.type = 'text';
        inp.name = k;
        inp.value = val;
        inp.readOnly = !editMode;

        // Validation Warnings in Edit Mode
        if (editMode) {
            inp.onblur = () => {
                // Check simple validations
                if (k === '2.027') { // Height
                    if (val < 400 || val > 711) showWarning(inp, "Height typically 400-711");
                    else clearWarning(inp);
                }
                // Add more if needed
            };
        }

        div.appendChild(inp);
        formGrid.appendChild(div);
    });

    document.getElementById('editor-title').textContent = editMode ? "Edit Mode (Advanced)" : "Read Only Mode";
    const btnSave = document.getElementById('btn-save-eft');
    if (editMode) {
        btnSave.classList.remove('hidden');
        btnSave.onclick = saveEFTChanges;
    } else {
        btnSave.classList.add('hidden');
    }

    // 3. Images
    const imgContainer = document.getElementById('edit-img-container');
    imgContainer.innerHTML = '';

    data.images.forEach(img => {
        if (!img.url) return;

        const card = document.createElement('div');
        card.className = 'review-img-card';
        card.style.margin = '10px';

        const lbl = document.createElement('label');
        lbl.textContent = `Finger ${img.fgp}`;
        card.appendChild(lbl);

        const elem = document.createElement('img');
        elem.src = img.url;
        elem.style.maxWidth = '100px';
        elem.style.border = '1px solid #555';
        card.appendChild(elem);

        imgContainer.appendChild(card);
    });
}

function showWarning(input, msg) {
    // Check if warning already exists
    let w = input.nextElementSibling;
    if (w && w.classList.contains('warning-text')) {
        w.textContent = msg;
    } else {
        w = document.createElement('small');
        w.className = 'warning-text';
        w.style.color = '#f39c12';
        w.textContent = msg;
        input.parentNode.appendChild(w);
    }
}

function clearWarning(input) {
    let w = input.nextElementSibling;
    if (w && w.classList.contains('warning-text')) {
        w.remove();
    }
}

async function saveEFTChanges() {
    const form = document.getElementById('edit-form');
    const formData = new FormData(form);
    const data = Object.fromEntries(formData.entries());

    showLoading(true);
    try {
        const payload = {
            session_id: editSessionId,
            type2_data: data
        };

        const res = await fetch('/api/save_eft', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (!res.ok) throw new Error("Save failed");
        const result = await res.json();

        // Show Download Panel
        editViewMain.classList.add('hidden');
        panelDownload.classList.remove('hidden');
        document.getElementById('main-footer').classList.add('hidden');

        const link = document.getElementById('download-link');
        link.href = result.download_url;
        link.setAttribute('download', result.filename);

    } catch (e) {
        alert(e.message);
    } finally {
        showLoading(false);
    }
}


// Init Dropdowns
async function initDropdowns() {
    try {
        const response = await fetch('/static/codes.js');
        const text = await response.text();
        const parseList = (str) => {
            str = str.trim().replace(/,$/, "");
            if (!str) return [];
            return eval(`[${str}]`);
        };

        let match = text.match(/const US_STATES = \[\s*([\s\S]*?)\];/);
        const usStates = match ? parseList(match[1]) : [];
        match = text.match(/const COUNTRIES = \[\s*([\s\S]*?)\];/);
        const countries = match ? parseList(match[1]) : [];

        const allPob = [...usStates, ...countries];
        const allCtz = [...usStates, ...countries];

        const pobSelect = document.getElementById('pob-select');
        const ctzSelect = document.getElementById('ctz-select');
        const addrStateSelect = document.getElementById('addr-state');

        const addOpts = (sel, list) => {
            list.forEach(obj => {
                const key = Object.keys(obj)[0];
                const val = obj[key];
                sel.add(new Option(`${val} (${key})`, key));
            });
        };

        addOpts(pobSelect, allPob);
        addOpts(ctzSelect, allCtz);
        ctzSelect.value = "US";
        addOpts(addrStateSelect, usStates);

        const hSelect = document.getElementById('height-select');
        for (let h = 400; h <= 711; h++) {
            let s = h.toString();
            let ft = s[0];
            let inch = parseInt(s.substring(1));
            if (inch <= 11) hSelect.add(new Option(`${ft}' ${inch}"`, h));
        }

    } catch (e) { console.error(e); }
}

const sentenceCase = (str) => {
    return str.replace(/[a-zA-Z]+/g, function (txt) {
        return txt.charAt(0).toUpperCase() + txt.substr(1).toLowerCase();
    });
};

document.querySelectorAll('.sentence-case').forEach(input => {
    input.addEventListener('blur', (e) => {
        let val = e.target.value.replace(/[^a-zA-Z\s\-]/g, "");
        e.target.value = sentenceCase(val);
    });
    input.addEventListener('input', (e) => {
        e.target.value = e.target.value.replace(/[^a-zA-Z\s\-]/g, "");
    });
});

document.getElementById('no-mn').addEventListener('change', (e) => {
    const mname = document.getElementById('mname');
    if (e.target.checked) {
        mname.value = "";
        mname.disabled = true;
    } else {
        mname.disabled = false;
    }
});

function showLoading(show) {
    if (show) loading.classList.remove('hidden');
    else loading.classList.add('hidden');
}

// Start
initDropdowns();
updateWizardUI();
checkLatestVersion();

async function checkLatestVersion() {
    const el = document.getElementById('latest-version-text');
    if (!el) return;

    try {
        const url = 'https://raw.githubusercontent.com/Robbbbbbbbb/OpenEFT-2/refs/heads/main/dynamic/version.md';
        const res = await fetch(url + "?t=" + new Date().getTime(), { cache: "no-store" });
        if (res.ok) {
            const ver = (await res.text()).trim();
            // Parse Markdown and remove wrapping <p> tags if present
            const html = marked.parse(ver);
            // marked.parse wraps inline content in <p> by default, which breaks layout in the footer span.
            // We strip the <p> tags to keep it inline.
            el.innerHTML = html.replace(/^<p>|<\/p>\s*$/g, "");

            // Open links in new tab
            el.querySelectorAll('a').forEach(a => {
                a.setAttribute('target', '_blank');
                a.setAttribute('rel', 'noopener noreferrer');
            });
        } else {
            el.textContent = "Unknown";
        }
    } catch (e) {
        el.textContent = "Error";
    }
}
