# GenomeLibrary
A genome library implemented in C++.

This library enables the following uses:

- Maintain a library of genomes from multiple organisms; the user can add new genomes to this library. 
- Allow the user to search the library for a specified DNA sequence and identify all genomes in the library that contain that DNA sequence or any SNiP of it. 
- Allow the user to present the genome/DNA of a new organism and quickly identify all genomes in the library that have a high percentage of matching DNA with the presented genome. 

The gene sequence is downloaded from https://www.ncbi.nlm.nih.gov/genome/ in FASTA file format, which is then parsed and loaded to be stored in the library.