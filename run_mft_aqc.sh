# chmod +x run_mft_aqc.sh
#!/bin/bash

do_aqc () {
    root -q 'download_qc_objects.cxx("'$1'")'
    root -q -b 'plot_qc_objects.cxx("'$1'")'
    root -q 'prepare_slides.cxx("'$1'")'
}

for file in input_to_run/*.txt; 
do
  echo "INPUT CONFIGURATION FILE: $file"
  do_aqc $file
done