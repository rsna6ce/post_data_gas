#!/usr/bin/env python3
"""
Send sin/cos values to Google Apps Script based on current seconds (60-second cycle).
Run multiple times with short delays to build a wave in the spreadsheet.
"""

import requests
import math
from datetime import datetime

# Google Apps Script Web App URL (replace with your actual URL)
GAS_URL = "https://script.google.com/macros/s/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/exec"

# Project name (must match PROJECT_NAME in GAS)
PROJECT_NAME = "PROJECT-XXX"

# Max count (optional, can be overridden in GAS)
MAX_COUNT = 1440


def main():
    # Get current seconds (0-59)
    now = datetime.now()
    seconds = now.second
    
    # Calculate phase for 60-second cycle
    phase = (seconds / 60) * 2 * math.pi
    
    # Calculate sin and cos
    sin_value = math.sin(phase)
    cos_value = math.cos(phase)
    
    # Prepare JSON payload
    payload = {
        "project_name": PROJECT_NAME,
        "headers": ["sin", "cos"],
        "datas": [sin_value, cos_value],
        "max_count": MAX_COUNT
    }
    
    print(f"[{now.strftime('%H:%M:%S')}] Sending: sin={sin_value:.6f}, cos={cos_value:.6f}")
    
    # Send POST request
    try:
        response = requests.post(GAS_URL, json=payload, timeout=10)
        print(f"Status Code: {response.status_code}")
        print(f"Response: {response.text.strip()}")
    except requests.RequestException as e:
        print(f"Request failed: {e}")


if __name__ == "__main__":
    main()
