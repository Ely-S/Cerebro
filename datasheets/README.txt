Datasheet Download Notes
========================

This folder contains downloaded datasheets for BOM items with publicly accessible links.

Downloaded as PDF:
- 01_arduino_nano33iot_ABX00027_datasheet.pdf
- 02_ti_ads1115_datasheet.pdf
- 03_microchip_mcp6022_datasheet.pdf
- 04_vishay_bpw34_datasheet.pdf
- 05_stmicroelectronics_tip31c_datasheet_view.html
- 06_yageo_cfr_series_datasheet.pdf
- 07_yageo_mfr_series_datasheet.pdf
- 08_kemet_goldmax_c0g_datasheet.pdf
- 09_kemet_goldmax_general_datasheet.pdf
- 14_3m_super33_tape_datasheet.pdf

Downloaded as HTML fallback pages (source sites blocked direct PDF):
- 10_panasonic_eeufr1a101_datasheet.html
- 11_nichicon_ufw_datasheet.html
- 12_littelfuse_215_fuse_datasheet.html
- 13_meanwell_gs06e_spec.html

Downloaded optics as HTML:
- optics_bpw34_datasheet.html
- optics_sfh4253b_datasheet.html
- optics_lst1_01g01_frd1_00_datasheet.html (superseded by SST-10-FR-B90-H730; archive)

Downloaded optics source PDFs (used for numeric validation):
- optics_sfh4253b_datasheet.pdf
- optics_sfh4253_datasheet.pdf
- optics_lst1_01g01_frd1_00_datasheet.pdf (superseded; 730nm primary now Cree JE2835AFR-N-0001A0000-N0000001)

Not publicly downloadable from the BOM links:
- Legacy option only: Epitex L850-05AU datasheet (vendor-provided on request)

Non-datasheet BOM items (N/A by design):
- Elastic headband, EVA foam, generic cable, prototyping supplies, USB cable

Required Tools & Equipment:
---------------------------
Hardware/Bench Tools:
- Digital Multimeter (DMM) (for continuity, resistance, diode, DC voltage)
- Oscilloscope or Logic Analyzer (2+ channels)
- Current-limited bench power supply (or 9V battery)
- Low-level signal/current injection setup
- Bench optical power measurement setup

Optical Calibration Materials:
- Opaque covering material (for dark readings)
- Stable, repeatable light source
- Phone camera (to verify 850nm IR LED emission)

Software/Compute:
- Computer with Arduino IDE and Arduino Serial Plotter
- Analysis script (Python or MATLAB) for +/- 2% precision validation

Demonstration Phase (In-Vivo):
- Reference pulse oximeter
- Supine surface (examination table) and safe standing area
