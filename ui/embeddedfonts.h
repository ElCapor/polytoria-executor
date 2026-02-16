/**
 * @file embeddedfonts.h
 * @brief Embedded font data for Polyhack v3
 * 
 * This file contains font data embedded directly into the binary.
 * Fonts are loaded from memory instead of external files for better portability.
 * 
 * AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
 * Generated: 2026-02-16 02:53:20
 * 
 * Font License: SNPro font family is used under appropriate licensing.
 */

#ifndef EMBEDDEDFONTS_H
#define EMBEDDEDFONTS_H

#include <cstddef>

namespace EmbeddedFonts {

// Font data structures
struct FontData {
    const char* name;           // Font name identifier
    const unsigned char* data;  // Pointer to font data
    size_t size;                // Size of font data in bytes
};

// SNPro_Black.ttf
extern const unsigned char SNPro_Black_Data[];
extern const size_t SNPro_Black_Size;

// SNPro_BlackItalic.ttf
extern const unsigned char SNPro_BlackItalic_Data[];
extern const size_t SNPro_BlackItalic_Size;

// SNPro_Bold.ttf
extern const unsigned char SNPro_Bold_Data[];
extern const size_t SNPro_Bold_Size;

// SNPro_BoldItalic.ttf
extern const unsigned char SNPro_BoldItalic_Data[];
extern const size_t SNPro_BoldItalic_Size;

// SNPro_ExtraBold.ttf
extern const unsigned char SNPro_ExtraBold_Data[];
extern const size_t SNPro_ExtraBold_Size;

// SNPro_ExtraBoldItalic.ttf
extern const unsigned char SNPro_ExtraBoldItalic_Data[];
extern const size_t SNPro_ExtraBoldItalic_Size;

// SNPro_ExtraLight.ttf
extern const unsigned char SNPro_ExtraLight_Data[];
extern const size_t SNPro_ExtraLight_Size;

// SNPro_ExtraLightItalic.ttf
extern const unsigned char SNPro_ExtraLightItalic_Data[];
extern const size_t SNPro_ExtraLightItalic_Size;

// SNPro_Italic.ttf
extern const unsigned char SNPro_Italic_Data[];
extern const size_t SNPro_Italic_Size;

// SNPro_Light.ttf
extern const unsigned char SNPro_Light_Data[];
extern const size_t SNPro_Light_Size;

// SNPro_LightItalic.ttf
extern const unsigned char SNPro_LightItalic_Data[];
extern const size_t SNPro_LightItalic_Size;

// SNPro_Medium.ttf
extern const unsigned char SNPro_Medium_Data[];
extern const size_t SNPro_Medium_Size;

// SNPro_MediumItalic.ttf
extern const unsigned char SNPro_MediumItalic_Data[];
extern const size_t SNPro_MediumItalic_Size;

// SNPro_Regular.ttf
extern const unsigned char SNPro_Regular_Data[];
extern const size_t SNPro_Regular_Size;

// SNPro_SemiBold.ttf
extern const unsigned char SNPro_SemiBold_Data[];
extern const size_t SNPro_SemiBold_Size;

// SNPro_SemiBoldItalic.ttf
extern const unsigned char SNPro_SemiBoldItalic_Data[];
extern const size_t SNPro_SemiBoldItalic_Size;

// Convenience array of all fonts
inline const FontData AllFonts[] = {
    {"SNPro_Black", SNPro_Black_Data, SNPro_Black_Size},
    {"SNPro_BlackItalic", SNPro_BlackItalic_Data, SNPro_BlackItalic_Size},
    {"SNPro_Bold", SNPro_Bold_Data, SNPro_Bold_Size},
    {"SNPro_BoldItalic", SNPro_BoldItalic_Data, SNPro_BoldItalic_Size},
    {"SNPro_ExtraBold", SNPro_ExtraBold_Data, SNPro_ExtraBold_Size},
    {"SNPro_ExtraBoldItalic", SNPro_ExtraBoldItalic_Data, SNPro_ExtraBoldItalic_Size},
    {"SNPro_ExtraLight", SNPro_ExtraLight_Data, SNPro_ExtraLight_Size},
    {"SNPro_ExtraLightItalic", SNPro_ExtraLightItalic_Data, SNPro_ExtraLightItalic_Size},
    {"SNPro_Italic", SNPro_Italic_Data, SNPro_Italic_Size},
    {"SNPro_Light", SNPro_Light_Data, SNPro_Light_Size},
    {"SNPro_LightItalic", SNPro_LightItalic_Data, SNPro_LightItalic_Size},
    {"SNPro_Medium", SNPro_Medium_Data, SNPro_Medium_Size},
    {"SNPro_MediumItalic", SNPro_MediumItalic_Data, SNPro_MediumItalic_Size},
    {"SNPro_Regular", SNPro_Regular_Data, SNPro_Regular_Size},
    {"SNPro_SemiBold", SNPro_SemiBold_Data, SNPro_SemiBold_Size},
    {"SNPro_SemiBoldItalic", SNPro_SemiBoldItalic_Data, SNPro_SemiBoldItalic_Size},
};

inline constexpr size_t FontCount = sizeof(AllFonts) / sizeof(AllFonts[0]);

} // namespace EmbeddedFonts

#endif // EMBEDDEDFONTS_H
