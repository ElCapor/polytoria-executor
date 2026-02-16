#!/usr/bin/env python3
"""
TTF to Header Generator for Polyhack v3

This script converts TTF font files to C++ header files with embedded binary data.
The generated files allow fonts to be loaded from memory instead of external files.

Usage:
    python ttf_to_header.py <input_dir> <output_header> <output_cpp>

Example:
    python ttf_to_header.py fonts/ ui/embeddedfonts.h ui/embeddedfonts.cpp

Author: Polyhack Team
License: CC-BY-SA 4.0
"""

import os
import sys
import argparse
from pathlib import Path
from datetime import datetime


def get_font_name(filename: str) -> str:
    """Convert filename to font variable name."""
    # Remove .ttf extension and convert to valid C++ identifier
    name = Path(filename).stem
    # Replace spaces and hyphens with underscores
    name = name.replace(' ', '_').replace('-', '_')
    # Remove any non-alphanumeric characters except underscore
    name = ''.join(c for c in name if c.isalnum() or c == '_')
    return name


def generate_header(output_path: str, fonts: list) -> None:
    """Generate the C++ header file with font declarations."""
    
    header_content = f'''/**
 * @file embeddedfonts.h
 * @brief Embedded font data for Polyhack v3
 * 
 * This file contains font data embedded directly into the binary.
 * Fonts are loaded from memory instead of external files for better portability.
 * 
 * AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
 * Generated: {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
 * 
 * Font License: SNPro font family is used under appropriate licensing.
 */

#ifndef EMBEDDEDFONTS_H
#define EMBEDDEDFONTS_H

#include <cstddef>

namespace EmbeddedFonts {{

// Font data structures
struct FontData {{
    const char* name;           // Font name identifier
    const unsigned char* data;  // Pointer to font data
    size_t size;                // Size of font data in bytes
}};

'''
    
    # Generate extern declarations for each font
    for font_name, _ in fonts:
        header_content += f'''// {font_name}.ttf
extern const unsigned char {font_name}_Data[];
extern const size_t {font_name}_Size;

'''
    
    # Generate convenience array
    header_content += '''// Convenience array of all fonts
inline const FontData AllFonts[] = {
'''
    
    for font_name, _ in fonts:
        header_content += f'    {{"{font_name}", {font_name}_Data, {font_name}_Size}},\n'
    
    header_content += '''};

inline constexpr size_t FontCount = sizeof(AllFonts) / sizeof(AllFonts[0]);

} // namespace EmbeddedFonts

#endif // EMBEDDEDFONTS_H
'''
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(header_content)
    
    print(f"[Generator] Generated header: {output_path}")


def generate_cpp(output_path: str, fonts: list) -> None:
    """Generate the C++ source file with font data."""
    
    cpp_content = f'''/**
 * @file embeddedfonts.cpp
 * @brief Embedded font data definitions for Polyhack v3
 * 
 * This file contains the actual font binary data embedded as byte arrays.
 * 
 * AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
 * Generated: {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
 * 
 * Font License: SNPro font family is used under appropriate licensing.
 */

#include <ui/embeddedfonts.h>

'''
    
    # Generate data arrays for each font
    for font_name, font_path in fonts:
        with open(font_path, 'rb') as f:
            font_data = f.read()
        
        cpp_content += f'''// ============================================================================
// {font_name}.ttf ({len(font_data)} bytes)
// ============================================================================
const unsigned char EmbeddedFonts::{font_name}_Data[] = {{
'''
        
        # Generate hex data with 16 bytes per line
        for i in range(0, len(font_data), 16):
            chunk = font_data[i:i+16]
            hex_bytes = ', '.join(f'0x{b:02X}' for b in chunk)
            if i + 16 < len(font_data):
                cpp_content += f'    {hex_bytes},\n'
            else:
                cpp_content += f'    {hex_bytes}\n'
        
        cpp_content += f'''}};
const size_t EmbeddedFonts::{font_name}_Size = sizeof(EmbeddedFonts::{font_name}_Data);

'''
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(cpp_content)
    
    print(f"[Generator] Generated source: {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description='Convert TTF font files to C++ embedded data files'
    )
    parser.add_argument(
        'input_dir',
        help='Directory containing TTF font files'
    )
    parser.add_argument(
        'output_header',
        help='Output path for the C++ header file (.h)'
    )
    parser.add_argument(
        'output_cpp',
        help='Output path for the C++ source file (.cpp)'
    )
    
    args = parser.parse_args()
    
    input_dir = Path(args.input_dir)
    output_header = args.output_header
    output_cpp = args.output_cpp
    
    if not input_dir.exists():
        print(f"Error: Input directory '{input_dir}' does not exist")
        sys.exit(1)
    
    # Find all TTF files
    ttf_files = sorted(input_dir.glob('*.ttf'))
    
    if not ttf_files:
        print(f"Error: No TTF files found in '{input_dir}'")
        sys.exit(1)
    
    print(f"[Generator] Found {len(ttf_files)} TTF files")
    
    # Prepare font list (name, path)
    fonts = []
    for ttf_path in ttf_files:
        font_name = get_font_name(ttf_path.name)
        fonts.append((font_name, str(ttf_path)))
        print(f"[Generator]   - {ttf_path.name} -> {font_name}")
    
    # Generate files
    generate_header(output_header, fonts)
    generate_cpp(output_cpp, fonts)
    
    # Calculate total size
    total_size = sum(os.path.getsize(str(p)) for _, p in fonts)
    print(f"[Generator] Total embedded font data: {total_size:,} bytes ({total_size / 1024:.1f} KB)")
    print("[Generator] Done!")


if __name__ == '__main__':
    main()
