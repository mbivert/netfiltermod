# http://www.cyberciti.biz/faq/howto-install-kernel-headers-package/
echo '===== MAKE ====='
sudo make

echo '===== INSMOD ====='
sudo insmod sample_module_for_techblog.ko

echo '===== DISPLAY ====='
sudo dmesg -c /dev/null

echo '===== RMMOD ====='
sudo rmmod sample_module_for_techblog.ko

echo '===== DISPLAY ====='
sudo dmesg -c /dev/null

echo '===== MAKE CLEAN ====='
sudo make clean
