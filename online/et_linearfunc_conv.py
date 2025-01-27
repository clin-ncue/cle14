#!/bin/python3
#
# et_linearfunc_conv.py
# - This python script converts a ET-linear function file into the online setting files for KOTO. 
#
#

import os
import sys

fin_name = "/Users/clin/workspace/online/et_linear/et_linearfunc_92.txt"
map_name = "/Users/clin/workspace/online/et_linear/ch_map_CSI.txt"
out_dir_name = "/Users/clin/workspace/online/et_linear/output"

n_csi = 2716
conv_factor = 2**16

#
# load map file
# - Should have the following format 
#    - n_mod
#    - mod-id crate-id adc-id ch-id
# ...
#
if not os.path.exists( map_name ):
    print("ERROR: Map file does not exist.")
    sys.exit()

fmap = open( map_name, 'r')
n_mod = int( fmap.readline() )
print("# mod read in the map = ", n_mod)

map_dict = {}
for line in fmap:
    map_list = [ int(x) for x in line.split() ]
    map_dict.update( {tuple(map_list[1:]) : map_list[0] })

fmap.close()

if len(map_dict) != n_mod:
    print("ERROR: #mod inconsistent.")
    sys.exit()

#
# load linear functions
#
if not os.path.exists( fin_name ):
    print("ERROR: Linear function file does not exist.")

fin = open( fin_name, 'r')
par_dict = {}
for line in fin:
    id, orig_p0, orig_p1 = line.split()
    p0 = int( float(orig_p0) * conv_factor )
    p1 = int( float(orig_p1) * conv_factor )
    par_dict.update( {int(id): [p0, p1]})

fin.close()
print(" # mod read in the input file = ", len(par_dict) )

#
# Generate online parameter setting file
#
for crate_id in range(11):
    fout = open( out_dir_name + '/Crate' + str(crate_id) + '_ETLinearFunc.txt','w')
    
    for adc_id in range(16):
        for ch_id in range(16):
            index = (crate_id, adc_id, ch_id)
            mod_id = map_dict.get(index, -1)

            if mod_id < 0 or mod_id >= n_csi :
                this_p0 = 0
                this_p1 = 0
            else:
                this_p0 = (par_dict[ mod_id ])[0]
                this_p1 = (par_dict[ mod_id ])[1]

            fout.write(f'{adc_id}\t{ch_id}\t{this_p0}\t{this_p1}\n')
    fout.close()
