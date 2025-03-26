wget https://raw.githubusercontent.com/Rainysponge/emp-sh2pc/master/install_test.py
python install_test.py --deps --tool --ot --sh2pc
cd emp-sh2pc
mkdir build
cmake ..
make
./bin/test_circuit_file_aes 1 1234 & ./bin/test_circuit_file_aes 2 1234
./bin/test_circuit_file_sha256 1 1234 & ./bin/test_circuit_file_sha256 2 1234
