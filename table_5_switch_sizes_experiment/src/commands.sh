#Commands used:
#For compile:

for i in {1..16}; do SWITCH_SIZE=$i make out; done;
scp ../build/main* acrypto@ToshibaLaptop:/tmp/

#To run:

for i in {1..16}; do echo "Switch size $i" >> /tmp/res.txt; /tmp/main$i test_gates >> /tmp/res.txt;  done