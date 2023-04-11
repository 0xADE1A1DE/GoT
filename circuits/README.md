# Gates of Time Circuit Evaluation Readme

## Step 1: Opening A Screen Session
Open a screen session by invoking the command
`
	screen
`

This is important as the result of the program is heavily affected by the environment in which it is executing. This ensures consistency with the results reported in our paper.

## Step 2: Run the Experiments with the Prefetcher Disabled
First we will run the set of circuit experiments with the prefefetcher disabled. To do this, run the command

`
	sudo -s
`

This will elevate your privilege to root which is required to write the machine specific registers (MSRs) to disable the prefetcher. Next, run the following command

`
	./disable_prefetcher.sh
`

This will disable the prefetcher. 

Next run the script that performs all the experiments. To do so, run the following command

`
	./run_experiment_all_no_prefetcher.sh <DESTINATION_FOLDER>
`

The folder is the folder you choose to store the experiment results.


## Step 3: Run the Experiments with the Prefetcher Enabled
Next perform the experiments with the prefetcher enabled. First we enable the prefetcher, run the following commands sequentially.

`
	sudo -s
`

This will give you the root privilege required to enable the prefetcher. Then we run the following command

`
	./enable_prefetcher
`

Next run the script that performs all the experiments. To do so, run the following command

`
	./run_experiment_all_with_prefetcher.sh <DESTINATION_FOLDER>
`

The folder is the folder you choose to store the experiment results. You should choose a different folder from the experiment with the prefetcher disabled.

## Step 4: Verify the Results of the Experiments


Inside each experiment directories, you will find a directory named `RESULT`. This directory contains the result of the experiment. The following files can be found inside this directory


	> all.csv
	> ALL_MEAN
	> ALL_MEDIAN
	> final.csv
	> FINAL_MEAN
	> FINAL_MEDIAN


The file `all.csv` contains the combined data from all samples of the experiment. To account for failed readings resulting from environmental noise, we collected more samples than required: specifically, 40,000 samples, with each sample consisting of 100 runs, instead of the 10,000 samples mentioned in the paper. From these excess samples, we randomly selected 10,000 samples using the Linux command `shuf -n 10000` and stored the output in `final.csv`. The `FINAL_MEAN` and `FINAL_MEDIAN` values represent the mean and median accuracy of the data in `final.csv` respectively. These values were used in the paper to describe the mean and median accuracy of the circuits.

## Step 5: Generate Figures

To replicate the figures presented in the paper, navigate to the `FIGURES` directory. This directory contains four sub-directories named `alu_combined`, `gol_glider`, `gol_spinner`, and `sha1`.

Some figures, such as those depicting the game of life glider and T-tetromino shapes, were obtained by capturing screenshots during the execution of the game of life experiment. You can locate these shapes in the log file generated during step 2 and 3.

Other figures, such as histograms, were produced using the raw experimental data gathered in step 2 and 3. To generate these histograms, copy the raw data (final.csv) from step 2 and 3 to their respective sub-directories under `FIGURES` and execute the command:

`
	./plot.sh
`

This should generate a pdf file in each sub-directory that displays histograms that are identical to those presented in the paper.
