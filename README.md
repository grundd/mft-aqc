Instructions on how to run the **MFT A-QC tool**:
- A local O2 or QualityControl build is needed.
- If QC objects need to be downloaded, first open a separate terminal window and connect to lxtunnel using `sshuttle`: specify your CERN username in `tunnel.sh`, then run the script and provide your sudo password + a CERN password. Leave the terminal window running as long as you need, all your internet connection goes through it. When finished, you may disconnect using CTRL+C.
- The tool can be run using `MFT-AQC_run.sh`. In the shell script, you need to specify a configuration file from the `input` folder - either create a new one or select one from the existing. The tool then uses the following macros:
    - `downloadQCObjects.cxx`: QC objects for selected runs are downloaded from ali-QCDB (data) or CCDB-test (MC). They will be stored inside .root files in `rootFiles/<period>/<run>_<pass>.root`.
    - `plotQCobjects.cxx`: plots of all requested QC objects are created and stored in `plots/<group-name>/...`. The list of histograms to be plotted is specified in `input/objectsToPlot.csv` (there must be 'y' in the next-to-last column of the .csv file).
    - `prepareSlides.cxx`: Latex presentation is created automatically for every input configuration. The .tex files are stored in the `latex/` folder.

How to **create an input configuration file**:
- To see the list of all supported options that can be requested, see: `include/loadConfiguration`.
- Parameters need to be provided as: `key=value` (no spaces)!
- The keyword `runs:` needs to be followed by a run list (each run on a separate line).

Use cases for the MFT A-QC tool:
1. To create a **run-by-run comparison**
    - In this case, all runs from the configuration file are plotted in the same canvas and ratios with respect to a selected reference run are calculated and shown in bottom panels. For visibility purposes, a maximum of 30 runs per one configuration file is allowed; if you need more, you may need to split them into multiple config files.
    - Mandatory configuration parameters:
        - `period`: period name; if the input configuration is a mixture of runs from more periods, use: `period=more`
        - `pass`: name of the pass; if you need different pass for each run, then use `pass=more` and specify the pass for each run individually, separatate it by a comma, i.e.: `555234,apass1` etc.
        - `refRun`: number of the reference run

2. To create a **comparison between multiple passes**
    - In this case, every run is plotted in a separate canvas, but with more passes. Ratios against a selected reference pass are created and shown in bottom panels. The configuration file can contain as many runs as needed.
    - Mandatory configuration parameters:
        - `period`: period name; if the input configuration is a mixture of runs from more periods, use: `period=more`
        - `pass`: a list of all passes that will be downloaded for each run, e.g.: `pass=cpass0,apass1,...` (as many as needed)
        - `refPass`: name of the pass that will be used as a reference




