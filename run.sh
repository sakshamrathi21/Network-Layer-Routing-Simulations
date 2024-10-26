make -j
./bin/main rp example_testcases/1_clique.netspec example_testcases/1_clique.msgs --delay 1 > out
./bin/main rp example_testcases/2_nhop.netspec example_testcases/2_nhop.msgs --delay 1  >> out
./bin/main rp example_testcases/3_lfmr.netspec example_testcases/3_lfmr.msgs --delay 1  >> out
./bin/main rp example_testcases/4_mrwl.netspec example_testcases/4_mrwl.msgs --delay 1  >> out