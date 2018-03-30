#!/bin/sh

export LD_LIBRARY_PATH=:/usr/local/lib

rm -rf data
ln -s  /opt/ifacesdk/data_908 data

./NanJinAlgTestD -t 0.92 -f 8 -v 12 -i /extra_disk/disk1/nanjin_test/nanjin20w -n /extra_disk/disk1/nanjin_test/nanjinlist/testimg.txt -o /extra_disk/disk1/nanjin_test/result

