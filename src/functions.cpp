#include "header.hpp"

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
