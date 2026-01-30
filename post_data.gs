// Constants
const PROJECT_NAME = "PROJECT-XXX";
const SHEET_NAME = "data";
const MAX_COUNT = 1440;  // default value (can be overridden by json.max_count)

/**
 * Entry point for POST requests (Web App)
 */
function doPost(e) {
  try {
    const jsonString = e.postData.contents;
    const json = JSON.parse(jsonString);

    // Validate project_name
    if (!json.project_name || json.project_name !== PROJECT_NAME) {
      return ContentService
        .createTextOutput(JSON.stringify({ error: "invalid project_name" }))
        .setMimeType(ContentService.MimeType.JSON);
    }

    // Add record
    addRecord(json);

    return ContentService
      .createTextOutput(JSON.stringify({ response: "ok" }))
      .setMimeType(ContentService.MimeType.JSON);

  } catch (error) {
    Logger.log(error);
    return ContentService
      .createTextOutput(JSON.stringify({ error: error.message || "unknown error" }))
      .setMimeType(ContentService.MimeType.JSON);
  }
}

/**
 * Add a new record to the sheet (latest data on top)
 * @param {Object} json - Parsed JSON from doPost
 */
function addRecord(json) {
  const ss = SpreadsheetApp.getActiveSpreadsheet();
  const sheet = ss.getSheetByName(SHEET_NAME) || ss.insertSheet(SHEET_NAME);

  // Basic input validation
  if (!json.headers || !Array.isArray(json.headers) ||
      !json.datas  || !Array.isArray(json.datas)  ||
      json.headers.length !== json.datas.length) {
    throw new Error("headers and datas must be arrays of the same length");
  }

  // Create header row if the sheet is empty or A1 is blank
  if (sheet.getLastRow() === 0 || sheet.getRange(1, 1).isBlank()) {
    const header = ["time", ...json.headers];
    sheet.getRange(1, 1, 1, header.length).setValues([header]);
  }

  // Current timestamp (Japan time)
  const now = Utilities.formatDate(
    new Date(),
    Session.getScriptTimeZone(),
    "yyyy/MM/dd HH:mm:ss"
  );

  // New row data
  const newRow = [now, ...json.datas];

  // Get existing data (from row 2 to last row)
  const lastRow = sheet.getLastRow();
  let data = [];
  if (lastRow >= 2) {
    const range = sheet.getRange(2, 1, lastRow - 1, newRow.length);
    data = range.getValues();
  }

  // Insert new row at the beginning (latest first)
  data.unshift(newRow);

  // Enforce maximum row count
  const maxCount = json.max_count || MAX_COUNT;
  if (data.length > maxCount) {
    data = data.slice(0, maxCount);
  }

  // Write back the data if there is any
  if (data.length > 0) {
    sheet.getRange(2, 1, data.length, data[0].length).setValues(data);
  }
}

/**
 * Debug helper: Test addRecord with sample data
 */
function testAddRecord() {
  const sampleJson = {
    "project_name": "PROJECT-XXX",
    "headers": ["temp", "humidity", "co2"],
    "datas": [23.5, 58.2, 845],
    "max_count": 1440
  };

  addRecord(sampleJson);
}