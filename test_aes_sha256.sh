wget https://raw.githubusercontent.com/Rainysponge/emp-sh2pc/master/install_test.py
python install_test.py --deps --tool --ot --sh2pc
cd emp-sh2pc
mkdir build
cmake ..
make
cd ..
bash aes_run.sh
bash sha256_run.sh
