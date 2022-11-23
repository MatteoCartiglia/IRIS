mkdir -p src/.obj/
mkdir -p data/customBiasValues/
mkdir -p data/customBiasValues/DAC/
mkdir -p data/customBiasValues/BIASGEN/

make clean
make
./alive_interface
