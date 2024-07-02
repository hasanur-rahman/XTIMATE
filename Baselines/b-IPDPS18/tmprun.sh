# List="dpot astro fish sedov-pres blast2-pres eddy_velx_f4 yf17_pres yf17_temp bump_dense"
List="temperature.d64.bin"
set -- $List

for i
do
    echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    echo $i
    echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    echo "interval block"
    ./run.sh -c zfp -i inputdata/blockintervalSample/$i.sample -t junk | egrep -h "time|compress"
    echo "..........................................."
done

