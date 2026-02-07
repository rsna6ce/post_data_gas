# post_data_gas - Simple Time-Series Data Logger to Google Sheets

A lightweight mechanism to easily accumulate time-series data from ESP32, Python, etc., into Google Sheets.  
It works as a Google Apps Script (GAS) Web App, receiving JSON POST requests and appending data.

## File Structure
```
post_data_gas
├── LICENSE                  # MIT License
├── post_data.gs             # Google Apps Script main code
└── post_data_example
    ├── post_data_example.ino    # ESP32 (Arduino) sample
    └── post_data_example.py     # Python3 sample
```


## Features

- Records data in descending order (latest on top – easy to read as time-series)
- Maximum row limit (default 1440 rows; oldest data auto-deleted)
- Simple authentication via project_name (acts like a password)
- doPost only – no read functionality (very low risk of data leakage)

## GAS Setup Instructions (post_data.gs)

1. Create a new Google Spreadsheet (or use an existing one)
2. Go to **Extensions → Apps Script**
3. Delete all default code and paste the entire content of `post_data.gs`
4. Customize the constants (recommended)

```javascript
const PROJECT_NAME = "PROJECT-XXX";          // ← Change to a long, unique string (strongly recommended)
const SHEET_NAME    = "data";                // Sheet name (optional)
const MAX_COUNT     = 1440;                  // Max rows to keep (optional)
```

5. Click **Deploy → New deployment**
6. Type: **Web app**
7. Execute as: **Me**
8. Who has access: **Anyone**
9. Click **Deploy**
10. If a security warning appears, click "Advanced" → "Go to [unsafe] (unsafe)" to proceed

After deployment, copy the **Web app URL** displayed (you'll need it for ESP32/Python).

### Security Tips

- This script only supports **doPost**. There is no GET/read functionality.
- Extremely low risk of information leakage.
- Anyone who knows the URL can send POST requests, but they will be rejected unless `project_name` matches.
- **Strongly recommend setting a long, unpredictable PROJECT_NAME** (e.g., "MySecretProject2025-xyz123").

## Debugging GAS (testAddRecord)

1. In the Apps Script editor, select `testAddRecord` from the function dropdown
2. Click the  **Run** button
3. Wait a few seconds and run it again multiple times

→ Each execution adds a row with sin/cos values based on the current second (60-second cycle wave).  
After several runs, you'll see the wave gradually forming in the sheet.

## ESP32 Sample Code Usage (post_data_example.ino)

Target board: DOIT ESP32 DEVKIT V1 (or compatible)

1. Open `post_data_example.ino` in Arduino IDE
2. Update the following lines with your values

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "https://script.google.com/macros/s/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/exec";
const char* projectName = "PROJECT-XXX";
```

3. Select board and port, then **Upload**
4. Open Serial Monitor (115200 baud) to check operation

→ Sends sin/cos values (60-minute cycle) approximately every 60 seconds after boot.

## Python Sample Code Usage (post_data_example.py)

1. Open `post_data_example.py` and update:

```python
GAS_URL = "https://script.google.com/macros/s/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/exec"
PROJECT_NAME = "PROJECT-XXX"
```

2. Install `requests` if needed

```bash
pip install requests
```

3. Run the script

```bash
chmod +x post_data_example.py     # Once (Linux/macOS)
./post_data_example.py
```

or

```bash
python3 post_data_example.py
```

→ Each run sends one row of sin/cos based on the current second (60-second cycle).  
Run it multiple times (with short delays) to build the wave in the sheet.
