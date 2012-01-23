echo '===== MAKE ====='
sudo make

echo '===== EMPTY /dev/null ====='
sudo dmesg -c /dev/null

echo '===== INSMOD ====='
sudo insmod sample_module_for_techblog.ko

echo '===== DISPLAY ====='
sudo dmesg -c

echo '===== RMMOD ====='
sudo rmmod sample_module_for_techblog # le .ko est facultatif

echo '===== DISPLAY ====='
sudo dmesg -c

echo '===== MAKE CLEAN ====='
sudo make clean
