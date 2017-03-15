---
title: 'Ensight4Matlab: let's you read, process, and write files in EnSight® Gold format from your C++ or MATLAB® code.'
tags:
  - Data import
  - Ensight Gold
  - CFD
  - Matlab
  - Matlab-Wrapper
  - C++
authors:
 - name: Andre Schmeißer
   orcid: 0000-0002-4400-9635
   affiliation: 1
 - name: Daniel Burkhardt
 - name: Dominik Linn
   affiliation: 1
 - name: Johannes Schnebele
   affiliation: 1
 - name: Manuel Ettmüller
   affiliation: 1
 - name: Simone Gramsch
   orcid: 0000-0002-6720-9840
   affiliation: 1
 - name: Walter Arne
   orcid: 0000-0001-5364-3788
   affiliation: 1
affiliations:
 - name: Fraunhofer Institute for Industrial Mathematics ITWM, Kaiserslautern, Germany
   index: 1
date: 15 March 2017
bibliography: paper.bib
---

# Summary

Ensight4Matlab let's you read, process, and write files in EnSight&reg; Gold format [1] from your C++ or MATLAB&reg; code [2].

Data in the EnSight Gold format represents a 3D mesh and variable fields defined over the domain of this mesh. The mesh and/or variables may be either static in time or time-varying (transient). The EnSight Gold format is used to save e.g. CFD and CAE data in research and industrial applications, and widely supported by many software tools.

This packages provides a C++ based library to process e.g. CFD data sets in EnSight Format, which you can directly link (statically or dynamically) to your application. Additionally, it provides a language binding for MATLAB to easily use the C++ library in your MATLAB scripts, e.g. for analysis and rapid protoyping.

We have extensively used this library to integrate our own simulations of fiber dynamics (e.g. [3]) with CFD data computed by the commercial CFD solver ANSYS&reg; Fluent [4].

Repository: https://github.com/ITWM-TVFS/Ensight4Matlab


# References
1. EnSight, CEI software, Inc. Apex, NC. https://www.ensight.com/

2. MATLAB version 9.1.0 (R2016b). Natick, Massachusetts: The MathWorks Inc., 2016. https://www.mathworks.com/

3. S. Gramsch, A. Schmeißer, R. Wegener, Simulation of Fiber Dynamics and Fiber-Wall Contacts for Airlay Processes, in: Progress in Industrial Mathematics at ECMI 2014, Springer, 2016.
 
4. ANSYS&reg; Academic Research, Release 17.2. http://www.ansys.com/

