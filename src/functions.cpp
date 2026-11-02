#include "header.hpp"

void applyPitch() {
  float ratio = pow(2.0, pitchSteps / 12.0);

  for (int i = 0; i < NUM_KEYS; i++) {
    setKeyFrequency(i, buttons[i].frequency * ratio);   // notes
  }

  setSongSpeed(ratio);                                  // songs
}

void loadFilesFromSD() {
  root = SD.open("/");
  fileCount = 0;
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;

    String name = String(entry.name());
    String lower = name;
    lower.toLowerCase();

    if (!entry.isDirectory() && fileCount < MAX_FILES
        && !name.startsWith(".") && lower.endsWith(".wav")) {
      fileNames[fileCount] = name;
      fileCount++;
    }
    entry.close();
  }
  root.close();
}
