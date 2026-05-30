# TAL — Texture AutoLookup

TAL (Texture AutoLookup) is a suite of soil texture classification tools developed by Christopher Teh to automatically determine soil texture classes from particle size distribution data.

First developed in 1996, TAL evolved across multiple computing platforms and programming environments, including:

* Lotus 1-2-3 add-ins
* Microsoft Excel add-ins
* standalone Windows applications
* reusable C++ source code libraries
* Psion 5 / 5mx handheld devices

The project was designed to provide a fast, lightweight, and practical solution for soil scientists, agronomists, students, and laboratory personnel who routinely classify soils from sand, silt, and clay percentages.

---

## Overview

TAL automatically determines soil texture classes based on user-supplied particle size distribution data.

The software supports multiple soil texture classification schemes and was progressively expanded into several implementations targeting different computing environments and user workflows.

Over time, TAL became a long-running scientific utility spanning several generations of scientific computing platforms.

---

## Included Components

This repository may contain several TAL implementations developed over different periods.

### TAL for Excel

A Microsoft Excel add-in for automatic soil texture classification directly within spreadsheets.

Features include:

* texture lookup within Excel worksheets,
* integration into laboratory workflows,
* lightweight deployment,
* compatibility with modern Windows systems,
* and continued maintenance.

This is currently the primary maintained TAL implementation.

---

### TAL for Windows

A standalone Windows application for soil texture classification independent of spreadsheet software.

Features include:

* graphical user interface,
* support for multiple classification systems,
* and standalone operation outside Excel.

This version remains functional on modern Windows systems.

---

### TAL for Lotus 1-2-3 *(Archived)*

The original TAL implementation developed for Lotus 1-2-3 during the 1990s.

This version represents the earliest stage of the project and reflects the spreadsheet environment commonly used in scientific and agricultural offices at the time.

This implementation is archived and no longer maintained.

---

### TALCPP — C++ Source Library

An object-oriented C++ implementation of the TAL classification engine.

This component was developed for programmers and scientific software developers wishing to integrate soil texture classification into their own applications.

The library emphasizes:

* modular design,
* reusable classification logic,
* and extensibility across classification systems.

---

### TAL for Psion 5 / 5mx *(Archived)*

A portable handheld implementation developed for the Psion 5 and Psion 5mx devices.

This version allowed field-based soil texture lookup during an era when handheld scientific computing devices were increasingly used in agriculture and environmental work.

This implementation is archived and no longer maintained.

---

## Purpose and Design Philosophy

TAL was designed around several core principles:

* simplicity,
* portability,
* low computational overhead,
* ease of use,
* and practical usefulness in routine soil laboratory work.

Rather than functioning as a large modelling system, TAL intentionally focused on solving a narrow but extremely common scientific task efficiently and reliably.

That simplicity contributed to the project's unusually long lifespan across multiple hardware generations and software ecosystems.

---

## Scientific Context

Soil texture classification remains fundamental in:

* soil survey,
* irrigation planning,
* hydrological modelling,
* crop modelling,
* soil fertility assessment,
* and environmental science.

TAL was developed to reduce repetitive manual lookup work and minimize interpretation errors during routine classification.

---

## Current Status

The repository contains both actively maintained and historical software components.

### Maintained

* TAL for Excel
* TAL for Windows

### Archived

* TAL for Lotus 1-2-3
* TAL for Psion 5 / 5mx

Some archived components may require legacy operating systems, emulators, or compatibility layers to run correctly on modern computers.

---

## Repository Contents

Depending on the release, the repository may include:

* source code,
* compiled executables,
* Excel add-ins,
* Lotus 1-2-3 add-ins,
* Psion application files,
* C++ libraries,
* manuals,
* screenshots,
* help documentation,
* and example datasets.

---

## License

This project is released under the MIT License.

See the `LICENSE` file for details.

---

## Citation

If referencing TAL in academic or educational work, please cite:

Teh, C. B. S. Texture AutoLookup (TAL): Soil Texture Classification Tools.

---

## Website

Project homepage:

https://www.christopherteh.com/tal

---

## Historical Note

TAL reflects an era when scientific software often evolved incrementally across changing computing platforms rather than being rewritten entirely for each new ecosystem.

The progression from Lotus 1-2-3 to Excel, standalone Windows applications, handheld devices, and reusable C++ libraries mirrors both the evolution of scientific computing and the enduring practical importance of soil texture classification in agricultural and environmental sciences.
