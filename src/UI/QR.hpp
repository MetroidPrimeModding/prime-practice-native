#pragma once

namespace GUI {
  enum class QR_ECC {
    LOW = 0,
    MEDIUM = 1,
    QUARTILE = 2,
    HIGH = 3

  };
  void initQR();
  void qrNewFrame();
  void drawQRCode(const char *data, float sizeMultiplier = 6.0f, QR_ECC ecc = QR_ECC::LOW);
}
