Instructions:
* Compile by invoking `make`
* Convert Prime+Store traces to square and multiply traces by issuing the command `./sigproc < <INPUT_FILE>`. You can pipe the result to a file for further processing in the next stage.
* Run the stiching algorithm, you can either run the any_pos version that expands the trace in both direction, or the single direction version that expands the trace in forward direction only. Invoke `./stitch_parallel_simd <OUTPUT_FILE_FROM_SIGPROC> <INITIAL TRACE>` or `stitch_parallel_simd_any_pos <OUTPUT_FILE_FROM_SIGPROC> <INITIAL TRACE>`
* You can choose the most relevant trace by using the find_start_sequences program that finds the most commonly occuring trace.
* Convert the stitched square and multiply sequences to binary using the `sm_to_exponent` program. This recocers the actual key.