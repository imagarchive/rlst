# read the verilog file
read_verilog xor_source.v

# elaborate design hierarchy (verifies completeness of all modules and sets the top module).
# If top module is not specified, yosys tries to infer it automatically
# hierarchy -check -top jk_ff
hierarchy -check -top top

# the hgih-level stuff (whatever the fuck that means. Again)
proc; opt; fsm; opt; memory; opt

# mapping to internal cell library (whatever the fuck that means)
techmap

# flatten netlist (no modules inside modules)
flatten

# clean after flattening
opt_clean -purge
opt_merge
opt_expr
opt_share
opt -fine
opt

setundef -zero
clean

# expand flip flops into logic gates
# techmap -map +/techmap.v

# further reduce to basic gates
simplemap

# optimization after flip flop decompositions
opt

# force use of only specific gates
# abc -g AND,OR,XOR,NAND,NOR,OAI3,AOI3,OAI4,AOI4,MUX,XNOR
abc -g AND,OR

# reduce bit vectors to one dimension
splitnets -ports

# final optimization
opt

# write gates data to json format
json -o gate_data.json

# write gates metadata to json format
jny -o gate_metadata.json

# cleanup
clean

# show -format dot -prefix ./mygraph
show
