List="velocityy.d64 temperature.d64.bin"
set -- $List

for i
do
    echo $i 
    #python sample.py -s interval -r block -i $i # for ZFP
    python sample.py -s interval -r chunk -i $i # for SZ
done
