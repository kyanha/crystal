// Proportional thick ~8x8 font

uint8 font_Police [257 * 8] =
{
  0x00, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x7e, 0x00,
  0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x81, 0x7E,
  0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E,
  0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x00,
  0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00,
  0x38, 0x7C, 0x38, 0xFE, 0xFE, 0x7C, 0x38, 0x7C,
  0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x7C,
  0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00,
  0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF,
  0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00,
  0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF,
  0x0F, 0x07, 0x0F, 0x7D, 0xCC, 0xCC, 0xCC, 0x78,
  0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18,
  0x3F, 0x33, 0x3F, 0x30, 0x30, 0x70, 0xF0, 0xE0,
  0x7F, 0x63, 0x7F, 0x63, 0x63, 0x67, 0xE6, 0xC0,
  0x99, 0x5A, 0x3C, 0xE7, 0xE7, 0x3C, 0x5A, 0x99,
  0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0xE0, 0x80, 0x00,
  0x02, 0x0E, 0x3E, 0xFE, 0x3E, 0x0E, 0x02, 0x00,
  0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18,
  0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,
  0x7F, 0xDB, 0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x00,
  0x3E, 0x63, 0x38, 0x6C, 0x6C, 0x38, 0xCC, 0x78,
  0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00,
  0x18, 0x3C, 0x7E, 0x18, 0x7E, 0x3C, 0x18, 0xFF,
  0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00,
  0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00,
  0x00, 0x18, 0x0C, 0xFE, 0x0C, 0x18, 0x00, 0x00,
  0x00, 0x30, 0x60, 0xFE, 0x60, 0x30, 0x00, 0x00,
  0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00,
  0x00, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x00, 0x00,
  0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x60, 0xF0, 0xF0, 0xF0, 0x60, 0x00, 0x60, 0x00,
  0xD8, 0xD8, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00,
  0x30, 0x7C, 0xC0, 0x78, 0x0C, 0xF8, 0x30, 0x00,
  0x00, 0xC6, 0xCC, 0x18, 0x30, 0x66, 0xC6, 0x00,
  0x38, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00,
  0x60, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x30, 0x60, 0xC0, 0xC0, 0xC0, 0x60, 0x30, 0x00,
  0xC0, 0x60, 0x30, 0x30, 0x30, 0x60, 0xC0, 0x00,
  0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00,
  0x00, 0x30, 0x30, 0xFC, 0x30, 0x30, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x60,
  0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00,
  0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00,
  0x7C, 0xC6, 0xCE, 0xDE, 0xF6, 0xE6, 0x7C, 0x00,
  0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xFC, 0x00,
  0x78, 0xCC, 0x0C, 0x38, 0x60, 0xCC, 0xFC, 0x00,
  0x78, 0xCC, 0x0C, 0x38, 0x0C, 0xCC, 0x78, 0x00,
  0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x1E, 0x00,
  0xFC, 0xC0, 0xF8, 0x0C, 0x0C, 0xCC, 0x78, 0x00,
  0x38, 0x60, 0xC0, 0xF8, 0xCC, 0xCC, 0x78, 0x00,
  0xFC, 0xCC, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00,
  0x78, 0xCC, 0xCC, 0x78, 0xCC, 0xCC, 0x78, 0x00,
  0x78, 0xCC, 0xCC, 0x7C, 0x0C, 0x18, 0x70, 0x00,
  0x00, 0x60, 0x60, 0x00, 0x00, 0x60, 0x60, 0x00,
  0x00, 0x60, 0x60, 0x00, 0x00, 0x60, 0x60, 0xC0,
  0x18, 0x30, 0x60, 0xC0, 0x60, 0x30, 0x18, 0x00,
  0x00, 0x00, 0xFC, 0x00, 0x00, 0xFC, 0x00, 0x00,
  0xC0, 0x60, 0x30, 0x18, 0x30, 0x60, 0xC0, 0x00,
  0x78, 0xCC, 0x0C, 0x18, 0x30, 0x00, 0x30, 0x00,
  0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x78, 0x00,
  0x30, 0x78, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0x00,
  0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x00,
  0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00,
  0xF0, 0xD8, 0xCC, 0xCC, 0xCC, 0xD8, 0xF0, 0x00,
  0xFC, 0xC0, 0xC0, 0xF0, 0xC0, 0xC0, 0xFC, 0x00,
  0xFC, 0xC0, 0xC0, 0xF0, 0xC0, 0xC0, 0xC0, 0x00,
  0x3C, 0x66, 0xC0, 0xC0, 0xCE, 0x66, 0x3E, 0x00,
  0xCC, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0xCC, 0x00,
  0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00,
  0x0C, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78, 0x00,
  0xCC, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xCC, 0x00,
  0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFC, 0x00,
  0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00,
  0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00,
  0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00,
  0xF8, 0xCC, 0xCC, 0xF8, 0xC0, 0xC0, 0xC0, 0x00,
  0x78, 0xCC, 0xCC, 0xCC, 0xDC, 0x78, 0x1C, 0x00,
  0xF8, 0xCC, 0xCC, 0xF8, 0xD8, 0xCC, 0xCC, 0x00,
  0x78, 0xCC, 0xE0, 0x70, 0x1C, 0xCC, 0x78, 0x00,
  0xFC, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00,
  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFC, 0x00,
  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00,
  0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00,
  0xC6, 0xC6, 0x6C, 0x38, 0x38, 0x6C, 0xC6, 0x00,
  0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x30, 0x30, 0x00,
  0xFE, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xFE, 0x00,
  0xF0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xF0, 0x00,
  0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02, 0x00,
  0xF0, 0x30, 0x30, 0x30, 0x30, 0x30, 0xF0, 0x00,
  0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
  0x60, 0x60, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x76, 0x00,
  0xE0, 0x60, 0x60, 0x7C, 0x66, 0x66, 0xDC, 0x00,
  0x00, 0x00, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x00,
  0x1C, 0x0C, 0x0C, 0x7C, 0xCC, 0xCC, 0x76, 0x00,
  0x00, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00,
  0x38, 0x6C, 0x60, 0xF0, 0x60, 0x60, 0xF0, 0x00,
  0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8,
  0xE0, 0x60, 0x6C, 0x76, 0x66, 0x66, 0xE6, 0x00,
  0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00,
  0x0C, 0x00, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78,
  0xE0, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0xE6, 0x00,
  0xE0, 0x60, 0x60, 0x60, 0x60, 0x60, 0xF0, 0x00,
  0x00, 0x00, 0xCC, 0xFE, 0xFE, 0xD6, 0xC6, 0x00,
  0x00, 0x00, 0xF8, 0xCC, 0xCC, 0xCC, 0xCC, 0x00,
  0x00, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00,
  0x00, 0x00, 0xDC, 0x66, 0x66, 0x7C, 0x60, 0xF0,
  0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0x1E,
  0x00, 0x00, 0xDC, 0x76, 0x66, 0x60, 0xF0, 0x00,
  0x00, 0x00, 0x7C, 0xC0, 0x78, 0x0C, 0xF8, 0x00,
  0x20, 0x60, 0xF8, 0x60, 0x60, 0x68, 0x30, 0x00,
  0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0x76, 0x00,
  0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00,
  0x00, 0x00, 0xC6, 0xD6, 0xFE, 0xFE, 0x6C, 0x00,
  0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00,
  0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8,
  0x00, 0x00, 0xFC, 0x98, 0x30, 0x64, 0xFC, 0x00,
  0x1C, 0x30, 0x30, 0xE0, 0x30, 0x30, 0x1C, 0x00,
  0x60, 0x60, 0x60, 0x00, 0x60, 0x60, 0x60, 0x00,
  0xE0, 0x30, 0x30, 0x1C, 0x30, 0x30, 0xE0, 0x00,
  0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x10, 0x38, 0x28, 0x6C, 0x44, 0xFE, 0x00,
  0x30, 0x78, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0x00,
  0xF8, 0xC0, 0xC0, 0xF8, 0xCC, 0xCC, 0xF8, 0x00,
  0xF8, 0xCC, 0xCC, 0xF8, 0xCC, 0xCC, 0xF8, 0x00,
  0xFC, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00,
  0x38, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0xFE, 0xC6,
  0xFC, 0xC0, 0xC0, 0xF8, 0xC0, 0xC0, 0xFC, 0x00,
  0xDB, 0xDB, 0x7E, 0x3C, 0x7E, 0xDB, 0xDB, 0x00,
  0x78, 0xCC, 0x0C, 0x38, 0x0C, 0xCC, 0x78, 0x00,
  0xCC, 0xCC, 0xDC, 0xFC, 0xEC, 0xCC, 0xCC, 0x00,
  0x78, 0xCC, 0xDC, 0xFC, 0xEC, 0xCC, 0xCC, 0x00,
  0xCC, 0xD8, 0xF0, 0xE0, 0xF0, 0xD8, 0xCC, 0x00,
  0x3C, 0x6C, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x00,
  0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00,
  0xCC, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0xCC, 0x00,
  0x78, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x78, 0x00,
  0xFC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x00,
  0xF8, 0xCC, 0xCC, 0xCC, 0xF8, 0xC0, 0xC0, 0x00,
  0x78, 0xCC, 0xC0, 0xC0, 0xC0, 0xCC, 0x78, 0x00,
  0xFC, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00,
  0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0xCC, 0x78, 0x00,
  0x7E, 0xDB, 0xDB, 0xDB, 0x7E, 0x18, 0x18, 0x00,
  0xCC, 0xCC, 0x78, 0x30, 0x78, 0xCC, 0xCC, 0x00,
  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFE, 0x06,
  0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0x0C, 0x0C, 0x00,
  0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xFF, 0x00,
  0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xFF, 0x03,
  0xE0, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x7C, 0x00,
  0xC6, 0xC6, 0xC6, 0xF6, 0xDE, 0xDE, 0xF6, 0x00,
  0xC0, 0xC0, 0xC0, 0xF8, 0xCC, 0xCC, 0xF8, 0x00,
  0x78, 0x8C, 0x06, 0x3E, 0x06, 0x8C, 0x78, 0x00,
  0xCE, 0xDB, 0xDB, 0xFB, 0xDB, 0xDB, 0xCE, 0x00,
  0x7C, 0xCC, 0xCC, 0xCC, 0x7C, 0x6C, 0xCC, 0x00,
  0x00, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x76, 0x00,
  0x00, 0x78, 0xC0, 0x78, 0xCC, 0xCC, 0x78, 0x00,
  0x00, 0x00, 0xF8, 0xCC, 0xF8, 0xCC, 0xF8, 0x00,
  0x00, 0x00, 0xFC, 0xC0, 0xC0, 0xC0, 0xC0, 0x00,
  0x00, 0x00, 0x3C, 0x6C, 0x6C, 0x6C, 0xFE, 0xC6,
  0x00, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00,
  0x00, 0x00, 0xDB, 0x7E, 0x3C, 0x7E, 0xDB, 0x00,
  0x00, 0x00, 0x78, 0xCC, 0x18, 0xCC, 0x78, 0x00,
  0x00, 0x00, 0xCC, 0xDC, 0xFC, 0xEC, 0xCC, 0x00,
  0x00, 0x30, 0xCC, 0xDC, 0xFC, 0xEC, 0xCC, 0x00,
  0x00, 0x00, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0x00,
  0x00, 0x00, 0x3C, 0x6C, 0xCC, 0xCC, 0xCC, 0x00,
  0x00, 0x00, 0xC6, 0xFE, 0xFE, 0xD6, 0xC6, 0x00,
  0x00, 0x00, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0x00,
  0x00, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00,
  0x00, 0x00, 0xFC, 0xCC, 0xCC, 0xCC, 0xCC, 0x00,
  0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88,
  0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA,
  0xDB, 0xF6, 0xDB, 0x6F, 0xDB, 0x7E, 0xD7, 0xED,
  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
  0x18, 0x18, 0x18, 0x18, 0xF8, 0x18, 0x18, 0x18,
  0x18, 0x18, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18,
  0x36, 0x36, 0x36, 0x36, 0xF6, 0x36, 0x36, 0x36,
  0x00, 0x00, 0x00, 0x00, 0xFE, 0x36, 0x36, 0x36,
  0x00, 0x00, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18,
  0x36, 0x36, 0xF6, 0x06, 0xF6, 0x36, 0x36, 0x36,
  0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
  0x00, 0x00, 0xFE, 0x06, 0xF6, 0x36, 0x36, 0x36,
  0x36, 0x36, 0xF6, 0x06, 0xFE, 0x00, 0x00, 0x00,
  0x36, 0x36, 0x36, 0x36, 0xFE, 0x00, 0x00, 0x00,
  0x18, 0x18, 0xF8, 0x18, 0xF8, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xF8, 0x18, 0x18, 0x18,
  0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, 0x00, 0x00,
  0x18, 0x18, 0x18, 0x18, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xFF, 0x18, 0x18, 0x18,
  0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x18, 0x18,
  0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
  0x18, 0x18, 0x18, 0x18, 0xFF, 0x18, 0x18, 0x18,
  0x18, 0x18, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18,
  0x36, 0x36, 0x36, 0x36, 0x37, 0x36, 0x36, 0x36,
  0x36, 0x36, 0x37, 0x30, 0x3F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3F, 0x30, 0x37, 0x36, 0x36, 0x36,
  0x36, 0x36, 0xF7, 0x00, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x00, 0xF7, 0x36, 0x36, 0x36,
  0x36, 0x36, 0x37, 0x30, 0x37, 0x36, 0x36, 0x36,
  0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
  0x36, 0x36, 0xF7, 0x00, 0xF7, 0x36, 0x36, 0x36,
  0x18, 0x18, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
  0x36, 0x36, 0x36, 0x36, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x00, 0xFF, 0x18, 0x18, 0x18,
  0x00, 0x00, 0x00, 0x00, 0xFF, 0x36, 0x36, 0x36,
  0x36, 0x36, 0x36, 0x36, 0x3F, 0x00, 0x00, 0x00,
  0x18, 0x18, 0x1F, 0x18, 0x1F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18,
  0x00, 0x00, 0x00, 0x00, 0x3F, 0x36, 0x36, 0x36,
  0x36, 0x36, 0x36, 0x36, 0xFF, 0x36, 0x36, 0x36,
  0x18, 0x18, 0xFF, 0x18, 0xFF, 0x18, 0x18, 0x18,
  0x18, 0x18, 0x18, 0x18, 0xF8, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1F, 0x18, 0x18, 0x18,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
  0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0xCC, 0xCC, 0xF8, 0xC0, 0x00,
  0x00, 0x00, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x00,
  0x00, 0x00, 0xFC, 0x30, 0x30, 0x30, 0x30, 0x00,
  0x00, 0x00, 0xCC, 0xCC, 0x7C, 0x0C, 0x78, 0x00,
  0x00, 0x00, 0x7E, 0xDB, 0xDB, 0x7E, 0x18, 0x00,
  0x00, 0x00, 0xCC, 0x78, 0x30, 0x78, 0xCC, 0x00,
  0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xFE, 0x06,
  0x00, 0x00, 0xCC, 0xCC, 0x7C, 0x0C, 0x0C, 0x00,
  0x00, 0x00, 0xDB, 0xDB, 0xDB, 0xDB, 0xFF, 0x00,
  0x00, 0x00, 0xDB, 0xDB, 0xDB, 0xDB, 0xFF, 0x03,
  0x00, 0x00, 0xE0, 0x60, 0x7C, 0x66, 0x7C, 0x00,
  0x00, 0x00, 0xC6, 0xC6, 0xF6, 0xDE, 0xF6, 0x00,
  0x00, 0x00, 0xC0, 0xC0, 0xF8, 0xCC, 0xF8, 0x00,
  0x00, 0x00, 0xF8, 0x0C, 0x7C, 0x0C, 0xF8, 0x00,
  0x00, 0x00, 0xCE, 0xDB, 0xFB, 0xDB, 0xCE, 0x00,
  0x00, 0x00, 0x7C, 0xCC, 0x7C, 0x6C, 0xCC, 0x00,
  0xD8, 0xFC, 0xC0, 0xF0, 0xC0, 0xC0, 0xFC, 0x00,
  0x6C, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00,
  0x60, 0x30, 0x18, 0x30, 0x60, 0x00, 0xFC, 0x00,
  0x18, 0x30, 0x60, 0x30, 0x18, 0x00, 0xFC, 0x00,
  0x0E, 0x1B, 0x1B, 0x18, 0x18, 0x18, 0x18, 0x18,
  0x18, 0x18, 0x18, 0x18, 0x18, 0xD8, 0xD8, 0x70,
  0x18, 0x18, 0x00, 0x7E, 0x00, 0x18, 0x18, 0x00,
  0x00, 0x76, 0xDC, 0x00, 0x76, 0xDC, 0x00, 0x00,
  0x38, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
  0x06, 0x0C, 0x08, 0x98, 0xD0, 0x70, 0x20, 0x20,
  0x78, 0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00,
  0xE0, 0x30, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x18, 0x3c, 0x46, 0xfb, 0xe7, 0x7e, 0x2c, 0x18
};

unsigned char width_Police [257] =
{
  8,8,8,8,8,8,8,7,8,8,8,8,8,8,8,8,		// 00-0f
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,		// 10-1f
  4,5,6,8,7,8,8,4,5,5,8,7,5,7,5,8,		// 20-2f
  8,7,7,7,8,7,7,7,7,7,4,4,6,7,6,7,		// 30-3f
  8,7,8,8,7,7,7,8,7,4,7,7,7,8,8,8,		// 40-4f
  7,7,7,7,7,7,7,8,8,7,8,5,8,5,8,4,		// 50-5f
  5,8,8,7,8,7,7,8,8,6,7,8,5,8,7,7,		// 60-6f
  8,8,8,7,6,8,7,8,8,7,7,7,4,7,8,8,		// 70-7f
  7,7,7,7,8,7,8,7,7,7,7,7,8,7,7,7,		// 80-8f
  7,7,7,7,8,7,8,7,8,8,8,8,7,8,8,7,		// 90-9f
  8,7,7,7,8,7,8,7,7,7,7,7,8,7,7,7,		// a0-af
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,		// b0-bf
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,		// c0-cf
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,		// d0-df
  7,7,7,7,8,7,8,7,8,8,8,8,7,7,8,7,		// e0-ef
  7,7,7,7,8,6,8,8,7,6,6,8,7,5,7,4,		// f0-ff
  8
};

csDefaultFont::CharRange ranges_Police[] =
{
  // 00 - 0f - Var. symbols
  {CS_FONT_DEFAULT_GLYPH,  1},
  {0x263a,  2},
  {0x2665,  2},
  {0x2663,  1},
  {0x2660,  1},
  {0x2022,  1},
  {0x25d8,  1},
  {0x25cb,  1},
  {0x25d9,  1},
  {0x2642,  1},
  {0x2640,  1},
  {0x266a,  2}, // Well, the glyphs really look like 16th notes...
		// but I doubt anyone cares anyway
  {0x263c,  1},
  // 10 - 1f - More symbols
  {0x25b6,  1},
  {0x25c0,  1},
  {0x2195,  1},
  {0x203c,  1},
  {0x00b6,  1},
  {0x00a7,  1},
  {0x25ac,  1},
  {0x21d5,  1}, // not really an exact match
  {0x2191,  1}, 
  {0x2193,  1}, 
  {0x2192,  1}, 
  {0x2190,  1}, 
  {0x2199,  1}, 
  {0x2194,  1}, 
  {0x25b2,  1}, 
  {0x25bc,  1}, 
  // 20 - 7e - ASCII
  {0x0020, 95}, 
  // 7f (Close match)
  {0x2616,  1}, 
  // 80 - af - Cyrillic uppercase, Cyrillic lowercase 1
  {0x0410, 48}, 
  // b0 - df - Blocks & borders
  {0x2591,  3}, 
  {0x2502,  1}, 
  {0x2524,  1}, 
  {0x2561,  2}, 
  {0x2556,  1}, 
  {0x2555,  1}, 
  {0x2563,  1}, 
  {0x2551,  1}, 
  {0x2557,  1}, 
  {0x255d,  1}, 
  {0x255c,  1}, 
  {0x255b,  1}, 
  {0x2510,  1}, 
  {0x2514,  1}, 
  {0x2534,  1}, 
  {0x252c,  1}, 
  {0x251c,  1}, 
  {0x2500,  1}, 
  {0x253c,  1}, 
  {0x255e,  2}, 
  {0x255a,  1}, 
  {0x2554,  1}, 
  {0x2569,  1}, 
  {0x2566,  1}, 
  {0x2560,  1}, 
  {0x2550,  1}, 
  {0x256c,  1}, 
  {0x2567,  2}, 
  {0x2564,  2}, 
  {0x2559,  1}, 
  {0x2558,  1}, 
  {0x2552,  2}, 
  {0x256b,  1}, 
  {0x256a,  1}, 
  {0x2518,  1}, 
  {0x250c,  1}, 
  {0x2588,  1}, 
  {0x2584,  1}, 
  {0x258c,  1}, 
  {0x2590,  1}, 
  {0x2580,  1}, 
  // e0 - ef - Cyrillic lowercase 2
  {0x0440, 16}, 
  // f0 - ff - Misc. stuff
  {0x00cb,  1}, 
  {0x00eb,  1}, 
  {0x2265,  1},
  {0x2264,  1},
  {0x2320,  2}, 
  {0x00f7,  1}, 
  {0x2248,  1}, 
  {0x00b0,  1}, 
  {0x2219,  1}, 
  {0x00b7,  1}, 
  {0x221a,  1}, 
  {0x207f,  1}, 
  {0x00b2,  1}, 
  {0x25a0,  1}, 
  {0x00a0,  1}, 
  // 100 - replacer
  {0xFFFD,  1}, 
  {0, 0}
};

int underline_position_Police = 1;

int underline_thickness_Police = 1;

int text_height_Police = 13;
